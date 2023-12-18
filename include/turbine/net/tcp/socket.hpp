#pragma once

#include <turbine/net/socket.hpp>
#include <turbine/net/socket_address.hpp>
#include <turbine/net/tcp/socket_address.hpp>

#include <memory>

namespace turbine::net::tcp {

  class socket : public net::socket {
    friend class net::socket;

  public:
    using ptr = std::shared_ptr<tcp::socket>;

  protected:
    template <class T>
    socket(int filedes, T const &addr)
        : net::socket{filedes, addr}
        , m_addr{address()} {
    }

    tcp::socket_address &address() noexcept {
      return m_addr;
    }

    tcp::socket_address const &address() const noexcept {
      return m_addr;
    }

    std::string ip() const {
      return m_addr.ip();
    }

    uint16_t port() const {
      return m_addr.port();
    }

  private:
    tcp::socket_address &m_addr;
  };

  using socket_ptr = socket::ptr;

} // namespace turbine::net::tcp
