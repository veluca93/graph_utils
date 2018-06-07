#ifndef GRAPH_IO_HPP
#define GRAPH_IO_HPP
#include "graph.hpp"

std::unique_ptr<Graph> ReadGraph(int options = GraphReadOptions::DEFAULT);
void WriteGraph(const Graph *g);

#endif
