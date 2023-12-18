#pragma once

#include <turbine/net/sockaddr.hpp>
#include <turbine/net/socket.hpp>

#include <memory>

namespace turbine::net {

  class tcp_socket : public socket {
  public:
    using ptr = std::shared_ptr<tcp_socket>;

  protected:
    template <class T>
    tcp_socket(int filedes, T const &addr)
        : socket{filedes, sockaddr{reinterpret_cast<::sockaddr const *>(&addr),
                                   sizeof addr}} {
      static_assert(sizeof(T) <= sizeof(sockaddr_storage));
    }
  };

  using tcp_socket_ptr = tcp_socket::ptr;

} // namespace turbine::net
