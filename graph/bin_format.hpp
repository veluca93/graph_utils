#ifndef BIN_FORMAT
#define BIN_FORMAT
#include "graph.hpp"
#include <memory>

std::unique_ptr<Graph> ReadBIN(int options = GraphReadOptions::DEFAULT);
void WriteBIN(const Graph *g);

#endif
