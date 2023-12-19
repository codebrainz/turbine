#pragma once

#include <turbine/net/address.hpp>
#include <turbine/net/address_info.hpp>
#include <turbine/net/socket.hpp>

#include <cstdint>
#include <memory>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace turbine::net::tcp {

  class socket : public net::socket {
    friend class net::socket;

  public:
    using ptr = std::shared_ptr<socket>;

  protected:
    socket(int filedes, net::address const &addr) : net::socket{filedes, addr} {
    }

    socket(address_info const &info) : net::socket{info} {
    }

  public:
    std::string ip() const {
      char ip[INET_ADDRSTRLEN + 1] = {0};
      ::sockaddr_in const &ai = address();
      inet_ntop(AF_INET, &ai.sin_addr, ip, sizeof ip);
      return ip;
    }

    uint16_t port() const {
      ::sockaddr_in const &ai = address();
      return ntohs(ai.sin_port);
    }

    template <class T, class... Args>
    auto make(Args &&...args) {
      static_assert(std::is_base_of_v<tcp::socket, T>);
      return std::shared_ptr<T>(new T{std::forward<Args>(args)...});
    }

  protected:
    auto bind() {
      return net::socket::bind();
    }

    auto listen(int backlog = default_backlog) {
      return net::socket::listen(backlog);
    }

    auto accept() {
      return net::socket::accept<socket>();
    }

    auto connect() {
      return net::socket::connect();
    }
  };

  using socket_ptr = socket::ptr;

} // namespace turbine::net::tcp
