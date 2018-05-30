#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <math.h>
#include <random>
#include <string>
#include <vector>

#include "pngwriter.h"

static const size_t buf_size = 32678;

size_t nextInt() {
  size_t n = 0;
  int ch = getchar_unlocked();
  if (ch == '#') {
    while (ch != EOF && ch != '\n')
      ch = getchar_unlocked();
  }
  while (ch != EOF && (ch < '0' || ch > '9')) {
    ch = getchar_unlocked();
    if (ch == '#') {
      while (ch != EOF && ch != '\n')
        ch = getchar_unlocked();
    }
  }
  if (ch == EOF)
    return EOF;
  while (ch >= '0' && ch <= '9') {
    n = 10 * n + ch - '0';
    ch = getchar_unlocked();
  }
  if (ch == '#') {
    while (ch != EOF && ch != '\n')
      ch = getchar_unlocked();
  }
  return n;
}

std::pair<std::vector<size_t>, std::vector<size_t>>
get_perm(const std::vector<size_t> *graph, size_t N) {
  std::vector<size_t> perm;
  std::vector<size_t> *d = new std::vector<size_t>[N];
  std::vector<size_t> degs(N);
  std::vector<size_t> positions(N);
  std::vector<bool> used(N);
  std::vector<size_t> degeneracies;
  unsigned degeneracy = 0;
  for (size_t i = 0; i < N; i++) {
    d[graph[i].size()].push_back(i);
    degs[i] = graph[i].size();
    positions[i] = d[degs[i]].size() - 1;
  }
  unsigned j = 0;
  for (size_t i = 0; i < N; i++) {
    while (d[j].empty())
      j++;
    size_t v = d[j].back();
    d[j].pop_back();
    perm.push_back(v);
    degeneracies.push_back(j);
    used[v] = true;
    if (degeneracy < j)
      degeneracy = j;
    for (auto g : graph[v]) {
      if (used[g])
        continue;
      size_t &to_swap = d[degs[g]][positions[g]];
      std::swap(to_swap, d[degs[g]].back());
      positions[to_swap] = positions[g];
      d[degs[g]].pop_back();
      degs[g]--;
      d[degs[g]].push_back(g);
      positions[g] = d[degs[g]].size() - 1;
    }
    if (j > 0)
      j--;
    if (i % 10000 == 0) {
      std::cerr << "Degeneracy order: " << i << "\r";
    }
  }
  std::cerr << "Degeneracy order: " << N << "\n";
  delete[] d;
  return {perm, degeneracies};
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " output.png [highlight [degen_thresh [fraction_to_delete]]]"
              << std::endl;
    return 1;
  }
  std::ios_base::sync_with_stdio(false);
  std::vector<std::pair<size_t, size_t>> edges;
  size_t a, b;
  size_t N = 0;
  size_t num_edges = 0;
  while (true) {
    a = nextInt();
    b = nextInt();
    if (a == (size_t)EOF || b == (size_t)EOF)
      break;
    if (a == b)
      continue;
    if (a > b)
      std::swap(a, b);
    edges.emplace_back(a, b);
    if (a > N)
      N = a;
    if (b > N)
      N = b;
    num_edges++;
    if (num_edges % 10000 == 0) {
      std::cerr << "Reading edges: " << num_edges << "\r";
    }
  }
  std::cerr << "Reading edges: " << num_edges << "\n";
  N++;

  std::cerr << "Sorting edges ..." << std::flush;
  std::sort(edges.begin(), edges.end());
  std::cerr << " cleaning ..." << std::flush;
  edges.resize(std::unique(edges.begin(), edges.end()) - edges.begin());
  std::cerr << " done" << std::endl;

  std::vector<bool> to_highlight(N);
  if (argc > 2) {
    std::ifstream fin(argv[2]);
    while (fin) {
      size_t a;
      fin >> a;
      to_highlight[a] = true;
    }
  }
  std::vector<bool> erased(N);
  size_t degen_thresh = 0;
  if (argc > 3) {
    degen_thresh = atoi(argv[3]);
  }

  double fraction_to_delete = 0;
  if (argc > 4) {
    fraction_to_delete = atof(argv[4]);
  }

  std::vector<size_t> node_degree(N);
  for (size_t i = 0; i < edges.size(); i++) {
    node_degree[edges[i].first]++;
    node_degree[edges[i].second]++;
    if (i % 10000 == 0) {
      std::cerr << "Computing degrees: " << i << "\r";
    }
  }
  std::cerr << "Computing degrees: " << edges.size() << "\n";
  std::vector<size_t> *graph = new std::vector<size_t>[N];
  for (size_t i = 0; i < N; i++) {
    graph[i].reserve(node_degree[i]);
  }
  for (size_t i = 0; i < edges.size(); i++) {
    graph[edges[i].first].push_back(edges[i].second);
    graph[edges[i].second].push_back(edges[i].first);
    if (i % 10000 == 0) {
      std::cerr << "Building graph: " << i << "\r";
    }
  }
  std::cerr << "Building graph: " << edges.size() << "\n";
  auto p = get_perm(graph, N);
  size_t max_degen = 0;
  size_t first_to_keep = N;
  for (size_t i = 0; i < N; i++) {
    size_t &degen = p.second[i];
    if (degen > max_degen)
      max_degen = degen;
    if (degen < max_degen)
      degen = max_degen;
    if (max_degen > degen_thresh && first_to_keep == N) {
      first_to_keep = i;
    }
  }
  std::vector<size_t> rev_perm(N);
  for (size_t i = 0; i < N; i++)
    rev_perm[p.first[i]] = i;
  delete[] graph;
  std::vector<double> xs(N);
  std::vector<double> ys(N);
  std::vector<double> color_position(N);
  std::mt19937 rng;
  std::uniform_real_distribution<> dist(0.66, 1.5);
  size_t erased_nodes = 0;
  for (size_t i = 0; i < N; i++) {
    // double degen = p.second[i];
    double radius = pow(N - i + 1, 0.4);
    double angle = std::uniform_real_distribution<>(0, M_PI * 2)(rng);
    // angle = sqrt(i);
    radius *= dist(rng);
    double x = radius * sin(angle);
    double y = radius * cos(angle);
    xs[p.first[i]] = x * 100;
    ys[p.first[i]] = y * 100;
    double coef = (double)i / N;
    double param = 10.;
    color_position[p.first[i]] =
        (pow(2, param * (coef - 1)) - pow(2, -param)) / (1 - pow(2, -param));
    if (i < fraction_to_delete * first_to_keep) {
      erased[p.first[i]] = true;
      erased_nodes++;
    }
    if (i % 10000 == 0) {
      std::cerr << "Computing node positions: " << i << "\r";
    }
  }
  std::cerr << "Computing node positions: " << N << "\n";
  std::cerr << "Erased nodes: " << erased_nodes << "\n";
  double min_x = xs[0];
  double max_x = xs[0];
  double min_y = ys[0];
  double max_y = ys[0];
  for (size_t i = 0; i < N; i++) {
    if (xs[i] < min_x)
      min_x = xs[i];
    if (xs[i] > max_x)
      max_x = xs[i];
    if (ys[i] < min_y)
      min_y = ys[i];
    if (ys[i] > max_y)
      max_y = ys[i];
  }
  double real_width = max_x - min_x;
  double real_height = max_y - min_y;
  double width = 1.1 * real_width;
  double height = 1.1 * real_height;
  double image_size = 2160;
  for (size_t i = 0; i < N; i++) {
    xs[i] -= min_x;
    xs[i] += (width - real_width) / 2;
    xs[i] /= width;
    xs[i] *= image_size;
    ys[i] -= min_y;
    ys[i] += (height - real_height) / 2;
    ys[i] /= height;
    ys[i] *= image_size;
  }
  std::cerr << "Real number of edges: " << edges.size() << "\n";

  double r0 = 0.0;
  double g0 = 0.01568;
  double b0 = 0.15686;
  double r1 = 0.0;
  double g1 = 0.30980;
  double b1 = 0.57255;

  pngwriter png(image_size, image_size, 1.0, argv[1]);
  size_t highlighted_edges = 0;
  size_t normal_edges = 0;
  std::vector<size_t> edge_increasing_cpos(edges.size());
  std::vector<double> edge_cpos(edges.size());
  for (size_t i = 0; i < edges.size(); i++) {
    size_t a = edges[i].first;
    size_t b = edges[i].second;
    edge_increasing_cpos[i] = i;
    edge_cpos[i] = color_position[a];
    if (edge_cpos[i] > color_position[b]) {
      edge_cpos[i] = color_position[b];
    }
  }
  std::sort(edge_increasing_cpos.begin(), edge_increasing_cpos.end(),
            [&](size_t a, size_t b) { return edge_cpos[a] < edge_cpos[b]; });
  for (size_t i : edge_increasing_cpos) {
    size_t a = edges[i].first;
    size_t b = edges[i].second;
    if (erased[a] || erased[b])
      continue;
    if (!(to_highlight[a] && to_highlight[b])) {
      normal_edges++;
      double color_pos = edge_cpos[i];
      png.line_blend(xs[a], ys[a], xs[b], ys[b], 0.0625,
                     r0 * (1 - color_pos) + r1 * color_pos,
                     g0 * (1 - color_pos) + g1 * color_pos,
                     b0 * (1 - color_pos) + b1 * color_pos);
      if (normal_edges % 10000 == 0) {
        std::cerr << "Drawing normal edges: " << normal_edges << "\r";
      }
    }
  }
  std::cerr << "Drawing normal edges: " << normal_edges << "\n";

  size_t normal_nodes = 0;
  for (size_t i = 0; i < N; i++) {
    if (erased[i])
      continue;
    if (!to_highlight[i]) {
      normal_nodes++;
      double color_pos = color_position[i];
      png.filledcircle(xs[i], ys[i], 0, r0 * (1 - color_pos) + r1 * color_pos,
                       g0 * (1 - color_pos) + g1 * color_pos,
                       b0 * (1 - color_pos) + b1 * color_pos);
    }
    if (normal_nodes % 10000 == 0) {
      std::cerr << "Drawing normal nodes: " << normal_nodes << "\r";
    }
  }
  std::cerr << "Drawing normal nodes: " << normal_nodes << "\n";

  double highlighted_r = 0.74117;
  double highlighted_g = 0.0;
  double highlighted_b = 0.69804;

  for (size_t i = 0; i < edges.size(); i++) {
    size_t a = edges[i].first;
    size_t b = edges[i].second;
    if (erased[a] || erased[b])
      continue;
    if (to_highlight[a] && to_highlight[b]) {
      highlighted_edges++;
      png.line_blend(xs[a], ys[a], xs[b], ys[b], 0.0625, highlighted_r,
                     highlighted_g, highlighted_b);
      if (highlighted_edges % 10000 == 0) {
        std::cerr << "Drawing highlighted edges: " << highlighted_edges << "\r";
      }
    }
  }
  std::cerr << "Drawing highlighted edges: " << highlighted_edges << "\n";
  size_t highlighted_nodes = 0;
  for (size_t i = 0; i < N; i++) {
    if (erased[i])
      continue;
    if (to_highlight[i]) {
      highlighted_nodes++;
      png.filledcircle(xs[i], ys[i], 0, highlighted_r, highlighted_g,
                       highlighted_b);
    }
    if (highlighted_nodes % 10000 == 0) {
      std::cerr << "Drawing highlighted nodes: " << highlighted_nodes << "\r";
    }
  }
  std::cerr << "Drawing highlighted nodes: " << highlighted_nodes << "\n";
  png.close();
}
