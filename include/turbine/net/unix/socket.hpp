#pragma once

#include <turbine/common/error.hpp>
#include <turbine/net/address.hpp>
#include <turbine/net/socket.hpp>

#include <cstring>
#include <memory>
#include <string>
#include <string_view>

#include <sys/un.h>
#include <unistd.h>

#undef unix

namespace turbine::net::unix {

  class socket : public net::socket {
    friend class net::socket;

  public:
    using ptr = std::shared_ptr<socket>;

  protected:
    socket(int filedes, net::address const &addr) : net::socket{filedes, addr} {
    }

    socket(std::string const &path) : socket{-1, make_address(path)} {
      if (auto f = ::socket(AF_UNIX, SOCK_STREAM, 0); f >= 0)
        fileno(f);
      else
        throw system_error{};
    }

    template <class T, class... Args>
    auto make(Args &&...args) {
      static_assert(std::is_base_of_v<unix::socket, T>);
      return std::shared_ptr<T>(new T{std::forward<Args>(args)...});
    }

  public:
    std::string_view path() const {
      ::sockaddr_un const &au = address();
      return au.sun_path;
    }

  private:
    static net::address make_address(std::string const &path) {
      ::sockaddr_un local;
      local.sun_family = AF_UNIX;
      size_t path_len = path.size();
      if (path_len >= sizeof(local.sun_path))
        path_len = sizeof(local.sun_path) - 1;
      std::strncpy(local.sun_path, path.c_str(), path_len);
      local.sun_path[path_len] = '\0';
      auto len = strlen(local.sun_path) + sizeof(local.sun_family);
      return {&local, static_cast<::socklen_t>(len)};
    }
  };

  using socket_ptr = socket::ptr;

} // namespace turbine::net::unix
