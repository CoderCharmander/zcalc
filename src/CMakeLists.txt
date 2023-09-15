add_executable(zcalc main.cpp u8g2_rp2040_hal.cpp keypad.cpp maths.cpp screens/calculator.cpp
        screens/menu.cpp
        screens/menu.h)
target_link_libraries(zcalc PRIVATE FreeRTOS-Kernel freertos_kernel_port freertos_kernel FreeRTOS-Kernel-Heap4 pico_stdlib pico_cyw43_arch_none hardware_spi u8g2 hardware_pio cln )
pico_add_extra_outputs(zcalc)
pico_enable_stdio_usb(zcalc 1)