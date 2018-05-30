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
  if (ch == '#') {
    while (ch != EOF && ch != '\n') 
      ch = getchar_unlocked();
  }
  while (ch != EOF && (ch < '0' || ch > '9')) {
    ch = getchar_unlocked();
    if (ch == '#') {
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
  if (ch == '#') {
    while (ch != EOF && ch != '\n') 
      ch = getchar_unlocked();
  }
  return n;
}

int main(int argc, char **argv) {
  std::ios_base::sync_with_stdio(false);
  std::vector<std::pair<size_t, size_t>> edges;
  size_t a, b;
  size_t N = 0;
  size_t num_edges = 0;
  nextInt();
  edges.reserve(nextInt());
  while (true) {
    a = nextInt();
    b = nextInt();
    if (a == (size_t)EOF || b == (size_t)EOF)
      break;
    if (a == b)
      continue;
    if (a > b) std::swap(a, b);
    edges.emplace_back(a, b);
    if (a > N) N = a;
    if (b > N) N = b;
    num_edges++;
    if (num_edges % 10000 == 0) {
        std::cerr << "Reading edges: " << num_edges << "\r";
    }
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
  N++;

  std::cerr << "Sorting edges ..." << std::flush;
  std::sort(edges.begin(), edges.end());
  std::cerr << " cleaning ..." << std::flush;
  edges.resize(std::unique(edges.begin(), edges.end())-edges.begin());
  std::cerr << " done" << std::endl;

  std::vector<size_t> node_degree(N);
  for (size_t i = 0; i < edges.size(); i++) {
    node_degree[edges[i].first]++;
    node_degree[edges[i].second]++;
    if (i % 10000 == 0) {
        std::cerr << "Computing degrees: " << i << "\r";
    }
  }
  std::cerr << "Computing degrees: " << edges.size() << "\n";

  init_output();
  write(N, '\n');
  for (size_t i = 0; i < N; i++) {
    write(i, ' ');
    write(node_degree[i], '\n');
    if (i % 10000 == 0) {
        std::cerr << "Outputting nodes: " << i << "\r";
    }
  }
  std::cerr << "Outputting nodes: " << N << "\n";
  flush();
  for (size_t i = 0; i < edges.size(); i++) {
    write(edges[i].first, ' ');
    write(edges[i].second, '\n');
    if (i % 10000 == 0) {
        std::cerr << "Outputting edges: " << i << "\r";
    }
  }
  std::cerr << "Outputting edges: " << edges.size() << "\n";
  flush();
}
