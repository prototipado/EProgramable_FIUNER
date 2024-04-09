/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 and LED_2 blink at different rates, using FreeRTOS tasks and timer interrupts.
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1_US 1000000
#define CONFIG_BLINK_PERIOD_LED_2_US 1300000
/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
TaskHandle_t led2_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(led1_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_1 */
}

/**
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR(led2_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_2 */
}

/**
 * @brief Tarea encargada de blinkear el LED_1
 */
static void Led1Task(void *pvParameter){
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
        printf("LED_1 Toggle\n");
        LedToggle(LED_1);
    }
}

/**
 * @brief Tarea encargada de blinkear el LED_2
 */
static void Led2Task(void *pvParameter){
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
        printf("LED_2 Toggle\n");
        LedToggle(LED_2);
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
    LedsInit();
    /* Inicialización de timers */
    timer_config_t timer_led_1 = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_LED_1_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_led_1);
    timer_config_t timer_led_2 = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_LED_2_US,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_led_2);
    /* Creación de tareas */
    xTaskCreate(&Led1Task, "LED_1", 512, NULL, 5, &led1_task_handle);
    xTaskCreate(&Led2Task, "LED_2", 512, NULL, 5, &led2_task_handle);
    /* Inicialización del conteo de timers */
    TimerStart(timer_led_1.timer);
    TimerStart(timer_led_2.timer);
}
