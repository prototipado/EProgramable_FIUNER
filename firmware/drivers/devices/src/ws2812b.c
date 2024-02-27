/**
 * @file ws2812.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2023-10-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*==================[inclusions]=============================================*/
#include "ws2812b.h"
#include "gpio_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_fast_out_mcu.h"
#include "delay_mcu.h"
/*==================[macros and definitions]=================================*/
#define RET_CMD (50)    // ret command 50us low
#define BIT_0   (1)     // bit 0
#define BIT_7   (1<<7)  // bit 0
/*==================[internal data declaration]==============================*/
gpio_t pin_number;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
static const uint8_t gamma_table[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   3,
    3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,
    6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,
    11,  11,  11,  12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  17,
    17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25,
    25,  26,  27,  27,  28,  29,  29,  30,  31,  31,  32,  33,  34,  34,  35,
    36,  37,  38,  38,  39,  40,  41,  42,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,  80,  81,
    82,  84,  85,  86,  88,  89,  90,  92,  93,  94,  96,  97,  99,  100, 102,
    103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 124, 125,
    127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148, 150, 152,
    154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
    184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252,
    255};
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
void IRAM_ATTR ws2812bSendHigh(gpio_t pin){
    GPIOFastWrite(1);
    //delay 0.8us
    __asm__ __volatile__ ("nop");   // 1
    __asm__ __volatile__ ("nop");   // 2
    __asm__ __volatile__ ("nop");   // 3
    __asm__ __volatile__ ("nop");   // 4
    __asm__ __volatile__ ("nop");   // 5
    __asm__ __volatile__ ("nop");   // 6
    __asm__ __volatile__ ("nop");   // 7
    __asm__ __volatile__ ("nop");   // 8
    __asm__ __volatile__ ("nop");   // 9
    __asm__ __volatile__ ("nop");   // 10
    __asm__ __volatile__ ("nop");   // 11
    __asm__ __volatile__ ("nop");   // 12
    __asm__ __volatile__ ("nop");   // 13
    __asm__ __volatile__ ("nop");   // 14
    __asm__ __volatile__ ("nop");   // 15
    __asm__ __volatile__ ("nop");   // 16
    __asm__ __volatile__ ("nop");   // 17
    __asm__ __volatile__ ("nop");   // 18
    __asm__ __volatile__ ("nop");   // 19
    __asm__ __volatile__ ("nop");   // 20
    __asm__ __volatile__ ("nop");   // 21
    __asm__ __volatile__ ("nop");   // 22
    __asm__ __volatile__ ("nop");   // 23
    __asm__ __volatile__ ("nop");   // 24  
    __asm__ __volatile__ ("nop");   // 25
    __asm__ __volatile__ ("nop");   // 26
    __asm__ __volatile__ ("nop");   // 27
    __asm__ __volatile__ ("nop");   // 28
    __asm__ __volatile__ ("nop");   // 29
    __asm__ __volatile__ ("nop");   // 30
    __asm__ __volatile__ ("nop");   // 31
    __asm__ __volatile__ ("nop");   // 32
    __asm__ __volatile__ ("nop");   // 33
    __asm__ __volatile__ ("nop");   // 34
    __asm__ __volatile__ ("nop");   // 35
    __asm__ __volatile__ ("nop");   // 36
    __asm__ __volatile__ ("nop");   // 37
    __asm__ __volatile__ ("nop");   // 38
    __asm__ __volatile__ ("nop");   // 39
    __asm__ __volatile__ ("nop");   // 40
    __asm__ __volatile__ ("nop");   // 41
    __asm__ __volatile__ ("nop");   // 42
    __asm__ __volatile__ ("nop");   // 43
    __asm__ __volatile__ ("nop");   // 44
    __asm__ __volatile__ ("nop");   // 45
    __asm__ __volatile__ ("nop");   // 46
    __asm__ __volatile__ ("nop");   // 47
    __asm__ __volatile__ ("nop");   // 48
    __asm__ __volatile__ ("nop");   // 49
    __asm__ __volatile__ ("nop");   // 50
    __asm__ __volatile__ ("nop");   // 51
    __asm__ __volatile__ ("nop");   // 52
    __asm__ __volatile__ ("nop");   // 53
    __asm__ __volatile__ ("nop");   // 54
    __asm__ __volatile__ ("nop");   // 55
    __asm__ __volatile__ ("nop");   // 56
    __asm__ __volatile__ ("nop");   // 57
    __asm__ __volatile__ ("nop");   // 58
    __asm__ __volatile__ ("nop");   // 59
    __asm__ __volatile__ ("nop");   // 47
    __asm__ __volatile__ ("nop");   // 50
    __asm__ __volatile__ ("nop");   // 51
    __asm__ __volatile__ ("nop");   // 52
    __asm__ __volatile__ ("nop");   // 53
    __asm__ __volatile__ ("nop");   // 54
    __asm__ __volatile__ ("nop");   // 55
    __asm__ __volatile__ ("nop");   // 56
    __asm__ __volatile__ ("nop");   // 57
    __asm__ __volatile__ ("nop");   // 58
    __asm__ __volatile__ ("nop");   // 59
    __asm__ __volatile__ ("nop");   // 60
    __asm__ __volatile__ ("nop");   // 61
    __asm__ __volatile__ ("nop");   // 62
    __asm__ __volatile__ ("nop");   // 63
    __asm__ __volatile__ ("nop");   // 64
    __asm__ __volatile__ ("nop");   // 65
    __asm__ __volatile__ ("nop");   // 66
    __asm__ __volatile__ ("nop");   // 67
    __asm__ __volatile__ ("nop");   // 68
    __asm__ __volatile__ ("nop");   // 69
    __asm__ __volatile__ ("nop");   // 70
    __asm__ __volatile__ ("nop");   // 71
    __asm__ __volatile__ ("nop");   // 72
    __asm__ __volatile__ ("nop");   // 73
    __asm__ __volatile__ ("nop");   // 74
    __asm__ __volatile__ ("nop");   // 75
    __asm__ __volatile__ ("nop");   // 76
    __asm__ __volatile__ ("nop");   // 77
    __asm__ __volatile__ ("nop");   // 78
    __asm__ __volatile__ ("nop");   // 79
    __asm__ __volatile__ ("nop");   // 80
    __asm__ __volatile__ ("nop");   // 81
    __asm__ __volatile__ ("nop");   // 82
    __asm__ __volatile__ ("nop");   // 83
    __asm__ __volatile__ ("nop");   // 84
    __asm__ __volatile__ ("nop");   // 85
    __asm__ __volatile__ ("nop");   // 86
    __asm__ __volatile__ ("nop");   // 87
    __asm__ __volatile__ ("nop");   // 88
    __asm__ __volatile__ ("nop");   // 89
    __asm__ __volatile__ ("nop");   // 90
    __asm__ __volatile__ ("nop");   // 91
    
    GPIOFastWrite(0);
	//delay 0.45us
    __asm__ __volatile__ ("nop");   // 1
    __asm__ __volatile__ ("nop");   // 2
    __asm__ __volatile__ ("nop");   // 3
    __asm__ __volatile__ ("nop");   // 4
    __asm__ __volatile__ ("nop");   // 5
    __asm__ __volatile__ ("nop");   // 6
    __asm__ __volatile__ ("nop");   // 7
    __asm__ __volatile__ ("nop");   // 8
    __asm__ __volatile__ ("nop");   // 9
    __asm__ __volatile__ ("nop");   // 10
    __asm__ __volatile__ ("nop");   // 11
    __asm__ __volatile__ ("nop");   // 12
    __asm__ __volatile__ ("nop");   // 13
    __asm__ __volatile__ ("nop");   // 14
    __asm__ __volatile__ ("nop");   // 15
    __asm__ __volatile__ ("nop");   // 16
    __asm__ __volatile__ ("nop");   // 17
    __asm__ __volatile__ ("nop");   // 18
    __asm__ __volatile__ ("nop");   // 19
    __asm__ __volatile__ ("nop");   // 20
    __asm__ __volatile__ ("nop");   // 21
    __asm__ __volatile__ ("nop");   // 22
    __asm__ __volatile__ ("nop");   // 23
    __asm__ __volatile__ ("nop");   // 24  
    __asm__ __volatile__ ("nop");   // 25
    __asm__ __volatile__ ("nop");   // 26
    __asm__ __volatile__ ("nop");   // 27
    __asm__ __volatile__ ("nop");   // 28
    __asm__ __volatile__ ("nop");   // 29
}

