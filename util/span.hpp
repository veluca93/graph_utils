#ifndef SPAN_HPP
#define SPAN_HPP
#include "assert.hpp"
#include <stdlib.h>

template <typename T> class span {
public:
  using iterator = T *;
  span(T *data, size_t size) : data_(data), size_(size) {}
  iterator begin() const { return data_; }
  iterator end() const { return data_ + size_; }
  T &operator[](size_t pos) { return data_[pos]; }
  const T &operator[](size_t pos) const { return data_[pos]; }
  T &at(size_t pos) {
    assert_e(pos < size_);
    return data_[pos];
  }
  const T &at(size_t pos) const {
    assert_e(pos < size_);
    return data_[pos];
  }

private:
  T *data_;
  size_t size_;
};

#endif
