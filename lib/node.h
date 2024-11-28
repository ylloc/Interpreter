#pragma once

#include <memory>
#include <string>
#include <utility>

class Object : public std::enable_shared_from_this<Object> {
public:
  explicit Object(bool unused) : is_unused_object(unused) {
  }

  Object() = default;

  bool IsTrash() const {
    return is_unused_object;
  }

  virtual ~Object() = default;

private:
  bool is_unused_object{false};
};

class Number : public Object {
public:
  int GetValue() const {
    return value_;
  }

  explicit Number(int value) : value_(value) {
  }

private:
  int value_;
};

class Symbol : public Object {
public:
  explicit Symbol(std::string name, bool quoted = false) : name_(std::move(name)), quoted_(quoted) {
  }

  [[nodiscard]] bool IsQuoted() const {
    return quoted_;
  }

  const std::string &GetName() const {
    return name_;
  }

private:
  std::string name_;
  bool quoted_ = false;
};

class Cell : public Object {
public:
  explicit Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
    : first_(std::move(first)), second_(std::move(second)) {
  }

  Cell() = default;

  std::shared_ptr<Object> GetFirst() const {
    return first_;
  }
  std::shared_ptr<Object> GetSecond() const {
    return second_;
  }

  void SetFirst(std::shared_ptr<Object> new_first) {
    first_ = std::move(new_first);
  }

  void SetSecond(std::shared_ptr<Object> new_second) {
    second_ = std::move(new_second);
  }

  ~Cell() override = default;

private:
  std::shared_ptr<Object> first_, second_;
};

class Dot : public Object {
};

class Quote : public Object {
public:
  Quote() = default;
};

template<class T>
bool Is(const std::shared_ptr<Object> &obj) {
  return std::dynamic_pointer_cast<T>(obj) != nullptr;
}


template<class T>
std::shared_ptr<T> As(const std::shared_ptr<Object> &obj) {
  return std::dynamic_pointer_cast<T>(obj);
}
