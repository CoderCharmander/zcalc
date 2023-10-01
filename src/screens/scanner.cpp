#include "scanner.h"
#include "../keypad.h"
#include <string>

namespace screens::scanner {

void enter() {
    curr_upd_fn = update;
}

std::string message = "= to scan";

void update(u8g2_t *u8g2) {
    keypad::main_kp.update();
    auto pressed = keypad::main_kp.get_pressed();
}

}