#ifndef HC_SR04_H
#define HC_SR04_H

/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup HC_SR04 HC SR04
 ** @{ */

/** \brief Driver for reading distance with HC-SR04 module.
 *
 * @note Maximun distance: 300cm (118 inches).
 * 
 * @note When disconnected return 0.
 * 
 * @note When ussing dedicated connector in ESP-EDU:
 * |   HC_SR04      |   EDU-CIAA	|
 * |:--------------:|:-------------:|
 * | 	Vcc 	    |	5V      	|
 * | 	Echo		| 	GPIO_3		|
 * | 	Trig	 	| 	GPIO_2		|
 * | 	Gnd 	    | 	GND     	|
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
#include "gpio_mcu.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief HC_SR04 initialization.
 * 
 * @param echo GPIO number wher echo pin is connected
 * @param trigger GPIO number wher trigger pin is connected
 * @return true 
 */
bool HcSr04Init(gpio_t echo, gpio_t trigger);

/**
 * @brief Read distance
 * 
 * @return uint16_t measured distance in cm.
 */
uint16_t HcSr04ReadDistanceInCentimeters(void);

/**
 * @brief Read distance
 * 
 * @return uint16_t measured distance in inches.
 */
uint16_t HcSr04ReadDistanceInInches(void);

/**
 * @brief HC_SR04 de-initialization.
 * 
 * @return true 
 */
bool HcSr04Deinit(void);

/*==================[end of file]============================================*/
#endif /* #ifndef HC_SR04_H */

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
