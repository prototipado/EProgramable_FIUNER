/**
 * @file lcditse0803.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "lcditse0803.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define GPIO_BCD_1	GPIO_20
#define GPIO_BCD_2	GPIO_21
#define GPIO_BCD_3	GPIO_22
#define GPIO_BCD_4	GPIO_23
#define GPIO_SEL_1	GPIO_19
#define GPIO_SEL_2	GPIO_18
#define GPIO_SEL_3	GPIO_9
/*==================[internal data definition]===============================*/
static uint16_t actual_value = 0; /*variable that saves the value to be shown in the display LCD*/
/*==================[internal functions declaration]=========================*/
/** @brief Aux function to load a digit to the LCD Display
 *
 */
bool LcdItsE0803BCDtoPin(uint8_t value){
	GPIOState(GPIO_BCD_1, (value & (1<<0))>>0);
	GPIOState(GPIO_BCD_2, (value & (1<<1))>>1);
	GPIOState(GPIO_BCD_3, (value & (1<<2))>>2);
	GPIOState(GPIO_BCD_4, (value & (1<<3))>>3);
	return true;
}
/*==================[external functions definition]==========================*/
bool LcdItsE0803Init(void){
	/* Configuration of pins of data*/
	GPIOInit(GPIO_BCD_1, GPIO_OUTPUT);
	GPIOInit(GPIO_BCD_2, GPIO_OUTPUT);
	GPIOInit(GPIO_BCD_3, GPIO_OUTPUT);
	GPIOInit(GPIO_BCD_4, GPIO_OUTPUT);

	/* Configuration of pins of control*/
	GPIOInit(GPIO_SEL_1, GPIO_OUTPUT);
	GPIOInit(GPIO_SEL_2, GPIO_OUTPUT);
	GPIOInit(GPIO_SEL_3, GPIO_OUTPUT);

	actual_value=0;
	LcdItsE0803Write(actual_value);
	return true;
};

bool LcdItsE0803Write(uint16_t value) {
	uint8_t units, tens, hundreds;
	if(value<1000)	 {
		actual_value = value;

		hundreds = value/100;
		tens = (value-(hundreds*100))/10;
		units = (value-(hundreds*100)-(tens*10));

		/* Write hundreds */
		LcdItsE0803BCDtoPin(hundreds);
		GPIOOn(GPIO_SEL_1);
		GPIOOff(GPIO_SEL_1);

		/* Write tens */
		LcdItsE0803BCDtoPin(tens);
		GPIOOn(GPIO_SEL_2);
		GPIOOff(GPIO_SEL_2);

		/* Write units */
		LcdItsE0803BCDtoPin(units);
		GPIOOn(GPIO_SEL_3);
		GPIOOff(GPIO_SEL_3);
		return true; /* return 1 for values lower than 999 */
	}
	else
		return false; /* return 0 for values higher than 999 */
}

uint16_t LcdItsE0803Read(void){
	return (actual_value);
}

void LcdItsE0803Off(void){
	LcdItsE0803BCDtoPin(0x0F);
	GPIOOn(GPIO_SEL_1);
	GPIOOff(GPIO_SEL_1);

	LcdItsE0803BCDtoPin(0x0F);
	GPIOOn(GPIO_SEL_2);
	GPIOOff(GPIO_SEL_2);

	LcdItsE0803BCDtoPin(0x0F);
	GPIOOn(GPIO_SEL_3);
	GPIOOff(GPIO_SEL_3);
}

bool LcdItsE0803DeInit(void){
	GPIODeinit();
	return true;
}

/*==================[end of file]============================================*/
