#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <u8g2.h>

namespace display_rp2040 {

extern spi_inst_t *HW_SPI_UNIT;
constexpr uint32_t CS = 13;
constexpr uint32_t CLOCK = 14;
constexpr uint32_t MOSI = 15;
constexpr uint32_t RESET = 11;
constexpr uint32_t DC = 12;
uint8_t cb_gpio_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                      void *arg_ptr);

uint8_t cb_spi_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                         void *arg_ptr);
} // namespace display_rp2040
uint8_t u8x8_gpio_and_delay_template(u8x8_t *u8x8, uint8_t msg,
                                                uint8_t arg_int,
                                                void *arg_ptr) ;
uint8_t u8x8_byte_pico_hw_spi(u8x8_t *u8x8, uint8_t msg,
                                         uint8_t arg_int, void *arg_ptr) ;