
/*==================[inclusions]=============================================*/
#include "servo_sg90.h"
#include "pwm_mcu.h"
/*==================[macros and definitions]=================================*/
#define SERVO_FREQ 	50
#define MIN_ANG		-90
#define MAX_ANG		90
#define ANG_RANGE	180.0
#define PERIOD_MS   20.0
#define PULSEW_MS   1.0
#define PERCENT   	100.0
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
uint8_t Angle2DutyCicle(int8_t angle){
	static float h_time;
	static float duty_cicle;
	static int16_t deg;
	deg = 2 * angle + MAX_ANG;	// NOTE: adjusted (angle x 2) for the available servos
	h_time = (float)(deg/ANG_RANGE) + PULSEW_MS;
	duty_cicle = (float)(h_time/PERIOD_MS) * PERCENT;
	return (uint8_t) duty_cicle;
}
/*==================[external functions definition]==========================*/

uint8_t ServoInit(servo_out_t servo, gpio_t gpio){
	switch(servo){
		case SERVO_0:
			PWMInit(PWM_0, gpio, SERVO_FREQ);
			break;
		case SERVO_1:
			PWMInit(PWM_1, gpio, SERVO_FREQ);
			break;
		case SERVO_2:
			PWMInit(PWM_2, gpio, SERVO_FREQ);
			break;
		case SERVO_3:
			PWMInit(PWM_3, gpio, SERVO_FREQ);
			break;
	}
	return 0;
}

void ServoMove(servo_out_t servo, int8_t ang){
	static uint8_t dc;
	if(ang < MIN_ANG){
		ang = MIN_ANG;
	} else if(ang > MAX_ANG){
		ang = MAX_ANG;
	}
	dc = Angle2DutyCicle(ang);
	switch(servo){
		case SERVO_0:
			PWMSetDutyCycle(PWM_0, dc);
			break;
		case SERVO_1:
			PWMSetDutyCycle(PWM_1, dc);
			break;
		case SERVO_2:
			PWMSetDutyCycle(PWM_2, dc);
			break;
		case SERVO_3:
			PWMSetDutyCycle(PWM_3, dc);
			break;
	}
}

/*==================[end of file]============================================*/
