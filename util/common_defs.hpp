#ifndef COMMON_DEFS
#define COMMON_DEFS
#include <stdlib.h>

const char degen_order_file[] = "/degen_order.dat";
const char rev_degen_order_file[] = "/rev_degen_order.dat";
const char kcoresize_file[] = "/kcoresize.dat";

const double image_size = 2160;

const char n_file[] = "/n.dat";
const char xs_file[] = "/xs.dat";
const char ys_file[] = "/ys.dat";
const char color_position_file[] = "/color_position.dat";
const char edge_info_file[] = "/edge_info.dat";

struct __attribute__((packed)) edge_info_t {
  size_t a;
  size_t b;
  double cpos;
};

#endif
