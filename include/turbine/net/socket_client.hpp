#pragma once

#include <turbine/net/socket.hpp>
#include <turbine/net/socket_address.hpp>

#include <memory>

namespace turbine::net {

  class socket_client : public socket {
  public:
    using ptr = std::shared_ptr<socket_client>;

  protected:
    template <class... Args>
    socket_client(Args &&...args) : socket{std::forward<Args>(args)...} {
    }

    void connect(socket_address const &addr) {
      if (::connect(fileno(), addr.data(), addr.size()) != 0)
        throw system_error{};
    }
  };

  using socket_client_ptr = socket_client::ptr;

} // namespace turbine::net
