#ifndef GRAPH_IO_HPP
#define GRAPH_IO_HPP
#include "bin_format.hpp"
#include "el_format.hpp"
#include "nde_format.hpp"
#include "oly_format.hpp"
#include "tsv_format.hpp"

std::unique_ptr<Graph> ReadGraph(int options = GraphReadOptions::DEFAULT);
void WriteGraph(const Graph *g);

#endif
