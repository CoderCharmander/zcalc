#include "calculator.h"
#include "cln/floatformat.h"
#include "cln/real_io.h"
#include <cln/output.h>
#include "u8g2.h"
#include "../keypad.h"
#include "../maths.h"
#include <array>
#include <pico/stdlib.h>
#include <string>
#include <sstream>

namespace screens::calculator {

uint cur_pos = 3;
constexpr uint8_t FONT_WIDTH = 6;
constexpr uint8_t FONT_HEIGHT = 9;
uint8_t cur_counter = 0;
bool cur_show = true;
std::string expr = "3+3=6";
std::string result = "";
bool altmode = false;

void insert_char(char c) {
    expr.insert(expr.begin() + cur_pos, c);
    ++cur_pos;
}

void handle_normkey(keypad::keyset pressed) {
    using namespace keypad;
    if (pressed[LEFT] && cur_pos > 0) --cur_pos;
    if (pressed[RIGHT] && cur_pos < expr.size()) ++cur_pos;

    for (auto i: (std::array<uint8_t, 15>{N0, N1, N2, N3, N4, N5, N6, N7, N8, N9, ADD, SUB, MUL, DIV, DECIMAL})) {
        if (pressed[i]) {
            insert_char(keypad::keychars[i]);
        }
    }
    if (pressed[BACKSPACE]&&cur_pos > 0) {
        expr.erase(cur_pos - 1, 1);
        --cur_pos;
    }
    if (pressed[ENTER]) {
        const char* out;
        std::stringstream ss;
        auto r = math::evaluate(expr);
        if (!r) out = r.error();
        else {
            extern cln::cl_print_flags default_print_flags;
            cln::print_real(ss, cln::default_print_flags, *r);
            out = ss.str().c_str();
        }
        result = out;
    }
}
void handle_altkey(keypad::keyset pressed) {
    using namespace keypad;
    if (pressed[MUL]) insert_char('^');
    if (pressed[ADD]) insert_char('(');
    if (pressed[SUB]) insert_char(')');
}

void update(u8g2_t *u8g2) {
    ++cur_counter;
    cur_counter %= 8;
    if (!cur_counter) {
        cur_show = !cur_show;
    }
    

    keypad::main_kp.update();
    auto pressed = keypad::main_kp.get_pressed();
    if (pressed[keypad::ALT]) {
        altmode = !altmode;
    } else if (altmode && pressed.any()) {
        // alt mode goes here
        handle_altkey(pressed);
        altmode = false;
    } else {
        handle_normkey(pressed);
    }

    u8g2_ClearBuffer(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_6x13_mf);
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_DrawStr(u8g2, 0, FONT_HEIGHT, expr.c_str());
    u8g2_DrawStr(u8g2, 128 - FONT_WIDTH * result.size(), 64, result.c_str());
    if (cur_show) {
        u8g2_SetDrawColor(u8g2, 2);
        u8g2_DrawBox(u8g2, cur_pos * FONT_WIDTH, 0, 2, FONT_HEIGHT);
    }
    if (altmode) {
        u8g2_SetFont(u8g2, u8g2_font_u8glib_4_hr);
        u8g2_DrawStr(u8g2, 0, 64, "ALT");
    }
    u8g2_SendBuffer(u8g2);
}
} // namespace screens::calculator