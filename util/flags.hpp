#ifndef FLAGS_HPP
#define FLAGS_HPP
#include <string>

namespace flags {

extern std::string input_file;
extern std::string output_file;
extern std::string input_format;
extern std::string output_format;
#if __linux__
extern bool dont_populate_cache;
#endif

} // namespace flags

#endif
