#ifndef EL_FORMAT
#define EL_FORMAT
#include "graph.hpp"
#include <memory>

std::unique_ptr<Graph> ReadEL(int options = GraphReadOptions::DEFAULT);
void WriteEL(const Graph *g);

#endif
