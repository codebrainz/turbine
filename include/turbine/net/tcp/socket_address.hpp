#pragma once

#include <turbine/common/error.hpp>
#include <turbine/net/socket_address.hpp>

#include <cassert>
#include <cstdint>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace turbine::net::tcp {

  class socket_address {
  public:
    socket_address(net::socket_address &a) : m_addr{a} {
    }

    std::string ip() const {
      char ip[INET_ADDRSTRLEN + 1] = {0};
      inet_ntop(AF_INET, &to_in()->sin_addr, ip, sizeof ip);
      return ip;
    }

    uint16_t port() const {
      return ntohs(to_in()->sin_port);
    }

    net::socket_address &address() {
      return m_addr;
    }

    net::socket_address const &address() const {
      return m_addr;
    }

  private:
    net::socket_address &m_addr;

    ::sockaddr_in *to_in() {
      assert(m_addr.size() >= sizeof(::sockaddr_in));
      return (::sockaddr_in *)m_addr.data();
    }

    ::sockaddr_in const *to_in() const {
      assert(m_addr.size() >= sizeof(::sockaddr_in));
      return (::sockaddr_in const *)m_addr.data();
    }
  };

} // namespace turbine::net::tcp
