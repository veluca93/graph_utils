#include "graph.hpp"
#include "io.hpp"
#include <vector>

namespace {

const constexpr size_t fingerprint = (sizeof(edge_t) << 4) | sizeof(node_t);

class MappedGraph : public Graph {
public:
  MappedGraph(const std::string &file) : f_(file) {
    const node_t *data = f_.data<node_t>();
    assert_m(fingerprint == *(size_t *)data,
             "This binary file was created with different node/edge sizes!");
    N = data[sizeof(size_t) / sizeof(node_t)];
    neigh_start_ = (edge_t *)data + 1 + sizeof(size_t) / sizeof(node_t);
    neighs_ = data + (sizeof(edge_t) / sizeof(node_t)) * N + 2 +
              sizeof(size_t) / sizeof(node_t);
  }
  node_t size() const { return N; }
  node_t degree(size_t i) const final {
    assert_m(i < size(), std::to_string(i) + " < " + std::to_string(size()));
    return node_t(neigh_start_[i + 1] - neigh_start_[i]);
  }
  span<const node_t> neighs(size_t i) const final {
    return span<const node_t>(neighs_ + neigh_start_[i], degree(i));
  }
  virtual span<const edge_t> offset_data() const final {
    return span<const edge_t>(neigh_start_, N + 1);
  }
  virtual span<const node_t> edge_data() const final {
    return span<const node_t>(neighs_, neigh_start_[N]);
  }

private:
  MemoryMappedFile f_;
  node_t N;
  const edge_t *neigh_start_;
  const node_t *neighs_;
};

std::unique_ptr<Graph> ReadBIN(int options) {
  assert_m(!(options & GraphReadOptions::BIDIRECTIONAL),
           "Reading bidirectional graphs in BIN format is not supported.");
  assert_m(!FLAGS_input_file.empty(),
           "Reading from stdin in BIN format is not supported.");
  Counter cnt("Reading data");
  return std::make_unique<MappedGraph>(FLAGS_input_file);
}

void WriteBIN(const Graph *g) {
  auto chg = SetupGraphOutput();
  {
    Counter cnt("Writing output");
    size_t N = g->size();
    write_span(span<const size_t>(&fingerprint, 1));
    write_span(span<const node_t>((node_t *)&N, 1));
    cnt++;
    write_span(g->offset_data());
    cnt++;
    write_span(g->edge_data());
    cnt++;
  }
}
GraphRegisterFormat r("bin", ReadBIN, WriteBIN);

} // namespace
