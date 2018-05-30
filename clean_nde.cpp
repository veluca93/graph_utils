#include "io.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  size_t N = nextInt();
  std::vector<size_t> *graph = new std::vector<size_t>[N];
  int a, b;
  for (size_t i = 0; i < N; i++) {
    a = nextInt();
    b = nextInt();
    graph[a].reserve(b);
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
    graph[a].push_back(b);
    graph[b].push_back(a);
    if (num_edges % 10000 == 0) {
      std::cerr << "Reading edges: " << num_edges << "\r";
    }
    num_edges++;
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
  for (size_t i = 0; i < N; i++) {
    sort(graph[i].begin(), graph[i].end());
    graph[i].erase(std::unique(graph[i].begin(), graph[i].end()),
                   graph[i].end());
    if (i % 10000 == 0) {
      std::cerr << "Cleaning adj lists: " << i << "\r";
    }
  }
  std::cerr << "Cleaning adj lists: " << N << "\n";
  std::vector<size_t> node_degree(N);
  for (size_t i = 0; i < N; i++) {
    for (size_t v : graph[i]) {
      if (i < v) {
        node_degree[i]++;
        node_degree[v]++;
      }
    }
    if (i % 10000 == 0) {
      std::cerr << "Computing degrees: " << i << "\r";
    }
  }
  std::cerr << "Computing degrees: " << N << "\n";

  write(N, '\n');
  for (size_t i = 0; i < N; i++) {
    write(i, ' ');
    write(node_degree[i], '\n');
    if (i % 10000 == 0) {
      std::cerr << "Outputting nodes: " << i << "\r";
    }
  }
  size_t output_edges = 0;
  std::cerr << "Outputting nodes: " << N << "\n";
  for (size_t i = 0; i < N; i++) {
    for (size_t v : graph[i]) {
      if (i < v) {
        write(i, ' ');
        write(v, '\n');
        output_edges++;
        if (output_edges % 10000 == 0) {
          std::cerr << "Outputting edges: " << output_edges << "\r";
        }
      }
    }
  }
  std::cerr << "Outputting edges: " << output_edges << "\n";
}
