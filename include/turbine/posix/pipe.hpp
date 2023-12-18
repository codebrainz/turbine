#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <utility>

#include <fcntl.h>
#include <unistd.h>

namespace turbine::posix::pipe {

  enum class flags : int {
    none = 0,
    close_on_exec = O_CLOEXEC,
    direct = O_DIRECT,
    non_blocking = O_NONBLOCK,
  };

  struct pair {
    fd_ptr read_end;
    fd_ptr write_end;
  };

  inline pair make(flags fl = flags::none) {
    int fds[2] = {-1, -1};
    if (pipe2(fds, static_cast<int>(fl)) != 0)
      throw system_error{};
    assert(fds[0] >= 0);
    assert(fds[1] >= 0);
    return {fd::make<fd>(fds[0]), fd::make<fd>(fds[1])};
  }

} // namespace turbine::posix::pipe

M_ENABLE_ENUM_FLAGS(turbine::posix::pipe::flags);
