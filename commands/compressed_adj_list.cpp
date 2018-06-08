#include "commands.hpp"
#include "common_defs.hpp"
#include "graph.hpp"
#include <sdsl/int_vector.hpp>
#include <sdsl/rrr_vector.hpp>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 31
#endif

#ifndef RANK_SAMPLE_DENS
#define RANK_SAMPLE_DENS 32
#endif

namespace {
using namespace sdsl;

void CompressedAdjListMain() {
  std::unique_ptr<Graph> g = Graph::Read();
  size_t bits_size = 0;
  size_t compressed_size = 0;
  uint32_t log2_N = 63 - __builtin_clzll(g->size());
  std::vector<size_t> temp;
  std::vector<bool> out, final_out;
  {
    Counter cnt("Computing compressed adj lists");
    for (size_t i = 0; i < g->size(); i++) {
      temp.clear();
      out.clear();
      if (g->degree(i) < 2) {
        bits_size += log2_N * g->degree(i);
        compressed_size += log2_N * g->degree(i);
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
      for (size_t i = 0; i < out.size(); i++) {
        final_out.push_back(out[i]);
      }
      cnt++;
    }
  }
  sdsl::int_vector<1> real_out;
  real_out.resize(final_out.size());
  for (size_t i = 0; i < final_out.size(); i++) {
    real_out[i] = final_out[i];
  }
  bits_size += size_in_bytes(real_out) * 8;
  using rrr_vector =
      sdsl::rrr_vector<BLOCK_SIZE, int_vector<>, RANK_SAMPLE_DENS>;
  rrr_vector compressed(real_out);
  compressed_size += sdsl::size_in_bytes(compressed) * 8;

  std::cerr << "Number of edges:           " << g->edges() << std::endl;
  std::cerr << "Bits size:                 " << bits_size << std::endl;
  std::cerr << "Bits per edge:             " << 1.0 * bits_size / g->edges()
            << std::endl;
  std::cerr << "Compressed size:           " << compressed_size << std::endl;
  std::cerr << "Compressed bits per edge:  "
            << 1.0 * compressed_size / g->edges() << std::endl;
}

void CompressedAdjList(CLI::App *app) {
  auto sub = app->add_subcommand("compressed_adj_list", "Compresses adj lists");
  sub->set_callback([]() { CompressedAdjListMain(); });
}
RegisterCommand r(CompressedAdjList);
} // namespace
