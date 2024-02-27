/**
 * @file pwm_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "pwm_mcu.h"
#include "driver/ledc.h"
/*==================[macros and definitions]=================================*/
#define DC_MAX  1023
#define DC_100  100
/*==================[internal data declaration]==============================*/
static ledc_timer_config_t pwm_timer_cfg = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .duty_resolution  = LEDC_TIMER_10_BIT,
    .clk_cfg          = LEDC_AUTO_CLK
};
static ledc_channel_config_t ledc_channel_cfg = {
    .speed_mode     = LEDC_LOW_SPEED_MODE,
    .intr_type      = LEDC_INTR_DISABLE,
    .duty           = 0,       /*!< Starts in 0% */
    .hpoint         = 0
};
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
uint8_t PWMInit(pwm_out_t out, gpio_t gpio, uint16_t freq){
    switch(out){
        case PWM_0:
            pwm_timer_cfg.freq_hz = freq;
            pwm_timer_cfg.timer_num = LEDC_TIMER_0;
            ledc_timer_config(&pwm_timer_cfg);
            ledc_channel_cfg.channel = LEDC_CHANNEL_0;
            ledc_channel_cfg.timer_sel = LEDC_TIMER_0;
            ledc_channel_cfg.gpio_num = gpio;
            ledc_channel_config(&ledc_channel_cfg);
            break;
        case PWM_1:
            pwm_timer_cfg.freq_hz = freq;
            pwm_timer_cfg.timer_num = LEDC_TIMER_1;
            ledc_timer_config(&pwm_timer_cfg);
            ledc_channel_cfg.channel = LEDC_CHANNEL_1;
            ledc_channel_cfg.timer_sel = LEDC_TIMER_1;
            ledc_channel_cfg.gpio_num = gpio;
            ledc_channel_config(&ledc_channel_cfg);
            break;
        case PWM_2:
            pwm_timer_cfg.freq_hz = freq;
            pwm_timer_cfg.timer_num = LEDC_TIMER_2;
            ledc_timer_config(&pwm_timer_cfg);
            ledc_channel_cfg.channel = LEDC_CHANNEL_2;
            ledc_channel_cfg.timer_sel = LEDC_TIMER_2;
            ledc_channel_cfg.gpio_num = gpio;
            ledc_channel_config(&ledc_channel_cfg);
            break;
        case PWM_3:
            pwm_timer_cfg.freq_hz = freq;
            pwm_timer_cfg.timer_num = LEDC_TIMER_3;
            ledc_timer_config(&pwm_timer_cfg);
            ledc_channel_cfg.channel = LEDC_CHANNEL_3;
            ledc_channel_cfg.timer_sel = LEDC_TIMER_3;
            ledc_channel_cfg.gpio_num = gpio;
            ledc_channel_config(&ledc_channel_cfg);
            break;
    }
    return 0;
}

void PWMOn(pwm_out_t out){
    switch(out){
        case PWM_0:
            ledc_timer_resume(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
            break;
        case PWM_1:
            ledc_timer_resume(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1);
            break;
        case PWM_2:
            ledc_timer_resume(LEDC_LOW_SPEED_MODE, LEDC_TIMER_2);
            break;
        case PWM_3:
            ledc_timer_resume(LEDC_LOW_SPEED_MODE, LEDC_TIMER_3);
            break;
    }
}

void PWMOff(pwm_out_t out){
    switch(out){
        case PWM_0:
            ledc_timer_pause(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
            break;
        case PWM_1:
            ledc_timer_pause(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1);
            break;
        case PWM_2:
            ledc_timer_pause(LEDC_LOW_SPEED_MODE, LEDC_TIMER_2);
            break;
        case PWM_3:
            ledc_timer_pause(LEDC_LOW_SPEED_MODE, LEDC_TIMER_3);
            break;
    }
}

void PWMSetDutyCycle(pwm_out_t out, uint8_t duty_cycle){
    if(duty_cycle > DC_100){
        duty_cycle = DC_100;
    }
    uint32_t dc = ((uint32_t)duty_cycle * DC_MAX) / DC_100;
    switch(out){
        case PWM_0:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, dc);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
            break;
        case PWM_1:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, dc);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1);
            break;
        case PWM_2:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_2, dc);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_2);
            break;
        case PWM_3:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_3, dc);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_TIMER_3);
            break;
    }
}

uint8_t PWMSetFreq(pwm_out_t out, uint32_t freq){
    switch(out){
        case PWM_0:
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
            break;
        case PWM_1:
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, freq);
            break;
        case PWM_2:
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_2, freq);
            break;
        case PWM_3:
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_3, freq);
            break;
    }
    return 0;
}

uint8_t PWMDeinit(pwm_out_t out){
    switch(out){
        case PWM_0:
            ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            break;
        case PWM_1:
            ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
            break;
        case PWM_2:
            ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
            break;
        case PWM_3:
            ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, 0);
            break;
    }
    return 0;
}

/*==================[end of file]============================================*/