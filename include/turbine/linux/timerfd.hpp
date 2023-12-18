#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/posix/fd.hpp>

#include <chrono>
#include <cstring>
#include <ctime>

#include <sys/timerfd.h>

#undef linux

namespace turbine::linux {

  class timerfd : public posix::fd {
  public:
    enum class clock : int {
      monotonic = CLOCK_MONOTONIC,
      realtime = CLOCK_REALTIME,
      boottime = CLOCK_BOOTTIME,
      realtime_alarm = CLOCK_REALTIME_ALARM,
      boottime_alarm = CLOCK_BOOTTIME_ALARM,
    };

    enum class flags : int {
      none = 0,
      close_on_exec = TFD_CLOEXEC,
      non_blocking = TFD_NONBLOCK,
    };

    timerfd(clock clk = clock::monotonic, flags fl = flags::none)
        : posix::fd{::timerfd_create(static_cast<int>(clk),
                                     static_cast<int>(fl))}
        , m_its{} {
      if (fileno() < 0)
        throw system_error{};
    }

    void arm() {
      if (::timerfd_settime(fileno(), 0, &m_its, nullptr) != 0)
        throw system_error{};
    }

    void disarm() {
      itimerspec its{{0, 0}, {0, 0}};
      if (::timerfd_settime(fileno(), 0, &its, nullptr) != 0)
        throw system_error{};
    }

    template <class T>
    void set(std::chrono::duration<T> value,
             std::chrono::duration<T> interval) {
      std::memset(&m_its, 0, sizeof(itimerspec));
      m_its.it_value =
          ns_to_ts(std::chrono::duration_cast<std::chrono::nanoseconds>(value));
      m_its.it_interval = ns_to_ts(
          std::chrono::duration_cast<std::chrono::nanoseconds>(interval));
      if (::timerfd_settime(fileno(), 0, &m_its, nullptr) != 0)
        throw system_error{};
    }

    template <class T>
    void get(std::chrono::duration<T> &value,
             std::chrono::duration<T> &interval) {
      value = std::chrono::duration_cast<T>(ts_to_ns(m_its.it_value));
      interval = std::chrono::duration_cast<T>(ts_to_ns(m_its.it_interval));
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return fd::make<timerfd>(std::forward<Args>(args)...);
    }

  private:
    itimerspec m_its;

    static std::chrono::nanoseconds ts_to_ns(timespec ts) {
      const auto dur = std::chrono::seconds{ts.tv_sec} +
                       std::chrono::nanoseconds{ts.tv_nsec};
      return std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
    }

    static timespec ns_to_ts(std::chrono::nanoseconds ns) {
      const auto secs = std::chrono::duration_cast<std::chrono::seconds>(ns);
      ns -= secs;
      return timespec{secs.count(), ns.count()};
    }
  };

} // namespace turbine::linux

M_ENABLE_ENUM_FLAGS(turbine::linux::timerfd::flags);
