#pragma once

#include <turbine/core/blade.hpp>

#include <algorithm>
#include <cstdio>
#include <thread>

namespace turbine::core {

  class rotor {
  public:
    rotor() : m_blades{} {
      const auto n_cores = std::thread::hardware_concurrency();
      m_blades.reserve(n_cores);
      for (auto i = 0u; i < n_cores; ++i)
        m_blades.emplace_back(blade::make(*this));
      m_blades.shrink_to_fit();
      std::printf("rotor::rotor()\n");
    }

    rotor(rotor &&other) : m_blades{} {
      std::swap(m_blades, other.m_blades);
    }

    ~rotor() noexcept {
      std::printf("rotor::~rotor()\n");
    }

    rotor &operator=(rotor &&other) {
      std::swap(m_blades, other.m_blades);
      return *this;
    }

    blade_list &blades() noexcept {
      return m_blades;
    }

    blade_list const &blades() const noexcept {
      return m_blades;
    }

  private:
    blade_list m_blades;

    rotor(rotor const &) = delete;
    rotor &operator=(rotor const &) = delete;
  };

} // namespace turbine::core
