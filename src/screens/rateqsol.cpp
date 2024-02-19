#include "rateqsol.h"
#include "calculator.h"
#include "ratio_eq.h"
#include "text_field.h"
#include <sstream>

namespace screens::rateqsol {

uint8_t curr_const;
uint8_t curr_eq;
gui::text_field input(gui::input_type::number);
std::vector<std::string> consts;
std::vector<cln::cl_R> reals;

void update(u8g2_t *u8g2);
extern zcas::ratio_eq equations[];

void solvinator(uint8_t eq) {
    curr_eq = eq;
    curr_const = 0;
    curr_upd_fn = update;
    consts.clear();
    consts.resize(equations[eq].monomial.size());
}

bool rigorize() {
    for (int i = 0; i < reals.size(); ++i) {
        try {
            if (!consts[i].empty()) { reals[i] = consts[i].c_str(); }
        } catch (cln::read_number_bad_syntax_exception &e) { return false; }
    }
    return true;
}

menu::menu_item eq_menuitems[] = {
    {"R = U/I", false, solvinator, 0},
    {"F = k * Q1*Q2 / r^2", false, solvinator, 1},
    {"F = ma", false, solvinator, 2}};

zcas::ratio_eq equations[] = {
    {1, {{cln::cl_symbol{"U"}, 1}, {cln::cl_symbol{"I"}, -1}, {cln::cl_symbol{"R"}, -1}}},
    {"9000000000", {{cln::cl_symbol{"Q1"}, 1}, {cln::cl_symbol{"Q2"}, 1}, {cln::cl_symbol{"r"}, -2}, {cln::cl_symbol{"F"}, -1}}},
    {1, {{cln::cl_symbol{"m"}, 1}, {cln::cl_symbol{"a"}, 1}, {cln::cl_symbol{"F"}, -1}}}};

menu::menu eqs_menu(eq_menuitems, sizeof(eq_menuitems) / sizeof(eq_menuitems[0]));

void update(u8g2_t *u8g2) {
    keypad::main_kp.update();
    auto pressed = keypad::main_kp.get_pressed();
    auto held = keypad::main_kp.get_keys();
    if (pressed[keypad::ALT]) {
        calculator::enter();
        return;
    }
    zcas::ratio_eq &eq = equations[curr_eq];

    if (pressed[keypad::ADD]) {
        consts[curr_const] = input.str;
        if (rigorize()) {
            --curr_const;
            curr_const += consts.size();
            curr_const %= consts.size();
            input.clear();
            input.str = consts[curr_const];
        }
    }
    if (pressed[keypad::SUB]) {
        consts[curr_const] = input.str;
        if (rigorize()) {
            ++curr_const;
            curr_const += consts.size();
            curr_const %= consts.size();
            input.clear();
            input.str = consts[curr_const];
        }
    }
    if (pressed[keypad::DIV]) {
        consts[curr_const] = input.str;
        if (rigorize()) {
            input.clear();
            std::stringstream ss;
            ss << eq.calculate(reals, curr_const);
            calculator::set_expr(ss.str().c_str());
            calculator::enter();
        }
    }

    input.update(pressed);
    u8g2_ClearBuffer(u8g2);
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_4x6_mf);
    uint8_t tw = 4 * (strlen(eq_menuitems[curr_eq].title));
    char ask[cln::cl_string(eq.monomial[curr_const].first).size() + 3];
    sprintf(ask, "%s =", cln::cl_string(eq.monomial[curr_const].first).asciz());
    u8g2_DrawStr(u8g2, 0, 6, ask);
    u8g2_DrawBox(u8g2, SCREEN_WIDTH - tw, 0, tw, 8);
    input.render(u8g2, 0, 9);
    u8g2_SetDrawColor(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_4x6_mf);
    u8g2_DrawStr(u8g2, SCREEN_WIDTH - tw, 6, eq_menuitems[curr_eq].title);

    u8g2_DrawStr(u8g2, 0, SCREEN_HEIGHT, "<Prev");
    u8g2_DrawStr(u8g2, SCREEN_WIDTH / 4, SCREEN_HEIGHT, "Next>");
    u8g2_DrawStr(u8g2, SCREEN_WIDTH / 2, SCREEN_HEIGHT, "Solve For");
    u8g2_SendBuffer(u8g2);
}

} // namespace screens::rateqsol