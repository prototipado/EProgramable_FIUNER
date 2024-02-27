#ifndef GPIO_MCU_H
#define GPIO_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup GIOP GPIO
 ** @{ */

/** \brief GPIO driver for the ESP-EDU Board.
 *
 * This driver provide functions to configure and handle the ESP-EDU General
 * Purpose Input-Outputs .
 * 
 * @note GPIO_4 and GPIO_15 are connected to ESP-EDU switches (only GPIO_15 is available in headers)
 * 
 * @note GPIO_5, GPIO_10 and GPIO_11 are connected to ESP-EDU LEDs.
 * 
 * @note GPIO_12 and GPIO_13 are not recommended for use, because using them will
 * overwrite the flash and debug functionalities via USB.
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
#include <stdbool.h>
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief GPIO direction (input or output).
 * 
 */
typedef enum {
	GPIO_INPUT = 0, 	/**< Input with pull-up resistor */
	GPIO_OUTPUT			/**< Output */
	} io_t;

/**
 * @brief ESP32-C6 available GPIOs (not all of them are available in ESP-EDU)
 * 
 */
typedef enum gpio_list{
	GPIO_0=0, 	/**< GPIO0 - shared with ADC CH0 and DAC */
	GPIO_1, 	/**< GPIO1 - shared with ADC CH1 */
	GPIO_2, 	/**< GPIO2 - shared with ADC CH2 */
	GPIO_3, 	/**< GPIO3 - shared with ADC CH3 */
	GPIO_4, 	/**< GPIO4 - connected to SWITCH_1 (TECLA 1) */
	GPIO_5, 	/**< GPIO5 - connected to LED_3 */
	GPIO_6, 	/**< GPIO6 - shared with I2C SDA */
	GPIO_7, 	/**< GPIO7 - shared with I2C SCL */
	GPIO_8, 	/**< GPIO8 - shared with NeoPixel */
	GPIO_9, 	/**< GPIO9 - shared with SPI CS0 */
	GPIO_10, 	/**< GPIO10 - connected to LED_2 */
	GPIO_11, 	/**< GPIO11 - connected to LED_1 */
	GPIO_12, 	/**< GPIO12 - shared with USB_D- (not recommended for use) */
	GPIO_13, 	/**< GPIO13 - shared with USB_D+ (not recommended for use) */
	GPIO_14, 	/**< not available */
	GPIO_15, 	/**< GPIO15 - connected to SWITCH_2 (TECLA 2) */
	GPIO_16, 	/**< GPIO16 - shared with UART_PC TX */
	GPIO_17, 	/**< GPIO17 - shared with UART_PC RX */
	GPIO_18, 	/**< GPIO18 - shared with UART_CONNECTOR TX and SPI CS2 */
	GPIO_19, 	/**< GPIO19 - shared with UART_CONNECTOR RX and SPI CS1 */
	GPIO_20, 	/**< GPIO20 - shared with SPI SCLK */
	GPIO_21,	/**< GPIO21 - shared with SPI MOSI  */
	GPIO_22, 	/**< GPIO22 - shared with SPI MISO  */
	GPIO_23, 	/**< GPIO23 */
} gpio_t;

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
/**
 * @brief GPIO initialization
 * 
 * @param pin GPIO number
 * @param io GPIO direction
 */
void GPIOInit(gpio_t pin, io_t io);

/**
 * @brief Change GPIO state to high
 * 
 * @param pin GPIO number
 */
void GPIOOn(gpio_t pin);

/**
 * @brief Change GPIO state to low
 * 
 * @param pin GPIO number
 */
void GPIOOff(gpio_t pin);

/**
 * @brief Change GPIO state
 * 
 * @param pin GPIO number
 * @param state GPIO state (true: high - false: low)
 */
void GPIOState(gpio_t pin, bool state);

/**
 * @brief Invert GPIO state
 * 
 * @param pin GPIO number
 */
void GPIOToggle(gpio_t pin);

/**
 * @brief Reads GPIO state
 * 
 * @param pin 
 * @return true GPIO input high
 * @return false GPIO input low
 */
bool GPIORead(gpio_t pin);

/**
 * @brief Configure GPIO input interruption
 * 
 * @param pin GPIO number
 * @param ptr_int_func Pointer to callback function
 * @param edge true: positive edge - false: negative edge
 * @param args 
 */
void GPIOActivInt(gpio_t pin, void *ptr_int_func, bool edge, void *args);

/**
 * @brief Configure an input glitch filter to a GPIO
 * 
 * @note You can add filters to up to 8 GPIO
 * 
 * @param pin GPIO number
 * @param filter filter number
 */
void GPIOInputFilter(gpio_t pin);

/**
 * @brief GPIO de-initialization
 * 
 */
void GPIODeinit(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* INC_GPIO_H_ */

/*==================[end of file]============================================*/
