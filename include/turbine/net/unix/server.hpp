#pragma once

#include <turbine/net/unix/socket.hpp>

#undef unix

namespace turbine::net::unix {

  class server : public unix::socket {
  public:
    using ptr = std::shared_ptr<server>;

    auto bind() {
      return net::socket::bind();
    }

    auto listen(int backlog = default_backlog) {
      ::unlink(path().data());
      return net::socket::listen(backlog);
    }

    auto accept() {
      return net::socket::accept<socket>();
    }

    server(std::string const &path, bool auto_bind_listen = false)
        : unix::socket{path} {
      if (auto_bind_listen) {
        bind();
        listen();
      }
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return unix::socket::make<server>(std::forward<Args>(args)...);
    }
  };

  using server_ptr = server::ptr;

} // namespace turbine::net::unix
