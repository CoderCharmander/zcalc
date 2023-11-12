#pragma once

#include <cln/cln.h>
#include <utility>
#include <vector>
#include "bytecode.h"

namespace zcas {

class ratio_eq {
  public:
    cln::cl_R constant;
    std::vector<std::pair<cln::cl_symbol, int8_t>> monomial;

    bytecode build(const std::vector<cln::cl_R> &definitions, size_t input, size_t output);
    cln::cl_R calculate(const std::vector<cln::cl_R> &definitions, size_t output);
};

} // namespace cas
