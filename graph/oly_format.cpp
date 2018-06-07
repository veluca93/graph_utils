#include "oly_format.hpp"
#include "io.hpp"
#include <vector>

std::unique_ptr<Graph> ReadOLY(int options) {
  auto chg = SetupGraphInput();
  /*size_t N =*/nextInt();
  size_t M = nextInt();
  std::vector<std::pair<node_t, node_t>> edges;
  edges.reserve((options & GraphReadOptions::BIDIRECTIONAL) ? 2 * M : M);
  {
    size_t a, b;
    Counter cnt("Reading edges");
    while (true) {
      a = nextInt();
      b = nextInt();
      if (a == (size_t)EOF || b == (size_t)EOF)
        break;
      if (a == b)
        continue;
      if (a < b || options & GraphReadOptions::BIDIRECTIONAL) {
        edges.emplace_back(a, b);
      }
      if (a > b || options & GraphReadOptions::BIDIRECTIONAL) {
        edges.emplace_back(b, a);
      }
      cnt++;
    }
  }
  {
    Counter cnt("Cleaning adj lists");
    std::sort(edges.begin(), edges.end());
    cnt++;
    edges.resize(std::unique(edges.begin(), edges.end()) - edges.begin());
    cnt++;
  }
  return std::make_unique<InMemoryGraph>(std::move(edges));
}

void WriteOLY(const Graph *g) {
  auto chg = SetupGraphOutput();
  size_t edge_count = 0;
  {
    Counter cnt("Counting edges");
    for (size_t i = 0; i < g->size(); i++) {
      for (size_t v : g->neighs(i)) {
        if (i < v) {
          edge_count++;
          cnt++;
        }
      }
    }
  }
  write(g->size(), ' ', edge_count, '\n');
  {
    Counter cnt("Outputting edges");
    for (size_t i = 0; i < g->size(); i++) {
      for (size_t v : g->neighs(i)) {
        if (i < v) {
          write(i, ' ', v, '\n');
          cnt++;
        }
      }
    }
  }
}
