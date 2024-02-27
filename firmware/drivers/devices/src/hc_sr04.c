/**
 * @file hc_sr04.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "hc_sr04.h"
#include "delay_mcu.h"
/*==================[macros and definitions]=================================*/
#define MAX_US		17700	/* maximun distance time in us (300cm or 118inch) */
#define MAX_CM		300		/* maximun distance time in cm */
#define MAX_INCH	118		/* maximun distance time in inch */
#define US2CM		59		/* scale factor to conver pulse width to cm */
#define US2INCH		150		/* scale factor to conver pulse width to inch */
#define WAIT_MAX	5900	/* maximun time to wait for echo signal */
/*==================[internal data declaration]==============================*/
static gpio_t echo_st, trigger_st; /**<  Stores the pin inicilization*/
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

bool HcSr04Init(gpio_t echo, gpio_t trigger){
	echo_st = echo;
	trigger_st = trigger;

	/** Configuration of the GPIO pins*/
	GPIOInit(echo, GPIO_INPUT);
	GPIOInit(trigger, GPIO_OUTPUT);

	return true;
}

uint16_t HcSr04ReadDistanceInCentimeters(void){
	uint16_t distance = 0, waiting = 0;
	GPIOOn(trigger_st);
	DelayUs(10);
	GPIOOff(trigger_st);
	while(!GPIORead(echo_st)){
		DelayUs(1);
		waiting++;
		if(waiting > WAIT_MAX){
			return 0;
		}
	}
	do{
		DelayUs(1);
		distance++;
		if(distance > MAX_US)
			return MAX_CM;
	}
	while(GPIORead(echo_st));
	return (distance/US2CM);
}

uint16_t HcSr04ReadDistanceInInches(void){
	uint16_t distance = 0, waiting = 0;
	GPIOOn(trigger_st);
	DelayUs(10);
	GPIOOff(trigger_st);
	while(!GPIORead(echo_st)){
		DelayUs(1);
		waiting++;
		if(waiting > WAIT_MAX){
			return 0;
		}
	}
	do{
		DelayUs(1);
		distance++;
		if(distance > MAX_US)
			return MAX_INCH;
	}
	while(GPIORead(echo_st));
	return (distance/US2INCH);
}

bool HcSr04Deinit(void){
	GPIODeinit();
	return true;
}

/*==================[end of file]============================================*/
