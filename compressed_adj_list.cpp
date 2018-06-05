#include "common_defs.hpp"
#include "graph_io.hpp"
#include <sdsl/int_vector.hpp>

using namespace sdsl;

unsigned int reverseBits(unsigned int n) {
  unsigned int rev = 0;
  while (n > 0) {
    rev <<= 1;
    if ((n & 1) == 1)
      rev ^= 1;
    n >>= 1;
  }
  return rev;
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadGraph();
  size_t bits_size = 0;
  int32_t log2_N = 63 - __builtin_clzll(g->size());
  std::vector<size_t> temp;
  {
    Counter cnt("Computing compressed adj lists");
    for (size_t i = 0; i < g->size(); i++) {
      temp.clear();
      // TODO: really, really slow. Only works with numbers <= 2**63 (not really
      // an issue)
      if (g->degree(i) < 2) {
        bits_size += log2_N * g->degree(i);
        continue;
      }
      temp.push_back(0);
      for (size_t v : g->neighs(i)) {
        for (int j = 0; j < log2_N + 1; j++) {
          temp.push_back((reverseBits(v >> j) >> j) + (1 << (64 - j)) - 1);
        }
      }
      std::sort(temp.begin(), temp.end());
      temp.resize(std::unique(temp.begin(), temp.end()) - temp.begin());
      cnt++;
      // We store two bits for all non-leaves nodes.
      bits_size += 2 * (temp.size() - g->degree(i));
    }
  }
  std::cerr << "Number of edges: " << g->edges() << std::endl;
  std::cerr << "Bits size:       " << bits_size << std::endl;
  std::cerr << "Bits per edge:   " << 1.0 * bits_size / g->edges() << std::endl;
}
