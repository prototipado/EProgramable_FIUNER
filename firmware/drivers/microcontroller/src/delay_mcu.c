/**
 * @file delay_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "delay_mcu.h"
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
/*==================[macros and definitions]=================================*/
#define US_RESOLUTION_HZ	1000000	/*!< 1usec */
#define MSEC				1000	/*!< 1msec = 1000usec */
#define SEC					1000000	/*!< 1sec = 1000msec */
#define MIN_US				50	    /*!< minimun delay in usec to use gptimer */
/*==================[internal data declaration]==============================*/
TaskHandle_t task_to_notify;
/*==================[internal functions declaration]=========================*/
static bool IRAM_ATTR delay_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	gptimer_stop(timer);
	vTaskNotifyGiveFromISR(task_to_notify, &xHigherPriorityTaskWoken);
	return (xHigherPriorityTaskWoken == pdTRUE);
}
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void DelaySec(uint16_t sec){
	task_to_notify = xTaskGetCurrentTaskHandle();
	gptimer_handle_t delay_timer = NULL;
    gptimer_config_t delay_timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = US_RESOLUTION_HZ,
    };
    gptimer_new_timer(&delay_timer_config, &delay_timer);
    gptimer_event_callbacks_t delay_alarm = {
        .on_alarm = delay_isr,
    };
	gptimer_register_event_callbacks(delay_timer, &delay_alarm, NULL);
	gptimer_enable(delay_timer);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = sec*SEC, 
    };
	gptimer_set_alarm_action(delay_timer, &alarm_config);
	gptimer_start(delay_timer);
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	gptimer_disable(delay_timer);
	gptimer_del_timer(delay_timer);
}

void DelayMs(uint16_t msec){
	task_to_notify = xTaskGetCurrentTaskHandle();
	gptimer_handle_t delay_timer = NULL;
    gptimer_config_t delay_timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = US_RESOLUTION_HZ,
    };
    gptimer_new_timer(&delay_timer_config, &delay_timer);
    gptimer_event_callbacks_t delay_alarm = {
        .on_alarm = delay_isr,
    };
	gptimer_register_event_callbacks(delay_timer, &delay_alarm, NULL);
	gptimer_enable(delay_timer);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = msec*MSEC, 
    };
	gptimer_set_alarm_action(delay_timer, &alarm_config);
	gptimer_start(delay_timer);
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	gptimer_disable(delay_timer);
	gptimer_del_timer(delay_timer);
}

void DelayUs(uint16_t usec){
    if(usec<=MIN_US){
        esp_rom_delay_us(usec);
    }else{
        task_to_notify = xTaskGetCurrentTaskHandle();
        gptimer_handle_t delay_timer = NULL;
        gptimer_config_t delay_timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = US_RESOLUTION_HZ,
        };
        gptimer_new_timer(&delay_timer_config, &delay_timer);
        gptimer_event_callbacks_t delay_alarm = {
            .on_alarm = delay_isr,
        };
        gptimer_register_event_callbacks(delay_timer, &delay_alarm, NULL);
        gptimer_enable(delay_timer);
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = usec, 
        };
        gptimer_set_alarm_action(delay_timer, &alarm_config);
        gptimer_start(delay_timer);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        gptimer_disable(delay_timer);
        gptimer_del_timer(delay_timer);
    }
}
