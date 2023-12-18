#pragma once

#include <turbine/common/error.hpp>

#include <algorithm>
#include <csignal>
#include <set>

namespace turbine::posix {

  class sigset {
  public:
    sigset() : m_signals{} {
    }

    sigset(sigset const &other) : m_signals{} {
      for (auto num : other.m_signals)
        add(num);
    }

    sigset(sigset &&other) : m_signals{} {
      std::swap(m_signals, other.m_signals);
    }

    sigset(sigset_t const &ss) : m_signals{} {
      operator=(ss);
    }

    sigset &operator=(sigset const &other) {
      sigset ss{other}; // copy
      return swap(ss);  // swap
    }

    sigset &operator=(sigset &&other) {
      std::swap(m_signals, other.m_signals);
      return *this;
    }

    sigset &operator=(sigset_t const &ss) {
      clear();
      for (auto i = SIGRTMIN; i < SIGRTMAX; ++i) {
        auto r = ::sigismember(&ss, i);
        if (r < 0)
          throw system_error{};
        else if (r > 0)
          add(i);
      }
      return *this;
    }

    sigset &operator+=(int num) {
      m_signals.emplace(num);
      return *this;
    }

    sigset &operator-=(int num) {
      m_signals.erase(num);
      return *this;
    }

    operator sigset_t() const {
      sigset_t ss{};
      if (::sigemptyset(&ss) != 0)
        throw system_error{};
      for (auto num : m_signals) {
        if (::sigaddset(&ss, num) != 0)
          throw system_error{};
      }
      return ss;
    }

    operator bool() const noexcept {
      return !(operator!());
    }

    bool operator!() const noexcept {
      return empty();
    }

    bool add(int num) {
      return m_signals.emplace(num).second;
    }

    bool del(int num) {
      return m_signals.erase(num) > 0;
    }

    bool contains(int num) const {
      return m_signals.contains(num);
    }

    size_t size() const noexcept {
      return m_signals.size();
    }

    bool empty() const noexcept {
      return m_signals.empty();
    }

    void clear() {
      m_signals.clear();
    }

    sigset &swap(sigset &other) noexcept {
      std::swap(m_signals, other.m_signals);
      return *this;
    }

    void process_block() const {
      return process_block(*this);
    }

    void process_unblock() const {
      return process_unblock(*this);
    }

    void process_set() const {
      return process_set(*this);
    }

    static void process_block(sigset const &ss) {
      sigset_t sst{ss};
      if (::sigprocmask(SIG_BLOCK, &sst, nullptr) != 0)
        throw system_error{};
    }

    static void process_unblock(sigset const &ss) {
      sigset_t sst{ss};
      if (::sigprocmask(SIG_UNBLOCK, &sst, nullptr) != 0)
        throw system_error{};
    }

    static void process_set(sigset const &ss) {
      sigset_t sst{ss};
      if (::sigprocmask(SIG_SETMASK, &sst, nullptr) != 0)
        throw system_error{};
    }

    static sigset process_get() {
      sigset_t sst{};
      if (::sigprocmask(0, nullptr, &sst) != 0)
        throw system_error{};
      return sst;
    }

  private:
    std::set<int> m_signals;
  };

} // namespace turbine::posix
