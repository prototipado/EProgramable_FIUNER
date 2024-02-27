/**
 * @file switch.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "switch.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define GPIO_SWITCH1 GPIO_4
#define GPIO_SWITCH2 GPIO_15
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
int8_t SwitchesInit(void){
	/* GPIO configurations */
	GPIOInit(GPIO_SWITCH1, GPIO_INPUT);	// SWITCH_1
	GPIOInit(GPIO_SWITCH2, GPIO_INPUT);	// SWITCH_2
	GPIOInputFilter(GPIO_SWITCH1);		
	GPIOInputFilter(GPIO_SWITCH2);
	return true;
}

int8_t SwitchesRead(void){
	int8_t mask = 0;
	if (!GPIORead(GPIO_SWITCH1))
		  mask |= SWITCH_1;
	if (!GPIORead(GPIO_SWITCH2))
		  mask |= SWITCH_2;
	return mask;
}

void SwitchActivInt(switch_t sw, void *ptr_int_func, void *args){
	switch(sw){
		case SWITCH_1:
			GPIOActivInt(GPIO_SWITCH1, ptr_int_func, false, args);
		break;
		case SWITCH_2:
			GPIOActivInt(GPIO_SWITCH2, ptr_int_func, false, args);
		break;
	}
}
/*==================[end of file]============================================*/
