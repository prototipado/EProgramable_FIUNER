/**
 * @file iir_filter.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-03-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "iir_filter.h"
#include "esp_dsp.h"
/*==================[macros and definitions]=================================*/
#define N_SOS       5
#define N_DELAY     2
// 2nd order Butterworth 
#define ORDER2_Q    (1 / 1.414)
// 4th order Butterworth 
#define ORDER4_Q1   (1 / 0.765)
#define ORDER4_Q2   (1 / 1.848)
// 6th order Butterworth 
#define ORDER6_Q1   (1 / 0.518)
#define ORDER6_Q2   (1 / 1.414)
#define ORDER6_Q3   (1 / 1.932)
// 8th order Butterworth 
#define ORDER8_Q1   (1 / 0.390)
#define ORDER8_Q2   (1 / 1.111)
#define ORDER8_Q3   (1 / 1.663)
#define ORDER8_Q4   (1 / 1.962)
/*==================[internal data declaration]==============================*/
static uint8_t lp_order, hp_order;
float hp2_delay[N_DELAY] = {0, 0};  // delay
float hp4_delay[N_DELAY] = {0, 0};  // delay
float hp6_delay[N_DELAY] = {0, 0};  // delay
float hp8_delay[N_DELAY] = {0, 0};  // delay
float lp2_delay[N_DELAY] = {0, 0};  // delay
float lp4_delay[N_DELAY] = {0, 0};  // delay
float lp6_delay[N_DELAY] = {0, 0};  // delay
float lp8_delay[N_DELAY] = {0, 0};  // delay
float lp2_sos_coeff[N_SOS]; 
float lp4_sos_coeff[N_SOS]; 
float lp6_sos_coeff[N_SOS]; 
float lp8_sos_coeff[N_SOS]; 
float hp2_sos_coeff[N_SOS]; 
float hp4_sos_coeff[N_SOS]; 
float hp6_sos_coeff[N_SOS]; 
float hp8_sos_coeff[N_SOS]; 
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void LowPassInit(float sample_frec, float cut_frec, filter_order_t order){
    float f = cut_frec / sample_frec;
    lp_order = order;
    switch(lp_order){
        case ORDER_2:
            dsps_biquad_gen_lpf_f32(lp2_sos_coeff, f, ORDER2_Q);
        break;
        case ORDER_4:
            dsps_biquad_gen_lpf_f32(lp2_sos_coeff, f, ORDER4_Q1);
            dsps_biquad_gen_lpf_f32(lp4_sos_coeff, f, ORDER4_Q2);
        break;
        case ORDER_6:
            dsps_biquad_gen_lpf_f32(lp2_sos_coeff, f, ORDER6_Q1);
            dsps_biquad_gen_lpf_f32(lp4_sos_coeff, f, ORDER6_Q2);
            dsps_biquad_gen_lpf_f32(lp6_sos_coeff, f, ORDER6_Q3);
        break;
        case ORDER_8:
            dsps_biquad_gen_lpf_f32(lp2_sos_coeff, f, ORDER8_Q1);
            dsps_biquad_gen_lpf_f32(lp4_sos_coeff, f, ORDER8_Q2);
            dsps_biquad_gen_lpf_f32(lp6_sos_coeff, f, ORDER8_Q3);
            dsps_biquad_gen_lpf_f32(lp8_sos_coeff, f, ORDER8_Q4);
        break;
    }
}

void HiPassInit(float sample_frec, float cut_frec, filter_order_t order){
    float f = cut_frec / sample_frec;
    hp_order = order;
    switch(hp_order){
        case ORDER_2:
            dsps_biquad_gen_hpf_f32(hp2_sos_coeff, f, ORDER2_Q);
        break;
        case ORDER_4:
            dsps_biquad_gen_hpf_f32(hp2_sos_coeff, f, ORDER4_Q1);
            dsps_biquad_gen_hpf_f32(hp4_sos_coeff, f, ORDER4_Q2);
        break;
        case ORDER_6:
            dsps_biquad_gen_hpf_f32(hp2_sos_coeff, f, ORDER6_Q1);
            dsps_biquad_gen_hpf_f32(hp4_sos_coeff, f, ORDER6_Q2);
            dsps_biquad_gen_hpf_f32(hp6_sos_coeff, f, ORDER6_Q3);
        break;
        case ORDER_8:
            dsps_biquad_gen_hpf_f32(hp2_sos_coeff, f, ORDER8_Q1);
            dsps_biquad_gen_hpf_f32(hp4_sos_coeff, f, ORDER8_Q2);
            dsps_biquad_gen_hpf_f32(hp6_sos_coeff, f, ORDER8_Q3);
            dsps_biquad_gen_hpf_f32(hp8_sos_coeff, f, ORDER8_Q4);
        break;
    }
}

void LowPassFilter(float * input_signal, float * output_signal, int16_t signal_lenght){
    switch(lp_order){
        case ORDER_2:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, lp2_sos_coeff, lp2_delay);
        break;
        case ORDER_4:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, lp2_sos_coeff, lp2_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, lp4_sos_coeff, lp4_delay);
        break;
        case ORDER_6:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, lp2_sos_coeff, lp2_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, lp4_sos_coeff, lp4_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, lp6_sos_coeff, lp6_delay);
        break;
        case ORDER_8:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, lp2_sos_coeff, lp2_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, lp4_sos_coeff, lp4_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, lp6_sos_coeff, lp6_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, lp8_sos_coeff, lp8_delay);
        break;
    }
}

void HiPassFilter(float * input_signal, float * output_signal, int16_t signal_lenght){
    switch(hp_order){
        case ORDER_2:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, hp2_sos_coeff, hp2_delay);
        break;
        case ORDER_4:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, hp2_sos_coeff, hp2_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, hp4_sos_coeff, hp4_delay);
        break;
        case ORDER_6:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, hp2_sos_coeff, hp2_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, hp4_sos_coeff, hp4_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, hp6_sos_coeff, hp6_delay);
        break;
        case ORDER_8:
            dsps_biquad_f32(input_signal, output_signal, signal_lenght, hp2_sos_coeff, hp2_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, hp4_sos_coeff, hp4_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, hp6_sos_coeff, hp6_delay);
            dsps_biquad_f32(output_signal, output_signal, signal_lenght, hp8_sos_coeff, hp8_delay);
        break;
    }
}

/*==================[end of file]============================================*/