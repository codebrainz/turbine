#pragma once

#include <turbine/common/error.hpp>
#include <turbine/net/tcp/socket.hpp>
#include <turbine/net/tcp/socket_address.hpp>

#include <memory>

namespace turbine::net::tcp {

  class socket_server : public tcp::socket {
  public:
    using ptr = std::shared_ptr<socket_server>;

  protected:
    template <class... Args>
    socket_server(Args &&...args) : tcp::socket{std::forward<Args>(args)...} {
    }

  public:
    void bind() {
      if (::bind(fileno(), net::socket::address().data(),
                 net::socket::address().size()) != 0) {
        throw system_error{};
      }
    }

    void listen(int backlog = default_backlog) {
      if (::listen(fileno(), backlog) != 0)
        throw system_error{};
    }

    tcp::socket::ptr accept() {
      ::sockaddr_storage addr{};
      ::socklen_t addr_len = sizeof addr;
      if (int f = ::accept(fileno(), (::sockaddr *)&addr, &addr_len); f >= 0) {
        net::socket_address a{(::sockaddr const *)&addr, addr_len};
        return make<tcp::socket>(f, a);
      }
      throw system_error{};
    }
  };

  using socket_server_ptr = socket_server::ptr;

} // namespace turbine::net::tcp
