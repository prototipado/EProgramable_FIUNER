idf_build_get_property(target IDF_TARGET)

# Always compiled source files
set(srcs
    "microcontroller/src/gpio_mcu.c"
    "microcontroller/src/delay_mcu.c"
    "microcontroller/src/timer_mcu.c"
    "microcontroller/src/uart_mcu.c"
    "microcontroller/src/spi_mcu.c"
    "microcontroller/src/pwm_mcu.c"
    "microcontroller/src/i2c_mcu.c"
    "microcontroller/src/gpio_fast_out_mcu.c"
    "microcontroller/src/analog_io_mcu.c"
    "devices/src/led.c"
    "devices/src/switch.c"
    "devices/src/lcditse0803.c"
    "devices/src/hc_sr04.c"
    "devices/src/ws2812b.c"
    "devices/src/neopixel_stripe.c"
    "devices/src/ili9341.c"
    "devices/src/fonts.c"
    "devices/src/servo_sg90.c"
    "devices/src/hx711.c"
    "devices/src/mpu6050.c"
    )

# Always included headers
set(includes "microcontroller/inc"
             "devices/inc")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES driver esp_adc)