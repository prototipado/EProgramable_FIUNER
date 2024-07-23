/**
 * @file analog_io_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "analog_io_mcu.h"
#include "driver/gptimer.h"
#include "driver/sdm.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
/*==================[macros and definitions]=================================*/
#define ADC_BITWIDTH 		SOC_ADC_DIGI_MAX_BITWIDTH	// 12 bit resolution
#define ADC_ATTENUATION		ADC_ATTEN_DB_12				// 12dB attenuation (for 0-3,3V ADC range)
/*==================[internal data declaration]==============================*/
adc_cali_handle_t adc_calibration_single_0, adc_calibration_single_1, adc_calibration_single_2, adc_calibration_single_3;
adc_oneshot_unit_handle_t adc1_single; 
adc_continuous_handle_t adc2_cont;
sdm_channel_handle_t dac = NULL;
bool adc1_single_used = false;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
adc_oneshot_unit_init_cfg_t init_config_single = {
	.unit_id = ADC_UNIT_1,
	.ulp_mode = ADC_ULP_MODE_DISABLE,
};
adc_oneshot_chan_cfg_t adc_config_single = {
	.bitwidth = ADC_BITWIDTH,
	.atten = ADC_ATTENUATION,
};					
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void AnalogInputInit(analog_input_config_t *config){
	
	// config adc channels
	switch(config->mode){
		case ADC_SINGLE:
        	if(!adc1_single_used){
				adc_oneshot_new_unit(&init_config_single, &adc1_single);
				adc1_single_used = true;
			}
			switch(config->input){
				case CH0:
    				adc_oneshot_config_channel(adc1_single, ADC_CHANNEL_0, &adc_config_single);
					// create calibration curve
					adc_cali_curve_fitting_config_t cali_config_0 = {
						.unit_id = ADC_UNIT_1,
						.chan = ADC_CHANNEL_0, 
						.atten = ADC_ATTENUATION,
						.bitwidth = ADC_BITWIDTH,
					};
					ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config_0, &adc_calibration_single_0));
				break;
				case CH1:
    				adc_oneshot_config_channel(adc1_single, ADC_CHANNEL_1, &adc_config_single);
					// create calibration curve
					adc_cali_curve_fitting_config_t cali_config_1 = {
						.unit_id = ADC_UNIT_1,
						.chan = ADC_CHANNEL_1, 
						.atten = ADC_ATTENUATION,
						.bitwidth = ADC_BITWIDTH,
					};
					ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config_1, &adc_calibration_single_1));
				break;
				case CH2:
    				adc_oneshot_config_channel(adc1_single, ADC_CHANNEL_2, &adc_config_single);
					// create calibration curve
					adc_cali_curve_fitting_config_t cali_config_2 = {
						.unit_id = ADC_UNIT_1,
						.chan = ADC_CHANNEL_2, 
						.atten = ADC_ATTENUATION,
						.bitwidth = ADC_BITWIDTH,
					};
					ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config_2, &adc_calibration_single_2));
				break;
				case CH3:
    				adc_oneshot_config_channel(adc1_single, ADC_CHANNEL_3, &adc_config_single);
					// create calibration curve
					adc_cali_curve_fitting_config_t cali_config_3 = {
						.unit_id = ADC_UNIT_1,
						.chan = ADC_CHANNEL_3, 
						.atten = ADC_ATTENUATION,
						.bitwidth = ADC_BITWIDTH,
					};
					ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config_3, &adc_calibration_single_3));
				break;
			}
		break;
		case ADC_CONTINUOUS:
			switch(config->input){
				case CH0:

				break;
				case CH1:

				break;
				case CH2:

				break;
				case CH3:

				break;
			}
		break;
	}
}

void AnalogOutputInit(void){
	sdm_config_t dac_config = {
		.clk_src = SDM_CLK_SRC_DEFAULT,
		.sample_rate_hz = 1 * 1000 * 1000,
		.gpio_num = 0,
	};
	sdm_new_channel(&dac_config, &dac);
	sdm_channel_enable(dac);
}

void AnalogInputReadSingle(adc_ch_t channel, uint16_t *value){
    switch(channel){
		case CH0:
			adc_oneshot_read(adc1_single, ADC_CHANNEL_0, (int*)value);
		break;
		case CH1:
			adc_oneshot_read(adc1_single, ADC_CHANNEL_1, (int*)value);
		break;
		case CH2:
			adc_oneshot_read(adc1_single, ADC_CHANNEL_2, (int*)value);
		break;
		case CH3:
			adc_oneshot_read(adc1_single, ADC_CHANNEL_3, (int*)value);
		break;
	}
}

void AnalogStartContinuous(adc_ch_t channel){

}

void AnalogStopContinuous(adc_ch_t channel){

}

void AnalogInputReadContinuous(adc_ch_t channel, uint16_t *values){

}

void AnalogOutputWrite(uint8_t value){
	int8_t density = value - 128;
	sdm_channel_set_pulse_density(dac, density);
}

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/
