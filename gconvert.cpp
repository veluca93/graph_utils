#include "graph.hpp"
#include <gflags/gflags_completions.h>

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  Graph::Write(Graph::Read().get());
}
