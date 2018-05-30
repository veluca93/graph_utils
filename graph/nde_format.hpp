#ifndef NDE_FORMAT
#define NDE_FORMAT
#include "graph.hpp"
#include <memory>
#include <stdio.h>

std::unique_ptr<Graph> ReadNDE(int options = GraphReadOptions::DEFAULT);
void WriteNDE(const Graph *g);

#endif
