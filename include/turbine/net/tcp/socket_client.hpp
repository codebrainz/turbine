#pragma once

#include <turbine/common/error.hpp>
#include <turbine/net/tcp/socket.hpp>
#include <turbine/net/tcp/socket_address.hpp>

#include <memory>

namespace turbine::net::tcp {

  class socket_client : public tcp::socket {
  public:
    using ptr = std::shared_ptr<socket_client>;

  protected:
    template <class... Args>
    socket_client(Args &&...args) : tcp::socket{std::forward<Args>(args)...} {
    }

    void connect(tcp::socket_address const &addr) {
      net::socket_address const &naddr{addr.address()};
      if (::connect(fileno(), naddr.data(), naddr.size()) != 0)
        throw system_error{};
    }
  };

  using socket_client_ptr = socket_client::ptr;

} // namespace turbine::net::tcp
