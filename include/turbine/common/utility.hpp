#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>

namespace turbine {

  namespace time {

    using namespace std::chrono;

    template <class ToT, class InputT, class ResultT = uint64_t>
    inline ResultT to(duration<InputT> dur) {
      return static_cast<ResultT>(duration_cast<ToT>(dur).count());
    }

    template <class InputT, class ResultT = uint64_t>
    inline auto to_sec(duration<InputT> dur) {
      return to<seconds, ResultT>(dur);
    }

    template <class InputT, class ResultT = uint64_t>
    inline auto to_ms(duration<InputT> dur) {
      return to<milliseconds, ResultT>(dur);
    }

    template <class InputT, class ResultT = uint64_t>
    inline auto to_us(duration<InputT> dur) {
      return to<microseconds, ResultT>(dur);
    }

    template <class InputT, class ResultT = uint64_t>
    inline auto to_ns(duration<InputT> dur) {
      return to<nanoseconds, ResultT>(dur);
    }

    template <class DurT>
    inline auto now() {
      auto now = high_resolution_clock::now().time_since_epoch();
      auto ms = duration_cast<DurT>(now);
      return static_cast<uint64_t>(ms.count());
    }

    inline auto now_sec() {
      return now<seconds>();
    }

    inline auto now_ms() {
      return now<milliseconds>();
    }

    inline auto now_us() {
      return now<microseconds>();
    }

    inline auto now_ns() {
      return now<nanoseconds>();
    }

  } // namespace time

} // namespace turbine
