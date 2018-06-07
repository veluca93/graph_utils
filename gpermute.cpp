#include "common_defs.hpp"
#include "graph_io.hpp"

DEFINE_string(
    permutation, "",
    "File to read the permutation from (perm[i] is the new label of node i)");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  MemoryMappedFile perm_mmf(FLAGS_permutation);
  const node_t *perm = perm_mmf.data<node_t>();

  std::unique_ptr<Graph> g = ReadGraph();

  std::vector<std::pair<node_t, node_t>> new_edges;
  new_edges.reserve(g->edges());
  {
    Counter cnt("Creating new edges");
    for (size_t i = 0; i < g->size(); i++) {
      for (size_t v : g->neighs(i)) {
        size_t a = perm[i];
        size_t b = perm[v];
        if (a > b)
          std::swap(a, b);
        new_edges.emplace_back(a, b);
        cnt++;
      }
    }
  }

  {
    Counter cnt("Sorting and cleaning edges");
    std::sort(new_edges.begin(), new_edges.end());
    cnt++;
    new_edges.resize(std::unique(new_edges.begin(), new_edges.end()) -
                     new_edges.begin());
    cnt++;
  }

  InMemoryGraph h(std::move(new_edges));
  WriteGraph(&h);
}
