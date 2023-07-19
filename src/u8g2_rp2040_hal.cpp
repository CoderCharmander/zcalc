#include <pico/stdlib.h>
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "u8g2_rp2040_hal.h"

namespace display_rp2040 {

spi_inst_t *HW_SPI_UNIT = spi0;

uint8_t cb_spi_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_BYTE_SET_DC:
        u8x8_gpio_SetDC(u8x8, arg_int);
        break;
    case U8X8_MSG_BYTE_SEND:
        if (spi_write_blocking(HW_SPI_UNIT, (const uint8_t *)arg_ptr, arg_int) < arg_int) {
            // How do I even do error handling at this point without everything
            // falling into ashes
            break;
        }
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, nullptr);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, nullptr);
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;
    case U8X8_MSG_BYTE_INIT:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        spi_init(HW_SPI_UNIT, 500000);
        spi_set_format(HW_SPI_UNIT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
        gpio_set_function(CLOCK, GPIO_FUNC_SPI);
        gpio_set_function(MOSI, GPIO_FUNC_SPI);
        gpio_init(CS);
        gpio_set_dir(CS, GPIO_OUT);
        //gpio_put(CS, u8x8->display_info->chip_disable_level);
        gpio_init(RESET);
        gpio_set_dir(RESET, GPIO_OUT);
        gpio_init(DC);
        gpio_set_dir(DC, GPIO_OUT);
        break;
    default:
        return 0;
    }
    return 1;
}

uint8_t cb_gpio_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
    #if 1
        for (uint32_t pin : (uint32_t[]){CS, CLOCK, MOSI, RESET, DC}) {
            printf("Init %d\n", pin);
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_OUT);
        }
    #endif
        break;
    case U8X8_MSG_DELAY_MILLI:
        sleep_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_NANO:
        sleep_us(arg_int == 0 ? 0 : 1);
        break;
    case U8X8_MSG_GPIO_CS:
        //printf("CS");
        gpio_put(CS, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        gpio_put(DC, arg_int);
        break;
    case U8X8_MSG_GPIO_SPI_CLOCK:
        gpio_put(CLOCK, arg_int);
        break;
    case U8X8_MSG_GPIO_SPI_DATA:
        gpio_put(MOSI, arg_int);
        break;
    case U8X8_MSG_GPIO_RESET:
        gpio_put(RESET, arg_int);
        break;
    default: printf("Badhandle %d\n", msg);return 0;
    }
    return 1;
}

} // namespace display_rp2040

uint8_t u8x8_byte_pico_hw_spi(u8x8_t *u8x8, uint8_t msg,
                                         uint8_t arg_int, void *arg_ptr) {
  uint8_t *data;
  uint8_t internal_spi_mode;
  switch (msg) {
  case U8X8_MSG_BYTE_SEND:
    data = (uint8_t *)arg_ptr;
    // while( arg_int > 0 ) {
    // printf("U8X8_MSG_BYTE_SEND %d \n", (int)data[0]);
    spi_write_blocking(display_rp2040::HW_SPI_UNIT, data, arg_int);
    // data++;
    // arg_int--;
    // }
    break;
  case U8X8_MSG_BYTE_INIT:
    // printf("U8X8_MSG_BYTE_INIT \n");
    u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
    // if(spiInit){
    //   spi_deinit(SPI_PORT);
    // }
    // spi_init(SPI_PORT, SPI_SPEED);
    // spiInit = true;
    break;
  case U8X8_MSG_BYTE_SET_DC:
    // printf("U8X8_MSG_BYTE_SET_DC %d \n", arg_int);
    u8x8_gpio_SetDC(u8x8, arg_int);
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    // printf("U8X8_MSG_BYTE_START_TRANSFER %d \n", arg_int);

    // if(spiInit){
    //   spi_deinit(SPI_PORT);
    // }
    // spi_init(SPI_PORT, SPI_SPEED);
    // spiInit = true;

    u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
    u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO,
                            u8x8->display_info->post_chip_enable_wait_ns, NULL);
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    // printf("U8X8_MSG_BYTE_END_TRANSFER %d \n", arg_int);
    u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO,
                            u8x8->display_info->pre_chip_disable_wait_ns, NULL);
    u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
    // spi_deinit(SPI_PORT);
    // spiInit = false;
    break;
  default:
    return 0;
  }
  return 1;
}

