/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 10/09/2025 | Document creation		                         |
 *
 * @author Angeles Perez (maria.perez@ingenieria.uner.edu.ar)
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
/*==================[macros and definitions]=================================*/
/**
 * @brief Periodo de parpadeo en milisegundos para la tarea de medición de distancia.
 */
#define CONFIG_BLINK_PERIOD1 1000

/**
 * @brief Periodo de parpadeo en milisegundos para la tarea de lectura de interruptores.
 */
#define CONFIG_BLINK_PERIOD 100

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

/*==================[internal functions declaration]=========================*/
/**
 * @brief Tarea de FreeRTOS que mide la distancia utilizando el sensor HC-SR04.
 * 
 * Esta función lee continuamente la distancia del sensor y actualiza la variable `distancia`.
 * 
 * @param pvParameter Parámetro opcional (no se usa en este caso).
 */

void medirdistancia(void *pvParameter)
{
	while (true)
	{
		if(encendido)
			{
				distancia=HcSr04ReadDistanceInCentimeters();
			}		
		vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);
	}
	
}

/**
 * @brief Tarea de FreeRTOS que lee el estado de los interruptores.
 * 
 * Esta función lee continuamente el estado de los interruptores y modifica las variables `encendido` y `hold` en función de las teclas presionadas.
 * 
 * @param pvParameter Parámetro opcional (no se usa en este caso).
 */

void valorteclas(void *pvParameter)
{
	uint8_t teclas;
	while(true)
	{
		teclas=SwitchesRead();
		switch (teclas)
		{
			case SWITCH_1:
			encendido=!encendido;
			break;
	
			case SWITCH_2:
			hold=!hold;
			break;
		}
	vTaskDelay(CONFIG_BLINK_PERIOD/portTICK_PERIOD_MS);	
	}
}
// deberia hacer con void isr tecla, y sustituir esta funcion por eso y flag hold
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
 * @param pvParameter Parámetro opcional (no se usa en este caso).
 */

void mostrarDistancia(void *pvParameter)
{
	
	while (true)
	{	
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
		vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);
	}
}

/*==================[external functions definition]==========================*/
/**
 * @brief Función principal que inicializa el sistema y las tareas de FreeRTOS.
 * 
 * Esta función configura los LEDs, los interruptores, la pantalla LCD y el sensor HC-SR04. Luego crea las tareas de FreeRTOS para medir la distancia, leer el estado de los interruptores y mostrar la distancia.
 */

void app_main(void)
{
LedsInit();
SwitchesInit();
LcdItsE0803Init();
HcSr04Init(GPIO_3,GPIO_2);

xTaskCreate(&medirdistancia, "mide", 2048, NULL, 5, NULL);
xTaskCreate(&valorteclas, "teclas", 512, NULL, 5, NULL);
xTaskCreate(&mostrarDistancia,"muestra",512,NULL,5,NULL);
}
