#pragma once

#include <turbine/common/utility.hpp>
#include <turbine/linux/epoll.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>

namespace turbine::io {

  class loop;

  class source {
    friend class loop;

  public:
    enum class result {
      remove = 0,
      keep_going = 1,
    };

    template <class T>
    using pointer_type = std::shared_ptr<T>;
    using ptr = pointer_type<source>;
    using callback = std::function<result(source &)>;

    enum class kind : uint8_t {
      timeout,
      poll,
      idle,
    };

    enum class priority : uint8_t {
      high = (UINT8_MAX / 4) * 0,
      medium_high = (UINT8_MAX / 4) * 1,
      medium = (UINT8_MAX / 4) * 2,
      medium_low = (UINT8_MAX / 4) * 3,
      low = (UINT8_MAX / 4) * 4,

      highest = high,
      lowest = low,
      normal = medium, // 'default' is a keyword
    };

    static constexpr priority default_priority = priority::normal;

  protected:
    source(io::loop &loop, priority pri, callback cb)
        : m_loop{loop}
        , m_priority{pri}
        , m_cb{std::move(cb)} {
      assert(m_cb);
    }

  public:
    virtual ~source() = default;

    io::loop &loop() noexcept {
      return m_loop;
    }

    io::loop const &loop() const noexcept {
      return m_loop;
    }

    uint32_t priority_level() const noexcept {
      const uint32_t k = static_cast<uint32_t>(kind()) & 0xFF;
      const uint32_t p = static_cast<uint32_t>(m_priority) & 0xFF;
      return (k << 8) | p;
    }

  protected:
    virtual enum kind kind() const noexcept = 0;

    virtual bool prepare(int64_t &timeout_ms) = 0;

    virtual bool check() = 0;

    virtual result dispatch() {
      if (M_LIKELY(m_cb))
        return m_cb(*this);
      return result::keep_going;
    }

  public:
    template <class T, class... Args>
    static auto make(Args &&...args) {
      return std::make_shared<T>(std::forward<Args>(args)...);
    }

  private:
    io::loop &m_loop;
    priority m_priority;
    callback m_cb;
  };

  using source_ptr = source::ptr;

} // namespace turbine::io
