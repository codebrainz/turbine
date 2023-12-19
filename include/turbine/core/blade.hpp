#pragma once

#include <turbine/io/loop.hpp>
#include <turbine/linux/eventfd.hpp>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace turbine::core {

  class rotor;

  class blade {
  public:
    using ptr = std::unique_ptr<blade>;

    blade(core::rotor &rotor)
        : m_id{next_id()}
        , m_rotor{rotor}
        , m_loop{}
        , m_read_event{linux::eventfd::make()}
        , m_write_event{linux::eventfd::make()}
        , m_lock{}
        , m_thread{&blade::thread_func, this} {
    }

    ~blade() noexcept {
      std::printf("blade::~blade()\n");
    }

    uint64_t id() const noexcept {
      return m_id;
    }

    core::rotor &rotor() noexcept {
      return m_rotor;
    }

    core::rotor const &rotor() const noexcept {
      return m_rotor;
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return ptr(new blade{std::forward<Args>(args)...});
    }

  private:
    uint64_t m_id;
    core::rotor &m_rotor;
    io::loop m_loop;
    linux::eventfd_ptr m_read_event;
    linux::eventfd_ptr m_write_event;
    std::mutex m_lock;
    std::jthread m_thread;

    static uint64_t next_id() {
      static std::atomic<uint64_t> id_{0};
      return id_++;
    }

    void thread_func() {
      m_loop.add_poll(m_read_event, io::poll_source::poll_events::in);
      m_loop.add_poll(m_write_event, io::poll_source::poll_events::out);
      // ...
    }
  };

  using blade_ptr = blade::ptr;
  using blade_list = std::vector<blade_ptr>;

} // namespace turbine::core
