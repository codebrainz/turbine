#include <turbine/turbine.hpp>

#include <cstdio>

int main() {
  int cnt = 0;
  turbine::io::loop loop;

  loop.setup_func([&loop, &cnt]() {
    auto orig_signals = turbine::posix::sigset::process_get();

    std::printf("io::loop -> setup\n");

    loop.add_timeout(1000, [&cnt]() {
      std::printf("timeout elapsed: %d\n", cnt++);
      return turbine::io::source::result::keep_going;
    });

    loop.teardown_func([&orig_signals]() {
      std::printf("io::loop -> teardown\n");
      turbine::posix::sigset::process_set(orig_signals);
    });

    loop.error_func([&loop](auto &exc) {
      std::printf("io::loop -> error(%s)\n", exc.what());
      loop.quit(1);
      return true;
    });
  });

  return loop.run();
}
