#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/net/address_info.hpp>
#include <turbine/net/socket_address.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace turbine::net {

  class socket : public posix::fd {
  public:
    using ptr = std::shared_ptr<socket>;

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

    static constexpr const int default_backlog = SOMAXCONN;

  protected:
    socket(int filedes, socket_address const &addr)
        : posix::fd{filedes}
        , m_addr{addr} {
    }

    socket(net::address_info const &info) : posix::fd{-1}, m_addr{} {
      int f = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
      if (f < 0)
        throw system_error{};
      fileno(f);
      m_addr.data(info->ai_addr, info->ai_addrlen);
    }

  public:
    socket_address &address() noexcept {
      return m_addr;
    }

    socket_address const &address() const noexcept {
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

    void bind() {
      auto const &na = address();
      if (::bind(fileno(), na.data(), na.size()) != 0)
        throw system_error{};
    }

    void listen(int backlog = default_backlog) {
      if (::listen(fileno(), backlog) != 0)
        throw system_error{};
    }

    template <class T>
    auto accept() {
      ::sockaddr_storage addr{};
      ::socklen_t addr_len = sizeof addr;
      if (int f = ::accept(fileno(), (::sockaddr *)&addr, &addr_len); f >= 0) {
        socket_address a{(::sockaddr const *)&addr, addr_len};
        return make<T>(f, a);
      }
      throw system_error{};
    }

    void connect() {
      auto const &na = address();
      if (::connect(fileno(), na.data(), na.size()) != 0)
        throw system_error{};
    }

    template <class T, class... Args>
    auto make(Args &&...args) {
      static_assert(std::is_base_of_v<socket, T>);
      return std::shared_ptr<T>(new T{std::forward<Args>(args)...});
    }

  private:
    socket_address m_addr;
  };

  using socket_ptr = socket::ptr;

} // namespace turbine::net

M_ENABLE_ENUM_FLAGS(turbine::net::socket::recv_flags);
M_ENABLE_ENUM_FLAGS(turbine::net::socket::send_flags);
