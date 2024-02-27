/**
 * @file neopixel_stripe.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "neopixel_stripe.h"
#include "ws2812b.h"
/*==================[macros and definitions]=================================*/
#define RED_MSK         0x00FF0000
#define GREEN_MSK       0x0000FF00
#define BLUE_MSK        0x000000FF
#define RED_OFFSET      16
#define GREEN_OFFSET    8
#define BLUE_OFFSET     0
#define MAX_BRIGHT  	255
#define BRIGHT_OFFSET   8
/*==================[internal data declaration]==============================*/
uint16_t stripe_length;
uint8_t stripe_bright = MAX_BRIGHT;
neopixel_color_t *stripe_colors; 
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void NeoPixelInit(gpio_t pin, uint16_t len, neopixel_color_t *color_array){
    stripe_length = len;
	stripe_colors = color_array;
    ws2812bInit(pin);
}

void NeoPixelAllOff(void){
    rgb_led_t led;
	for (uint16_t i = 0; i < stripe_length; i++){
		led.red = 0;
		led.green = 0;
		led.blue = 0;
		ws2812bSend(led);
	}
	ws2812bSendRet();
}

void NeoPixelAllColor(neopixel_color_t color){
	for (uint16_t i = 0; i < stripe_length; i++){
		stripe_colors[i] = color;
	}
	NeoPixelSetArray(stripe_colors);
}

void NeoPixelSetPixel(uint16_t pixel, neopixel_color_t color){
	stripe_colors[pixel] = color;
	NeoPixelSetArray(stripe_colors);
}

void NeoPixelSetArray(neopixel_color_t *color_array){
    rgb_led_t led;
	uint16_t red, green, blue;
	for (uint16_t i = 0; i < stripe_length; i++){
		red = ((color_array[i] & RED_MSK) >> RED_OFFSET) * stripe_bright;
		green = ((color_array[i] & GREEN_MSK) >> GREEN_OFFSET) * stripe_bright;
		blue = ((color_array[i] & BLUE_MSK) >> BLUE_OFFSET) * stripe_bright;
		led.red = red >> BRIGHT_OFFSET;
		led.green = green >> BRIGHT_OFFSET;
		led.blue = blue >> BRIGHT_OFFSET;
		ws2812bSend(led);
	}
	ws2812bSendRet();
}

void NeoPixelShift(bool upwards){
	neopixel_color_t carry;

	if(upwards){
		carry = stripe_colors[stripe_length-1];
		for (uint16_t i = 0; i < stripe_length-1; i++){
			stripe_colors[stripe_length-1-i] = stripe_colors[stripe_length-2-i]; 
		}
		stripe_colors[0] = carry;
	}else{
		carry = stripe_colors[0];
		for (uint16_t i = 0; i < stripe_length-1; i++){
			stripe_colors[i] = stripe_colors[i+1]; 
		}
		stripe_colors[stripe_length-1] = carry;
	}
	NeoPixelSetArray(stripe_colors);
}

void NeoPixelBrightness(uint8_t bright){
	stripe_bright = bright;
	NeoPixelSetArray(stripe_colors);
}

void NeoPixelRainbow(uint16_t first_hue, uint8_t sat, uint8_t val, uint8_t reps){
	for (uint16_t i=0; i<stripe_length; i++) {
		uint16_t hue = first_hue + (i * reps * 65536) / stripe_length;
		neopixel_color_t color = NeoPixelHSV2Color(hue, sat, val);
		stripe_colors[i] = color;
  	}
	NeoPixelSetArray(stripe_colors);
}

neopixel_color_t NeoPixelRgb2Color(uint8_t red, uint8_t green, uint8_t blue){
	return (red << RED_OFFSET) | (green << GREEN_OFFSET) | (blue << BLUE_OFFSET);
}

neopixel_color_t NeoPixelHSV2Color(uint16_t hue, uint8_t sat, uint8_t val){

  uint8_t r, g, b;

  hue = (hue * 1530L + 32768) / 65536;
  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if (hue < 510) { // Red to Green-1
    b = 0;
    if (hue < 255) { //   Red to Yellow-1
      r = 255;
      g = hue;       //     g = 0 to 254
    } else {         //   Yellow to Green-1
      r = 510 - hue; //     r = 255 to 1
      g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    r = 0;
    if (hue < 765) { //   Green to Cyan-1
      g = 255;
      b = hue - 510;  //     b = 0 to 254
    } else {          //   Cyan to Blue-1
      g = 1020 - hue; //     g = 255 to 1
      b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    g = 0;
    if (hue < 1275) { //   Blue to Magenta-1
      r = hue - 1020; //     r = 0 to 254
      b = 255;
    } else { //   Magenta to Red-1
      r = 255;
      b = 1530 - hue; //     b = 255 to 1
    }
  } else { // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  uint16_t s1 = 1 + sat;  // 1 to 256; same reason
  uint8_t s2 = 255 - sat; // 255 to 0
  return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
         (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
         (((((b * s1) >> 8) + s2) * v1) >> 8);
}

/*==================[end of file]============================================*/
