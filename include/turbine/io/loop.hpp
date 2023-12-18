#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/macros.hpp>
#include <turbine/io/idle_source.hpp>
#include <turbine/io/poll_source.hpp>
#include <turbine/io/source.hpp>
#include <turbine/io/timeout_source.hpp>
#include <turbine/linux/epoll.hpp>
#include <turbine/posix/fd.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <exception>
#include <functional>
#include <unordered_map>
#include <vector>

namespace turbine::io {

  class loop {
    using event_map = std::unordered_map<int, poll_source_ptr>;
    using ep_events = std::array<epoll_event, 1024>;

    class source_wrapper {
    public:
      source_wrapper(source_ptr src) : m_id{next_id()}, m_src{std::move(src)} {
        assert(m_src);
      }

      source *operator->() {
        return m_src.get();
      }

      source const *operator->() const {
        return m_src.get();
      }

      source_ptr operator*() {
        return m_src;
      }

      bool operator<(source_wrapper const &other) const noexcept {
        assert(m_src);
        assert(other.m_src);
        const auto p1 = m_src->priority_level();
        const auto p2 = other.m_src->priority_level();
        return (p1 < p2) || (p1 == p2 && m_id < other.m_id);
      }

    private:
      uint64_t m_id;
      source_ptr m_src;

      static uint64_t next_id() {
        static std::atomic<uint64_t> id_{0};
        return id_++;
      }
    };

    using source_list = std::vector<source_wrapper>;

  public:
    using func_setup = std::function<void(void)>;
    using func_teardown = std::function<void(void)>;
    using func_error = std::function<bool(turbine::exception const &)>;

    loop(linux::epoll::flags ep_fl = linux::epoll::flags::none)
        : m_running{false}
        , m_exit_code{0}
        , m_ep{ep_fl}
        , m_map{}
        , m_events{}
        , m_sources{}
        , m_ready_sources{}
        , m_setup{}
        , m_teardown{}
        , m_error{} {
    }

    func_setup setup_func(func_setup fnc) noexcept {
      std::swap(m_setup, fnc);
      return fnc;
    }

    func_teardown teardown_func(func_teardown fnc) noexcept {
      std::swap(m_teardown, fnc);
      return fnc;
    }

    func_error error_func(func_error fnc) noexcept {
      std::swap(m_error, fnc);
      return fnc;
    }

    int run() {
      if (m_running)
        throw error{"event loop is already running"};
      m_running = true;
      m_exit_code = 0;
      if (m_setup)
        m_setup();
      while (m_running) {
        if (!m_error)
          iterate();
        else {
          try {
            iterate();
          } catch (turbine::exception const &e) {
            if (!m_error(e))
              throw e;
          }
        }
      }
      if (m_teardown)
        m_teardown();
      return m_exit_code;
    }

    void quit(int exit_code) {
      m_running = false;
      m_exit_code = exit_code;
    }

    template <class T, class... Args>
    source::pointer_type<T> emplace(Args &&...args) {
      auto s = source::make<T>(*this, std::forward<Args>(args)...);
      if (add(s))
        return s;
      return nullptr;
    }

    template <class... Args>
    auto add_idle(Args &&...args) {
      return emplace<idle_source>(std::forward<Args>(args)...);
    }

    template <class... Args>
    auto add_timeout(Args &&...args) {
      return emplace<timeout_source>(std::forward<Args>(args)...);
    }

    template <class... Args>
    auto add_poll(Args &&...args) {
      return emplace<poll_source>(std::forward<Args>(args)...);
    }

    bool remove(source &src) {
      if (src.kind() == source::kind::poll) {
        int fd = static_cast<poll_source *>(&src)->fileno();
        m_ep.del(fd);
        m_map.erase(fd);
      }
      for (size_t i = 0; i < m_sources.size(); ++i) {
        if ((*m_sources[i]).get() == &src) {
          m_sources.erase(m_sources.begin() + i);
          return true;
        }
      }
      return false;
    }

    bool remove(source_ptr src) {
      if (src)
        return remove(*src);
      return false;
    }

  private:
    bool m_running;
    int m_exit_code;
    linux::epoll m_ep;
    event_map m_map;
    ep_events m_events;
    source_list m_sources;
    source_list m_ready_sources;
    func_setup m_setup;
    func_teardown m_teardown;
    func_error m_error;

    bool add(source_ptr src) {
      assert(src);
      switch (src->kind()) {
        case source::kind::timeout:
        case source::kind::idle: {
          for (auto &s : m_sources) {
            if ((*s).get() == src.get())
              return false;
          }
          m_sources.emplace_back(src);
          return true;
        }
        case source::kind::poll: {
          auto s = std::static_pointer_cast<poll_source>(src);
          assert(s);
          int fno = s->fileno();
          epoll_event ev{};
          ev.data.fd = fno;
          ev.events = static_cast<uint32_t>(s->m_watch_events);
          m_ep.add(fno, ev);
          if (m_map.emplace(fno, std::move(s)).second) {
            m_sources.emplace_back(s);
            return true;
          }
          return false;
        }
      }
      M_UNREACHABLE();
    }

    void sort_sources(source_list &sources) {
      std::sort(std::begin(sources), std::end(sources));
    }

    void iterate() {
      int64_t timeout = -1;

      m_ready_sources.clear();

      // step 1: prepare
      for (auto &src : m_sources) {
        int64_t to = -1;
        if (src->prepare(to) && src->kind() != source::kind::idle) {
          m_ready_sources.push_back(src);
        } else {
          // FIXME: re-write this logic better
          if (to < 0 && timeout < 0) {
            // no change
          } else if (timeout < 0 && to >= 0) {
            timeout = to;
          } else if (to >= 0 && to < timeout) {
            timeout = to;
          }
        }
      }

      // step 2: dispatch already ready sources
      if (!m_ready_sources.empty()) {
        sort_sources(m_ready_sources);
        for (auto &src : m_ready_sources) {
          if (src->dispatch() == source::result::remove)
            remove(*src);
        }
        m_ready_sources.clear();
      }

      // step 3: poll file descriptors
      if (timeout < 0)
        timeout = -1;
      auto n = m_ep.wait(m_events, timeout);
      for (auto i = 0u; i < n; i++) {
        auto const &e = m_events[i];
        if (auto found = m_map.find(e.data.fd); found != m_map.end()) {
          found->second->m_ready_events =
              static_cast<poll_source::poll_events>(e.events);
        }
      }

      // step 4: check which sources are now ready
      for (auto &src : m_sources) {
        if (src->check())
          m_ready_sources.push_back(src);
      }

      // step 5: dispatch now ready sources
      if (!m_ready_sources.empty()) {
        sort_sources(m_ready_sources);
        for (auto &src : m_ready_sources) {
          if (src->dispatch() == source::result::remove)
            remove(*src);
        }
        m_ready_sources.clear();
      }
    }
  };

} // namespace turbine::io
