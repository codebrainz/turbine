#pragma once

#include <turbine/common/error.hpp>
#include <turbine/net/address_info.hpp>
#include <turbine/net/tcp/socket.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace turbine::net::tcp {

  class server : public tcp::socket {
  public:
    using ptr = std::shared_ptr<server>;

    auto bind() {
      return tcp::socket::bind();
    }

    auto listen(int backlog = default_backlog) {
      return tcp::socket::listen(backlog);
    }

    auto accept() {
      return tcp::socket::accept();
    }

    server(uint16_t port, bool auto_bind_listen = false)
        : server{"", port, auto_bind_listen} {
    }

    server(std::string const &host, uint16_t port,
           bool auto_bind_listen = false)
        : tcp::socket{address_info{host, port}} {
      if (auto_bind_listen) {
        bind();
        listen();
      }
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return tcp::socket::make<server>(std::forward<Args>(args)...);
    }
  };

  using server_ptr = server::ptr;

} // namespace turbine::net::tcp
