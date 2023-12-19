#pragma once

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <vector>

namespace turbine::core {

  class rotor;

  class blade {
  public:
    using ptr = std::unique_ptr<blade>;

    const uint64_t id;
    core::rotor &rotor;

  private:
    blade(core::rotor &rotor) : id{next_id()}, rotor{rotor} {
      std::printf("blade::blade()\n");
    }

  public:
    ~blade() noexcept {
      std::printf("blade::~blade()\n");
    }

    template <class... Args>
    static auto make(Args &&...args) {
      return ptr(new blade{std::forward<Args>(args)...});
    }

  private:
    static uint64_t next_id() {
      static std::atomic<uint64_t> id_{0};
      return id_++;
    }
  };

  using blade_ptr = blade::ptr;
  using blade_list = std::vector<blade_ptr>;

} // namespace turbine::core
