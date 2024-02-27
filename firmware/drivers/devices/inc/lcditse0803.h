#ifndef LCD_ITSE0803_H
#define LCD_ITSE0803_H

/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup LCD_ITSE0803 LCD ITSE0803
 ** @{
 */

/** \brief Driver for using the 3 digits numeric display in ESP-EDU.
 *
 * |   Display      |   EDU-CIAA	|
 * |:--------------:|:-------------:|
 * | 	Vcc 	    |	5V      	|
 * | 	BCD1		| 	GPIO_20		|
 * | 	BCD2	 	| 	GPIO_21		|
 * | 	BCD3	 	| 	GPIO_22		|
 * | 	BCD4	 	| 	GPIO_23		|
 * | 	SEL1	 	| 	GPIO_19		|
 * | 	SEL2	 	| 	GPIO_18		|
 * | 	SEL3	 	| 	GPIO_9		|
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
#include <stdint.h>
#include <stdbool.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief ESP-EDU LCD Module initialization.
 * 
 * @return true 
 */
bool LcdItsE0803Init(void);

/** @brief Function to display value a LCD Module for EDU-CIAA.
 *
 * @param[in] value to show 0..999
 *
 * @return false if an error occurs (out of scale), in other case returns true
 */

/**
 * @brief Display a value in LCD Module.
 * 
 * @param value Number to display (o to 999)
 * @return true if value < 999
 * @return false if value > 999
 */
bool LcdItsE0803Write(uint16_t value);

/**
 * @brief Read value displayed in LCD.
 * 
 * @return uint16_t value displayed.
 */
uint16_t LcdItsE0803Read(void);

/**
 * @brief Turn off display.
 * 
 */
void LcdItsE0803Off(void);

/**
 * @brief ESP-EDU LCD Module initialization.
 * 
 * @return true 
 */
bool LcdItsE0803DeInit(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* #ifndef DISPLAYITS_E0803_H */

/*==================[end of file]============================================*/
