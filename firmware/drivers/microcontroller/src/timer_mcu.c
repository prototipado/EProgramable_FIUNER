/**
 * @file timer_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "timer_mcu.h"
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/
#define US_RESOLUTION_HZ	1000000	/*!< 1usec */
#define RESET_COUNT_VALUE	0		/*!< Reset timer count to 0 */
/*==================[internal data declaration]==============================*/
gptimer_handle_t timer_a = NULL;	/*!< Handle for timer A */	
gptimer_handle_t timer_b = NULL;	/*!< Handle for timer B */			
gptimer_handle_t timer_c = NULL;	/*!< Handle for timer C */	
/**
 * @brief Configuration for the timer
 * 
 * @details The configuration for the timer specifies the clock source,
 *          count direction, and resolution in Hz.
 */
const gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,	/*!< Default clock source */
    .direction = GPTIMER_COUNT_UP,		/*!< Count up */
    .resolution_hz = US_RESOLUTION_HZ,	/*!< Resolution in Hz */
};
void (*timer_a_isr_p)(void*);	        /*!< Pointer to the ISR function for timer A */	
void (*timer_b_isr_p)(void*);	        /*!< Pointer to the ISR function for timer B */	
void (*timer_c_isr_p)(void*);	        /*!< Pointer to the ISR function for timer C */	


void *timer_a_user_data;	            /*!< User data for timer A */	
void *timer_b_user_data;	            /*!< User data for timer B */	
void *timer_c_user_data;	            /*!< User data for timer C */	

gptimer_alarm_config_t alarm_config_a;  /*!< Configuration for alarm A */
gptimer_alarm_config_t alarm_config_b;	/*!< Configuration for alarm B */
gptimer_alarm_config_t alarm_config_c;	/*!< Configuration for alarm C */
/*==================[internal functions declaration]=========================*/
static bool IRAM_ATTR timer_a_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
	timer_a_isr_p(timer_a_user_data);
	return true;
}
static bool IRAM_ATTR timer_b_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
	timer_b_isr_p(timer_b_user_data);
	return true;
}
static bool IRAM_ATTR timer_c_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
	timer_c_isr_p(timer_c_user_data);
	return true;
}
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void TimerInit(timer_config_t *timer_ini){
	switch(timer_ini->timer){
	 	case TIMER_A:
			timer_a_isr_p = timer_ini->func_p;
			timer_a_user_data = timer_ini->param_p;
	 		gptimer_new_timer(&timer_config, &timer_a);
			alarm_config_a.alarm_count = timer_ini->period; 
			alarm_config_a.reload_count = RESET_COUNT_VALUE;
			alarm_config_a.flags.auto_reload_on_alarm = true;
			gptimer_set_alarm_action(timer_a, &alarm_config_a);
			gptimer_event_callbacks_t alarm_a = {
				.on_alarm = timer_a_isr,
			};
			gptimer_register_event_callbacks(timer_a, &alarm_a, NULL);
			gptimer_enable(timer_a);
	 	break;

	 	case TIMER_B:
			timer_b_isr_p = timer_ini->func_p;
			timer_b_user_data = timer_ini->param_p;
	 		gptimer_new_timer(&timer_config, &timer_b);
			alarm_config_b.alarm_count = timer_ini->period; 
			alarm_config_b.reload_count = RESET_COUNT_VALUE;
			alarm_config_b.flags.auto_reload_on_alarm = true;
			gptimer_set_alarm_action(timer_b, &alarm_config_b);
			gptimer_event_callbacks_t alarm_b = {
				.on_alarm = timer_b_isr,
			};
			gptimer_register_event_callbacks(timer_b, &alarm_b, NULL);
			gptimer_enable(timer_b);
	 	break;

	 	case TIMER_C:
			timer_c_isr_p = timer_ini->func_p;
			timer_c_user_data = timer_ini->param_p;
	 		gptimer_new_timer(&timer_config, &timer_c);
			alarm_config_c.alarm_count = timer_ini->period; 
			alarm_config_c.reload_count = RESET_COUNT_VALUE;
			alarm_config_c.flags.auto_reload_on_alarm = true;
			gptimer_set_alarm_action(timer_c, &alarm_config_c);
			gptimer_event_callbacks_t alarm_c = {
				.on_alarm = timer_c_isr,
			};
			gptimer_register_event_callbacks(timer_c, &alarm_c, NULL);
			gptimer_enable(timer_c);
	 	break;
	}
}

void TimerStart(timer_mcu_t timer){
	switch(timer){
	 	case TIMER_A:
	 		gptimer_start(timer_a);
		break;
	 	case TIMER_B:
	 		gptimer_start(timer_b);
		break;
	 	case TIMER_C:
	 		gptimer_start(timer_c);
		break;
	}
}

uint32_t TimerRead(timer_mcu_t timer){
	uint64_t raw_count = 0;
	switch(timer){
	 	case TIMER_A:
	 		gptimer_get_raw_count(timer_a, &raw_count);
	 	break;
	 	case TIMER_B:
	 		gptimer_get_raw_count(timer_b, &raw_count);
	 	break;
	 	case TIMER_C:
	 		gptimer_get_raw_count(timer_c, &raw_count);
	 	break;
	}
	return raw_count;
}

void TimerStop(timer_mcu_t timer){
	switch(timer){
	 	case TIMER_A:
	 		gptimer_stop(timer_a);
	 	break;
	 	case TIMER_B:
	 		gptimer_stop(timer_b);
	 	break;
	 	case TIMER_C:
	 		gptimer_stop(timer_c);
	 	break;
	}
}

void TimerReset(timer_mcu_t timer){
	switch(timer){
	 	case TIMER_A:
			gptimer_set_raw_count(timer_a, RESET_COUNT_VALUE);
	 	break;
	 	case TIMER_B:
			gptimer_set_raw_count(timer_b, RESET_COUNT_VALUE);
	 	break;
	 	case TIMER_C:
			gptimer_set_raw_count(timer_c, RESET_COUNT_VALUE);
	 	break;
	}
}

void TimerUpdatePeriod(timer_mcu_t timer, uint32_t period){
	switch(timer){
	 	case TIMER_A:
			alarm_config_a.alarm_count = period;
			gptimer_set_alarm_action(timer_a, &alarm_config_a);
	 	break;
	 	case TIMER_B:
			alarm_config_b.alarm_count = period;
			gptimer_set_alarm_action(timer_b, &alarm_config_b);
	 	break;
	 	case TIMER_C:
			alarm_config_c.alarm_count = period;
			gptimer_set_alarm_action(timer_c, &alarm_config_c);
	 	break;
	}
}

/*==================[end of file]============================================*/
