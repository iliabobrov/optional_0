#pragma once
#include <memory>
#include <exception>
#include <type_traits>

struct BadOptionalAccess : public std::exception {
  const char *What() {
    return "bulat_geniy";
  }
};

template <class T>
class Optional {
 private:
  bool valid_;
  std::aligned_storage<sizeof(T), alignof(T)> bufer_[sizeof(T)];

 public:
  Optional() {
    valid_ = false;
  }
  Optional(const Optional &oth) {
    if (oth.valid_) {
      valid_ = true;
      new (bufer_) T(*reinterpret_cast<const T *>(oth.bufer_));
    } else {
      valid_ = false;
    }
  }
  Optional(Optional &&oth) {
    if (oth.valid_) {
      valid_ = true;
      new (bufer_) T(std::move(*reinterpret_cast<T *>(oth.bufer_)));
    } else {
      valid_ = false;
    }
  }
  Optional(const T &oth) {  // NOLINT
    valid_ = true;
    new (bufer_) T(*reinterpret_cast<const T *>(&oth));
  }
  Optional(T &&oth) {  // NOLINT
    valid_ = true;
    new (bufer_) T(std::move(*reinterpret_cast<T *>(&oth)));
  }
  ~Optional() {
    Reset();
  }
  Optional &operator=(const Optional &oth) {
    if (this != &oth) {
      Reset();
      if (oth.valid_) {
        valid_ = true;
        new (bufer_) T(*reinterpret_cast<const T *>(oth.bufer_));
      } else {
        valid_ = false;
      }
    }
    return *this;
  }
  Optional &operator=(Optional &&oth) {
    if (this != &oth) {
      Reset();
      if (oth.valid_) {
        valid_ = true;
        new (bufer_) T(std::move(*reinterpret_cast<T *>(oth.bufer_)));
      } else {
        valid_ = false;
      }
    }
    return *this;
  }
  Optional &operator=(const T &oth) {
    Reset();
    valid_ = true;
    valid_ = true;
    new (bufer_) T(*reinterpret_cast<const T *>(&oth));
    return *this;
  }
  Optional &operator=(T &&oth) {
    Reset();
    valid_ = true;
    valid_ = true;
    new (bufer_) T(std::move(*reinterpret_cast<T *>(&oth)));
    return *this;
  }
  bool HasValue() const {
    return valid_;
  }
  operator bool() const {  // NOLINT
    return valid_;
  }
  T &Value() {
    if (valid_) {
      return *reinterpret_cast<T *>(bufer_);
    }
    throw BadOptionalAccess{};
  }
  const T Value() const {
    if (valid_) {
      return *reinterpret_cast<const T *>(bufer_);
    }
    throw BadOptionalAccess{};
  }
  T &operator*() {
    return *reinterpret_cast<T *>(bufer_);
  }
  const T operator*() const {
    return *reinterpret_cast<const T *>(bufer_);
  }
  template <class... Args>
  T &Emplace(Args &&... args) {
    Reset();
    auto cat = new (bufer_) T(std::forward<Args>(args)...);
    valid_ = true;
    return *cat;
  }
  void Reset() {
    if (valid_) {
      reinterpret_cast<T *>(bufer_)->~T();
      valid_ = false;
    }
  }
  void Swap(Optional &oth) {
    Optional tmp(std::move(oth));
    oth = std::move(*this);
    *this = std::move(tmp);
  }
};