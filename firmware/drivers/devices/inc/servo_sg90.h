#ifndef SERVO_SG90_H
#define SERVO_SG90_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup SERVO Servo
 ** @{ */

/** \brief Servo driver for the ESP-EDU Board.
 *
 * @note This driver can handle up to 4 SG90 microservos.
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 23/01/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include "gpio_mcu.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
typedef enum servo_out {
	SERVO_0,    /**< uses PWM_0 out */
	SERVO_1,	/**< uses PWM_1 out */
	SERVO_2,	/**< uses PWM_2 out */
	SERVO_3		/**< uses PWM_3 out */
} servo_out_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Servo initialization.
 * 
 * @param servo Servo number.
 * @param gpio  GPIO number to connect servo's PWM pin.
 * @return uint8_t 
 */
uint8_t ServoInit(servo_out_t servo, gpio_t gpio);

/**
 * @brief Change servo angle.
 * 
 * @param servo Servo number
 * @param ang Servo angle (from -90 to 90 degrees)
 * @return uint8_t 
 */
void ServoMove(servo_out_t servo, int8_t ang);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* #ifndef SERVO_SG90_H */

/*==================[end of file]============================================*/

