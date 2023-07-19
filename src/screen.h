#pragma once
#include <u8g2.h>
#include <memory>
#include "keypad.h"
#include <pico/stdlib.h>

using update_fn = void (*) (u8g2_t *);

extern update_fn curr_upd_fn;