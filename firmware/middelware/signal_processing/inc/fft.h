#ifndef FFT_H_
#define FFT_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Middelware Middelware
 ** @{ */
/** \addtogroup FFT Fast Fourier Transform
 */

/** \brief Functionalities to calculate FFT
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
#include <stdbool.h>
/*==================[macros]=================================================*/
#define MAX_SIGNAL_LENGHT   2048
/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Initialize the FFT calculation module
 * 
 * @return true     FFT initialized
 * @return false    Not possible to initialize FFT
 */
bool FFTInit(void);

/**
 * @brief Calculates the Fast Fourier Transform of a given signal
 * 
 * @note  Lenght of signal array must be a power of two (with maximun value = MAX_SIGNAL_LENGHT)
 * 
 * @param signal            Array with signal values (of lenght = signal_lenght)
 * @param fft               Array to store FFT magnitude values (of lenght = signal_lenght / 2)
 * @param signal_lenght     Lenght of signal arrays
 */
void FFTMagnitude(float * signal, float * fft, uint16_t signal_lenght);

/**
 * @brief Return the FFT frequency axis vector
 * 
 * @param sample_freq       Sample frequency 
 * @param signal_lenght     Lenght of signal array
 * @param f                 Array to store frequency values (of lenght = signal_lenght / 2)
 */
void FFTFrequency(float sample_freq, uint16_t signal_lenght, float * f);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* FFT_H_ */

/*==================[end of file]============================================*/