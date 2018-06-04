#include "graph_io.hpp"
#include <gflags/gflags_completions.h>

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadGraph();
  WriteGraph(g.get());
}
