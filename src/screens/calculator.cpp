#include "calculator.h"
#include "../keypad.h"
#include "cas.h"
#include "cln/floatformat.h"
#include "cln/object.h"
#include "cln/rational.h"
#include "cln/real_io.h"
#include "menu.h"
#include "u8g2.h"
#include <array>
#include <cln/output.h>
#include <pico/stdlib.h>
#include <sstream>
#include <string>
#include <variant>

namespace screens::calculator {

uint cur_pos = 0;
constexpr uint8_t FONT_WIDTH = 6;
constexpr uint8_t FONT_HEIGHT = 9;
uint8_t cur_counter = 0;
bool cur_show = true;
std::string expr;
std::string result;
std::expected<cln::cl_R, const char *> result_number(std::unexpected{""});
bool altmode = false;

namespace output {

struct rational {
    std::string a, b;
};

struct exact_decimal {
    std::string a;
    // First decimal digit that is repeating, counted from the back. 0 is no repetition. 1 is the last digit.
    uint8_t first_repeating;
};

struct approx_decimal {
    std::string a;
};

struct error {
    const char *msg;
};

struct empty {};

using data = std::variant<rational, exact_decimal, approx_decimal, error, empty>;

} // namespace output

output::data out_data{output::empty{}};
output_format out_format{RATIONAL};

// dear CLN, please expose an API for this
inline bool is_float(const cln::cl_R &x) {
    printf("IF \n");
    if (!x.pointer_p()) {
        if (x.nonpointer_tag() == cl_SF_tag) { return true; }
    } else if (x.pointer_type()->flags & cl_class_flags_subclass_float) {
        printf("NPtr\n");
        return true;
    }
    printf("Exit\n");
    return false;
}

inline std::string to_string(const cln::cl_I &x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

void set_fraction_format(uint8_t arg) {
    out_format.fformat = (fraction_format)arg;
    printf("SFF \n");
    if (!result_number.has_value()) {
        printf("SFF print error\n");
        out_data = output::error{result_number.error()};
        return;
    }
    printf("SFF number\n");
    if (is_float(*result_number) || arg == APPROX_DECIMAL) {
        printf("SFF float\n");
        double d = cln::double_approx(*result_number);
        char out[16];
        snprintf(out, sizeof(out), "%.8f", d);
        out_data = output::approx_decimal{std::to_string(cln::double_approx(*result_number))};
        return;
    }
    printf("SFF rat\n");
    cln::cl_RA r = As(cln::cl_RA)(*result_number);
    printf("SFF conv done\n");
    cln::cl_I n = cln::numerator(r), d = cln::denominator(r);
    if (d == 1) {
        out_data = output::approx_decimal{to_string(n)};
        return;
    }
    output::rational thing;
    switch (arg) {
    case RATIONAL:
        printf("SFF rat\n");
        thing = output::rational{to_string(n), to_string(d)};
        out_data = thing;
        printf("SFF rat ok %s %s\n", thing.a.c_str(), thing.b.c_str());
        break;
    case EXACT_DECIMAL:
        printf("SFF exdec\n");
        // TODO magic
        out_data = output::approx_decimal{std::to_string(cln::double_approx(*result_number))};
        break;
    }
}

screens::menu::menu_item display_opts_menu[] = {{"Exact fraction", false, set_fraction_format, fraction_format::RATIONAL},
                                                {"Exact decimal", false, set_fraction_format, fraction_format::EXACT_DECIMAL},
                                                {"Approximate decimal", false, set_fraction_format, fraction_format::APPROX_DECIMAL}};

menu::menu disp_menu(display_opts_menu, 3);
void enter() { curr_upd_fn = update; }

void insert_char(char c) {
    expr.insert(expr.begin() + cur_pos, c);
    ++cur_pos;
}

void set_expr(const char* s) {
    expr = s;
    cur_pos = expr.size();
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
        auto r = zcas::evaluate(expr);
        result_number = r;
        printf("Eval complete \n");
        set_fraction_format(out_format.fformat);
        printf("SFF complete \n");
    }
}
void handle_altkey(keypad::keyset pressed) {
    using namespace keypad;
    if (pressed[MUL]) { insert_char('^'); }
    if (pressed[ADD]) { insert_char('('); }
    if (pressed[SUB]) { insert_char(')'); }
    if (pressed[N0]) { screens::menu::enter(); }
    if (pressed[BACKSPACE]) {
        expr.clear();
        cur_pos = 0;
    }
    if (pressed[ENTER]) {
        expr.insert(expr.begin(), '(');
        expr += ')';
        cur_pos = expr.size();
    }
    if (pressed[LEFT]) { cur_pos = 0; }
    if (pressed[RIGHT]) { cur_pos = expr.size(); }
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
    if (cur_show) {
        u8g2_SetDrawColor(u8g2, 2);
        u8g2_DrawBox(u8g2, cur_pos * FONT_WIDTH, 0, 2, FONT_HEIGHT);
    }
    if (altmode) {
        u8g2_SetFont(u8g2, u8g2_font_u8glib_4_hr);
        u8g2_DrawStr(u8g2, 0, 64, "ALT");
    }
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_6x13_mf);
    if (std::holds_alternative<output::error>(out_data)) {
        output::error err = std::get<output::error>(out_data);
        u8g2_DrawStr(u8g2, SCREEN_WIDTH - FONT_WIDTH * strlen(err.msg), SCREEN_HEIGHT - 3, err.msg);
    } else if (std::holds_alternative<output::rational>(out_data)) {
        output::rational rat = std::get<output::rational>(out_data);
        // Draw a fraction nicely. The smaller number is centered.
        unsigned int max = std::max(rat.a.size(), rat.b.size());
        u8g2_DrawStr(u8g2, SCREEN_WIDTH - FONT_WIDTH * rat.b.size() - FONT_WIDTH * (max - rat.b.size()) / 2, SCREEN_HEIGHT, rat.b.c_str());
        u8g2_DrawHLine(u8g2, SCREEN_WIDTH - FONT_WIDTH * max, SCREEN_HEIGHT - FONT_HEIGHT - 2, FONT_WIDTH * max);
        u8g2_DrawStr(u8g2, SCREEN_WIDTH - FONT_WIDTH * rat.a.size() - FONT_WIDTH * (max - rat.a.size()) / 2,
                     SCREEN_HEIGHT - FONT_HEIGHT - 4, rat.a.c_str());
    } else if (std::holds_alternative<output::approx_decimal>(out_data)) {
        output::approx_decimal app = std::get<output::approx_decimal>(out_data);
        u8g2_DrawStr(u8g2, SCREEN_WIDTH - FONT_WIDTH * app.a.size(), SCREEN_HEIGHT, app.a.c_str());
    }
    u8g2_SendBuffer(u8g2);
}
} // namespace screens::calculator