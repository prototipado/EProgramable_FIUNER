/**
 * @file led.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "led.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define GPIO_LED1 GPIO_11
#define GPIO_LED2 GPIO_10
#define GPIO_LED3 GPIO_5
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

uint8_t LedsInit(void){
	/** Configuration of the GPIO */
	GPIOInit(GPIO_LED1, GPIO_OUTPUT);
	GPIOInit(GPIO_LED2, GPIO_OUTPUT);
	GPIOInit(GPIO_LED3, GPIO_OUTPUT);

	/** Turn off leds*/
	GPIOOff(GPIO_LED1);
	GPIOOff(GPIO_LED2);
	GPIOOff(GPIO_LED3);

	return true;
}

/** \brief Function to turn on a specific led */
uint8_t LedOn(led_t led){
	uint8_t result = false;
	switch (led){
	case LED_1:
		GPIOOn(GPIO_LED1);
		result = true;
		break;
	case LED_2:
		GPIOOn(GPIO_LED2);
		result = true;
		break;
	case LED_3:
		GPIOOn(GPIO_LED3);
		result = true;
		break;
	}
	return result;
}

uint8_t LedOff(led_t led){
	uint8_t result = false;
	switch (led){
	case LED_1:
		GPIOOff(GPIO_LED1);
		result = true;
		break;
	case LED_2:
		GPIOOff(GPIO_LED2);
		result = true;
		break;
	case LED_3:
		GPIOOff(GPIO_LED3);
		result = true;
		break;
	}
	return result;
}

uint8_t LedToggle(led_t led){
	uint8_t result = false;

	switch (led){
	case LED_1:
		GPIOToggle(GPIO_LED1);
		result = true;
		break;
	case LED_2:
		GPIOToggle(GPIO_LED2);
		result = true;
		break;
	case LED_3:
		GPIOToggle(GPIO_LED3);
		result = true;
		break;
	}

	return result;
}

uint8_t LedsOffAll(void){
	GPIOOff(GPIO_LED1);
	GPIOOff(GPIO_LED2);
	GPIOOff(GPIO_LED3);
	
	return true;
}

uint8_t LedsMask(uint8_t mask){
	GPIOState(GPIO_LED1, (mask & LED_1) >> 2);
	GPIOState(GPIO_LED2, (mask & LED_2) >> 1);
	GPIOState(GPIO_LED3, (mask & LED_3));
	return true;
}

/*==================[end of file]============================================*/
