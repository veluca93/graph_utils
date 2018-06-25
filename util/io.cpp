#include "io.hpp"
#include "assert.hpp"
#include "flags.hpp"
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

static const size_t buf_size = 1 << 16;

static char buf[buf_size] = {};
static size_t buf_position = 0;

static FILE *outfile = stdout;
static FILE *infile = stdin;

static void flush() {
  fwrite(buf, 1, buf_position, outfile);
  buf_position = 0;
}

void SetOutputFile(const std::string &filename) {
  flush();
  if (outfile != stdout) {
    fclose(outfile);
  }
  if (filename == "") {
    outfile = stdout;
  } else {
    outfile = fopen(filename.c_str(), "w");
    assert_m(outfile, strerror(errno));
  }
}

ChangeOutputFile::ChangeOutputFile(const std::string &new_file) {
  flush();
  saved_output_file = outfile;
  if (new_file == "") {
    outfile = stdout;
  } else {
    outfile = fopen(new_file.c_str(), "w");
    assert_m(outfile, strerror(errno));
  }
}
ChangeOutputFile::~ChangeOutputFile() {
  flush();
  if (outfile != stdout) {
    fclose(outfile);
  }
  outfile = saved_output_file;
}

void SetInputFile(const std::string &filename) {
  flush();
  if (infile != stdin) {
    fclose(infile);
  }
  if (filename == "") {
    infile = stdin;
  } else {
    infile = fopen(filename.c_str(), "r");
    assert_m(infile, strerror(errno));
  }
}

ChangeInputFile::ChangeInputFile(const std::string &new_file) {
  flush();
  saved_input_file = infile;
  if (new_file == "") {
    infile = stdin;
  } else {
    infile = fopen(new_file.c_str(), "r");
    assert_m(infile, strerror(errno));
  }
}
ChangeInputFile::~ChangeInputFile() {
  flush();
  if (infile != stdin) {
    fclose(infile);
  }
  infile = saved_input_file;
}

void AddToBuffer(char c) {
  buf[buf_position++] = c;
  if (buf_position > buf_size) {
    flush();
  }
}

void AddToBuffer(const char *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    buf[buf_position++] = data[i];
    if (buf_position > buf_size) {
      flush();
    }
  }
}

void AddToBuffer(span<const char> s) { AddToBuffer(s.begin(), s.size()); }

void AddToBuffer(const std::string &s) { AddToBuffer(s.c_str(), s.size()); }

template <> void write(const char &param) { AddToBuffer(param); }

size_t nextInt(bool comments) {
  size_t n = 0;
  int ch = getc_unlocked(infile);
  if (comments && ch == '#') {
    while (ch != EOF && ch != '\n')
      ch = getc_unlocked(infile);
  }
  while (ch != EOF && (ch < '0' || ch > '9')) {
    ch = getc_unlocked(infile);
    if (comments && ch == '#') {
      while (ch != EOF && ch != '\n')
        ch = getc_unlocked(infile);
    }
  }
  if (ch == EOF)
    return EOF;
  while (ch >= '0' && ch <= '9') {
    n = 10 * n + ch - '0';
    ch = getc_unlocked(infile);
  }
  if (comments && ch == '#') {
    while (ch != EOF && ch != '\n')
      ch = getc_unlocked(infile);
  }
  return n;
}

class IOSetup {
public:
  IOSetup() {}
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

MemoryMappedFile::MemoryMappedFile(const std::string &filename) {
#if defined(__unix__) || defined(__APPLE__)
  struct stat st;
  assert_m(stat(filename.c_str(), &st) == 0, strerror(errno));
  size_ = st.st_size;
  fd_ = open(filename.c_str(), O_RDONLY, 0);
  auto flags = MAP_SHARED;
#if __linux__
  if (!flags::dont_populate_cache) {
    flags |= MAP_POPULATE;
  }
#endif
  data_ = mmap(NULL, size_, PROT_READ, flags, fd_, 0);
  assert_e(data_ != MAP_FAILED);
#else
  assert_m(false, "Memory mapped files not implemented on non-unix systems");
#endif
}

MemoryMappedFile::~MemoryMappedFile() {
#if defined(__unix__) || defined(__APPLE__)
  munmap(data_, size_);
  close(fd_);
#endif
}
