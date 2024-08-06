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
#include "freertos/semphr.h"
#include "esp_rom_sys.h"
/*==================[macros and definitions]=================================*/
#define US_RESOLUTION_HZ	1000000	/*!< 1usec */
#define MSEC				1000	/*!< 1msec = 1000usec */
#define SEC					1000000	/*!< 1sec = 1000msec */
#define MIN_US				50	    /*!< minimun delay in usec to use gptimer */
#define MIN_MS				100	    /*!< minimun delay in msec to use vTaskDelay */
/*==================[internal data declaration]==============================*/
SemaphoreHandle_t xDelaySemaphore = NULL;
/*==================[internal functions declaration]=========================*/
static bool IRAM_ATTR delay_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	gptimer_stop(timer);
	xSemaphoreGiveFromISR(xDelaySemaphore, &xHigherPriorityTaskWoken);
	return (xHigherPriorityTaskWoken == pdTRUE);
}
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void DelaySec(uint16_t sec){
    vTaskDelay(sec * MSEC / portTICK_PERIOD_MS);
}

void DelayMs(uint16_t msec){
    // If the delay is too short, use the ESP32's internal timer
    if(msec<=MIN_MS){ 
        if(xDelaySemaphore == NULL){
            xDelaySemaphore = xSemaphoreCreateBinary();
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
            if(xDelaySemaphore != NULL){
                xSemaphoreTake(xDelaySemaphore, portMAX_DELAY);
                xDelaySemaphore = NULL;
            }
            gptimer_disable(delay_timer);
            gptimer_del_timer(delay_timer);
        }
    }else{       
        // If the delay is longer than the minimum delay, use vTaskDelay
        vTaskDelay(msec / portTICK_PERIOD_MS);
    }
}

void DelayUs(uint16_t usec){
    if(usec<=MIN_US){
        /* If the delay is too short, use the ROM delay function */
        esp_rom_delay_us(usec);
    }else{
        /* If the delay is longer than the minimum, use the ESP32's internal timer */
        if(xDelaySemaphore == NULL){
	        xDelaySemaphore = xSemaphoreCreateBinary();
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
            if(xDelaySemaphore != NULL){
                /* Wait for the timer to finish */
                xSemaphoreTake(xDelaySemaphore, portMAX_DELAY);
                xDelaySemaphore = NULL;
            }
            gptimer_disable(delay_timer);
            gptimer_del_timer(delay_timer);
        }
    }
}

