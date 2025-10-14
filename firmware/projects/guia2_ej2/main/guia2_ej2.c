/*! @mainpage guia2_ej2.c
 *
 * @section genDesc General Description
 *
 * Este programa utiliza FreeRTOS para gestionar tareas relacionadas con la medición de distancia utilizando un sensor ultrasónico HC-SR04 y el control de LEDs y una pantalla LCD. El sistema está diseñado para utilizar temporizadores y notificaciones de tareas para gestionar la medición de distancia y la actualización de la pantalla LCD basada en eventos de temporizador.
 *
 * @section hardConn Hardware Connection
 *
 * | Peripheral   | ESP32   |
 * |:-------------|:--------|
 * | SENSOR_TRIG   | GPIO_3  |
 * | SENSOR_ECHO   | GPIO_2  |
 * | SWITCH_1      | GPIO_X  |
 * | SWITCH_2      | GPIO_Y  |
 * | LED_1         | GPIO_A  |
 * | LED_2         | GPIO_B  |
 * | LED_3         | GPIO_C  |
 * | LCD           | GPIO_D  |
 *
 * @section changelog Changelog
 *
 * | Date       | Description                             |
 * |:-----------|:----------------------------------------|
 * | 24/09/2025 | Documentación creada para el ejercicio 2|
 *
 * @section author Author
 * 
 * Angeles Perez (maria.perez@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "math.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/
/**
 * @brief Periodo de la interrupción del temporizador A en microsegundos.
 */
#define CONFIG_BLINK_PERIOD1 1000000

/**
 * @brief Periodo de la interrupción del temporizador B en microsegundos.
 */
#define CONFIG_BLINK_PERIOD 500000

/*==================[internal data definition]===============================*/
/**
 * @brief Variable que indica si el sistema está en modo de encendido.
 */
bool hold = false;

/**
 * @brief Variable que indica si la medición de distancia está activada.
 */
bool encendido = true;

/**
 * @brief Valor de distancia medida por el sensor HC-SR04.
 */
uint16_t distancia = 0;

/**
 * @brief Handle para la tarea de medición de distancia.
 */
TaskHandle_t Mido = NULL;

/**
 * @brief Handle para la tarea de muestra de distancia.
 */
TaskHandle_t Muestro = NULL;
/*==================[internal functions declaration]=========================*/

/**
 * @brief Función que se ejecuta en la interrupción del temporizador A.
 * 
 * @param Parámetro opcional (no utilizado en esta función).
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR( Mido, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_1 */
}

/**
 * @brief Función invocada en la interrupción del timer B
 * @param Parámetro opcional (no utilizado en esta función).
 */
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR(Muestro, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_2 */
}

/**
 * @brief Tarea de FreeRTOS que mide la distancia utilizando el sensor HC-SR04.
 * 
 * Esta función lee continuamente la distancia del sensor y actualiza la variable `distancia`.
 * 
 * @param pvParameter Parámetro opcional (no se utiliza en esta función).
 */


void medirdistancia(void *pvParameter)
{
	while (true)
	{
		 ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 
		if(encendido)
			{
				distancia=HcSr04ReadDistanceInCentimeters();
			}		
	}

}


/**
 * @brief Función que cambia el estado de la variable `encendido`.
 */

void CambioEncendido()
{
	encendido=!encendido;
}

/**
 * @brief Función que cambia el estado de la variable `hold`.
 */

void CambioHold()
{
	hold=!hold;
}

/**
 * @brief Controla el estado de los LEDs en función de la distancia medida.
 * 
 * Esta función enciende o apaga los LEDs basándose en el valor de `distancia`.
 */

void Leds()
{

			if (distancia < 10)
			{
				LedOff(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			if (distancia >= 10 && distancia < 20)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
				if (distancia >= 20 && distancia < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}
				if (distancia >= 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);

			}
}

/**
 * @brief Tarea de FreeRTOS que muestra la distancia en la pantalla LCD y controla los LEDs.
 * 
 * Esta función actualiza el estado de los LEDs y la pantalla LCD en función de la distancia medida y el estado de `encendido` y `hold`.
 * 
 * @param pvParameter Parámetro opcional (no se utiliza en esta función).
 */


void mostrarDistancia(void *pvParameter)
{
	while (true)
	{	
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 
		if(encendido)
			{
				Leds();
			if (!hold)
				{
					LcdItsE0803Write(distancia);
				}
			}
			 else 
				{
					LcdItsE0803Off();
					LedsOffAll();
				}
	}
}

/*==================[external functions definition]==========================*/
/**
 * @brief Función principal que inicializa el sistema y las tareas de FreeRTOS.
 * 
 * Esta función configura los LEDs, los interruptores, la pantalla LCD y el sensor HC-SR04. Luego crea las tareas de FreeRTOS para medir la distancia y mostrar la distancia en la pantalla LCD.
 */

void app_main(void)
{
LedsInit();
timer_config_t timer_led_1 = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_led_1);
    timer_config_t timer_led_2 = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD1,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_led_2);

SwitchesInit();
LcdItsE0803Init();
HcSr04Init(GPIO_3,GPIO_2);
SwitchActivInt(SWITCH_1,CambioEncendido,NULL);
SwitchActivInt(SWITCH_2,CambioHold,NULL);

xTaskCreate(&medirdistancia, "mide", 2048, NULL, 5, &Mido);
xTaskCreate(&mostrarDistancia,"muestra",512,NULL,5, &Muestro);
 TimerStart(timer_led_1.timer);
 TimerStart(timer_led_2.timer);
}