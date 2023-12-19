# Turbine

**Status:** experimental, not well-tested or meant for production.

Turbine in a single-header async IO library for C++ on Linux.

## Features

- IO event loop similar to GLib's main loop (`io` directory)
- Wrappers over Linux, POSIX and Unix IO primatives:
  - Posix: fd, fifo, pipe, sigset
  - Networking: socket, addrinfo, sockaddr, tcp/udp/unix client and server
  - Linux: epoll, eventfd, inotify, signalfd, timerfd

## Quick Start

To use the library, first generate the header, using the Python script that
combines all of the library's headers into a single file:

```console
$ cd /path/to/turbine
$ scripts/amalgamation.py > /path/where/you/want/the/header/turbine.hpp
```

The header is Linux-specific (at this point) and requires C++20 (with GNU
extensions). You need to define `_DEFAULT_SOURCE` and use the `-std=gnu++20`
compiler flag in order to build with the header.

See the file `src/main.cpp` for a basic example on how to use it.
