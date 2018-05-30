#include "graph_io.hpp"
#include "strtk.hpp"
#include <map>

DEFINE_string(input_format, "",
              "Format to be read from input (leave empty to autodetect)");
DEFINE_string(output_format, "",
              "Format to be written to the output (leave empty to autodetect)");

std::map<std::string, std::function<std::unique_ptr<Graph>(int)>> readers{
    {"nde", ReadNDE},
    {"tsv", ReadTSV},
    {"oly", ReadOLY},
    {"el", ReadEL},
    {"bin", ReadBIN}};
std::map<std::string, std::function<void(const Graph *g)>> writers{
    {"nde", WriteNDE},
    {"tsv", WriteTSV},
    {"oly", WriteOLY},
    {"el", WriteEL},
    {"bin", WriteBIN}};

static bool ValidateInputFormat(const char *flagname,
                                const std::string &value) {
  if (value == "")
    return true;
  if (readers.count(value))
    return true;
  std::cout << "Invalid value for --" << flagname << ": " << value << std::endl;
  return false;
}
static bool ValidateOutputFormat(const char *flagname,
                                 const std::string &value) {
  if (value == "")
    return true;
  if (writers.count(value))
    return true;
  std::cout << "Invalid value for --" << flagname << ": " << value << std::endl;
  return false;
}
DEFINE_validator(input_format, &ValidateInputFormat);
DEFINE_validator(output_format, &ValidateOutputFormat);

std::unique_ptr<Graph> ReadGraph(int options) {
  std::string format = FLAGS_input_format;
  if (format.empty()) {
    for (const auto &kv : readers) {
      if (strtk::ends_with(kv.first, FLAGS_input_file)) {
        format = kv.first;
        break;
      }
    }
  }
  assert_m(!format.empty(), "Could not detect input format");
  assert_m(readers.count(format), "Invalid input format");
  return readers[format](options);
}
void WriteGraph(const Graph *g) {
  std::string format = FLAGS_output_format;
  if (format.empty()) {
    for (const auto &kv : writers) {
      if (strtk::ends_with(kv.first, FLAGS_output_file)) {
        format = kv.first;
        break;
      }
    }
  }
  assert_m(!format.empty(), "Could not detect output format");
  assert_m(writers.count(format), "Invalid output format");
  return writers[format](g);
}
