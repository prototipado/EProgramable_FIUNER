#ifndef PWM_MCU_H
#define PWM_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup PWM PWM
 ** @{ */

/** @brief PWM driver for ESP-EDU board
 *
 * This driver provide functions to generate PWM signals 
 *
 * @note It can setup up to 4 PWM outputs, with independet duty 
 * cycle and frequency configuration
 *
 * @author Albano Peñalva
 * 
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 23/01/2024 | Document creation		                         |
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <gpio_mcu.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
typedef enum pwm_out {
	PWM_0,      /**< PWM output 1 */
	PWM_1,		/**< PWM output 2 */
	PWM_2,		/**< PWM output 3 */
	PWM_3		/**< PWM output 4 */
} pwm_out_t;
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/**
 * @brief Single PWM output inicialization in selected GPIO
 * 
 * @note When initialized PWM output start On with duty cycle 0%.
 * 
 * @param out PWM output
 * @param gpio GPIO pin number
 * @param freq PWM wave frequency
 * @return uint8_t 
 */
uint8_t PWMInit(pwm_out_t out, gpio_t gpio, uint16_t freq);

/**
 * @brief Resume PWM output
 * 
 * @param out PWM output
 */
void PWMOn(pwm_out_t out);

/**
 * @brief Pause PWM output
 * 
 * @param out PWM output 
 */
void PWMOff(pwm_out_t out);

/**
 * @brief Change PWM duty cycle of an PWM output
 * 
 * @param out PWM output 
 * @param duty_cycle duty cycle in % (0 to 100)
 */
void PWMSetDutyCycle(pwm_out_t out, uint8_t duty_cycle);

/**
 * @brief Change frequency of an PWM output
 * 
 * @param out PWM output 
 * @param freq Frequency of PWM output (40kHz máx)
 * @return uint8_t 
 */
uint8_t PWMSetFreq(pwm_out_t out, uint32_t freq);

/**
 * @brief PWM output de-inicialization
 * 
 * @param out PWM output 
 * @return uint8_t 
 */
uint8_t PWMDeinit(pwm_out_t out);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* PWM_MCU_H_ */

/*==================[end of file]============================================*/
