#include "bin_format.hpp"
#include "io.hpp"
#include <vector>

class MappedGraph : public Graph {
public:
  MappedGraph(const std::string &file) : f_(file) {
    const size_t *data = f_.data<size_t>();
    N = data[0];
    neigh_start_ = data + 1;
    neighs_ = data + N + 2;
  }
  size_t size() const { return N; }
  size_t degree(size_t i) const final {
    assert_e(i < size());
    return neigh_start_[i + 1] - neigh_start_[i];
  }
  span<const size_t> neighs(size_t i) const final {
    return span<const size_t>(neighs_ + neigh_start_[i], degree(i));
  }
  virtual span<const size_t> offset_data() const final {
    return span<const size_t>(neigh_start_, N);
  }
  virtual span<const size_t> edge_data() const final {
    return span<const size_t>(neighs_, neigh_start_[N]);
  }

private:
  MemoryMappedFile f_;
  size_t N;
  const size_t *neigh_start_;
  const size_t *neighs_;
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
    write_span(span<const size_t>(&N, 1));
    cnt++;
    write_span(g->offset_data());
    cnt++;
    write_span(g->edge_data());
    cnt++;
  }
}
