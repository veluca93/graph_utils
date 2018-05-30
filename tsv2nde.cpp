#include "nde_format.hpp"
#include "tsv_format.hpp"

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadTSV();
  WriteNDE(g.get());
}
