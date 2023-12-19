#pragma once

#include <turbine/common/error.hpp>
#include <turbine/net/tcp/socket.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace turbine::net::tcp {

  class client : public tcp::socket {
  public:
    using ptr = std::shared_ptr<client>;

    auto connect() {
      return tcp::socket::connect();
    }

    client(uint16_t port, bool auto_connect = false)
        : client{"", port, auto_connect} {
    }

    client(std::string const &host, uint16_t port, bool auto_connect = false)
        : tcp::socket{address_info{host, port}} {
      if (auto_connect)
        connect();
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return tcp::socket::make<client>(std::forward<Args>(args)...);
    }
  };

  using client_ptr = client::ptr;

} // namespace turbine::net::tcp
