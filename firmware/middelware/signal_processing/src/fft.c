/**
 * @file fft.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-03-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include <string.h>
#include <math.h>
#include "fft.h"
#include "esp_dsp.h"
#include "esp_log.h"
/*==================[macros and definitions]=================================*/
#define TAG "FFT Module"
/*==================[internal data declaration]==============================*/
static float fft_complex[2 * MAX_SIGNAL_LENGHT];
static float wind[MAX_SIGNAL_LENGHT];
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
bool FFTInit(void){
    esp_err_t ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret != ESP_OK){
        return false;
    }
    return true;
}

void FFTMagnitude(float * signal, float * fft, uint16_t signal_lenght){
    // Generate Hann window
    dsps_wind_hann_f32(wind, signal_lenght);
    // Clear fft array
    memset(fft_complex, 0, 2 * MAX_SIGNAL_LENGHT * sizeof(float));
    // Multiply input array with window and store as real part
    dsps_mul_f32(signal, wind, fft_complex, signal_lenght, 1, 1, 2);    
    // Calculate FFT  
    dsps_fft2r_fc32(fft_complex, signal_lenght);
    // Bit reverse
    dsps_bit_rev_fc32(fft_complex, signal_lenght);
    // Convert one complex vector to two complex vectors
    dsps_cplx2reC_fc32(fft_complex, signal_lenght);
    // Calculate FFT magnitude 
    for (int j = 0; j < signal_lenght; j++){
            fft_complex[j] = 2*(sqrt(fft_complex[j*2+0]*fft_complex[j*2+0] + fft_complex[j*2+1]*fft_complex[j*2+1])) / (signal_lenght/2);
    }
    fft_complex[0] = fft_complex[0] / 2;
    // Copy result in fft array
    memcpy(fft, fft_complex, (signal_lenght / 2) * sizeof(float));
}

void FFTFrequency(float sample_freq, uint16_t signal_lenght, float * f){
    float freq_step = sample_freq / (float)signal_lenght;
    for(uint16_t i=0; i<(signal_lenght/2); i++){
        f[i] = i * freq_step;
    }
}

/*==================[end of file]============================================*/