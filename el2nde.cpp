#include "el_format.hpp"
#include "nde_format.hpp"

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadEL();
  WriteNDE(g.get());
}
