#include "CLI11.hpp"
#include "commands.hpp"
#include "flags.hpp"
#include "graph.hpp"

int main(int argc, char **argv) {
  using namespace flags;
  CLI::App app;
  app.require_subcommand();
  app.fallthrough();
  app.add_option("--input-file,-i,input_file", input_file,
                 "File to read the graph from");
  app.add_option("--output-file,-o,output_file", output_file,
                 "Which file the graph should be written to");
  app.add_set("--input-format,-f", input_format, Graph::ReadFormats(),
              "Format to be read from input (leave empty to autodetect)");
  app.add_set("--output-format,-F", output_format, Graph::WriteFormats(),
              "Format to be written to the output (leave empty to autodetect)");
  app.add_flag("--dont-populate-cache", dont_populate_cache,
               "Do not read memory mappings as soon as they are created");

  Commands::AddCommands(&app);

  CLI11_PARSE(app, argc, argv);
}
