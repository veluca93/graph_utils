#include "nde_format.hpp"

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadNDE();
  WriteNDE(g.get());
}
