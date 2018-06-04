#include "common_defs.hpp"
#include "graph_io.hpp"
#include <gflags/gflags_completions.h>
#include <iomanip>

DEFINE_string(save, "",
              "Folder where the produced degeneracy info should be saved");

std::pair<std::vector<size_t>, std::vector<size_t>>
get_perm(const Graph *graph) {
  size_t N = graph->size();
  std::vector<size_t> perm;
  std::vector<size_t> *d = new std::vector<size_t>[N];
  std::vector<size_t> degs(N);
  std::vector<size_t> positions(N);
  std::vector<bool> used(N);
  std::vector<size_t> degeneracies;
  perm.reserve(N);
  for (size_t i = 0; i < N; i++) {
    d[graph->degree(i)].push_back(i);
    degs[i] = graph->degree(i);
    positions[i] = d[degs[i]].size() - 1;
  }
  unsigned j = 0;
  Counter cnt("Degeneracy order");
  for (size_t i = 0; i < N; i++) {
    while (d[j].empty())
      j++;
    size_t v = d[j].back();
    d[j].pop_back();
    perm.push_back(v);
    degeneracies.push_back(j);
    used[v] = true;
    for (auto g : graph->neighs(v)) {
      if (used[g])
        continue;
      size_t &to_swap = d[degs[g]][positions[g]];
      std::swap(to_swap, d[degs[g]].back());
      positions[to_swap] = positions[g];
      d[degs[g]].pop_back();
      degs[g]--;
      d[degs[g]].push_back(g);
      positions[g] = d[degs[g]].size() - 1;
    }
    if (j > 0)
      j--;
    cnt++;
  }
  delete[] d;
  return {perm, degeneracies};
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadGraph(GraphReadOptions::BIDIRECTIONAL);
  auto pp = get_perm(g.get());
  size_t max_degen = 0;
  for (size_t i = 0; i < g->size(); i++) {
    if (max_degen < pp.second[i])
      max_degen = pp.second[i];
    else
      pp.second[i] = max_degen;
  }
  std::vector<size_t> degen_counts(g->size());
  for (size_t i = 0; i < g->size(); i++)
    degen_counts[pp.second[i]]++;
  for (size_t i = g->size() - 1; i > 0; i--)
    degen_counts[i - 1] += degen_counts[i];
  std::vector<std::pair<size_t, size_t>> degen_stats;
  for (size_t i = 0; i < degen_counts.size(); i++) {
    if (degen_counts[i])
      degen_stats.emplace_back(i, degen_counts[i]);
  }
  std::cerr << "Degeneracy:   ";
  for (size_t i = 1; i <= degen_stats.size(); i *= 2)
    std::cerr << std::setw(12) << (degen_stats.rbegin() + i - 1)->first;
  std::cerr << "\nK-core size:  ";
  for (size_t i = 1; i <= degen_stats.size(); i *= 2)
    std::cerr << std::setw(12) << (degen_stats.rbegin() + i - 1)->second;
  std::cerr << std::endl;
  if (!FLAGS_save.empty()) {
    std::vector<size_t> rev_perm(g->size());
    for (size_t i = 0; i < g->size(); i++) {
      assert_m(pp.first[i] < g->size(), std::to_string(pp.first[i]));
      rev_perm[pp.first[i]] = i;
    }
    std::string command = "mkdir -p " + FLAGS_save;
    assert_m(system(command.c_str()) == 0, command);
    Counter cnt("Saving data");
    span_to_file(FLAGS_save + degen_order_file, pp.first);
    cnt++;
    span_to_file(FLAGS_save + rev_degen_order_file, rev_perm);
    cnt++;
    span_to_file(FLAGS_save + kcoresize_file, pp.second);
    cnt++;
  }
}
