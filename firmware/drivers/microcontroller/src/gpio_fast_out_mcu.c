/**
 * @file gpio_fast_out_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-11-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "gpio_fast_out_mcu.h"
#include "gpio_mcu.h"
#include <stdint.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/dedic_gpio.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
dedic_gpio_bundle_handle_t bundleA = NULL;
int bundleA_gpios[16];
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void GPIOFastInit(gpio_t *pin_list, uint8_t pin_qty){
    memcpy(bundleA_gpios, pin_list, pin_qty);
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
    };
    for (int i = 0; i < pin_qty; i++) {
        io_conf.pin_bit_mask = 1ULL << bundleA_gpios[i];
        gpio_config(&io_conf);
    }
    // Create bundleA, output only
    dedic_gpio_bundle_config_t bundleA_config = {
        .gpio_array = bundleA_gpios,
        .array_size = pin_qty,
        .flags = {
            .out_en = 1,
        },
    };
    ESP_ERROR_CHECK(dedic_gpio_new_bundle(&bundleA_config, &bundleA));
}

void GPIOFastWrite(uint16_t value){
    dedic_gpio_bundle_write(bundleA, 0xFF, value);
}

/*==================[end of file]============================================*/