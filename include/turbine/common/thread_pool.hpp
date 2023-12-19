#pragma once

#include <turbine/common/macros.hpp>

#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace turbine::common {

  class thread_pool {

  public:
    using task_func = std::function<void(void)>;

    thread_pool(uint32_t n_threads = 0)
        : m_lock{}
        , m_threads{}
        , m_tasks{}
        , m_cond{}
        , m_stop{false} {
      if (n_threads == 0)
        n_threads = std::thread::hardware_concurrency();
      m_threads.reserve(n_threads);
      for (uint32_t i = 0; i < n_threads; ++i)
        m_threads.emplace_back(std::bind(&thread_pool::thread_func, this));
    }

    ~thread_pool() {
      {
        std::unique_lock lk{m_lock};
        m_stop = true;
      }
      m_cond.notify_all();
      for (auto &thrd : m_threads)
        thrd.join();
    }

    void push(task_func fnc) {
      M_ASSERT(fnc);
      if (M_LIKELY(fnc)) {
        {
          std::lock_guard lk{m_lock};
          m_tasks.push_back(std::move(fnc));
        }
        m_cond.notify_one();
      }
    }

  private:
    std::mutex m_lock;
    std::vector<std::jthread> m_threads;
    std::deque<task_func> m_tasks;
    std::condition_variable m_cond;
    bool m_stop;

    void thread_func() {
      bool running = true;
      while (running) {
        task_func task{};
        {
          std::unique_lock lk{m_lock};
          m_cond.wait(lk, [this]() { return m_stop || !m_tasks.empty(); });
          if (!m_tasks.empty()) {
            task = std::move(m_tasks.front());
            m_tasks.pop_front();
          }
          if (m_stop)
            running = false;
          lk.unlock();
        }
        if (task)
          task();
      }
    }
  };

} // namespace turbine::common
