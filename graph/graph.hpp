#ifndef GRAPH_HPP
#define GRAPH_HPP
#include "io.hpp"
#include "span.hpp"
#include <gflags/gflags.h>
#include <vector>

#ifndef NODE_T
#define NODE_T uint32_t
#endif

#ifndef EDGE_T
#define EDGE_T uint32_t
#endif

struct node_t {
  explicit node_t(size_t val) : value(val) {}
  node_t() {}
  operator NODE_T() const { return value; }

  NODE_T value;
} __attribute__((packed));

struct edge_t {
  explicit edge_t(size_t val) : value(val) {}
  edge_t() {}
  operator EDGE_T() const { return value; }

  EDGE_T value;
} __attribute__((packed));

static_assert(sizeof(edge_t) % sizeof(node_t) == 0,
              "Size of edge_t should be a multiple of the size of node_t");
static_assert(sizeof(size_t) % sizeof(node_t) == 0,
              "Size of size_t should be a multiple of the size of node_t");

enum GraphReadOptions { DEFAULT = 0x0, BIDIRECTIONAL = 0x1 };

DECLARE_string(input_file);
DECLARE_string(output_file);

ChangeOutputFile SetupGraphOutput() __attribute__((warn_unused_result));
ChangeInputFile SetupGraphInput() __attribute__((warn_unused_result));

class GraphRegisterFormat;

class Graph {
public:
  virtual node_t size() const = 0;
  virtual node_t degree(size_t i) const = 0;
  virtual span<const node_t> neighs(size_t i) const = 0;
  virtual span<const edge_t> offset_data() const = 0;
  virtual span<const node_t> edge_data() const = 0;
  virtual edge_t edges() const { return edge_t{(EDGE_T)edge_data().size()}; };

  using read_graph_t = std::function<std::unique_ptr<Graph>(int)>;
  using write_graph_t = std::function<void(const Graph *)>;

private:
  static auto &Readers() {
    static std::map<std::string, read_graph_t> readers_;
    return readers_;
  }
  static auto &Writers() {
    static std::map<std::string, write_graph_t> writers_;
    return writers_;
  }

public:
  static std::unique_ptr<Graph> Read(int options = GraphReadOptions::DEFAULT);
  static void Write(const Graph *);
  static bool HasReader(const std::string &r) { return Readers().count(r); }
  static bool HasWriter(const std::string &w) { return Writers().count(w); }
  friend class GraphRegisterFormat;
};

class InMemoryGraph : public Graph {
public:
  // Edge list constructor
  InMemoryGraph(std::vector<std::pair<node_t, node_t>> &&edges);
  // Adjacency list constructor
  InMemoryGraph(std::vector<std::vector<node_t>> &&adj);
  node_t size() const { return node_t{NODE_T(neigh_start_.size() - 1)}; }
  node_t degree(size_t i) const final {
    assert_m(i < size(), std::to_string(i) + " < " + std::to_string(size()));
    return node_t{(NODE_T)(neigh_start_[i + 1] - neigh_start_[i])};
  }
  span<const node_t> neighs(size_t i) const final {
    return span<const node_t>(neighs_.data() + neigh_start_[i], degree(i));
  }
  virtual span<const edge_t> offset_data() const final {
    return span<const edge_t>(neigh_start_.data(), neigh_start_.size());
  }
  virtual span<const node_t> edge_data() const final {
    return span<const node_t>(neighs_.data(), neighs_.size());
  }

private:
  std::vector<edge_t> neigh_start_;
  std::vector<node_t> neighs_;
};

class GraphRegisterFormat {
public:
  GraphRegisterFormat(const std::string &ext, Graph::read_graph_t reader,
                      Graph::write_graph_t writer);
};

#endif
