#pragma once

#include <turbine/common/macros.hpp>
#include <turbine/io/source.hpp>

#include <cstdint>
#include <functional>

namespace turbine::io {

  class loop;

  class idle_source final : public source {
    friend class loop;

  public:
    using ptr = pointer_type<idle_source>;
    using callback = std::function<result()>;
    using callback_self = std::function<result(idle_source &)>;

    idle_source(io::loop &loop, callback cb)
        : idle_source{loop, [cb = std::move(cb)](auto &) {
                        if (M_LIKELY(cb))
                          return cb();
                        return result::keep_going;
                      }} {
    }

    idle_source(io::loop &loop, callback_self cb)
        : source{loop, priority::highest, [this, cb = std::move(cb)](auto &) {
                   if (M_LIKELY(cb))
                     return cb(*this);
                   return result::keep_going;
                 }} {
    }

  protected:
    enum kind kind() const noexcept final {
      return kind::idle;
    }

    bool prepare(int64_t &timeout_ms) final {
      timeout_ms = 0; // no wait needed
      return true;    // ignored by io::loop
    }

    bool check() final {
      return true; // always ready
    }
  };

  using idle_source_ptr = idle_source::ptr;

} // namespace turbine::io
