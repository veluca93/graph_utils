#include "tsv_format.hpp"
#include "io.hpp"
#include <vector>

std::unique_ptr<Graph> ReadTSV(int options) {
  auto chg = SetupGraphInput();
  std::vector<std::pair<size_t, size_t>> edges;
  {
    size_t a, b;
    Counter cnt("Reading edges");
    while (true) {
      a = nextInt();
      b = nextInt();
      nextInt();
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

void WriteTSV(const Graph *g) {
  auto chg = SetupGraphOutput();
  {
    Counter cnt("Outputting edges");
    for (size_t i = 0; i < g->size(); i++) {
      for (size_t v : g->neighs(i)) {
        if (i < v) {
          write(i, '\t', v, '\t', '1', '\n');
          write(v, '\t', i, '\t', '1', '\n');
          cnt++;
        }
      }
    }
  }
}
