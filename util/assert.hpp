#ifndef ASSERT_HPP
#define ASSERT_HPP
#include <stdexcept>

#define assert_e(expr)                                                         \
  if (!(expr)) {                                                               \
    throw std::runtime_error(#expr);                                           \
  }

#define assert_m(expr, msg)                                                    \
  if (!(expr)) {                                                               \
    throw std::runtime_error(std::string(msg) + ": " + std::string(#expr));    \
  }

#endif
