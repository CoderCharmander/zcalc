#include "calculator.h"
#include "../keypad.h"
#include "../maths.h"
#include "cln/floatformat.h"
#include "cln/real_io.h"
#include "menu.h"
#include "u8g2.h"
#include <array>
#include <cln/output.h>
#include <pico/stdlib.h>
#include <sstream>
#include <string>

namespace screens::calculator {

uint cur_pos = 0;
constexpr uint8_t FONT_WIDTH = 6;
constexpr uint8_t FONT_HEIGHT = 9;
uint8_t cur_counter = 0;
bool cur_show = true;
std::string expr;
std::string result;
std::expected<cln::cl_R, const char *> result_number;
bool altmode = false;

void set_display(uint8_t arg) {}

screens::menu::menu_item display_opts_menu[] = {
    {"Exact fraction", false, set_display, fraction_format::RATIONAL},
    {"Exact decimal", false, set_display, fraction_format::EXACT_DECIMAL},
    {"Approximate decimal", false, set_display, fraction_format::APPROX_DECIMAL}
};


union output_data {
    struct {
        std::string a, b;
    } rational;
    struct {
        std::string num;
        // First decimal digit that is repeating, counted from the back. 0 is no repetition. 1 is the last digit.
        uint8_t first_repeating;
    } exact_decimal;
    struct {
        double num;
    } approx_decimal;
};


void enter() { curr_upd_fn = update; }

void insert_char(char c) {
    expr.insert(expr.begin() + cur_pos, c);
    ++cur_pos;
}

void handle_normkey(keypad::keyset pressed) {
    using namespace keypad;
    if (pressed[LEFT] && cur_pos > 0) { --cur_pos; }
    if (pressed[RIGHT] && cur_pos < expr.size()) { ++cur_pos; }

    for (auto i : (std::array<uint8_t, 15>{N0, N1, N2, N3, N4, N5, N6, N7, N8, N9, ADD, SUB, MUL, DIV, DECIMAL})) {
        if (pressed[i]) { insert_char(keychars[i]); }
    }
    if (pressed[BACKSPACE] && cur_pos > 0) {
        expr.erase(cur_pos - 1, 1);
        --cur_pos;
    }
    if (pressed[ENTER]) {
        const char *out;
        std::stringstream ss;
        auto r = math::evaluate(expr);
        result_number = r;
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
    if (pressed[MUL]) { insert_char('^'); }
    if (pressed[ADD]) { insert_char('('); }
    if (pressed[SUB]) { insert_char(')'); }
    if (pressed[N0]) { screens::menu::enter(); }
}

void update(u8g2_t *u8g2) {
    ++cur_counter;
    cur_counter %= 8;
    if (!cur_counter) { cur_show = !cur_show; }

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
    u8g2_DrawStr(u8g2, SCREEN_WIDTH - FONT_WIDTH * result.size(), SCREEN_HEIGHT, result.c_str());
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