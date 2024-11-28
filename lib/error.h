#pragma once

#include <stdexcept>
#include <string>

class SyntaxError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
  explicit SyntaxError(const std::string &arg) : runtime_error(arg) {
  }
};

class RuntimeError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class NameError : public std::runtime_error {
public:
  explicit NameError(const std::string &name) : std::runtime_error{"Name not found: " + name} {
  }
};
