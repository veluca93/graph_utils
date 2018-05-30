#include "io.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
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
  write(N, ' ');
  write(num_edges, '\n');
  num_edges = 0;
  for (auto p : edges) {
    write(p.first, ' ');
    write(p.second, '\n');
    num_edges++;
    if (num_edges % 10000 == 0) {
      std::cerr << "Reading edges: " << num_edges << "\r";
    }
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
}
