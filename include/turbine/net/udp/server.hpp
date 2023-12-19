#pragma once

#include <turbine/net/udp/socket.hpp>

namespace turbine::net::udp {

  class server : public udp::socket {
  public:
    using ptr = std::shared_ptr<server>;

    auto bind() {
      return udp::socket::bind();
    }

    auto listen(int backlog = default_backlog) {
      return udp::socket::listen(backlog);
    }

    server(uint16_t port, bool auto_bind_listen = false)
        : server{"", port, auto_bind_listen} {
    }

    server(std::string const &host, uint16_t port,
           bool auto_bind_listen = false)
        : udp::socket{address_info{host, port}} {
      if (auto_bind_listen) {
        bind();
        listen();
      }
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return udp::socket::make<server>(std::forward<Args>(args)...);
    }
  };

  using server_ptr = server::ptr;

} // namespace turbine::net::udp
