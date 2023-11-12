//
// Created by umbreon on 11/10/23.
//

#include "bytecode.h"
#include <cln/complex.h>
#include <cln/exception.h>

namespace zcas {

bytecode::bytecode(uint8_t *bc, size_t bc_size, uint8_t register_count, uint8_t param_count, cln::cl_R *consts, uint8_t const_count)
    : code(bc, bc + bc_size), registers(register_count), parameter_count(param_count), constants(consts, consts + const_count) {}

std::optional<cln::cl_R> bytecode::evaluate(cln::cl_R *params) {
    cln::cl_R output;
    try {
        while (pc < code.size()) {
            uint8_t instr = code[pc++];
            uint8_t arg_count = (instr & 0xC0) >> 6;
            cln::cl_R *a = nullptr, *b = nullptr, *c = nullptr, *o;
            if (arg_count >= 1) { a = &fetch_data(output, params); }
            if (arg_count >= 2) { b = &fetch_data(output, params); }
            if (arg_count >= 3) { c = &fetch_data(output, params); }
            o = &fetch_data(output, params);

            switch (instr) {
            case ADD:
                *o = *a + *b;
                break;
            case SUB:
                *o = *a - *b;
                break;
            case MUL:
                *o = *a * *b;
                break;
            case DIV:
                *o = *a / *b;
                break;
            case EXP:
                *o = cln::realpart(cln::expt(*a, *b));
                break;
            case COPY:
                *o = *a;
                break;
            case NEG:
                *o = -*a;
                break;
            }
        }
    } catch (cln::runtime_exception &e) { return {}; }
    return output;
}

// Data ID:
// 0..63 registers
// 64..127 parameters
// 128..191 constants
// 255 output
cln::cl_R &bytecode::fetch_data(cln::cl_R &output, cln::cl_R *params) {
    uint8_t data_spec = code[pc++];
    if (data_spec == 255) { return output; }

    cln::cl_R *source = nullptr;
    switch ((data_spec & 0xC0) >> 6) {
    case 0:
        source = registers.data();
        break;
    case 1:
        source = params;
        break;
    case 2:
        source = constants.data();
        break;
    }
    return *(source + (data_spec & 0x3f));
}

} // namespace zcas