#ifndef WS2812B_H
#define WS2812B_H

/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup WS2812B WS2812B
 ** @{ */

/** \brief Driver for handling WS2812B RGB leds.
 *
 * @note For handling NeoPixels arrays use "neopixel_stripe.h".
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 23/10/2023 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "gpio_mcu.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief NeoPixel color
 * 
 */
typedef struct rgb_led{
	 uint8_t green;  		// Green
	 uint8_t red;  			// Red
	 uint8_t blue;  		// Blue
} rgb_led_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief NeoPixel initialization.
 * 
 * @param pin GPIO number where NeoPixel data pin (DIN) will be connected
 */
void ws2812bInit(gpio_t pin);

/**
 * @brief Send color information to NeoPixel.
 * 
 * @param data NeoPixel color
 */
void ws2812bSend(rgb_led_t led_color);

/**
 * @brief Send a ret command to NeoPixel.
 * 
 */
void ws2812bSendRet(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif

/*==================[end of file]============================================*/
