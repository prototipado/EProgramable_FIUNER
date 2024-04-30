/**
 * @file gpio_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "gpio_mcu.h"
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/gpio_filter.h"
/*==================[macros and definitions]=================================*/
#define GPIO_QTY 	24
#define FILTER_QTY	8
typedef struct{
	uint64_t pin;				/*!< GPIO pin */
	gpio_mode_t mode;			/*!< Input/Output mode */
	gpio_pull_mode_t pull;		/*!< GPIO pull-up/pull-down resistor */
	bool state;					/*!< GPIO output state */
} digital_io_t;
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
digital_io_t gpio_list[GPIO_QTY] = {
	{GPIO_NUM_0, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO0*/
	{GPIO_NUM_1, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO1*/
	{GPIO_NUM_2, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO2*/
	{GPIO_NUM_3, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO3*/
	{GPIO_NUM_4, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO4*/
	{GPIO_NUM_5, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO5*/
	{GPIO_NUM_6, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO6*/
	{GPIO_NUM_7, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO7*/
	{GPIO_NUM_8, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO8*/
	{GPIO_NUM_9, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO9*/
	{GPIO_NUM_10, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO10*/
	{GPIO_NUM_11, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO11*/
	{GPIO_NUM_12, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO12*/
	{GPIO_NUM_13, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO13*/
	{GPIO_NUM_14, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO14*/
	{GPIO_NUM_15, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO15*/
	{GPIO_NUM_16, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO16*/
	{GPIO_NUM_17, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO17*/
	{GPIO_NUM_18, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO18*/
	{GPIO_NUM_19, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO19*/
	{GPIO_NUM_20, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO20*/
	{GPIO_NUM_21, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO21*/
	{GPIO_NUM_22, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO22*/
	{GPIO_NUM_23, GPIO_MODE_DISABLE, GPIO_PULLUP_ONLY, false}, /* Configuration GPIO23*/
};
gpio_flex_glitch_filter_config_t filter_config = {
	.clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
	.window_width_ns = 700,
	.window_thres_ns = 600,
};
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void GPIOInit(gpio_t pin, io_t io){
	if((pin == GPIO_14) || (pin > GPIO_23)){
		return;
	}
	if(io == GPIO_INPUT){
		gpio_list[pin].mode = GPIO_MODE_INPUT;
	} else if(io == GPIO_OUTPUT){
		gpio_list[pin].mode = GPIO_MODE_OUTPUT;
	}
	gpio_reset_pin(gpio_list[pin].pin);
	gpio_set_direction(gpio_list[pin].pin, gpio_list[pin].mode);
	gpio_set_pull_mode(gpio_list[pin].pin, gpio_list[pin].pull);
}

void GPIOOn(gpio_t pin){
	gpio_list[pin].state = true;
	gpio_set_level(gpio_list[pin].pin, gpio_list[pin].state);
}

void GPIOOff(gpio_t pin){
	gpio_list[pin].state = false;
	gpio_set_level(gpio_list[pin].pin, gpio_list[pin].state);
}

void GPIOState(gpio_t pin, bool state){
	gpio_list[pin].state = state;
	gpio_set_level(gpio_list[pin].pin, gpio_list[pin].state);
}

void GPIOToggle(gpio_t pin){
	gpio_list[pin].state = !gpio_list[pin].state;
	gpio_set_level(gpio_list[pin].pin, gpio_list[pin].state);
}

bool GPIORead(gpio_t pin){
	return gpio_get_level(gpio_list[pin].pin);
}

void GPIOActivInt(gpio_t pin, void *ptr_int_func, bool edge, void *args){
	static bool isr_service_installed = false;
	if(edge){
		gpio_set_intr_type(gpio_list[pin].pin, GPIO_INTR_POSEDGE);
	} else{
		gpio_set_intr_type(gpio_list[pin].pin, GPIO_INTR_NEGEDGE);
	}
	if(!isr_service_installed){	
		gpio_install_isr_service(0);
		isr_service_installed = true;
	}
    gpio_isr_handler_add(gpio_list[pin].pin, ptr_int_func, (void *)args);	
}

void GPIOInputFilter(gpio_t pin){
	static uint8_t filter_count = 0;
	gpio_glitch_filter_handle_t filter;
	if(filter_count < FILTER_QTY){
		filter_count++;
		filter_config.gpio_num = pin;
		gpio_new_flex_glitch_filter(&filter_config, &filter);
		gpio_glitch_filter_enable(filter);
	}
}

void GPIODeinit(void){
	
}

/*==================[end of file]============================================*/
