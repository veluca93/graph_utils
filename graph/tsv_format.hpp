#ifndef TSV_FORMAT
#define TSV_FORMAT
#include "graph.hpp"
#include <memory>

std::unique_ptr<Graph> ReadTSV(int options = GraphReadOptions::DEFAULT);
void WriteTSV(const Graph *g);

#endif
