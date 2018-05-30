#include "common_defs.hpp"
#include "io.hpp"
#include "pngwriter.h"
#include <gflags/gflags.h>
#include <iomanip>
#include <random>

DEFINE_string(drawing, "", "Folder where the drawing info has been saved");

DEFINE_string(degeneracy, "",
              "Folder where the degeneracy info has been saved");

DEFINE_string(output, "output.png", "Where the produced image is saved");
DEFINE_string(highlight, "",
              "File containing the nodes that should be highlighted");
DEFINE_uint64(degen_thresh, 0,
              "Mark as to-be-deleted nodes with less then this degeneracy");
DEFINE_double(fraction_to_delete, 0.0,
              "How many to-be-deleted nodes should actually be deleted");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t N = *MemoryMappedFile(FLAGS_drawing + n_file).data<size_t>();

  std::vector<bool> to_highlight(N);
  if (!FLAGS_highlight.empty()) {
    std::ifstream fin(FLAGS_highlight);
    while (fin) {
      size_t a;
      fin >> a;
      to_highlight[a] = true;
    }
  }

  std::vector<bool> erased(N);

  MemoryMappedFile kcoresize_mmf(FLAGS_degeneracy + kcoresize_file);
  MemoryMappedFile perm_mmf(FLAGS_degeneracy + degen_order_file);
  const size_t *kcoresize = kcoresize_mmf.data<size_t>();
  const size_t *perm = perm_mmf.data<size_t>();
  size_t first_to_keep = N;
  for (size_t i = 0; i < N; i++) {
    if (kcoresize[i] > FLAGS_degen_thresh && first_to_keep == N) {
      first_to_keep = i;
    }
  }

  {
    Counter cnt("Erasing nodes");
    for (size_t i = 0; i < N; i++) {
      if (i < FLAGS_fraction_to_delete * first_to_keep) {
        erased[perm[i]] = true;
        cnt++;
      }
    }
  }

  double r0 = 0.0;
  double g0 = 0.01568;
  double b0 = 0.15686;
  double r1 = 0.0;
  double g1 = 0.30980;
  double b1 = 0.57255;

  pngwriter png(image_size, image_size, 1.0, FLAGS_output.c_str());
  MemoryMappedFile xs_mmf(FLAGS_drawing + xs_file);
  const double *xs = xs_mmf.data<double>();
  MemoryMappedFile ys_mmf(FLAGS_drawing + ys_file);
  const double *ys = ys_mmf.data<double>();
  MemoryMappedFile color_position_mmf(FLAGS_drawing + color_position_file);
  const double *color_position = color_position_mmf.data<double>();
  MemoryMappedFile edge_info_mmf(FLAGS_drawing + edge_info_file);
  const span<const edge_info_t> edge_info = edge_info_mmf.span<edge_info_t>();

  {
    Counter cnt("Drawing normal edges");
    for (const edge_info_t &e : edge_info) {
      size_t a = e.a;
      size_t b = e.b;
      if (erased[a] || erased[b])
        continue;
      if (!(to_highlight[a] && to_highlight[b])) {
        double color_pos = e.cpos;
        png.line_blend(xs[a], ys[a], xs[b], ys[b], 0.0625,
                       r0 * (1 - color_pos) + r1 * color_pos,
                       g0 * (1 - color_pos) + g1 * color_pos,
                       b0 * (1 - color_pos) + b1 * color_pos);
        cnt++;
      }
    }
  }

  {
    Counter cnt("Drawing normal nodes");
    for (size_t i = 0; i < N; i++) {
      if (erased[i])
        continue;
      if (!to_highlight[i]) {
        double color_pos = color_position[i];
        png.filledcircle(xs[i], ys[i], 0, r0 * (1 - color_pos) + r1 * color_pos,
                         g0 * (1 - color_pos) + g1 * color_pos,
                         b0 * (1 - color_pos) + b1 * color_pos);
        cnt++;
      }
    }
  }

  double highlighted_r = 0.74117;
  double highlighted_g = 0.0;
  double highlighted_b = 0.69804;

  {
    Counter cnt("Drawing highlighted edges");
    for (const edge_info_t &e : edge_info) {
      size_t a = e.a;
      size_t b = e.b;
      if (erased[a] || erased[b])
        continue;
      if (to_highlight[a] && to_highlight[b]) {
        png.line_blend(xs[a], ys[a], xs[b], ys[b], 0.0625, highlighted_r,
                       highlighted_g, highlighted_b);
        cnt++;
      }
    }
  }

  {
    Counter cnt("Drawing highlighted nodes");
    for (size_t i = 0; i < N; i++) {
      if (erased[i])
        continue;
      if (!to_highlight[i]) {
        png.filledcircle(xs[i], ys[i], 0, highlighted_r, highlighted_g,
                         highlighted_b);
        cnt++;
      }
    }
  }

  png.close();
}
