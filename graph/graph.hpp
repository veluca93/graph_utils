#ifndef GRAPH_HPP
#define GRAPH_HPP
#include "io.hpp"
#include "span.hpp"
#include <gflags/gflags.h>
#include <vector>

enum GraphReadOptions { DEFAULT = 0x0, BIDIRECTIONAL = 0x1 };

ChangeOutputFile SetupGraphOutput() __attribute__((warn_unused_result));
ChangeInputFile SetupGraphInput() __attribute__((warn_unused_result));

class Graph {
public:
  virtual size_t size() const = 0;
  virtual size_t degree(size_t i) const = 0;
  virtual span<const size_t> neighs(size_t i) const = 0;
};

class InMemoryGraph : public Graph {
public:
  // Edge list constructor
  InMemoryGraph(std::vector<std::pair<size_t, size_t>> &&edges);
  // Adjacency list constructor
  InMemoryGraph(std::vector<std::vector<size_t>> &&adj);
  size_t size() const { return neigh_start_.size() - 1; }
  size_t degree(size_t i) const final {
    assert_e(i < size());
    return neigh_start_[i + 1] - neigh_start_[i];
  }
  span<const size_t> neighs(size_t i) const final {
    return span<const size_t>(neighs_.data() + neigh_start_[i], degree(i));
  }

private:
  std::vector<size_t> neigh_start_;
  std::vector<size_t> neighs_;
};

#endif
