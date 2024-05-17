/**
 * @file l293d.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-05-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "l293.h"
#include "gpio_mcu.h"
#include "pwm_mcu.h"
/*==================[macros and definitions]=================================*/
#define MAX_F_SPEED 	100		/*!< Max foward speed  */
#define MAX_B_SPEED 	-100	/*!< Max backward speed */
#define PWM_FREQ 		50		/*!< PWM frequency (Hz) */
#define N_MOTORS		2		/*!< Number of motors */
#define EN_1_2			GPIO_22
#define A_1				GPIO_21
#define A_2				GPIO_20
#define EN_3_4			GPIO_19
#define A_3				GPIO_18
#define A_4				GPIO_9
/*==================[typedef]================================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external data definition]===============================*/

/*==================[external functions definition]==========================*/
uint8_t L293Init(void){
	PWMInit(PWM_0, EN_1_2, PWM_FREQ);
	PWMInit(PWM_1, EN_3_4, PWM_FREQ);
	GPIOInit(A_1, GPIO_OUTPUT);
	GPIOInit(A_2, GPIO_OUTPUT);
	GPIOInit(A_3, GPIO_OUTPUT);
	GPIOInit(A_4, GPIO_OUTPUT);

	return 1;
}

uint8_t L293SetSpeed(l293_motor_t motor, int8_t speed){
	uint8_t err = 0;

	switch(motor){
	case MOTOR_1:
		if(speed == 0){
			PWMSetDutyCycle(PWM_0, speed);
			GPIOOff(A_1);
			GPIOOff(A_2);
		}
		if(speed > 0){
			if (speed > MAX_F_SPEED) speed = MAX_F_SPEED;
			PWMSetDutyCycle(PWM_0, speed);
			GPIOOn(A_1);
			GPIOOff(A_2);
		}
		if(speed < 0){
			if (speed < MAX_B_SPEED) speed = MAX_B_SPEED;
			PWMSetDutyCycle(PWM_0, -speed);
			GPIOOn(A_1);
			GPIOOff(A_2);
		}
		break;
	case MOTOR_2:
		if(speed == 0){
			PWMSetDutyCycle(PWM_1, speed);
			GPIOOff(A_3);
			GPIOOff(A_4);
		}
		if(speed > 0){
			if (speed > MAX_F_SPEED) speed = MAX_F_SPEED;
			PWMSetDutyCycle(PWM_1, speed);
			GPIOOn(A_3);
			GPIOOff(A_4);
		}
		if(speed < 0){
			if (speed < MAX_B_SPEED) speed = MAX_B_SPEED;
			PWMSetDutyCycle(PWM_1, -speed);
			GPIOOn(A_3);
			GPIOOff(A_4);
		}
		break;
	default:
		err = 1;
		break;
	}

	return err;
}

uint8_t L293DeInit(void){
	PWMOff(PWM_0);
	PWMOff(PWM_1);
	return 1;
}
