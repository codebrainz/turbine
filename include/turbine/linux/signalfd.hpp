#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/posix/fd.hpp>
#include <turbine/posix/sigset.hpp>

#include <cassert>
#include <csignal>

#include <sys/signalfd.h>

#undef linux

namespace turbine::linux {

  class signalfd : public posix::fd {
  public:
    enum class flags : int {
      none = 0,
      close_on_exec = SFD_CLOEXEC,
      non_blocking = SFD_NONBLOCK,
    };

    signalfd(enum flags fl = flags::none) : signalfd{posix::sigset{}, fl} {
    }

    signalfd(signalfd &&other) noexcept
        : posix::fd{-1}
        , m_signals{}
        , m_flags{} {
      fileno(other.fileno(-1));
      std::swap(m_signals, other.m_signals);
      std::swap(m_flags, other.m_flags);
    }

    signalfd(posix::sigset const &ss, enum flags fl = flags::none)
        : posix::fd{-1}
        , m_signals{ss}
        , m_flags{fl} {
      sigset_t s{m_signals};
      const auto old_fd = fileno();
      if (auto f = ::signalfd(old_fd, &s, static_cast<int>(fl)); f >= 0) {
        assert(f == old_fd || old_fd == -1);
        fileno(f);
      } else {
        throw system_error{};
      }
    }

    signalfd &operator=(posix::sigset const &ss) {
      signals(ss);
      return *this;
    }

    posix::sigset &signals() noexcept {
      return m_signals;
    }

    posix::sigset const &signals() const noexcept {
      return m_signals;
    }

    void signals(posix::sigset const &ss) {
      m_signals = ss;
      sigset_t s{m_signals};
      const auto old_fd = fileno();
      if (auto f = ::signalfd(old_fd, &s, static_cast<int>(m_flags)); f >= 0) {
        assert(f == old_fd || old_fd == -1);
        fileno(f);
      } else {
        throw system_error{};
      }
    }

    enum flags flags() const noexcept {
      return m_flags;
    }

    void flags(enum flags fl) {
      m_flags = fl;
      sigset_t s{m_signals};
      const auto old_fd = fileno();
      if (auto f = ::signalfd(old_fd, &s, static_cast<int>(m_flags)); f >= 0) {
        assert(f == old_fd || old_fd == -1);
        fileno(f);
      } else {
        throw system_error{};
      }
    }

    bool add(int num) {
      bool added = m_signals.add(num);
      sigset_t s{m_signals};
      const auto old_fd = fileno();
      if (auto f = ::signalfd(old_fd, &s, static_cast<int>(m_flags)); f >= 0) {
        assert(f == old_fd || old_fd == -1);
        fileno(f);
      } else {
        throw system_error{};
      }
      return added;
    }

    bool del(int num) {
      bool removed = m_signals.del(num);
      if (!removed)
        return false;
      sigset_t s{m_signals};
      const auto old_fd = fileno();
      if (auto f = ::signalfd(old_fd, &s, static_cast<int>(m_flags)); f >= 0) {
        assert(f == old_fd || old_fd == -1);
        fileno(f);
      } else {
        throw system_error{};
      }
      return true;
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return fd::make<signalfd>(std::forward<Args>(args)...);
    }

  private:
    posix::sigset m_signals;
    enum flags m_flags;
  };

} // namespace turbine::linux

M_ENABLE_ENUM_FLAGS(enum turbine::linux::signalfd::flags);
