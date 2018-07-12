#include "graph.hpp"
#include "flags.hpp"
#include "io.hpp"

ChangeOutputFile SetupGraphOutput() {
  return ChangeOutputFile(flags::output_file);
}

ChangeInputFile SetupGraphInput() { return ChangeInputFile(flags::input_file); }

InMemoryGraph::InMemoryGraph(std::vector<std::pair<node_t, node_t>> &&edges) {
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
  std::fill(neigh_start_.begin(), neigh_start_.end(), edge_t{0});
  {
    Counter cnt("Copying edges");
    for (size_t i = 0; i < edges.size(); i++) {
      neigh_start_[edges[i].first + 1].value++;
      neighs_[i] = edges[i].second;
      cnt++;
    }
  }
  {
    Counter cnt("Computing degrees");
    for (size_t i = 1; i < N + 1; i++) {
      neigh_start_[i].value += neigh_start_[i - 1].value;
      cnt++;
    }
  }
}

InMemoryGraph::InMemoryGraph(std::vector<std::vector<node_t>> &&adj) {
  size_t accum_degree = 0;
  neigh_start_.resize(adj.size() + 1);
  neigh_start_[0] = edge_t{0};
  {
    Counter cnt("Computing degrees");
    for (size_t i = 0; i < adj.size(); i++) {
      accum_degree += adj[i].size();
      neigh_start_[i + 1].value = accum_degree;
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
  std::vector<std::vector<node_t>> v;
  std::swap(adj, v);
}

GraphRegisterFormat::GraphRegisterFormat(const std::string &ext,
                                         Graph::read_graph_t reader,
                                         Graph::write_graph_t writer) {
  Graph::Readers()[ext] = reader;
  Graph::Writers()[ext] = writer;
}

std::unique_ptr<Graph> Graph::Read(int options) {
  std::string format = flags::input_format;
  if (format.empty()) {
    for (const auto &kv : Readers()) {
      if (strtk::ends_with("." + kv.first, flags::input_file)) {
        format = kv.first;
        break;
      }
    }
  }
  assert_m(!format.empty(), "Could not detect input format");
  assert_m(Readers().count(format), "Invalid input format");
  return Readers()[format](options);
}

void Graph::Write(const Graph *g) {
  std::string format = flags::output_format;
  if (format.empty()) {
    for (const auto &kv : Writers()) {
      if (strtk::ends_with("." + kv.first, flags::output_file)) {
        format = kv.first;
        break;
      }
    }
  }
  assert_m(!format.empty(), "Could not detect output format");
  assert_m(Writers().count(format), "Invalid output format");
  return Writers()[format](g);
}

std::set<std::string> Graph::ReadFormats() {
  std::set<std::string> ret;
  for (auto p : Readers())
    ret.insert(p.first);
  return ret;
}

std::set<std::string> Graph::WriteFormats() {
  std::set<std::string> ret;
  for (auto p : Writers())
    ret.insert(p.first);
  return ret;
}
