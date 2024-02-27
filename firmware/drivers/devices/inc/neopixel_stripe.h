#ifndef NEOPIXEL_H
#define NEOPIXEL_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup NeoPixel_Stripe NeoPixel_Stripe
 ** @{ */

/** \brief NeoPixel driver for the ESP-EDU Board.
 *
 * @note This driver can handle only one stripe of NeoPixel at a time
 * (with no limits in the qty of leds in the array).
 * 
 * @note ESP-EDU have one individual NeoPixel connected to GPIO_8, that can be used with this driver.
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
#define BUILT_IN_RGB_LED_PIN          GPIO_8        /*> ESP32-C6-DevKitC-1 NeoPixel it's connected at GPIO_8 */
#define BUILT_IN_RGB_LED_LENGTH       1             /*> ESP32-C6-DevKitC-1 NeoPixel has one pixel */

#define NEOPIXEL_COLOR_WHITE          0x00FFFFFF  /*> Color white */
#define NEOPIXEL_COLOR_RED            0x00FF0000  /*> Color red */
#define NEOPIXEL_COLOR_ORANGE         0x00FF7D00  /*> Color orange */
#define NEOPIXEL_COLOR_YELLOW         0x007F7F00  /*> Color yellow */
#define NEOPIXEL_COLOR_LGREEN         0x007DFF00  /*> Color ligth green */
#define NEOPIXEL_COLOR_GREEN          0x0000FF00  /*> Color green */
#define NEOPIXEL_COLOR_TURQUOISE      0x0000FF7D  /*> Color turquoise */
#define NEOPIXEL_COLOR_CYAN           0x0000F7F7  /*> Color cyan */
#define NEOPIXEL_COLOR_OCEAN          0x00007DFF  /*> Color ocean */
#define NEOPIXEL_COLOR_BLUE           0x000000FF  /*> Color blue */
#define NEOPIXEL_COLOR_VIOLET         0x007D00FF  /*> Color violet */
#define NEOPIXEL_COLOR_MAGENTA        0x007F007F  /*> Color magenta */
#define NEOPIXEL_COLOR_ROSE           0x00FF007D  /*> Color rose */

#define NEOPIXEL_HUE_RED              0x0000      /*> Hue red */
#define NEOPIXEL_HUE_ORANGE           0x1555      /*> Hue orange */
#define NEOPIXEL_HUE_YELLOW           0x2AAA      /*> Hue yellow */
#define NEOPIXEL_HUE_LGREEN           0x4000      /*> Hue ligth green */
#define NEOPIXEL_HUE_GREEN            0x5555      /*> Hue green */
#define NEOPIXEL_HUE_TURQUOISE        0x6AAA      /*> Hue turquoise */
#define NEOPIXEL_HUE_CYAN             0x8000      /*> Hue cyan */
#define NEOPIXEL_HUE_OCEAN            0x9555      /*> Hue ocean */
#define NEOPIXEL_HUE_BLUE             0xAAAA      /*> Hue blue */
#define NEOPIXEL_HUE_VIOLET           0xC000      /*> Hue violet */
#define NEOPIXEL_HUE_MAGENTA          0xD555      /*> Hue magenta */
#define NEOPIXEL_HUE_ROSE             0xEAAA      /*> Hue rose */

/*==================[typedef]================================================*/
/**
 * @brief 24 bits color
 * 
 * 0x00FF0000 -> Red
 * 0x0000FF00 -> Green
 * 0x000000FF -> Blue
 */
typedef uint32_t neopixel_color_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief NeoPixel array initialization.
 * 
 * @param pin           GPIO number where NeoPixel data pin (DIN) will be connected
 * @param len           Number of NeoPixels in the stripe
 * @param color_array   Array of len length, to store each NeoPixel color
 */
void NeoPixelInit(gpio_t pin, uint16_t len, neopixel_color_t *color_array);

/**
 * @brief Turn off all NeoPixels.
 * 
 */
void NeoPixelAllOff(void);

/**
 * @brief Change all NeoPixels in the stripe to the same color.
 * 
 * @param color 24 bits color
 */
void NeoPixelAllColor(neopixel_color_t color);

/**
 * @brief Set an individual pixel to a color.
 * 
 * @param pixel     NeoPixel number on the stripe
 * @param color     24 bits color
 */
void NeoPixelSetPixel(uint16_t pixel, neopixel_color_t color);

/**
 * @brief Set all NeoPixels in the array with the color stored in an array.
 * 
 * @param color_array Array of 24 bits color
 */
void NeoPixelSetArray(neopixel_color_t *color_array);

/**
 * @brief Shift the all NeoPixel colors in the array 1 position (up or down)
 * 
 * @note The last color'll be moved to the first position.
 * @param upwards Shift direction: true: upwars, false: downwards.
 */
void NeoPixelShift(bool upwards);

/**
 * @brief Change NeoPixel brightness.
 * 
 * @note: by default NeoPixels bright is at maximum (255)
 * @param bright Brightness level (0 to 255).
 */
void NeoPixelBrightness(uint8_t bright);

/**
 * @brief Convert 3 individual color levels (R, G, B) to a 24bits color data.
 * 
 * @param red       Red level
 * @param green     Green level
 * @param blue      Blue level
 * @return neopixel_color_t 24 bits color
 */
neopixel_color_t NeoPixelRgb2Color(uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Convert hue, value and saturation values to 24 bits color
 * (using the HSV color model)
 * 
 * @param hue 16 bits color or shade (HSV color model)
 * @param sat color saturation (HSV color model)
 * @param val color value or brightness (HSV color model)
 * @return neopixel_color_t 24 bits color
 */
neopixel_color_t NeoPixelHSV2Color(uint16_t hue, uint8_t sat, uint8_t val);

/**
 * @brief Set all NeoPixels with a gradient of colors. 
 * 
 * @param first_hue Hue (HSV color model) of the first NeoPixel in the stripe.
 * @param sat Color saturation of all the NeoPixels (HSV color model)
 * @param val Color value or brightness of all the NeoPixels (HSV color model)
 * @param reps number of repeticion of the color pattern
 */
void NeoPixelRainbow(uint16_t first_hue, uint8_t sat, uint8_t val, uint8_t reps);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif

/*==================[end of file]============================================*/
