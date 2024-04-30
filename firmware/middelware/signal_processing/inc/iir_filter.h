#ifndef IIR_FILTER_H_
#define IIR_FILTER_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Middelware Middelware
 ** @{ */
/** \addtogroup IIR_Filter IIR Filter
 */

/** \brief Functionalities to design and use filters
 * 
 * @author Peñalva Albano
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 15/03/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
typedef enum filter_order {
    ORDER_2 = 2,        /*!< 2nd order filter */
    ORDER_4 = 4,        /*!< 4th order filter */ 
    ORDER_6 = 6,        /*!< 6th order filter */
    ORDER_8 = 8         /*!< 8th order filter */
} filter_order_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Initialize a 2nd order Butterwotrh Low Pass Filter
 * 
 * @param sample_frec   Signal's sample frequency
 * @param cut_frec      Filter's cut-off frequency
 * @param order         Filter's order (2, 4, 6 or 8)
 */
void LowPassInit(float sample_frec, float cut_frec, filter_order_t order);

/**
 * @brief Initialize a 2nd order Butterwotrh Hi Pass Filter
 * 
 * @param sample_frec   Signal's sample frequency
 * @param cut_frec      Filter's cut-off frequency
 * @param order         Filter's order (2, 4, 6 or 8)
 */
void HiPassInit(float sample_frec, float cut_frec, filter_order_t order);

/**
 * @brief Apply a low pass filter to a signal array
 * 
 * @param input_signal      Input signal array
 * @param output_signal     Filtered signal array
 * @param signal_lenght     Number of samples of both signals
 */
void LowPassFilter(float * input_signal, float * output_signal, int16_t signal_lenght);

/**
 * @brief Apply a hi pass filter to a signal array
 * 
 * @param input_signal      Input signal array
 * @param output_signal     Filtered signal array
 * @param signal_lenght     Number of samples of both signals
 */
void HiPassFilter(float * input_signal, float * output_signal, int16_t signal_lenght);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* IIR_FILTER_H_ */

/*==================[end of file]============================================*/