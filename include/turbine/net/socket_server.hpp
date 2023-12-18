#pragma once

#include <turbine/net/sockaddr.hpp>
#include <turbine/net/socket.hpp>

namespace turbine::net {

  class socket_server : public socket {
  public:
    static constexpr const int default_backlog = 32;

  protected:
    template <class... Args>
    socket_server(Args &&...args) : socket{std::forward<Args>(args)...} {
    }

    void bind() {
      if (::bind(fileno(), address().data(), address().size()) != 0)
        throw system_error{};
    }

    void listen(int backlog = default_backlog) {
      if (::listen(fileno(), backlog) != 0)
        throw system_error{};
    }
  };

} // namespace turbine::net