void IRAM_ATTR ws2812bSendLow(gpio_t pin){
    GPIOFastWrite(1);
    //delay 0.4us
    __asm__ __volatile__ ("nop");   // 1
    __asm__ __volatile__ ("nop");   // 2
    __asm__ __volatile__ ("nop");   // 3
    __asm__ __volatile__ ("nop");   // 4
    __asm__ __volatile__ ("nop");   // 5
    __asm__ __volatile__ ("nop");   // 6
    __asm__ __volatile__ ("nop");   // 7
    __asm__ __volatile__ ("nop");   // 8
    __asm__ __volatile__ ("nop");   // 9
    __asm__ __volatile__ ("nop");   // 10
    __asm__ __volatile__ ("nop");   // 11
    __asm__ __volatile__ ("nop");   // 12
    __asm__ __volatile__ ("nop");   // 13
    __asm__ __volatile__ ("nop");   // 14
    __asm__ __volatile__ ("nop");   // 15
    __asm__ __volatile__ ("nop");   // 16
    __asm__ __volatile__ ("nop");   // 17
    __asm__ __volatile__ ("nop");   // 18
    __asm__ __volatile__ ("nop");   // 19
    __asm__ __volatile__ ("nop");   // 20
    __asm__ __volatile__ ("nop");   // 21
    __asm__ __volatile__ ("nop");   // 22
    __asm__ __volatile__ ("nop");   // 23
    __asm__ __volatile__ ("nop");   // 24  
    __asm__ __volatile__ ("nop");   // 25
    __asm__ __volatile__ ("nop");   // 26
    __asm__ __volatile__ ("nop");   // 27

    GPIOFastWrite(0);
	//delay 0.85us
    __asm__ __volatile__ ("nop");   // 1
    __asm__ __volatile__ ("nop");   // 2
    __asm__ __volatile__ ("nop");   // 3
    __asm__ __volatile__ ("nop");   // 4
    __asm__ __volatile__ ("nop");   // 5
    __asm__ __volatile__ ("nop");   // 6
    __asm__ __volatile__ ("nop");   // 7
    __asm__ __volatile__ ("nop");   // 8
    __asm__ __volatile__ ("nop");   // 9
    __asm__ __volatile__ ("nop");   // 10
    __asm__ __volatile__ ("nop");   // 11
    __asm__ __volatile__ ("nop");   // 12
    __asm__ __volatile__ ("nop");   // 13
    __asm__ __volatile__ ("nop");   // 14
    __asm__ __volatile__ ("nop");   // 15
    __asm__ __volatile__ ("nop");   // 16
    __asm__ __volatile__ ("nop");   // 17
    __asm__ __volatile__ ("nop");   // 18
    __asm__ __volatile__ ("nop");   // 19
    __asm__ __volatile__ ("nop");   // 20
    __asm__ __volatile__ ("nop");   // 21
    __asm__ __volatile__ ("nop");   // 22
    __asm__ __volatile__ ("nop");   // 23
    __asm__ __volatile__ ("nop");   // 24  
    __asm__ __volatile__ ("nop");   // 25
    __asm__ __volatile__ ("nop");   // 26
    __asm__ __volatile__ ("nop");   // 27
    __asm__ __volatile__ ("nop");   // 28
    __asm__ __volatile__ ("nop");   // 29
    __asm__ __volatile__ ("nop");   // 30
    __asm__ __volatile__ ("nop");   // 31
    __asm__ __volatile__ ("nop");   // 32
    __asm__ __volatile__ ("nop");   // 33
    __asm__ __volatile__ ("nop");   // 34
    __asm__ __volatile__ ("nop");   // 35
    __asm__ __volatile__ ("nop");   // 36
    __asm__ __volatile__ ("nop");   // 37
    __asm__ __volatile__ ("nop");   // 38
    __asm__ __volatile__ ("nop");   // 39
    __asm__ __volatile__ ("nop");   // 40
    __asm__ __volatile__ ("nop");   // 41
    __asm__ __volatile__ ("nop");   // 42
    __asm__ __volatile__ ("nop");   // 43
    __asm__ __volatile__ ("nop");   // 44
    __asm__ __volatile__ ("nop");   // 45
    __asm__ __volatile__ ("nop");   // 46
    __asm__ __volatile__ ("nop");   // 47
    __asm__ __volatile__ ("nop");   // 48
    __asm__ __volatile__ ("nop");   // 49
    __asm__ __volatile__ ("nop");   // 50
    __asm__ __volatile__ ("nop");   // 51
    __asm__ __volatile__ ("nop");   // 52
    __asm__ __volatile__ ("nop");   // 53
    __asm__ __volatile__ ("nop");   // 54
    __asm__ __volatile__ ("nop");   // 55
    __asm__ __volatile__ ("nop");   // 56
    __asm__ __volatile__ ("nop");   // 57
    __asm__ __volatile__ ("nop");   // 58
    __asm__ __volatile__ ("nop");   // 59
    __asm__ __volatile__ ("nop");   // 47
    __asm__ __volatile__ ("nop");   // 50
    __asm__ __volatile__ ("nop");   // 51
    __asm__ __volatile__ ("nop");   // 52
    __asm__ __volatile__ ("nop");   // 53
    __asm__ __volatile__ ("nop");   // 54
    __asm__ __volatile__ ("nop");   // 55
    __asm__ __volatile__ ("nop");   // 56
    __asm__ __volatile__ ("nop");   // 57
    __asm__ __volatile__ ("nop");   // 58
    __asm__ __volatile__ ("nop");   // 59
    __asm__ __volatile__ ("nop");   // 60
    __asm__ __volatile__ ("nop");   // 61
    __asm__ __volatile__ ("nop");   // 62
    __asm__ __volatile__ ("nop");   // 63
    __asm__ __volatile__ ("nop");   // 64
    __asm__ __volatile__ ("nop");   // 65
    __asm__ __volatile__ ("nop");   // 66
    __asm__ __volatile__ ("nop");   // 67
    __asm__ __volatile__ ("nop");   // 68
    __asm__ __volatile__ ("nop");   // 69
    __asm__ __volatile__ ("nop");   // 70
    __asm__ __volatile__ ("nop");   // 71
    __asm__ __volatile__ ("nop");   // 72
    __asm__ __volatile__ ("nop");   // 73
    __asm__ __volatile__ ("nop");   // 74
    __asm__ __volatile__ ("nop");   // 75
    __asm__ __volatile__ ("nop");   // 76
    __asm__ __volatile__ ("nop");   // 77
    __asm__ __volatile__ ("nop");   // 78
    __asm__ __volatile__ ("nop");   // 79
    __asm__ __volatile__ ("nop");   // 80
    __asm__ __volatile__ ("nop");   // 81
    __asm__ __volatile__ ("nop");   // 82
    __asm__ __volatile__ ("nop");   // 83
    __asm__ __volatile__ ("nop");   // 84
    __asm__ __volatile__ ("nop");   // 85
    __asm__ __volatile__ ("nop");   // 86
    __asm__ __volatile__ ("nop");   // 87
    __asm__ __volatile__ ("nop");   // 88
    __asm__ __volatile__ ("nop");   // 89
    __asm__ __volatile__ ("nop");   // 90
    __asm__ __volatile__ ("nop");   // 91
    __asm__ __volatile__ ("nop");   // 92
    __asm__ __volatile__ ("nop");   // 93
    __asm__ __volatile__ ("nop");   // 94
}

uint8_t ws2812bGammaCorrection(uint8_t component){
    return gamma_table[component];
}

/*==================[external functions definition]==========================*/

void ws2812bInit(gpio_t pin){
    pin_number = pin;
    GPIOFastInit(&pin, 1);
}

void ws2812bSend(rgb_led_t led_color){
    uint8_t i;
	// Blue
	for(i=0; i<=7; i++){
        if(ws2812bGammaCorrection(led_color.green) & (BIT_7>>i)){
            ws2812bSendHigh(pin_number);
        }
        else{
            ws2812bSendLow(pin_number);
        }
    }
	// Red
    for(i=0; i<=7; i++){
        if(ws2812bGammaCorrection(led_color.red) & (BIT_7>>i)){
            ws2812bSendHigh(pin_number);
        }
        else{
            ws2812bSendLow(pin_number);
        }
    }
	// Green
    for(i=0; i<=7; i++){
        if(ws2812bGammaCorrection(led_color.blue) & (BIT_7>>i)){
            ws2812bSendHigh(pin_number);
        }
        else{
            ws2812bSendLow(pin_number);
        }
    }
}

void ws2812bSendRet(void){
    GPIOFastWrite(0);
    DelayUs(RET_CMD);
}

/*==================[end of file]============================================*/
