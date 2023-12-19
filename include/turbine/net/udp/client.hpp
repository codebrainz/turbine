#pragma once

#include <turbine/net/udp/socket.hpp>

namespace turbine::net::udp {

  class client : public udp::socket {
  public:
    using ptr = std::shared_ptr<client>;

    client(uint16_t port) : client{"", port} {
    }

    client(std::string const &host, uint16_t port)
        : udp::socket{address_info{host, port}} {
    }

    auto connect() {
      return udp::socket::connect();
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return udp::socket::make<client>(std::forward<Args>(args)...);
    }
  };

  using client_ptr = client::ptr;

} // namespace turbine::net::udp
