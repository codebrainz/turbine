#pragma once

#include <cassert>
#include <cstring>

#include <sys/socket.h>

namespace turbine::net {

  class sockaddr {
  public:
    sockaddr(::sockaddr const *data, ::socklen_t len) : m_data{}, m_size{len} {
      assert(data);
      assert(m_size > 0);
      std::memcpy(&m_data, data, m_size);
    }

    ::sockaddr *data() noexcept {
      return reinterpret_cast<::sockaddr *>(&m_data);
    }

    ::sockaddr const *data() const noexcept {
      return reinterpret_cast<::sockaddr const *>(&m_data);
    }

    ::socklen_t size() const noexcept {
      return m_size;
    }

  private:
    ::sockaddr_storage m_data;
    ::socklen_t m_size;
  };

} // namespace turbine::net
