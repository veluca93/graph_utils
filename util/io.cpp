#include "io.hpp"
#include <iostream>
#include <stdio.h>

static const size_t buf_size = 32678;

static std::string buf;

static void init_output() { buf.reserve(1024); }

static void flush() {
  std::cout << buf;
  buf.clear();
}

void AddToBuffer(const std::string &s) {
  buf += strtk::type_to_string(s);
  if (buf.size() > buf_size) {
    flush();
  }
}

size_t nextInt(bool comments) {
  size_t n = 0;
  int ch = getchar_unlocked();
  if (comments && ch == '#') {
    while (ch != EOF && ch != '\n')
      ch = getchar_unlocked();
  }
  while (ch != EOF && (ch < '0' || ch > '9')) {
    ch = getchar_unlocked();
    if (comments && ch == '#') {
      while (ch != EOF && ch != '\n')
        ch = getchar_unlocked();
    }
  }
  if (ch == EOF)
    return EOF;
  while (ch >= '0' && ch <= '9') {
    n = 10 * n + ch - '0';
    ch = getchar_unlocked();
  }
  if (comments && ch == '#') {
    while (ch != EOF && ch != '\n')
      ch = getchar_unlocked();
  }
  return n;
}

class IOSetup {
public:
  IOSetup() {
    std::ios_base::sync_with_stdio(false);
    init_output();
  }
  ~IOSetup() { flush(); }
};

static IOSetup iosetup;

void Counter::operator++(int) {
  cnt_++;
  if (cnt_ % interval_ == 0) {
    std::cerr << base_msg_ << ": " << cnt_ << "\r";
  }
}
Counter::~Counter() { std::cerr << base_msg_ << ": " << cnt_ << "\n"; }
