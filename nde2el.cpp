#include "io.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  size_t N = nextInt();
  int a, b;
  for (size_t i = 0; i < N; i++) {
    a = nextInt();
    b = nextInt();
    if (i % 10000 == 0) {
      std::cerr << "Reading nodes: " << i << "\r";
    }
  }
  std::cerr << "Reading nodes: " << N << "\n";
  size_t num_edges = 0;
  while (true) {
    a = nextInt();
    b = nextInt();
    if (a == (size_t)EOF || b == (size_t)EOF)
      break;
    if (a == b)
      continue;
    write(a, ' ');
    write(b, '\n');
    num_edges++;
    if (num_edges % 10000 == 0) {
      std::cerr << "Reading edges: " << num_edges << "\r";
    }
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
}
