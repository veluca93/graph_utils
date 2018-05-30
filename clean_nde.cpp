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
  {
    Counter cnt("Reading nodes");
    for (size_t i = 0; i < N; i++) {
      a = nextInt();
      b = nextInt();
      graph[a].reserve(b);
      cnt++;
    }
  }
  {
    Counter cnt("Reading edges");
    while (true) {
      a = nextInt();
      b = nextInt();
      if (a == (size_t)EOF || b == (size_t)EOF)
        break;
      if (a == b)
        continue;
      graph[a].push_back(b);
      graph[b].push_back(a);
      cnt++;
    }
  }
  {
    Counter cnt("Cleaning adj lists");
    for (size_t i = 0; i < N; i++) {
      sort(graph[i].begin(), graph[i].end());
      graph[i].erase(std::unique(graph[i].begin(), graph[i].end()),
                     graph[i].end());
      cnt++;
    }
  }

  std::vector<size_t> node_degree(N);
  {
    Counter cnt("Computing degrees");
    for (size_t i = 0; i < N; i++) {
      for (size_t v : graph[i]) {
        if (i < v) {
          node_degree[i]++;
          node_degree[v]++;
        }
      }
    }
  }

  write(N, '\n');
  {
    Counter cnt("Outputting nodes");
    for (size_t i = 0; i < N; i++) {
      write(i, ' ');
      write(node_degree[i], '\n');
      cnt++;
    }
  }
  {
    Counter cnt("Outputting edges");
    for (size_t i = 0; i < N; i++) {
      for (size_t v : graph[i]) {
        if (i < v) {
          write(i, ' ');
          write(v, '\n');
          cnt++;
        }
      }
    }
  }
}
