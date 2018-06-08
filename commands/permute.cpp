#include "commands.hpp"
#include "common_defs.hpp"
#include "graph.hpp"

namespace {
std::string permutation;

void PermuteMain() {
  MemoryMappedFile perm_mmf(permutation);
  const node_t *perm = perm_mmf.data<node_t>();

  std::unique_ptr<Graph> g = Graph::Read();

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
  Graph::Write(&h);
}

void Permute(CLI::App *app) {
  auto sub = app->add_subcommand("permute", "Permutes the given graph");
  sub->add_option("--permutation,-p", permutation,
                  "File to read the permutation from (perm[i] is the new label "
                  "of node i)")
      ->required();
  sub->set_callback([]() { PermuteMain(); });
}
RegisterCommand r(Permute);
} // namespace
