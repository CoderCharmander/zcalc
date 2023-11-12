#pragma once

#include "../cmake-build-debug/_deps/cln-src/src/real/cl_R.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace zcas {

class bytecode {
  private:
    std::vector<uint8_t> code;
    uint8_t parameter_count;
    size_t pc;
    std::vector<cln::cl_R> registers;
    std::vector<cln::cl_R> constants;
    cln::cl_R &fetch_data(cln::cl_R &output, cln::cl_R *params);

  public:
    static constexpr uint8_t ARG_COUNT_0 = 0;
    static constexpr uint8_t ARG_COUNT_1 = 0x40;
    static constexpr uint8_t ARG_COUNT_2 = 0x80;
    static constexpr uint8_t ARG_COUNT_3 = 0xC0;
    enum {
        NEG = ARG_COUNT_1 | 0,
        COPY,
        ADD = ARG_COUNT_2 | 0,
        SUB,
        MUL,
        DIV,
        EXP,
    };

    using data_id = uint8_t;

    std::optional<cln::cl_R> evaluate(cln::cl_R *params);

    bytecode(uint8_t *bc, size_t bc_size, uint8_t register_count, uint8_t param_count, cln::cl_R *consts, uint8_t const_count);
};

}