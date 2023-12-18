#pragma once

#include <cerrno>
#include <cstring>
#include <exception>
#include <string>

namespace turbine {

  class exception : public std::exception {};

  class error : public exception {
  public:
    error(std::string const &what) : m_what{what} {
    }

    const char *what() const noexcept override {
      return m_what.c_str();
    }

  private:
    std::string m_what;
  };

  class system_error : public exception {
  public:
    system_error(int error_number = errno)
        : system_error{error_number, std::strerror(error_number)} {
    }

    system_error(int error_number, std::string const &what)
        : exception{}
        , m_code{error_number}
        , m_message{what} {
    }

    int code() const noexcept {
      return m_code;
    }

    std::string const &message() const noexcept {
      return m_message;
    }

    const char *what() const noexcept override {
      return m_message.c_str();
    }

  private:
    int m_code;
    std::string m_message;
  };

} // namespace turbine
