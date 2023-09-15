#pragma once
#include "../screen.h"

namespace screens::calculator {
void enter();
void update(u8g2_t *u8g2);

enum class fraction_format {
    APPROX_DECIMAL,
    EXACT_DECIMAL,
    RATIONAL,
};
struct output_format {
    fraction_format fformat;
};
void set_output_format(output_format const &format);
} // namespace screens::calculator