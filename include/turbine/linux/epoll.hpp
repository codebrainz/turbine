#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>

#include <sys/epoll.h>

#undef linux

namespace turbine::linux {

  class epoll : public posix::fd {
  public:
    enum class flags : int {
      none = 0,
      close_on_exec = EPOLL_CLOEXEC,
    };

    enum class events : uint32_t {
      none = 0,
      in = EPOLLIN,
      out = EPOLLOUT,
      read_hangup = EPOLLRDHUP,
      priority = EPOLLPRI,
      error = EPOLLERR,
      hangup = EPOLLHUP,
    };

    enum class input_flags : uint32_t {
      none = 0,
      edge_triggered = EPOLLET,
      one_shot = EPOLLONESHOT,
      wakeup = EPOLLWAKEUP,
      exclusive = EPOLLEXCLUSIVE,
    };

    epoll(flags fl = flags::none)
        : posix::fd{::epoll_create1(static_cast<int>(fl))} {
      if (fileno() < 0)
        throw system_error{};
    }

    void add(int fd, epoll_event const &ev) {
      if (epoll_ctl(fileno(), EPOLL_CTL_ADD, fd,
                    const_cast<epoll_event *>(&ev)) != 0) {
        throw system_error{};
      }
    }

    template <class T>
    void add(int fd, events events, input_flags input_flags, T data) {
      static_assert(sizeof(T) <= sizeof(uint64_t),
                    "T must be less than 8 bytes");
      epoll_event e{};
      std::memcpy(&e.data.u64, &data, sizeof(T));
      e.events =
          static_cast<uint32_t>(events) | static_cast<uint32_t>(input_flags);
      return add(fd, e);
    }

    void add(int fd, events events, input_flags input_flags) {
      return add(fd, events, input_flags, fd);
    }

    void mod(int fd, epoll_event const &ev) {
      if (epoll_ctl(fileno(), EPOLL_CTL_MOD, fd,
                    const_cast<epoll_event *>(&ev)) != 0) {
        throw system_error{};
      }
    }

    template <class T>
    void mod(int fd, events events, input_flags input_flags, T data) {
      static_assert(sizeof(T) <= sizeof(uint64_t),
                    "T must be less than 8 bytes");
      epoll_event e{};
      std::memcpy(&e.data.u64, &data, sizeof(T));
      e.events =
          static_cast<uint32_t>(events) | static_cast<uint32_t>(input_flags);
      return mod(fd, e);
    }

    void mod(int fd, events events, input_flags input_flags) {
      return mod(fd, events, input_flags, fd);
    }

    void del(int fd, epoll_event const &ev) {
      if (epoll_ctl(fileno(), EPOLL_CTL_DEL, fd,
                    const_cast<epoll_event *>(&ev)) != 0) {
        throw system_error{};
      }
    }

    void del(int fd) {
      epoll_event e{};
      return del(fd, e);
    }

    template <class T>
    uint32_t wait(T &events, int64_t timeout = -1) {
      assert(timeout <= static_cast<int64_t>(INT32_MAX));
      if (auto n = epoll_wait(fileno(), events.data(), events.size(),
                              static_cast<int32_t>(timeout));
          n >= 0) {
        return static_cast<uint32_t>(n);
      }
      throw system_error{};
    }

    template <class T, class U>
    uint32_t wait(T &events, std::chrono::duration<U> timeout) {
      const auto timeout_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
      return wait(events, static_cast<int64_t>(timeout_ms.count()));
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return fd::make<epoll>(std::forward<Args>(args)...);
    }
  };

} // namespace turbine::linux

M_ENABLE_ENUM_FLAGS(turbine::linux::epoll::flags);
M_ENABLE_ENUM_FLAGS(turbine::linux::epoll::events);
M_ENABLE_ENUM_FLAGS(turbine::linux::epoll::input_flags);
