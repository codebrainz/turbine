#pragma once

#include <type_traits>

namespace turbine {

  template <typename E>
  struct enable_enum_flags {
    static constexpr const bool value = false;
  };

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E>::type // |
  operator|(E lhs, E rhs) {
    using UT = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
  }

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E>::type // &
  operator&(E lhs, E rhs) {
    using UT = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<UT>(lhs) & static_cast<UT>(rhs));
  }

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E>::type // ^
  operator^(E lhs, E rhs) {
    using UT = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<UT>(lhs) ^ static_cast<UT>(rhs));
  }

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E>::type // ~
  operator~(E lhs) {
    using UT = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<UT>(lhs));
  }

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E &>::type // |=
  operator|=(E & lhs, E rhs) {
    using UT = std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
    return lhs;
  }

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E &>::type // &=
  operator&=(E & lhs, E rhs) {
    using UT = std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<UT>(lhs) & static_cast<UT>(rhs));
    return lhs;
  }

  template <typename E>
  typename std::enable_if<enable_enum_flags<E>::value, E &>::type // ^=
  operator^=(E & lhs, E rhs) {
    using UT = std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<UT>(lhs) ^ static_cast<UT>(rhs));
    return lhs;
  }

} // namespace turbine

// Don't use this inside a namespace
#define M_ENABLE_ENUM_FLAGS(T)                \
  template <>                                 \
  struct turbine::enable_enum_flags<T> {      \
    static constexpr const bool value = true; \
  }
