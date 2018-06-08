#include "commands.hpp"
#include "graph.hpp"

namespace {
void Convert(CLI::App *app) {
  auto sub = app->add_subcommand("convert",
                                 "Converts between different graph formats");
  sub->set_callback([]() { Graph::Write(Graph::Read().get()); });
}
RegisterCommand r(Convert);
} // namespace
