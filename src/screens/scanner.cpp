#include "scanner.h"
#include "../keypad.h"
#include "../wireless.h"
#include "calculator.h"
#include <array>
#include <string>
#include <variant>

namespace screens::scanner {

bool invalidated = false;

void enter() {
    curr_upd_fn = update;
    invalidated = true;
}

using image = std::array<uint8_t, 128 * 64 / 8>;

std::variant<std::string, image> message = "Press ENTER to scan";

void update(u8g2_t *u8g2) {
    keypad::main_kp.update();
    auto pressed = keypad::main_kp.get_pressed();
    if (pressed[keypad::ENTER]) { bluetooth::notify_if_conn(); }
    if (pressed[keypad::BACKSPACE]) { calculator::enter(); }
    if (invalidated) {
        u8g2_ClearBuffer(u8g2);
        if (std::holds_alternative<std::string>(message)) {
            u8g2_SetFont(u8g2, u8g2_font_6x13_mr);
            u8g2_DrawStr(u8g2, 0, 38, std::get<std::string>(message).c_str());
        } else {
            u8g2_DrawXBM(u8g2, 0, 0, 128, 64, std::get<image>(message).data());
        }
        u8g2_SendBuffer(u8g2);
        invalidated = false;
    }
}
void set_message(char *buf, uint16_t len) {
    message = std::string(buf, buf + len);
    invalidated = true;
}

uint16_t image_buf_position = 0;

void set_image(uint8_t *buf, uint16_t buf_size) {
    static image img;
    if (buf_size > 1024) {
        buf += 1024 * (buf_size / 1024);
        buf_size %= 1024;
    }
    uint16_t first_copy_len = std::min(buf_size, uint16_t(1024 - image_buf_position));
    std::copy(buf, buf + first_copy_len, img.begin() + image_buf_position);
    if (buf_size > 1024 - image_buf_position) { std::copy(buf + buf_size - 1024 + image_buf_position, buf + buf_size, img.begin()); }
    image_buf_position += buf_size;
    image_buf_position %= 1024;
    message = img;
    invalidated = true;
}

void reset_image() {
    image_buf_position = 0;
}

} // namespace screens::scanner