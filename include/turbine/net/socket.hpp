#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/net/sockaddr.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <cstdint>
#include <cstring>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace turbine::net {

  class socket : public posix::fd {
  public:
    enum class recv_flags : int {
      none = 0,
      close_on_exec = MSG_CMSG_CLOEXEC,
      dont_wait = MSG_DONTWAIT,
      error_queue = MSG_ERRQUEUE,
      out_of_band = MSG_OOB,
      peek = MSG_PEEK,
      real_length = MSG_TRUNC,
      wait_all = MSG_WAITALL,
    };

    enum class send_flags : int {
      none = 0,
      confirm = MSG_CONFIRM,
      dont_route = MSG_DONTROUTE,
      dont_wait = MSG_DONTWAIT,
      end_of_record = MSG_EOR,
      more = MSG_MORE,
      no_signal = MSG_NOSIGNAL,
      out_of_band = MSG_OOB,
      fast_open = MSG_FASTOPEN,
    };

  protected:
    socket(int filedes, sockaddr const &addr)
        : posix::fd{filedes}
        , m_addr{addr} {
    }

  public:
    sockaddr &address() noexcept {
      return m_addr;
    }

    sockaddr const &address() const noexcept {
      return m_addr;
    }

    size_t recv(void *out, size_t count, recv_flags fl = recv_flags::none) {

      if (auto n = ::recv(fileno(), out, count, static_cast<int>(fl)); n >= 0) {
        return static_cast<size_t>(n);
      }
      throw system_error{};
    }

    template <class T>
    size_t recv(T &out, recv_flags fl = recv_flags::none) {
      return recv(&out, sizeof out, fl);
    }

    size_t send(void const *out, size_t count,
                send_flags fl = send_flags::none) {
      if (auto n = ::send(fileno(), out, count, static_cast<int>(fl)); n >= 0) {
        return static_cast<size_t>(n);
      }
      throw system_error{};
    }

    template <class T>
    size_t send(T const &out, send_flags fl = send_flags::none) {
      return send(&out, sizeof out, fl);
    }

  private:
    sockaddr m_addr;
  };

} // namespace turbine::net

M_ENABLE_ENUM_FLAGS(turbine::net::socket::recv_flags);
M_ENABLE_ENUM_FLAGS(turbine::net::socket::send_flags);
