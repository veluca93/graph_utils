#include "flags.hpp"
#include "graph.hpp"
#include "io.hpp"
#include <vector>

namespace {

const constexpr size_t fingerprint = (sizeof(edge_t) << 4) | sizeof(node_t) | 1;

std::unique_ptr<Graph> ReadSBIN(int options) {
  auto chg = SetupGraphInput();
  size_t fg = bin_read<size_t>();
  std::cerr << fg << std::endl;
  assert_m(fg == fingerprint,
           "This binary file was created with different node/edge sizes!");
  node_t N = bin_read<node_t>();
  std::vector<std::pair<node_t, node_t>> edges;
  {
    size_t a, b;
    Counter cnt("Reading edges");
    for (size_t i = 0; i < N; i++) {
      node_t count = bin_read<node_t>();
      for (size_t _ = 0; _ < count; _++) {
        a = i;
        b = bin_read<node_t>();
        if (a == b)
          continue;
        if (a < b || options & GraphReadOptions::BIDIRECTIONAL) {
          edges.emplace_back(node_t(a), node_t(b));
        }
        if (a > b || options & GraphReadOptions::BIDIRECTIONAL) {
          edges.emplace_back(node_t(b), node_t(a));
        }
      }
    }
    cnt++;
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

void WriteSBIN(const Graph *g) {
  auto chg = SetupGraphOutput();
  {
    Counter cnt("Writing output");
    size_t N = g->size();
    write_span(span<const size_t>(&fingerprint, 1));
    for (size_t i = 0; i < N; i++) {
      node_t degree = g->degree(i);
      write_span(span<const node_t>(&degree, 1));
      write_span(g->neighs(i));
      cnt++;
    }
  }
}
GraphRegisterFormat r("sbin", ReadSBIN, WriteSBIN);

} // namespace
