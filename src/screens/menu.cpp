#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-raw-string-literal"
#include "menu.h"
#include "calculator.h"
#include "rateqsol.h"
#include "scanner.h"

namespace screens::menu {

menu *curr_menu = &main_menu;

menu_item main_menu_items[]{
    {"Fraction mode", true, [](uint8_t arg) { set_submenu(calculator::disp_menu); }},
    {"Open scanner", true, [](uint8_t arg) { scanner::enter(); }},
    {"RatEqSol", true, [](uint8_t arg) { set_submenu(rateqsol::eqs_menu); }},
    {"Test menu item", false, nullptr},
    {"Test menu item", true, nullptr},
    {"Test menu item", true, nullptr},
};

size_t cur_pos;
size_t cur_offset;

constexpr int ITEM_HEIGHT = 16;
constexpr int ITEM_TEXT_POS = 13;
constexpr size_t MAX_ITEMS = SCREEN_HEIGHT / ITEM_HEIGHT;
constexpr int SUBMENU_ARROW_POS_X = -12;
constexpr int SUBMENU_ARROW_POS_Y = 12;
constexpr const char *SUBMENU_ARROW_STRING = "\x4e";
#define SUBMENU_ARROW_FONT u8g2_font_open_iconic_arrow_1x_t

menu main_menu(main_menu_items, sizeof(main_menu_items) / sizeof(*main_menu_items));

void update(u8g2_t *u8g2) {
    keypad::main_kp.update();
    auto pressed = keypad::main_kp.get_pressed();
    if (pressed[keypad::BACKSPACE]) {
        if (curr_menu->super_menu) {
            set_menu(**curr_menu->super_menu);
        } else {
            screens::calculator::enter();
        }
        return;
    }
    if (pressed[keypad::LEFT]) {
        if (cur_pos > 1 || cur_offset == 0 && cur_pos > 0) {
            --cur_pos;
        } else if (cur_pos <= 1 && cur_offset > 0) {
            --cur_offset;
        }
    }
    if (pressed[keypad::RIGHT]) {
        if (cur_pos < MAX_ITEMS - 2 || cur_offset == curr_menu->size - MAX_ITEMS && cur_pos < MAX_ITEMS - 1) {
            ++cur_pos;
        } else if (cur_pos >= MAX_ITEMS - 2 && cur_offset < curr_menu->size - MAX_ITEMS) {
            ++cur_offset;
        }
    }
    if (cur_pos + cur_offset >= curr_menu->size) { cur_pos = curr_menu->size - 1 - cur_offset; }

    if (pressed[keypad::ENTER]) {
        if (curr_menu->items[cur_offset + cur_pos].action) {
            curr_menu->items[cur_offset + cur_pos].action(curr_menu->items[cur_offset + cur_pos].action_param);
        }
    }

    u8g2_ClearBuffer(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_6x13_tf);
    for (int i = 0; i < std::min(MAX_ITEMS, curr_menu->size - cur_offset); ++i) {
        if (i == cur_pos) {
            u8g2_SetDrawColor(u8g2, 1);
            u8g2_DrawBox(u8g2, 0, ITEM_HEIGHT * i, SCREEN_WIDTH, ITEM_HEIGHT);
            u8g2_SetDrawColor(u8g2, 0);
        } else {
            u8g2_SetDrawColor(u8g2, 1);
        }
        u8g2_DrawStr(u8g2, 2, ITEM_HEIGHT * i + ITEM_TEXT_POS, curr_menu->items[cur_offset + i].title);
        if (curr_menu->items[cur_offset + i].submenu) {
            u8g2_SetFont(u8g2, SUBMENU_ARROW_FONT);
            u8g2_DrawStr(u8g2, SCREEN_WIDTH + SUBMENU_ARROW_POS_X, ITEM_HEIGHT * i + SUBMENU_ARROW_POS_Y, SUBMENU_ARROW_STRING);
            u8g2_SetFont(u8g2, u8g2_font_6x13_tf);
        }
    }
    u8g2_SendBuffer(u8g2);
}

void enter() { curr_upd_fn = update; }

void set_menu(menu &menu) { curr_menu = &menu; }

void set_submenu(menu &menu) {
    menu.super_menu = curr_menu;
    curr_menu = &menu;
}

} // namespace screens::menu
#pragma clang diagnostic pop