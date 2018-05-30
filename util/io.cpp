#include "io.hpp"
#include <iomanip>
#include <iostream>
#include <stdio.h>

static const size_t buf_size = 1 << 16;

static char buf[buf_size + 1] = {};
static size_t buf_position = 0;

static void init_output() {}

static void flush() {
  buf[buf_position] = 0;
  std::cout << buf;
  buf_position = 0;
}

void AddToBuffer(char c) {
  buf[buf_position++] = c;
  if (buf_position > buf_size) {
    flush();
  }
}
void AddToBuffer(const std::string &s) {
  for (char c : s) {
    buf[buf_position++] = c;
  }
  if (buf_position > buf_size) {
    flush();
  }
}

template <> void write(const char &param) { AddToBuffer(param); }

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

static void msg(const std::string &base, size_t cnt,
                const std::chrono::high_resolution_clock::time_point &start,
                char endc) {
  size_t time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::high_resolution_clock::now() - start)
                       .count();
  std::cerr << std::setw(30) << base << ": " << std::setw(15) << cnt
            << "    Time taken: " << std::setw(10) << time_ms << "ms " << endc;
}

void Counter::operator++(int) {
  cnt_++;
  if (cnt_ % interval_ == 0) {
    msg(base_msg_, cnt_, start_, '\r');
  }
}
Counter::~Counter() { msg(base_msg_, cnt_, start_, '\n'); }
