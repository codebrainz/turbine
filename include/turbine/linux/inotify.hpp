#pragma once

#include <turbine/common/error.hpp>
#include <turbine/common/flags.hpp>
#include <turbine/posix/fd.hpp>

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <string>

#include <sys/inotify.h>

#undef linux

namespace turbine::linux {

  class inotify : public posix::fd {
  public:
    using fs_path = std::filesystem::path;

    enum class flags : int {
      none = 0,
      non_blocking = IN_NONBLOCK,
      close_on_exec = IN_CLOEXEC,
    };

    enum class events : int {
      accessed = IN_ACCESS,
      attributes_changed = IN_ATTRIB,
      closed_write = IN_CLOSE_WRITE,
      closed_no_write = IN_CLOSE_NOWRITE,
      created = IN_CREATE,
      deleted = IN_DELETE,
      deleted_self = IN_DELETE_SELF,
      modified = IN_MODIFY,
      moved_self = IN_MOVE_SELF,
      moved_from = IN_MOVED_FROM,
      moved_to = IN_MOVED_TO,
      opened = IN_OPEN,
    };

    struct watch_descriptor {
      inotify &owner;
      const int handle;
      const fs_path path;
      const events mask;
    };

    struct event {
      int wd;
      events mask;
      uint32_t cookie;
      fs_path path;
    };

    inotify(flags fl = flags::none) : posix::fd{-1} {
      if (auto f = ::inotify_init1(static_cast<int>(fl)); f >= 0)
        fileno(f);
      else
        throw system_error{};
    }

    watch_descriptor add_watch(fs_path const &path, events mask) {
      if (auto f = ::inotify_add_watch(fileno(), path.c_str(),
                                       static_cast<int>(mask));
          f != -1) {
        return {*this, f, path, mask};
      }
      throw system_error{};
    }

    void remove_watch(int wd) {
      if (::inotify_rm_watch(fileno(), wd) == -1)
        throw system_error{};
    }

    void remove_watch(watch_descriptor const &wd) {
      return remove_watch(wd.handle);
    }

    void read(event &e) {
      inotify_event ev{};
      auto n = ::read(fileno(), &ev, sizeof ev);
      if (n < 0)
        throw system_error{};
      assert(static_cast<size_t>(n) == sizeof ev);
      e.wd = ev.wd;
      e.mask = static_cast<events>(ev.mask);
      e.cookie = ev.cookie;
      if (ev.len > 0)
        e.path = std::string{ev.name, ev.len};
    }
  };

} // namespace turbine::linux

M_ENABLE_ENUM_FLAGS(turbine::linux::inotify::flags);
M_ENABLE_ENUM_FLAGS(turbine::linux::inotify::events);
