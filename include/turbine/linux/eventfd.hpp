#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/common/macros.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <cstdint>

#include <sys/eventfd.h>

#undef linux

namespace turbine::linux {

  class eventfd : public posix::fd {
  public:
    enum class flags : int {
      none = 0,
      close_on_exec = EFD_CLOEXEC,
      non_blocking = EFD_NONBLOCK,
      semaphore = EFD_SEMAPHORE,
    };

    eventfd(uint32_t init_val = 0, flags fl = flags::none)
        : posix::fd{::eventfd(init_val, static_cast<int>(fl))} {
      if (fileno() < 0)
        throw system_error{};
    }

    uint64_t read() {
      uint64_t value = 0;
      auto n = fd::read(value);
      assert(n == sizeof value);
      M_UNUSED(n);
      return value;
    }

    void write(uint64_t value = 1) {
      auto n = fd::write(value);
      assert(n == sizeof value);
      M_UNUSED(n);
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return fd::make<eventfd>(std::forward<Args>(args)...);
    }
  };

} // namespace turbine::linux

M_ENABLE_ENUM_FLAGS(turbine::linux::eventfd::flags);
