/*
 * HX711.c
 *
 *  Created on: 11 abr. 2023
 *      Author: Usuario
 */


#include <stdbool.h>
#include <stdint.h>

#include "hx711.h"

#include <delay_mcu.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
uint8_t GAIN;		             /*!<  Amplification factor */
double OFFSET;	                 /*!<  Used for tare weight */
float SCALE;	                 /*!<  Used to return weight in grams, kg, ounces, whatever */ 


gpio_t internal_pd_sck;
gpio_t internal_dout;

/*==================[internal functions declaration]=========================*/

uint8_t shiftIn(void)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; ++i)
    {
    	GPIOOn(internal_pd_sck);//PD_SCK_SET_HIGH;
        value |= GPIORead(internal_dout) << (7 - i);
        GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
    }
    return value;
}

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void HX711_Init(uint8_t gain, gpio_t pd_sck, gpio_t dout)
{
	internal_pd_sck = pd_sck;
	internal_dout = dout;
	GPIOInit(pd_sck, GPIO_OUTPUT);//PD_SCK_SET_OUTPUT;
	GPIOInit(dout, GPIO_INPUT);//DOUT_SET_INPUT;
    HX711_setGain(gain);

}

int HX711_isReady(void)
{
    return (GPIORead(internal_dout)) == 0;
}

void HX711_setGain(uint8_t gain)
{
	switch (gain)
	{
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
	HX711_read();
}

uint32_t HX711_read(void)
{
	// wait for the chip to become ready
	while (!HX711_isReady());

    unsigned long count;
    unsigned char i;

    GPIOOn(internal_dout);//DOUT_SET_HIGH;

    DelayUs(1);

    GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
    DelayUs(1);

    count=0;
    while(GPIORead(internal_dout));
    for(i=0;i<24;i++)
    {
    	 GPIOOn(internal_pd_sck);//PD_SCK_SET_HIGH;
    	 DelayUs(1);
        count=count<<1;
        GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
        DelayUs(1);
        if(GPIORead(internal_dout))
            count++;
    }
    count = count>>6;
    GPIOOn(internal_pd_sck);//PD_SCK_SET_HIGH;
    DelayUs(1);
    GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
    DelayUs(1);
    count ^= 0x800000;
    return(count);
}

uint32_t HX711_readAverage(uint8_t times)
{
	uint32_t sum = 0;
	for (uint8_t i = 0; i < times; i++)
	{
		sum += HX711_read();
		// TODO: See if yield will work | yield();
	}
	return sum / times;
}

double HX711_getValue(uint8_t times)
{
	return HX711_readAverage(times) - OFFSET;
}

float HX711_getUnits(uint8_t times)
{
	return HX711_get_value(times) / SCALE;
}

void HX711_tare(uint8_t times)
{
	double sum = HX711_readAverage(times);
	HX711_setOffset(sum);
}

void HX711_setScale(float scale)
{
	SCALE = scale;
}

float HX711_getScale(void)
{
	return SCALE;
}

void HX711_setOffset(double offset)
{
    OFFSET = offset;
}

double HX711_getOffset(void)
{
	return OFFSET;
}

void HX711_powerDown(void)
{
	GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
	GPIOOn(internal_pd_sck);//PD_SCK_SET_HIGH;
	DelayUs(70);
}

void HX711_powerUp(void)
{
	GPIOOff(internal_pd_sck);//PD_SCK_SET_LOW;
}


