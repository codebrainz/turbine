#pragma once

#include <turbine/net/unix/socket.hpp>

#undef unix

namespace turbine::net::unix {

  class client : public socket {
  public:
    using ptr = std::shared_ptr<client>;

    auto connect() {
      return net::socket::connect();
    }

    client(std::string const &path, bool auto_connect = false) : socket{path} {
      if (auto_connect)
        connect();
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return unix::socket::make<client>(std::forward<Args>(args)...);
    }
  };

  using client_ptr = client::ptr;

} // namespace turbine::net::unix
