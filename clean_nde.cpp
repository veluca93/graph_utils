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
    for (size_t v: graph[i]) {
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

  init_output();
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
    for (size_t v: graph[i]) {
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
  flush();
  std::cerr << "Outputting edges: " << output_edges << "\n";
}
