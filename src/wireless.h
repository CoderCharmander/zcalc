#pragma once

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "btstack.h"

namespace bluetooth {

void init();
void notify_if_conn();

}