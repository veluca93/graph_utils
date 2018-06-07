#include "common_defs.hpp"
#include "graph_io.hpp"
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support_v.hpp>

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
sdsl::int_vector<1> real_out;
sdsl::int_vector<1>::rank_1_type rank_support;
size_t computation_size = 0;

size_t intersection_size(size_t t1b, size_t t1br, size_t t1, size_t t1sz,
                         size_t t2b, size_t t2br, size_t t2, size_t t2sz,
                         size_t val) {
  computation_size++;
  if (t1 >= t1sz) {
    assert_e(t2 >= t2sz);
    return 1;
  }
  size_t res = 0;
  if (real_out[t1b + 2 * t1] == 1 && real_out[t2b + 2 * t2] == 1) {
    res += intersection_size(
        t1b, t1br, ::rank_support(t1b + 2 * t1 + 1) - t1br, t1sz, t2b, t2br,
        ::rank_support(t2b + 2 * t2 + 1) - t2br, t2sz, val << 1);
  }
  if (real_out[t1b + 2 * t1 + 1] == 1 && real_out[t2b + 2 * t2 + 1] == 1) {
    res += intersection_size(
        t1b, t1br, ::rank_support(t1b + 2 * t1 + 2) - t1br, t1sz, t2b, t2br,
        ::rank_support(t2b + 2 * t2 + 2) - t2br, t2sz, (val << 1) | 1);
  }
  return res;
}

size_t intersection_size(size_t t1s, size_t t1e, size_t t2s, size_t t2e) {
  return intersection_size(t1s, ::rank_support(t1s), 0, (t1e - t1s) / 2, t2s,
                           ::rank_support(t2s), 0, (t2e - t2s) / 2, 0);
}

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  // TODO: fix this for no bidirectional
  std::unique_ptr<Graph> g = ReadGraph(GraphReadOptions::BIDIRECTIONAL);
  size_t bits_size = 0;
  uint32_t log2_N = 63 - __builtin_clzll(g->size());
  std::vector<size_t> temp;
  std::vector<bool> out, final_out;
  std::vector<size_t> bit_start;
  {
    Counter cnt("Computing compressed adj lists");
    for (size_t i = 0; i < g->size(); i++) {
      temp.clear();
      out.clear();
      bit_start.push_back(final_out.size());
      cnt++;
      if (g->degree(i) == 0)
        continue;
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
    }
  }
  uint32_t N = g->size();
  size_t M = g->edges();
  bit_start.push_back(final_out.size());
  {
    Counter cnt("Computing rank/select", 1);
    real_out.resize(final_out.size());
    cnt++;
    for (size_t i = 0; i < final_out.size(); i++) {
      real_out[i] = final_out[i];
    }
    cnt++;
    sdsl::util::init_support(::rank_support, &real_out);
    cnt++;
  }
  bits_size += size_in_bytes(real_out) * 8;
  size_t triangle_count = 0;
  {
    Counter cnt("Triangle counting");
    for (uint32_t i = 0; i < N; i++) {
      for (uint32_t n : g->neighs(i)) {
        if (i < n) {
          triangle_count += intersection_size(bit_start[i], bit_start[i + 1],
                                              bit_start[n], bit_start[n + 1]);
          cnt++;
        }
      }
    }
  }
  triangle_count /= 3;

  std::cerr << "Number of edges:           " << M << std::endl;
  std::cerr << "Bits size:                 " << bits_size << std::endl;
  std::cerr << "Bits per edge:             " << 1.0 * bits_size / M
            << std::endl;
  std::cerr << "Triangles:   " << triangle_count << std::endl;
  std::cerr << "Computation: " << computation_size << std::endl;
}
