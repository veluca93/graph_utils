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
  uint32_t log2_N = 63 - __builtin_clzll(g->size());
  std::vector<size_t> temp;
  std::vector<bool> out;
  {
    Counter cnt("Computing compressed adj lists");
    for (size_t i = 0; i < g->size(); i++) {
      temp.clear();
      out.clear();
      if (g->degree(i) < 2) {
        bits_size += log2_N * g->degree(i);
        continue;
      }
      for (size_t v : g->neighs(i))
        temp.push_back(v);
      for (size_t i = 0; i < log2_N + 1; i++) {
        for (size_t j = temp.size(); j > 0; j--) {
          if (temp[j - 1] & 1) {
            if (j - 1 && temp[j - 2] + 1 == temp[j - 1]) {
              out.push_back(1);
              out.push_back(1);
              j--;
              continue;
            }
            out.push_back(1);
            out.push_back(0);
            continue;
          }
          out.push_back(0);
          out.push_back(1);
        }
        for (size_t j = temp.size(); j > 0; j--) {
          temp[j - 1] >>= 1;
        }
        temp.resize(std::unique(temp.begin(), temp.end()) - temp.begin());
      }
      std::reverse(out.begin(), out.end());
      bits_size += out.size();

      cnt++;
    }
  }
  std::cerr << "Number of edges: " << g->edges() << std::endl;
  std::cerr << "Bits size:       " << bits_size << std::endl;
  std::cerr << "Bits per edge:   " << 1.0 * bits_size / g->edges() << std::endl;
}
