#ifndef BEL_FORMAT
#define BEL_FORMAT
#include "graph.hpp"
#include <memory>

std::unique_ptr<Graph> ReadBEL(int options = GraphReadOptions::DEFAULT);
void WriteBEL(const Graph *g);

#endif
