add_executable(zcalc main.cpp u8g2_rp2040_hal.cpp keypad.cpp maths.cpp screens/calculator.cpp
        screens/menu.cpp
        screens/scanner.cpp
        wireless.cpp
        btstack_config.h)
#target_include_directories(zcalc PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(zcalc PRIVATE
        FreeRTOS-Kernel
        freertos_kernel_port
        freertos_kernel
        FreeRTOS-Kernel-Heap4
        pico_stdlib
        pico_cyw43_arch_none
        pico_btstack_ble
        pico_btstack_cyw43
        hardware_spi
        u8g2
        hardware_pio
        cln)

pico_btstack_make_gatt_header(zcalc PRIVATE "${CMAKE_CURRENT_LIST_DIR}/comms.gatt")

pico_add_extra_outputs(zcalc)
pico_enable_stdio_usb(zcalc 1)