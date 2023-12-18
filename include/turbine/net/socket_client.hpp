#pragma once

#include <turbine/net/sockaddr.hpp>
#include <turbine/net/socket.hpp>

namespace turbine::net {

  class socket_client : public socket {
  public:
    static constexpr const int default_backlog = 32;

  protected:
    template <class... Args>
    socket_client(Args &&...args) : socket{std::forward<Args>(args)...} {
    }

    void connect(sockaddr const &addr) {
      if (::connect(fileno(), addr.data(), addr.size()) != 0)
        throw system_error{};
    }
  };

} // namespace turbine::net
