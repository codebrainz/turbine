#pragma once

#include <turbine/common/utility.hpp>
#include <turbine/io/source.hpp>

#include <cstdint>
#include <functional>
#include <memory>

namespace turbine::io {

  class loop;

  class timeout_source final : public source {
    friend class loop;

  public:
    using ptr = pointer_type<timeout_source>;
    using callback = std::function<result()>;
    using callback_self = std::function<result(timeout_source &)>;

    template <class T>
    timeout_source(io::loop &loop, std::chrono::duration<T> timeout,
                   callback cb)
        : timeout_source{loop, time::to_ms(timeout), std::move(cb)} {
    }

    timeout_source(io::loop &loop, uint64_t timeout_ms, callback cb)
        : timeout_source{loop, timeout_ms,
                         [cb = std::move(cb)](auto &) { return cb(); }} {
    }

    template <class T>
    timeout_source(io::loop &loop, std::chrono::duration<T> timeout,
                   callback_self cb)
        : timeout_source{loop, time::to_ms(timeout), std::move(cb)} {
    }

    timeout_source(io::loop &loop, uint64_t timeout_ms, callback_self cb)
        : source{loop, priority::lowest,
                 [this, cb = std::move(cb)](auto &) {
                   if (M_LIKELY(cb))
                     return cb(*this);
                   return result::keep_going;
                 }}
        , m_start{turbine::time::now_ms()}
        , m_timeout{timeout_ms} {
    }

  protected:
    enum kind kind() const noexcept final {
      return kind::timeout;
    }

    bool prepare(int64_t &timeout_ms) final {
      const auto el = elapsed_ms();
      if (el >= m_timeout) {
        timeout_ms = 0; // ready now
        return true;
      } else {
        timeout_ms = el; // will be ready in el ms
        return false;
      }
    }

    bool check() final {
      return is_elapsed();
    }

    result dispatch() final {
      const auto now = turbine::time::now_ms();
      auto r = source::dispatch();
      m_start = now;
      return r;
    }

  private:
    uint64_t m_start;
    uint64_t m_timeout;

    uint64_t elapsed_ms() const {
      return turbine::time::now_ms() - m_start;
    }

    bool is_elapsed() const noexcept {
      return elapsed_ms() >= m_timeout;
    }
  };

  using timeout_source_ptr = timeout_source::ptr;

} // namespace turbine::io
