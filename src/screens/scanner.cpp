#include "scanner.h"
#include "../keypad.h"
#include "../wireless.h"
#include "calculator.h"
#include <string>

namespace screens::scanner {

void enter() { curr_upd_fn = update; }

std::string message = "Press ENTER to scan";

void update(u8g2_t *u8g2) {
    keypad::main_kp.update();
    auto pressed = keypad::main_kp.get_pressed();
    if (pressed[keypad::ENTER]) { bluetooth::notify_if_conn(); }
    if (pressed[keypad::BACKSPACE]) { calculator::enter(); }
    u8g2_ClearBuffer(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_6x13_mr);
    u8g2_DrawStr(u8g2, 0, 38, message.c_str());
    u8g2_SendBuffer(u8g2);
}
void set_message(char* buf, uint16_t len) {
    message.assign(buf, buf + len);
}

} // namespace screens::scanner