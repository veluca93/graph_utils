#include <vector>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "strtk.hpp"


static const size_t buf_size = 32678;

std::string buf;

void init_output() {
    buf.reserve(1024);
}

void flush() {
    std::cout << buf;
    buf.clear();
}

void write(size_t x, char then) {
    buf += strtk::type_to_string(x);
    buf += then;
    if (buf.size() > buf_size) {
        flush();
    }
}

size_t nextInt() {
  size_t n = 0;
  int ch = getchar_unlocked();
  while (ch != EOF && (ch < '0' || ch > '9'))
    ch = getchar_unlocked();
  if (ch == EOF)
    return EOF;
  while (ch >= '0' && ch <= '9') {
    n = 10 * n + ch - '0';
    ch = getchar_unlocked();
  }
  return n;
}

int main(int argc, char **argv) {
  std::ios_base::sync_with_stdio(false);
  size_t N = nextInt();
  int a, b;
  size_t num_edges = 0;
  for (size_t i = 0; i < N; i++) {
    a = nextInt();
    b = nextInt();
    num_edges += b;
    if (i % 10000 == 0) {
        std::cerr << "Reading nodes: " << i << "\r";
    }
  }
  num_edges /= 2;
  std::cerr << "Reading nodes: " << N << "\n";
  std::vector<std::pair<size_t, size_t>> edges;
  edges.reserve(num_edges);
  num_edges = 0;
  while (true) {
    a = nextInt();
    b = nextInt();
    if (a == (size_t)EOF || b == (size_t)EOF)
      break;
    if (a == b)
      continue;
    edges.emplace_back(a, b);
    num_edges++;
    if (num_edges % 10000 == 0) {
        std::cerr << "Reading edges: " << num_edges << "\r";
    }
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
  init_output();
  write(N, ' ');
  write(num_edges, '\n');
  num_edges = 0;
  for (auto p: edges) {
    write(p.first, ' '); 
    write(p.second, '\n'); 
    num_edges++;
    if (num_edges % 10000 == 0) {
        std::cerr << "Reading edges: " << num_edges << "\r";
    }
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
  flush();
}
