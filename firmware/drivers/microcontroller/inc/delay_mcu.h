#ifndef DELAY_MCU_H
#define DELAY_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup Delay Delay
 ** @{ */

/** \brief Functions to generate delays.
 *
 * This driver provide functions to generate delays FreeRTOS friendly, using one timer.
 * 
 * @note All delays will block the current RTOS task, with the exception of 
 * DelayUs with usec < 50.
 *
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 20/10/2023 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
/**
 * @brief Delay in seconds
 * @param[in] sec seconds to be in delay
 * @return None
 */
void DelaySec(uint16_t sec);

/**
 * @brief Delay in milliseconds
 * @param[in] msec milliseconds to be in delay
 * @return None
 */
void DelayMs(uint16_t msec);

/**
 * @brief Delay in microseconds
 * @param[in] usec microseconds to be in delay
 * @return None
 */
void DelayUs(uint16_t usec);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* DELAY_H_ */

/*==================[end of file]============================================*/
