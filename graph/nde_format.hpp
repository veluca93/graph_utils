#ifndef NDE_FORMAT
#define NDE_FORMAT
#include "graph.hpp"
#include <memory>

std::unique_ptr<Graph> ReadNDE(int options = GraphReadOptions::DEFAULT);
void WriteNDE(const Graph *g);

#endif
