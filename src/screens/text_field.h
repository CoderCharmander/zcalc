#pragma once
#include <string>
#include "../keypad.h"
#include "../screen.h"
namespace gui {

enum class input_type {
    text,
    number,
    whole,
};

class text_field {
  private:
    void insert_char(char c) {
        str.insert(str.begin() + cur_pos, c);
        ++cur_pos;
    }
  public:
    std::string str;
    size_t cur_pos;
    input_type type;

    void update(const keypad::keyset &keys);
    void render(u8g2_t *u8g2, uint16_t x, uint16_t y);
    explicit text_field(input_type t);
    void clear();
};

} // namespace gui
