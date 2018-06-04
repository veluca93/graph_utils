#include "common_defs.hpp"
#include "io.hpp"
#include <gflags/gflags.h>
#include <gflags/gflags_completions.h>
#include <iomanip>
#include <random>

DEFINE_string(drawing, "", "Folder where the drawing info has been saved");

DEFINE_string(degeneracy, "",
              "Folder where the degeneracy info has been saved");

DEFINE_string(output, "output.ppm", "Where the produced image is saved");
DEFINE_string(nodes_output, "",
              "Where the number of nodes in this frame is saved");
DEFINE_string(highlight, "",
              "File containing the nodes that should be highlighted");
DEFINE_uint64(degen_thresh, 0,
              "Mark as to-be-deleted nodes with less then this degeneracy");
DEFINE_double(fraction_to_delete, 0.0,
              "How many to-be-deleted nodes should actually be deleted");

const constexpr size_t block_size = 1;
const constexpr int img_size = image_size;

struct __attribute__((packed)) pixel_draw {
  int x;
  int y;
  uint8_t opacity;
  uint8_t r, g, b;
};

constexpr size_t coord(int x, int y) {
  // TODO change layout?
  return img_size * 3 * x + 3 * y;
}

constexpr size_t max_coord(int image_size) {
  return coord((image_size + block_size - 1) / block_size * block_size,
               (image_size + block_size - 1) / block_size * block_size);
}

uint8_t img[max_coord(image_size)];

void draw_point(int x, int y, uint32_t opacity, uint8_t r, uint8_t g,
                uint8_t b) {
  size_t base_coord = coord(x, y);
  img[base_coord] = ((255 - opacity) * img[base_coord] + opacity * r) / 255;
  img[base_coord + 1] =
      ((255 - opacity) * img[base_coord + 1] + opacity * g) / 255;
  img[base_coord + 2] =
      ((255 - opacity) * img[base_coord + 2] + opacity * b) / 255;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t opacity, uint8_t r,
               uint8_t g, uint8_t b) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, e2;

  for (;;) {
    draw_point(x0, y0, opacity, r, g, b);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

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
    size_t left_nodes = 0;
    Counter cnt("Erasing nodes");
    for (size_t i = 0; i < N; i++) {
      if (i < FLAGS_fraction_to_delete * first_to_keep) {
        erased[perm[i]] = true;
        cnt++;
      } else {
        left_nodes++;
      }
    }
    if (!FLAGS_nodes_output.empty()) {
      ChangeOutputFile chg(FLAGS_nodes_output);
      write(left_nodes, '\n');
    }
  }

  uint8_t r0 = 0x00;
  uint8_t g0 = 0x04;
  uint8_t b0 = 0x28;
  uint8_t r1 = 0x00;
  uint8_t g1 = 0x4f;
  uint8_t b1 = 0x92;

  MemoryMappedFile xs_mmf(FLAGS_drawing + xs_file);
  const int *xs = xs_mmf.data<int>();
  MemoryMappedFile ys_mmf(FLAGS_drawing + ys_file);
  const int *ys = ys_mmf.data<int>();
  MemoryMappedFile color_position_mmf(FLAGS_drawing + color_position_file);
  const uint8_t *color_position = color_position_mmf.data<uint8_t>();
  MemoryMappedFile edge_info_mmf(FLAGS_drawing + edge_info_file);
  const span<const edge_info_t> edge_info = edge_info_mmf.span<edge_info_t>();

  std::fill((uint8_t *)img, (uint8_t *)img + sizeof(img), 255);
  {
    Counter cnt("Drawing normal nodes");
    for (size_t i = 0; i < N; i++) {
      if (erased[i])
        continue;
      if (!to_highlight[i]) {
        uint32_t color_pos = color_position[i];
        uint8_t rc = (r0 * (255 - color_pos) + r1 * color_pos) / 255;
        uint8_t gc = (g0 * (255 - color_pos) + g1 * color_pos) / 255;
        uint8_t bc = (b0 * (255 - color_pos) + b1 * color_pos) / 255;
        draw_point(xs[i], ys[i], 255, rc, gc, bc);
        cnt++;
      }
    }
  }
  {
    Counter cnt("Drawing normal edges");
    for (const edge_info_t &e : edge_info) {
      size_t a = e.a;
      size_t b = e.b;
      if (erased[a] || erased[b])
        continue;
      if (!(to_highlight[a] && to_highlight[b])) {
        uint32_t color_pos = e.cpos;
        uint8_t rc = (r0 * (255 - color_pos) + r1 * color_pos) / 255;
        uint8_t gc = (g0 * (255 - color_pos) + g1 * color_pos) / 255;
        uint8_t bc = (b0 * (255 - color_pos) + b1 * color_pos) / 255;
        draw_line(xs[a], ys[a], xs[b], ys[b], 16, rc, gc, bc);
        cnt++;
      }
    }
  }

  uint8_t highlighted_r = 0xbd;
  uint8_t highlighted_g = 0x00;
  uint8_t highlighted_b = 0xa2;

  {
    Counter cnt("Drawing highlighted nodes");
    for (size_t i = 0; i < N; i++) {
      if (erased[i])
        continue;
      if (to_highlight[i]) {
        draw_point(xs[i], ys[i], 1, highlighted_r, highlighted_g,
                   highlighted_b);
        cnt++;
      }
    }
  }

  {
    Counter cnt("Drawing highlighted edges");
    for (const edge_info_t &e : edge_info) {
      size_t a = e.a;
      size_t b = e.b;
      if (erased[a] || erased[b])
        continue;
      if (to_highlight[a] && to_highlight[b]) {
        draw_line(xs[a], ys[a], xs[b], ys[b], 16, highlighted_r, highlighted_g,
                  highlighted_b);
        cnt++;
      }
    }
  }
  ChangeOutputFile chg(FLAGS_output);
  write("P6\n", (int)image_size, ' ', (int)image_size, "\n255\n");
  {
    Counter cnt("Writing pixels");
    for (size_t x = 0; x < (size_t)image_size; x++) {
      for (size_t y = 0; y < (size_t)image_size; y++) {
        size_t base_coord = coord(x, y);
        write_bytes(img[base_coord]);
        write_bytes(img[base_coord + 1]);
        write_bytes(img[base_coord + 2]);
      }
    }
  }
}
