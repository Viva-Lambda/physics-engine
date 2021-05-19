// debugging utils
#pragma once

#include <external.hpp>

namespace vivaphysics {

#define D_CHECK_MSG(call, msg)                             \
  do {                                                     \
    bool res = call;                                       \
    if (!res) {                                            \
      std::stringstream txt;                               \
      txt << #call << " :: "                               \
          << "false"                                       \
          << " :: " << __FILE__ << " :: " << __LINE__      \
          << std::endl                                     \
          << msg << std::endl;                             \
      throw std::runtime_error(txt.str());                 \
    }                                                      \
  } while (0)

#define COMP_CHECK_MSG(call, el1, el2, msg)                \
  do {                                                     \
    bool res = call;                                       \
    auto el1v = el1;                                       \
    auto el2v = el2;                                       \
    if (!res) {                                            \
      std::stringstream txt;                               \
      txt << #call << " values: " << std::endl             \
          << #el1 << ": " << el1v << std::endl             \
          << #el2 << ": " << el2v << std::endl             \
          << " :: "                                        \
          << "false"                                       \
          << " :: " << __FILE__ << " :: " << __LINE__      \
          << " :: " << msg << std::endl;                   \
      throw std::runtime_error(txt.str());                 \
    }                                                      \
  } while (0)
};