uint8_t u8x8_gpio_and_delay_template(u8x8_t *u8x8, uint8_t msg,
                                                uint8_t arg_int,
                                                void *arg_ptr) {
  switch (msg) {
  case U8X8_MSG_GPIO_AND_DELAY_INIT: // called once during init phase of
                                     // u8g2/u8x8
    // printf("U8X8_MSG_GPIO_AND_DELAY_INIT\n");
    spi_init(display_rp2040::HW_SPI_UNIT, 500000);
    //gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(display_rp2040::CS, GPIO_FUNC_SIO);
    gpio_set_function(display_rp2040::CLOCK, GPIO_FUNC_SPI);
    gpio_set_function(display_rp2040::MOSI, GPIO_FUNC_SPI);
    // spiInit = true;
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(display_rp2040::DC);
    gpio_init(display_rp2040::CS);
    gpio_set_dir(display_rp2040::DC, GPIO_OUT);
    gpio_set_dir(display_rp2040::CS, GPIO_OUT);
    gpio_put(display_rp2040::CS, 1);
    gpio_put(display_rp2040::DC, 0);
    break;                  // can be used to setup pins
  case U8X8_MSG_DELAY_NANO: // delay arg_int * 1 nano second
    // printf("U8X8_MSG_DELAY_NANO %d\n", arg_int);
    sleep_us(1000 * arg_int);
    break;
  case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
    // printf("U8X8_MSG_DELAY_100NANO %d\n", arg_int);
    sleep_us(1000 * 100 * arg_int);
    break;
  case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
    // printf("U8X8_MSG_DELAY_10MICRO %d\n", arg_int);
    sleep_us(arg_int * 10);
    break;
  case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
    // printf("U8X8_MSG_DELAY_MILLI %d\n", arg_int);
    sleep_ms(arg_int);
    break;
  case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400
                           // KHz
    break; // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
  case U8X8_MSG_GPIO_D0: // D0 or SPI clock pin: Output level in arg_int
                         // case U8X8_MSG_GPIO_SPI_CLOCK:
    break;
  case U8X8_MSG_GPIO_D1: // D1 or SPI data pin: Output level in arg_int
                         // case U8X8_MSG_GPIO_SPI_DATA:
    break;
  case U8X8_MSG_GPIO_D2: // D2 pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_D3: // D3 pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_D4: // D4 pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_D5: // D5 pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_D6: // D6 pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_D7: // D7 pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_E: // E/WR pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_CS: // CS (chip select) pin: Output level in arg_int
                         // printf("U8X8_MSG_GPIO_CS %d\n", arg_int);
    gpio_put(display_rp2040::CS, arg_int);
    break;
  case U8X8_MSG_GPIO_DC: // DC (data/cmd, A0, register select) pin: Output level
                         // in arg_int
    // printf("U8X8_MSG_GPIO_DC %d\n", arg_int);
    gpio_put(display_rp2040::DC, arg_int);
    break;
  case U8X8_MSG_GPIO_RESET: // Reset pin: Output level in arg_int
                            // printf("U8X8_MSG_GPIO_RESET %d\n", arg_int);
    break;
  case U8X8_MSG_GPIO_CS1: // CS1 (chip select) pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_CS2: // CS2 (chip select) pin: Output level in arg_int
    break;
  case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
    break; // arg_int=1: Input dir with pullup high for I2C clock pin
  case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
    break; // arg_int=1: Input dir with pullup high for I2C data pin
  case U8X8_MSG_GPIO_MENU_SELECT:
    u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
    break;
  case U8X8_MSG_GPIO_MENU_NEXT:
    u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
    break;
  case U8X8_MSG_GPIO_MENU_PREV:
    u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
    break;
  case U8X8_MSG_GPIO_MENU_HOME:
    u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
    break;
  default:
    u8x8_SetGPIOResult(u8x8, 1); // default return value
    break;
  }
  return 1;
}