#include "commands.hpp"
#include "common_defs.hpp"
#include "graph.hpp"
#include <immintrin.h>

namespace {
#ifdef __SSE4_1__
static const int32_t cyclic_shift1_sse = _MM_SHUFFLE(0, 3, 2, 1);
static const int32_t cyclic_shift2_sse = _MM_SHUFFLE(1, 0, 3, 2);
static const int32_t cyclic_shift3_sse = _MM_SHUFFLE(2, 1, 0, 3);
#endif

size_t computation_size = 0;
size_t intersection_size_smaller(span<const node_t> a, span<const node_t> b) {
  size_t l = 0;
  size_t res = 0;
  for (size_t k = 0; k < a.size(); k++) {
    computation_size++;
    if (l >= b.size())
      break;
    if (a[k] < b[l]) {
      continue;
    }
    if (a[k] == b[l]) {
      res++;
      continue;
    }
    size_t offset;
    for (offset = 1; l + offset < b.size(); offset *= 2) {
      computation_size++;
      if (b[l + offset] >= a[k])
        break;
    }
    if (l + offset >= b.size()) {
      offset = b.size() - l;
    }
    size_t lower = l;
    size_t upper = l + offset;
    while (upper > lower + 1) {
      computation_size++;
      size_t middle = lower + (upper - lower) / 2;
      if (b[middle] == a[k]) {
        upper = middle;
        break;
      } else if (b[middle] < a[k]) {
        lower = middle;
      } else if (b[middle] > a[k]) {
        upper = middle;
      }
    }
    assert_e(a[k] > b[lower]);
    l = upper;
    if (l < b.size() && a[k] == b[l]) {
      res++;
      continue;
    }
  }
  return res;
}

size_t intersection_size_same(span<const node_t> a, span<const node_t> b) {
  size_t k = 0;
  size_t l = 0;
  size_t sz = 0;
#if defined(__SSE4_1__) && NODE_T == uint32_t
  size_t sse_end1;
  size_t sse_end2;
  // trim lengths to be a multiple of 4
  sse_end1 = (a.size() / 4) * 4;
  sse_end2 = (b.size() / 4) * 4;

  while (k < sse_end1 && l < sse_end2) {
    computation_size += 4;
    __m128i v1_orig = _mm_loadu_si128((__m128i *)&a[k]);
    __m128i v2_orig = _mm_loadu_si128((__m128i *)&b[l]);
    __m128i v2 = v2_orig;

    //[ move pointers
    int32_t a_max = _mm_extract_epi32(v1_orig, 3);
    int32_t b_max = _mm_extract_epi32(v2, 3);
    k += (a_max <= b_max) * 4;
    l += (a_max >= b_max) * 4;
    //]

    //[ compute mask of common elements
    __m128i cmp_mask1_v1 = _mm_cmpeq_epi32(v1_orig, v2); // pairwise comparison
    v2 = _mm_shuffle_epi32(v2_orig, cyclic_shift1_sse);  // shuffling
    __m128i cmp_mask2_v1 = _mm_cmpeq_epi32(v1_orig, v2); // again...
    v2 = _mm_shuffle_epi32(v2_orig, cyclic_shift2_sse);
    __m128i cmp_mask3_v1 = _mm_cmpeq_epi32(v1_orig, v2); // and again...
    v2 = _mm_shuffle_epi32(v2_orig, cyclic_shift3_sse);
    __m128i cmp_mask4_v1 = _mm_cmpeq_epi32(v1_orig, v2); // and again.
    __m128i cmp_mask_v1 =
        _mm_or_si128(_mm_or_si128(cmp_mask1_v1, cmp_mask2_v1),
                     _mm_or_si128(cmp_mask3_v1, cmp_mask4_v1));
    int32_t mask_v1 = _mm_movemask_ps((__m128)cmp_mask_v1);
    sz += mask_v1 ? __builtin_popcountll(mask_v1) : 0;
  }
#endif

  while (k < a.size() && l < b.size()) {
    computation_size++;
    size_t av = a[k];
    size_t bv = b[l];
    if (av < bv) {
      k++;
    } else if (av > bv) {
      l++;
    } else {
      sz++;
      k++;
      l++;
    }
  }
  return sz;
}

size_t intersection_size(span<const node_t> a, span<const node_t> b) {
  if (a.size() > b.size())
    std::swap(a, b);
  if (a.size() * 2 < b.size()) {
    return intersection_size_smaller(a, b);
  }
  return intersection_size_same(a, b);
}

void TriangleCountingFingerMain() {
  std::unique_ptr<Graph> g = Graph::Read();
  size_t triangle_count = 0;
  {
    Counter cnt("Triangle counting");
    for (uint32_t i = 0; i < g->size(); i++) {
      for (uint32_t n : g->neighs(i)) {
        assert_e(i < n);
        triangle_count += intersection_size(g->neighs(i), g->neighs(n));
        cnt++;
      }
    }
  }
  std::cerr << "Triangles:   " << triangle_count << std::endl;
  std::cerr << "Computation: " << computation_size << std::endl;
}
void TriangleCountingFinger(CLI::App *app) {
  auto sub = app->add_subcommand(
      "triangle_counting_finger",
      "Counts triangles with finger search in the adjacency lists");
  sub->set_callback([]() { TriangleCountingFingerMain(); });
}
RegisterCommand r(TriangleCountingFinger);
} // namespace
