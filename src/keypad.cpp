#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <stdio.h>

#include "keypad.h"

namespace keypad {
keypad::keypad(int _rows, int _cols, const uint8_t *_row_pins, const uint8_t *_col_pins, const key *_key_map) {
    row_pins.resize(_rows);
    col_pins.resize(_cols);
    for (int i = 0; i < _rows; ++i) {
        row_pins[i] = _row_pins[i];
    }
    for (int i = 0; i < _cols; ++i) {
        col_pins[i] = _col_pins[i];
    }
    for (auto r: row_pins) {
        gpio_init(r);
        gpio_set_dir(r, GPIO_IN);
        gpio_pull_up(r);
    }
    for (auto c: col_pins) {
        gpio_init(c);
        gpio_set_dir(c, GPIO_OUT);
        gpio_put(c, 1);
    }

    key_map.resize(_rows * _cols);
    for (int i = 0; i < _rows * _cols; ++i) {
        key_map[i] = _key_map[i];
    }
}

void keypad::update() {
    std::bitset<key::NONE> keys{0};
    for (int c = 0; c < col_pins.size(); ++c) {
        gpio_put(col_pins[c], 0);
        for (int r = 0; r < row_pins.size(); ++r) {
            keys[key_map[r * col_pins.size() + c]] = !gpio_get(row_pins[r]);
            //printf("%d", state);
        }
        gpio_put(col_pins[c], 1);
        //printf("\n");
    }
    //printf("\n");
    prev_keys = curr_keys;
    curr_keys = keys;
}

keypad main_kp(ROWS, COLUMNS, DEFAULT_MAIN_KP_ROWS, DEFAULT_MAIN_KP_COLS, DEFAULT_MAIN_KP_KEYS);

std::bitset<key::NONE> keypad::get_keys() {
    return curr_keys;
}

std::bitset<key::NONE> keypad::get_pressed() {
    return ~prev_keys & curr_keys;
}
}