#pragma once

#include <turbine/common/error.hpp>

#include <algorithm>
#include <memory>

#include <fcntl.h>
#include <unistd.h>

namespace turbine::posix {

  class fd {
  public:
    explicit fd(int f = -1) : m_fd{f} {
    }

    fd(fd &&other) noexcept : m_fd{-1} {
      std::swap(m_fd, other.m_fd);
    }

    virtual ~fd() noexcept {
      if (is_open())
        ::close(m_fd);
    }

    fd &operator=(fd &&other) noexcept {
      std::swap(m_fd, other.m_fd);
      return *this;
    }

    bool operator==(fd const &other) const noexcept {
      return m_fd == other.m_fd;
    }

    bool operator!=(fd const &other) const noexcept {
      return !(operator==(other));
    }

    bool operator!() const noexcept {
      return !is_open();
    }

    operator bool() const noexcept {
      return !(operator!());
    }

    int fileno() const noexcept {
      return m_fd;
    }

    bool is_open() const noexcept {
      return m_fd >= 0;
    }

    void close() {
      if (is_open()) {
        if (::close(m_fd) != 0)
          throw system_error{};
      }
      m_fd = -1;
    }

    template <class T>
    size_t read(T *data, size_t count) {
      if (auto n = ::read(m_fd, data, count * sizeof(T)); n >= 0)
        return static_cast<size_t>(n);
      throw system_error{};
    }

    template <class T>
    size_t read(T &value) {
      return read(&value, 1);
    }

    template <class T>
    size_t write(T const *data, size_t count) {
      if (auto n = ::write(m_fd, data, count * sizeof(T)); n >= 0)
        return static_cast<size_t>(n);
      throw system_error{};
    }

    template <class T>
    size_t write(T const &value) {
      return write(&value, 1);
    }

    template <class T>
    auto dup() const {
      if (auto f = ::dup(m_fd); f >= 0)
        return fd::make<T>(f);
      throw system_error{};
    }

    template <class... Args>
    int fcntl(Args &&...args) {
      if (auto r = ::fcntl(m_fd, std::forward<Args>(args)...); r != -1)
        return r;
      throw system_error{};
    }

    template <class T>
    T *as() noexcept {
      static_assert(std::is_base_of_v<T, fd>, "T must derive from fd");
      return static_cast<T *>(this);
    }

    template <class T>
    T const *as() const noexcept {
      static_assert(std::is_base_of_v<T, fd>, "T must derive from fd");
      return static_cast<T const *>(this);
    }

    template <class T, class... Args>
    static auto make(Args &&...args) {
      static_assert(std::is_base_of_v<fd, T>, "T must derive from fd");
      return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    static auto open(Args &&...args) {
      static_assert(std::is_base_of_v<fd, T>, "T must derive from fd");
      if (auto f = ::open(std::forward<Args>(args)...); f >= 0)
        return make(f);
      throw system_error{};
    }

  protected:
    int fileno(int f) noexcept {
      std::swap(f, m_fd);
      return f;
    }

  private:
    int m_fd;

    fd(fd const &) = delete;
    fd &operator=(fd const &) = delete;
  };

  using fd_ptr = std::shared_ptr<fd>;

} // namespace turbine::posix
