#ifndef L293_H_
#define L293_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup L293D L293D
 ** @{ 
 * @brief  Quadruple half-H driver
 *
 * This driver provide functions to configure and control a dual DC motor driver
 * using the L293D.
 *
 * @author Albano Peñalva
 *
 * @note Hardware connections:
 *
 * |   	L293D		|   EDU-CIAA	|
 * |:--------------:|:--------------|
 * | 	5V		 	|	5V			|
 * | 	GND		 	| 	GND			|
 * | 	1,2EN	 	| 	GPIO_22		|
 * | 	1A		 	| 	GPIO_21		|
 * | 	2A		 	| 	GPIO_20		|
 * | 	3,4EN	 	| 	GPIO_19		|
 * | 	3A		 	| 	GPIO_18		|
 * | 	4A		 	| 	GPIO_9		|
 *
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 17/05/2024 | Document creation		                         |
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief  Motors
 */
typedef enum
{
	MOTOR_1, 	/*!< Motor 1 */
	MOTOR_2,  	/*!< Motor 2 */
} l293_motor_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief  		Initializes L293 driver
 * @param[in]  	None
 * @retval 		1 when success, 0 when fails
 */
uint8_t L293Init(void);

/**
 * @brief  		Initializes L293 driver
 * @param[in]  	motor: 	motor to be configured
 * @param[in]  	speed: 	from -100 to 100
 * 						0: 			stop
 * 						1 to 100: 	foward
 * 						-1 to -100: backward
 * @retval 		1 when success, 0 when fails
 */
uint8_t L293SetSpeed(l293_motor_t motor, int8_t speed);

/**
 * @brief  	De-initializes L293 Driver
 * @param	None
 * @retval 	1 when success, 0 when fails
 */
uint8_t L293DeInit(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* L293_H_ */

/*==================[end of file]============================================*/
