#pragma once

#include <bitset>
#include <stdint.h>
#include <vector>
namespace keypad {

enum key {
    N0,
    N1,
    N2,
    N3,
    N4,
    N5,
    N6,
    N7,
    N8,
    N9,
    LEFT,
    RIGHT,
    ADD,
    SUB,
    MUL,
    DIV,
    BACKSPACE,
    DECIMAL,
    ALT,
    ENTER,
    NONE,
};
using keyset = std::bitset<key::NONE>;

const char keychars[]{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '<', '>', '+', '-', '*', '/', '~', '.', 'A', '\\'};

constexpr uint32_t PIN_BEGIN = 2;
constexpr uint32_t COLUMNS = 4;
constexpr uint32_t ROWS = 5;
constexpr key DEFAULT_MAIN_KP_KEYS[ROWS * COLUMNS] = {
    ADD, SUB, MUL, DIV,
    N7, N8, N9, LEFT,
    N4, N5, N6, RIGHT,
    N1, N2, N3, BACKSPACE,
    ALT, N0, DECIMAL, ENTER,
};

constexpr uint8_t DEFAULT_MAIN_KP_ROWS[] = {10, 9, 8, 7, 6};
constexpr uint8_t DEFAULT_MAIN_KP_COLS[] = {2, 3, 4, 5};

class keypad {
  public:
    keypad(int _rows, int _cols, const uint8_t *_row_pins, const uint8_t *_col_pins, const key *_key_map);
    void update();
    std::bitset<key::NONE> get_keys();
    std::bitset<key::NONE> get_pressed();

  private:
    std::bitset<key::NONE> curr_keys;
    std::bitset<key::NONE> prev_keys;
    std::vector<uint8_t> row_pins;
    std::vector<uint8_t> col_pins;
    std::vector<key> key_map;
};

extern keypad main_kp;

std::bitset<key::NONE> get_keys();
std::bitset<key::NONE> get_pressed();
} // namespace keypad