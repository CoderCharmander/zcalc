#include "text_field.h"

namespace gui {
void text_field::update(const keypad::keyset &keys) {
    for (uint8_t i = keypad::N0; i <= keypad::N9; ++i) {
        if (keys[i]) { insert_char(i - keypad::N0 + '0'); }
    }
    if (keys[keypad::DECIMAL]) insert_char('.');
    if (keys[keypad::BACKSPACE] && cur_pos > 0) {
        str.erase(str.begin() + cur_pos - 1);
        --cur_pos;
    }
    if (keys[keypad::LEFT] && cur_pos > 0) --cur_pos;
    if (keys[keypad::RIGHT] && cur_pos < str.size()) ++cur_pos;
}

constexpr uint8_t FONT_WIDTH = 6;
constexpr uint8_t FONT_HEIGHT = 9;

void text_field::render(u8g2_t *u8g2, uint16_t x, uint16_t y) {
    u8g2_SetFont(u8g2, u8g2_font_6x13_mf);
    u8g2_DrawStr(u8g2, x, y + FONT_HEIGHT, str.c_str());
    u8g2_SetDrawColor(u8g2, 2);
    u8g2_DrawBox(u8g2, x + FONT_WIDTH * cur_pos, y, 2, FONT_HEIGHT);
}
text_field::text_field(input_type t) : type(t), str(), cur_pos(0) {}
void text_field::clear() {
    cur_pos = 0;
    str.clear();
}
} // namespace gui