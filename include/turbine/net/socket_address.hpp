#pragma once

#include <turbine/net/address_info.hpp>

#include <cassert>
#include <cstring>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

namespace turbine::net {

  class socket_address {
  public:
    socket_address() : m_data{}, m_size{0} {
    }

    socket_address(socket_address const &addr) : m_data{}, m_size{addr.m_size} {
      data(&addr.m_data, m_size);
    }

    socket_address(net::address_info const &info)
        : socket_address{info->ai_addr, info->ai_addrlen} {
    }

    socket_address(::addrinfo const &addr_info)
        : socket_address{addr_info.ai_addr, addr_info.ai_addrlen} {
    }

    socket_address(void const *data, ::socklen_t len) : m_data{}, m_size{len} {
      assert(data);
      assert(m_size > 0);
      std::memcpy(&m_data, data, m_size);
    }

    socket_address(::sockaddr_storage const &addr) : socket_address{} {
      data(&addr, sizeof(::sockaddr_storage));
    }

    socket_address(::sockaddr const &addr) : socket_address{} {
      data(&addr, sizeof(::sockaddr));
    }

    socket_address(::sockaddr_in const &addr) : socket_address{} {
      data(&addr, sizeof(::sockaddr_in));
    }

    socket_address(::sockaddr_in6 const &addr) : socket_address{} {
      data(&addr, sizeof(::sockaddr_in6));
    }

    socket_address(::sockaddr_un const &addr) : socket_address{} {
      data(&addr, sizeof(::sockaddr_un));
    }

    socket_address &operator=(socket_address const &rhs) {
      if (&rhs != this)
        data(&rhs.m_data, rhs.m_size);
      return *this;
    }

    socket_address &operator=(net::address_info const &rhs) {
      data(rhs->ai_addr, rhs->ai_addrlen);
      return *this;
    }

    socket_address &operator=(::addrinfo const &rhs) {
      data(rhs.ai_addr, rhs.ai_addrlen);
      return *this;
    }

    socket_address &operator=(::sockaddr const &rhs) {
      data(&rhs, sizeof(::sockaddr));
      return *this;
    }

    socket_address &operator=(::sockaddr_in const &rhs) {
      data(&rhs, sizeof(::sockaddr_in));
      return *this;
    }

    socket_address &operator=(::sockaddr_in6 const &rhs) {
      data(&rhs, sizeof(::sockaddr_in6));
      return *this;
    }

    socket_address &operator=(::sockaddr_un const &rhs) {
      data(&rhs, sizeof(::sockaddr_un));
      return *this;
    }

    socket_address &operator=(::sockaddr_storage const &rhs) {
      data(&rhs, sizeof(::sockaddr_storage));
      return *this;
    }

    operator ::sockaddr &() {
      return *reinterpret_cast<::sockaddr *>(data());
    }

    operator ::sockaddr const &() const {
      return *reinterpret_cast<::sockaddr const *>(data());
    }

    operator ::sockaddr_in &() {
      return *reinterpret_cast<::sockaddr_in *>(data());
    }

    operator ::sockaddr_in const &() const {
      return *reinterpret_cast<::sockaddr_in const *>(data());
    }

    operator ::sockaddr_in6 &() {
      return *reinterpret_cast<::sockaddr_in6 *>(data());
    }

    operator ::sockaddr_in6 const &() const {
      return *reinterpret_cast<::sockaddr_in6 const *>(data());
    }

    operator ::sockaddr_un &() {
      return *reinterpret_cast<::sockaddr_un *>(data());
    }

    operator ::sockaddr_un const &() const {
      return *reinterpret_cast<::sockaddr_un const *>(data());
    }

    operator ::sockaddr_storage &() {
      return m_data;
    }

    operator ::sockaddr_storage const &() const {
      return m_data;
    }

    ::socklen_t size() const noexcept {
      return m_size;
    }

    ::sockaddr *data() noexcept {
      return reinterpret_cast<::sockaddr *>(&m_data);
    }

    ::sockaddr const *data() const noexcept {
      return reinterpret_cast<::sockaddr const *>(&m_data);
    }

    void data(void const *dat, ::socklen_t len) {
      m_size = len;
      std::memcpy(&m_data, dat, m_size);
    }

  private:
    ::sockaddr_storage m_data;
    ::socklen_t m_size;
  };

} // namespace turbine::net
