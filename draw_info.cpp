#include "common_defs.hpp"
#include "graph_io.hpp"
#include <gflags/gflags_completions.h>
#include <iomanip>
#include <random>

DEFINE_string(save, "",
              "Folder where the produced drawing info should be saved");

DEFINE_string(degeneracy, "",
              "Folder where the degeneracy info has been saved");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::unique_ptr<Graph> g = ReadGraph();

  MemoryMappedFile perm_mmf(FLAGS_degeneracy + degen_order_file);
  // MemoryMappedFile rev_perm_mmf(FLAGS_degeneracy + rev_degen_order_file);
  // MemoryMappedFile kcoresize_mmf(FLAGS_degeneracy + kcoresize_file);
  const size_t *perm = perm_mmf.data<size_t>();
  // const size_t *rev_perm = rev_perm_mmf.data<size_t>();
  // const size_t *kcoresize = kcoresize_mmf.data<size_t>();
  std::vector<double> xs(g->size());
  std::vector<double> ys(g->size());
  std::vector<uint8_t> color_position(g->size()); // Fixed point value
  std::mt19937 rng;
  std::uniform_real_distribution<> dist(0.66, 1.5);
  size_t N = g->size();
  {
    Counter cnt("Computing node positions");
    for (size_t i = 0; i < N; i++) {
      double radius = pow(N - i + 1, 0.4);
      double angle = std::uniform_real_distribution<>(0, M_PI * 2)(rng);
      // angle = sqrt(i);
      radius *= dist(rng);
      double x = radius * sin(angle);
      double y = radius * cos(angle);
      xs[perm[i]] = x * 100;
      ys[perm[i]] = y * 100;
      double coef = (double)i / N;
      double param = 10.;
      color_position[perm[i]] = (pow(2, param * (coef - 1)) - pow(2, -param)) /
                                (1 - pow(2, -param)) * 255;
      cnt++;
    }
  }
  double min_x = xs[0];
  double max_x = xs[0];
  double min_y = ys[0];
  double max_y = ys[0];
  {
    Counter cnt("Computing image boundaries");
    for (size_t i = 0; i < N; i++) {
      if (xs[i] < min_x)
        min_x = xs[i];
      if (xs[i] > max_x)
        max_x = xs[i];
      if (ys[i] < min_y)
        min_y = ys[i];
      if (ys[i] > max_y)
        max_y = ys[i];
      cnt++;
    }
  }
  double real_width = max_x - min_x;
  double real_height = max_y - min_y;
  double width = 1.1 * real_width;
  double height = 1.1 * real_height;
  {
    Counter cnt("Converting coordinates");
    for (size_t i = 0; i < N; i++) {
      xs[i] -= min_x;
      xs[i] += (width - real_width) / 2;
      xs[i] /= width;
      xs[i] *= image_size;
      ys[i] -= min_y;
      ys[i] += (height - real_height) / 2;
      ys[i] /= height;
      ys[i] *= image_size;
      cnt++;
    }
  }
  std::vector<int> xs_i(g->size());
  std::vector<int> ys_i(g->size());
  for (size_t i = 0; i < g->size(); i++) {
    xs_i[i] = xs[i];
    ys_i[i] = ys[i];
  }
  std::vector<edge_info_t> sorted_edges(g->edges());
  {
    Counter cnt("Computing edge info positions");
    size_t i = 0;
    for (size_t a = 0; a < N; a++) {
      for (size_t b : g->neighs(a)) {
        i++;
        sorted_edges[i].a = a;
        sorted_edges[i].b = b;
        sorted_edges[i].cpos = color_position[a];
        if (sorted_edges[i].cpos > color_position[b]) {
          sorted_edges[i].cpos = color_position[b];
        }
        cnt++;
      }
    }
  }
  {
    Counter cnt("Sorting edges");
    std::sort(sorted_edges.begin(), sorted_edges.end(),
              [&](const edge_info_t &a, const edge_info_t &b) {
                return a.cpos < b.cpos;
              });
    cnt++;
  }
  if (!FLAGS_save.empty()) {
    std::string command = "mkdir -p " + FLAGS_save;
    assert_m(system(command.c_str()) == 0, command);
    Counter cnt("Saving data");
    span_to_file(FLAGS_save + n_file, span<size_t>(&N, 1));
    cnt++;
    span_to_file(FLAGS_save + xs_file, xs_i);
    cnt++;
    span_to_file(FLAGS_save + ys_file, ys_i);
    cnt++;
    span_to_file(FLAGS_save + color_position_file, color_position);
    cnt++;
    span_to_file(FLAGS_save + edge_info_file, sorted_edges);
    cnt++;
  }
}
