#include "flags.hpp"

namespace flags {

std::string input_file;
std::string output_file;
std::string input_format;
std::string output_format;
#if __linux__
bool dont_populate_cache;
#endif

} // namespace flags
