#include "nde_format.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

DEFINE_bool(exact_degrees, false, "Compute exact degrees");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g =
      ReadNDE(FLAGS_exact_degrees ? GraphReadOptions::BIDIRECTIONAL
                                  : GraphReadOptions::DEFAULT);
  WriteNDE(g.get());
}
