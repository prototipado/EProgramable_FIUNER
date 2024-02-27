#ifndef GPIO_FAST_MCU_H
#define GPIO_FAST_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup GIOP_FAST GIOP_FAST
 ** @{ */

/** \brief GPIO driver to use gpio ouputs with faster functions than gpio_mcu.
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 20/11/2023 | Document creation		                         						|
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
 * @brief 
 * 
 * @param pin_list 
 * @param pin_qty 
 */
void GPIOFastInit(gpio_t *pin_list, uint8_t pin_qty);

/**
 * @brief 
 * 
 * @param value 
 */
void GPIOFastWrite(uint16_t value);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif 

/*==================[end of file]============================================*/
