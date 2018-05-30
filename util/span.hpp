#ifndef SPAN_HPP
#define SPAN_HPP
#include "assert.hpp"
#include <stdlib.h>
#include <vector>

template <typename T> class span_impl {
public:
  using iterator = const T *;
  span_impl(const T *data, size_t size) : data_(data), size_(size) {}
  iterator begin() const { return data_; }
  iterator end() const { return data_ + size_; }
  const T &operator[](size_t pos) const { return data_[pos]; }
  T &at(size_t pos) {
    assert_e(pos < size_);
    return data_[pos];
  }
  const T &at(size_t pos) const {
    assert_e(pos < size_);
    return data_[pos];
  }
  size_t size() const { return size_; }

private:
  const T *data_;
  size_t size_;
};

template <typename T> class span : public span_impl<T> {
public:
  span(const T *data, size_t size) : span_impl<T>(data, size) {}
  explicit span(const std::vector<std::remove_const_t<T>> &v)
      : span(v.data(), v.size()) {}
};

template <> class span<const char> : public span_impl<const char> {
public:
  span(const char *data, size_t size) : span_impl<const char>(data, size) {}
  template <typename T>
  explicit span(span<T> s)
      : span_impl((const char *)s.begin(), s.size() * sizeof(T)) {}
};

#endif
