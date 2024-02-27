#ifndef SWITCH_H
#define SWITCH_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup Switch
 ** @{ */

/** \brief Switches driver for the ESP-EDU Board.
 * 
 * @note ESP-EDU have 2 switches connected to GPIO_4 and GPIO_15. 
 * The latter is also routed to J2 connector.
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
typedef enum switches {
    SWITCH_1 = (1 << 0),  /**< Routed to GPIO_4 */
    SWITCH_2 = (1 << 1),  /**< Routed to GPIO_15 */
} switch_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief ESP-EDU switches initialization
 * 
 * @return int8_t 
 */
int8_t SwitchesInit(void);

/**
 * @brief Read all the switches state.
 * 
 * @return int8_t 0 if no key pressed, SWITCH_1, SWITCH_2 or (SWITCH_1 | SWITCH_2) in other case.
 */
int8_t SwitchesRead(void);

/**
 * @brief Enables the interruption of a particular key and assigns a callback function.
 * 
 * @param tec Selected switch
 * @param ptrIntFunc Pointer to callback function
 * @param args Pointer to callback function parameters
 */
void SwitchActivInt(switch_t tec, void *ptrIntFunc, void *args);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif

/*==================[end of file]============================================*/
