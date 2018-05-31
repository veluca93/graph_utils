#ifndef IO_HPP
#define IO_HPP
#include "span.hpp"
#include "strtk.hpp"
#include <chrono>

void AddToBuffer(const std::string &s);
void AddToBuffer(span<const char> s);
void AddToBuffer(char c);

template <typename T> void write(const T &param) {
  AddToBuffer(strtk::type_to_string(param));
}

template <typename First, typename... Args>
void write(const First &first, const Args &... args) {
  write(first);
  write(args...);
}
void SetOutputFile(const std::string &filename);

class ChangeOutputFile {
public:
  ChangeOutputFile(const std::string &new_file);
  ~ChangeOutputFile();
  ChangeOutputFile(const ChangeOutputFile &) = delete;
  void operator=(const ChangeOutputFile &) = delete;
  ChangeOutputFile(ChangeOutputFile &&) = default;
  ChangeOutputFile &operator=(ChangeOutputFile &&) = default;

private:
  FILE *saved_output_file;
};

class ChangeInputFile {
public:
  ChangeInputFile(const std::string &new_file);
  ~ChangeInputFile();
  ChangeInputFile(const ChangeInputFile &) = delete;
  void operator=(const ChangeInputFile &) = delete;
  ChangeInputFile(ChangeInputFile &&) = default;
  ChangeInputFile &operator=(ChangeInputFile &&) = default;

private:
  FILE *saved_input_file;
};

template <typename T> void write_span(span<T> s) {
  AddToBuffer(span<const char>(s));
}
template <typename T> void write_bytes(const T &t) {
  AddToBuffer(span<const char>((const char *)&t, sizeof(T)));
}
template <typename T>
void span_to_file(const std::string &filename, span<T> s) {
  ChangeOutputFile chg(filename);
  write_span(s);
}
template <typename T>
void span_to_file(const std::string &filename, const std::vector<T> &v) {
  ChangeOutputFile chg(filename);
  write_span(span<T>(v));
}

class MemoryMappedFile {
public:
  MemoryMappedFile(const std::string &filename);
  ~MemoryMappedFile();
  MemoryMappedFile(const MemoryMappedFile &) = delete;
  void operator=(const MemoryMappedFile &) = delete;
  MemoryMappedFile(MemoryMappedFile &&) = default;
  MemoryMappedFile &operator=(MemoryMappedFile &&) = default;
  template <typename T> const T *data() const {
    assert_e(size_ % sizeof(T) == 0);
    return (const T *)data_;
  }
  template <typename T> size_t size() const {
    assert_e(size_ % sizeof(T) == 0);
    return size_ / sizeof(T);
  }
  template <typename T> span<const T> span() const {
    return ::span<const T>(data<T>(), size<T>());
  }

private:
  size_t size_;
  void *data_;
  int fd_;
};

size_t nextInt(bool comments = false);

class Counter {
public:
  Counter(const std::string &msg, size_t interval = 8192)
      : base_msg_(msg), cnt_(0), interval_(interval),
        start_(std::chrono::high_resolution_clock::now()) {}
  void operator++(int);
  void operator++() { (*this)++; }
  ~Counter();
  Counter(const Counter &) = delete;
  void operator=(const Counter &) = delete;
  Counter(Counter &&) = delete;
  void operator=(const Counter &&) = delete;

private:
  std::string base_msg_;
  size_t cnt_;
  size_t interval_;
  std::chrono::high_resolution_clock::time_point start_;
};

#endif
