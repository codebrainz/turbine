#pragma once

#include <turbine/common/error.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <string>

namespace turbine::net {

  class address_info_error : public system_error {
  public:
    address_info_error(int code) : system_error{code, ::gai_strerror(code)} {
    }
  };

  class address_info {
  protected:
    address_info(::addrinfo *addr, bool free_rest = false) noexcept
        : m_addr{addr} {
      assert(addr);
      if (free_rest && m_addr->ai_next) {
        ::freeaddrinfo(m_addr->ai_next);
        m_addr->ai_next = nullptr;
      }
    }

  public:
    address_info(uint16_t port) : address_info{"", port} {
    }

    address_info(std::string const &host, uint16_t port)
        : m_addr{get_address_info(host, port)} {
    }

    address_info(address_info &&other) noexcept : m_addr{nullptr} {
      std::swap(m_addr, other.m_addr);
    }

    ~address_info() noexcept {
      if (m_addr)
        ::freeaddrinfo(m_addr);
    }

    address_info &operator=(address_info &&other) noexcept {
      std::swap(m_addr, other.m_addr);
      return *this;
    }

    bool operator!() const noexcept {
      return m_addr == nullptr;
    }

    operator bool() const noexcept {
      return !(operator!());
    }

    ::addrinfo &operator*() noexcept {
      assert(m_addr);
      return *m_addr;
    }

    ::addrinfo const &operator*() const noexcept {
      assert(m_addr);
      return *m_addr;
    }

    ::addrinfo *operator->() noexcept {
      return m_addr;
    }

    ::addrinfo const *operator->() const noexcept {
      return m_addr;
    }

    ::sockaddr *addr() noexcept {
      return m_addr->ai_addr;
    }

    ::sockaddr *addr(::socklen_t &len) noexcept {
      len = addr_len();
      return addr();
    }

    ::sockaddr const *addr() const noexcept {
      return m_addr->ai_addr;
    }

    ::sockaddr const *addr(::socklen_t &len) const noexcept {
      len = addr_len();
      return addr();
    }

    ::socklen_t addr_len() const noexcept {
      return m_addr->ai_addrlen;
    }

  private:
    ::addrinfo *m_addr;

    address_info(address_info const &) = delete;
    address_info &operator=(address_info const &) = delete;

    static ::addrinfo *get_address_info(std::string const &host, uint16_t port,
                                        bool free_rest = false) {
      ::addrinfo hints{};
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_NUMERICSERV;

      const char *host_cstr = nullptr;
      if (host.empty())
        hints.ai_flags |= AI_PASSIVE;
      else
        host_cstr = host.c_str();

      auto port_str = std::to_string(port);
      const char *port_cstr = port_str.c_str();

      ::addrinfo *res = nullptr;
      int status = ::getaddrinfo(host_cstr, port_cstr, &hints, &res);
      if (status != 0)
        throw address_info_error{status};
      assert(res);

      if (free_rest) {
        if (res->ai_next) {
          ::freeaddrinfo(res->ai_next);
          res->ai_next = nullptr;
        }
      }

      return res;
    }
  };

} // namespace turbine::net
