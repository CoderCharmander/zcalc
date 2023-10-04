#pragma once
#include "../screen.h"

namespace screens::scanner {

void enter();
void update(u8g2_t *u8g2);
void set_message(char *buf, uint16_t len);
void set_image(uint8_t *buf, uint16_t buf_size);

} // namespace screens::scanner