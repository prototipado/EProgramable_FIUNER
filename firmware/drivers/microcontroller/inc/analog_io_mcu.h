#ifndef ANALOG_IO_MCU_H
#define ANALOG_IO_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup Analog_IO Analog IO
 ** @{ */

/** \brief AD and DA Converter driver for the ESP-EDU Board.
 *
 * This driver provide functions to configure and handle the ESP-EDU Analogs Inputs/Outputs.
 * 
 * @note The ESP-EDU have 4 analog inputs and 1 analog output, but the designated pin for 
 * the latter is shared with analog output 0 (CH0).
 *
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 24/02/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include "stdint.h"
/*==================[macros]=================================================*/
typedef enum adc_ch {
	CH0 = 0,				/*!< Channel 0 */
	CH1,					/*!< Channel 1 */
	CH2,					/*!< Channel 2 */
	CH3						/*!< Channel 3 */
} adc_ch_t;

typedef enum adc_mode {
	ADC_SINGLE,				/*!< Single read */
	ADC_CONTINUOUS,			/*!< Continuous read */
} adc_mode_t;

#define DAC	0    			/*!< DAC pin. Override CH0 declaration*/
/*==================[typedef]================================================*/
/**
 * @brief Analog inputs config structure
 * 
 */
typedef struct {			
	adc_ch_t input;			/*!< Inputs: CH0, CH1, CH2, CH3 */
	adc_mode_t mode;		/*!< Mode: single read or continuous read */
	void *func_p;			/*!< Pointer to callback function for convertion end (only for continuous mode) */
	void *param_p;			/*!< Pointer to callback function parameters (only for continuous mode) */
	uint16_t sample_frec;	/*!< Sample frequency min: 20kHz - max: 2MHz (only for continuous mode)  */
} analog_input_config_t;	

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Analog input initialization
 * 
 * @param config Analog inputs config structure
 * @return null
 */
void AnalogInputInit(analog_input_config_t *config);

/**
 * @brief Analog output initialization (DAC)
 * 
 * @return null
 */
void AnalogOutputInit(void);

/**
 * @brief Read single channel.
 * 
 * @param channel Channel selected
 * @param value Read variable pointer
 * @return null
 */
void AnalogInputReadSingle(adc_ch_t channel, uint16_t *value);

/**
 * @brief Start convertion for ADC module in continuous mode
 * 
 * @param channel Channel selected
 */
void AnalogStartContinuous(adc_ch_t channel);

/**
 * @brief Stop convertion for ADC module
 * 
 * @param channel Channel selected
 */
void AnalogStopContinuous(adc_ch_t channel);

/**
 * @brief 
 * 
 * @param channel Channel selected.
 * @param values Read variable array
 */
void AnalogInputReadContinuous(adc_ch_t channel, uint16_t *values);

/**
 * @brief Convert raw value from ADC to mV, using a calibration curve.
 * 
 * @param value Raw value from ADC.
 * @return uint16_t Calibrated value from ADC in mV.
 */
uint16_t AnalogRaw2mV(uint16_t value);

/**
 * @brief Digital-to-Analog convert.
 * 
 * @param value Digital value to convert (from 0 to 255)
 */
void AnalogOutputWrite(uint8_t value);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* #ifndef ANALOG_IO_H */

/*==================[end of file]============================================*/
