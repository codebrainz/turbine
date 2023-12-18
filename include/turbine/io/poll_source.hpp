#pragma once

#include <turbine/io/source.hpp>
#include <turbine/linux/epoll.hpp>

#include <functional>

namespace turbine::io {

  class loop;

  class poll_source : public source {
    friend class loop;

  public:
    using ptr = pointer_type<poll_source>;
    using poll_events = linux::epoll::events;
    using callback = std::function<result(poll_events)>;
    using callback_self = std::function<result(poll_source &, poll_events)>;

  protected:
    poll_source(io::loop &loop, posix::fd_ptr f, poll_events watch_events,
                callback cb, enum source::priority pri = default_priority)
        : poll_source{loop, std::move(f), watch_events,
                      [cb = std::move(cb)](auto &, auto e) { return cb(e); },
                      pri} {
    }

    poll_source(io::loop &loop, posix::fd_ptr f, poll_events watch_events,
                callback_self cb, enum source::priority pri = default_priority)
        : source{loop, pri,
                 [this, cb = std::move(cb)](auto &) {
                   if (M_LIKELY(cb))
                     return cb(*this, this->ready_events());
                   return result::keep_going;
                 }}
        , m_fd{std::move(f)}
        , m_watch_events{watch_events}
        , m_ready_events{poll_events::none} {
      assert(m_fd);
    }

  public:
    posix::fd &fd() noexcept {
      assert(m_fd);
      return *m_fd;
    }

    posix::fd const &fd() const noexcept {
      assert(m_fd);
      return *m_fd;
    }

    int fileno() const noexcept {
      return fd().fileno();
    }

    poll_events ready_events() const noexcept {
      return m_ready_events;
    }

  protected:
    enum kind kind() const noexcept final {
      return kind::poll;
    }

    bool prepare(int64_t &timeout_ms) final {
      timeout_ms = -1;
      return false;
    }

    bool check() final {
      return (m_watch_events & m_ready_events) != poll_events::none;
    }

    result dispatch() override {
      if ((m_watch_events & m_ready_events) != poll_events::none)
        return source::dispatch();
      return result::keep_going;
    }

    template <class T>
    constexpr static bool has_event(T events, T event) {
      static_assert(sizeof(T) <= sizeof(uint64_t));
      return static_cast<uint64_t>(events) & static_cast<uint64_t>(event);
    }

  private:
    posix::fd_ptr m_fd;
    poll_events m_watch_events;
    poll_events m_ready_events; // set by io::loop
  };

  using poll_source_ptr = poll_source::ptr;

} // namespace turbine::io
