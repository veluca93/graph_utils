#include "graph.hpp"
#include "io.hpp"

DEFINE_string(input_file, "", "File to read the graph from");
DEFINE_string(output_file, "", "Which file the graph should be written to");

ChangeOutputFile SetupGraphOutput() {
  return ChangeOutputFile(FLAGS_output_file);
}

ChangeInputFile SetupGraphInput() { return ChangeInputFile(FLAGS_input_file); }

InMemoryGraph::InMemoryGraph(std::vector<std::pair<size_t, size_t>> &&edges) {
  size_t N = 0;
  {
    Counter cnt("Computing maximum node");
    for (size_t i = 0; i < edges.size(); i++) {
      if (edges[i].first > N)
        N = edges[i].first;
      if (edges[i].second > N)
        N = edges[i].second;
      cnt++;
    }
  }
  N++;

  neigh_start_.resize(N + 1);
  neighs_.resize(edges.size());
  neigh_start_[0] = 0;
  {
    Counter cnt("Copying edges");
    for (size_t i = 0; i < edges.size(); i++) {
      neigh_start_[edges[i].first + 1]++;
      neighs_[i] = edges[i].second;
      cnt++;
    }
  }
  {
    Counter cnt("Computing degrees");
    for (size_t i = 1; i < N + 1; i++) {
      neigh_start_[i] += neigh_start_[i - 1];
      cnt++;
    }
  }
}

InMemoryGraph::InMemoryGraph(std::vector<std::vector<size_t>> &&adj) {
  size_t accum_degree = 0;
  neigh_start_.resize(adj.size() + 1);
  neigh_start_[0] = 0;
  {
    Counter cnt("Computing degrees");
    for (size_t i = 0; i < adj.size(); i++) {
      accum_degree += adj[i].size();
      neigh_start_[i + 1] = accum_degree;
      cnt++;
    }
  }
  neighs_.resize(accum_degree);
  {
    Counter cnt("Copying edges");
    for (size_t i = 0; i < adj.size(); i++) {
      for (size_t j = 0; j < adj[i].size(); j++) {
        neighs_[neigh_start_[i] + j] = adj[i][j];
        cnt++;
      }
    }
  }
  // Clear out the vector
  std::vector<std::vector<size_t>> v;
  std::swap(adj, v);
}
