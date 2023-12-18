#pragma once

#include <turbine/common/error.hpp>
#include <turbine/posix/fd.hpp>

#include <filesystem>

#include <sys/stat.h>
#include <sys/types.h>

namespace turbine::posix {

  class fifo : public fd {
  public:
    fifo(std::filesystem::path const &path, mode_t mode)
        : fd{-1}
        , m_path{path}
        , m_mode{mode} {
      if (auto f = ::mkfifo(m_path.c_str(), m_mode); f >= 0)
        fileno(f);
      else
        throw system_error{};
    }

    std::filesystem::path const &path() const noexcept {
      return m_path;
    }

    mode_t mode() const noexcept {
      return m_mode;
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return fd::make<fifo>(std::forward<Args>(args)...);
    }

  private:
    std::filesystem::path m_path;
    mode_t m_mode;
  };

} // namespace turbine::posix
