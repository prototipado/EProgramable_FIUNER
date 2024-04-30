#ifndef RTC_MCU_H
#define RTC_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup RTC Real Time Clock
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
#include <stdint.h>
#include <stdbool.h>
/*==================[macros]=================================================*/
#define MAX_MONTH   12
#define MAX_MDAY    31
#define MAX_WDAY    7
#define MAX_HOUR    24
#define MAX_MIN     59
#define MAX_SEC     59
/*==================[typedef]================================================*/
/**
 * @brief Date and time structure 
 */
typedef struct {
   uint16_t year;	    /* 1 to 4095 */
   uint8_t  month;      /* 1 to 12   */
   uint8_t  mday;	    /* 1 to 31   */
   uint8_t  wday;	    /* 1 to 7    */
   uint8_t  hour;	    /* 0 to 23   */
   uint8_t  min;	    /* 0 to 59   */
   uint8_t  sec;	    /* 0 to 59   */
} rtc_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief Initial configuration of Date and Time.
 * 
 * @param rtc       Pointer to structure containing initial date and time.
 * @return true     Correct configuration.
 * @return false    Invalid parameters.
 */
bool RtcConfig(rtc_t * rtc);

/**
 * @brief Reads actual date and time.
 * 
 * @param rtc       Pointer to structure to store actual date and time.
 */
void RtcRead(rtc_t * rtc);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* RTC_MCU_H */

/*==================[end of file]============================================*/

