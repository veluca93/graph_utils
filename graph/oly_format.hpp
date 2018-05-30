#ifndef OLY_FORMAT
#define OLY_FORMAT
#include "graph.hpp"
#include <memory>

std::unique_ptr<Graph> ReadOLY(int options = GraphReadOptions::DEFAULT);
void WriteOLY(const Graph *g);

#endif
