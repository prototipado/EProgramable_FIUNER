/**
 * @file buzzer.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-04-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "buzzer.h"
#include "delay_mcu.h"
#include "pwm_mcu.h"
/*==================[macros and definitions]=================================*/
#define PWM_BUZZER      PWM_3
#define PWM_DC          50
#define OCTAVE_OFFSET   0
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
uint16_t notes[] = {
    0,
    NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
    NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
    NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
    NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
bool isDigit(char c){
    if((c >= '0') && (c <= '9')){
        return true;
    }else{
        return false;
    }
}
/*==================[external functions definition]==========================*/
void BuzzerInit(gpio_t pin){
    PWMInit(PWM_BUZZER, pin, NOTE_C4);
    PWMSetDutyCycle(PWM_BUZZER, PWM_DC);
    PWMOff(PWM_BUZZER);
}

void BuzzerOn(void){
    PWMOn(PWM_BUZZER);
}

void BuzzerOff(void){
    PWMOff(PWM_BUZZER);
}

void BuzzerSetFrec(uint16_t freq){
    PWMSetFreq(PWM_BUZZER, freq);
}

void BuzzerPlayTone(uint16_t freq, uint16_t duration){
	PWMSetFreq(PWM_BUZZER, freq);
	PWMOn(PWM_BUZZER);
	DelayMs(duration);
	PWMOff(PWM_BUZZER);
}

void BuzzerPlayRtttl(const char * rtttl_melody){
    uint8_t default_dur = 4;
    uint8_t default_oct = 6;
    int bpm = 63;
    int num;
    long wholenote;
    long duration;
    uint8_t note;
    uint8_t scale;

    /* find the start (skip name, etc) */
    while(*rtttl_melody != ':') rtttl_melody++; // ignore name
    rtttl_melody++;                             // skip ':'

    /* get default duration */
    if(*rtttl_melody == 'd'){
        rtttl_melody++; 
        rtttl_melody++;     // skip "d="
        num = 0;
        while(isDigit(*rtttl_melody)){
        num = (num * 10) + (*rtttl_melody++ - '0');
        }
        if(num > 0) default_dur = num;
        rtttl_melody++;     // skip comma
    }

    /* get default octave */
    if(*rtttl_melody == 'o'){
        rtttl_melody++; 
        rtttl_melody++;     // skip "o="
        num = *rtttl_melody++ - '0';
        if(num >= 3 && num <=7) default_oct = num;
        rtttl_melody++;     // skip comma
    }

    /* get BPM */
    if(*rtttl_melody == 'b'){
        rtttl_melody++; 
        rtttl_melody++;     // skip "b="
        num = 0;
        while(isDigit(*rtttl_melody)){
        num = (num * 10) + (*rtttl_melody++ - '0');
        }
        bpm = num;
        rtttl_melody++;     // skip colon
    }

    /* BPM usually expresses the number of quarter notes per minute */
    wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

    /* now begin note loop */
    while(*rtttl_melody){
        /* first, get note duration, if available */
        num = 0;
        while(isDigit(*rtttl_melody)){
            num = (num * 10) + (*rtttl_melody++ - '0');
        }
        if(num){
            duration = wholenote / num;
        }else{
            duration = wholenote / default_dur;  // we will need to check if we are a dotted note after
        } 
        /* now get the note */
        note = 0;
        switch(*rtttl_melody){
        case 'c':
            note = 1;
            break;
        case 'd':
            note = 3;
            break;
        case 'e':
            note = 5;
            break;
        case 'f':
            note = 6;
            break;
        case 'g':
            note = 8;
            break;
        case 'a':
            note = 10;
            break;
        case 'b':
            note = 12;
            break;
        case 'p':
        default:
            note = 0;
        }
        rtttl_melody++;
        /* now, get optional '#' sharp */
        if(*rtttl_melody == '#'){
            note++;
            rtttl_melody++;
        }
        /* now, get optional '.' dotted note */
        if(*rtttl_melody == '.'){
            duration += duration/2;
            rtttl_melody++;
        }
        /* now, get scale */
        if(isDigit(*rtttl_melody)){
            scale = *rtttl_melody - '0';
            rtttl_melody++;
        }else{
            scale = default_oct;
        }
        scale += OCTAVE_OFFSET;

        if(*rtttl_melody == ','){
            rtttl_melody++; // skip comma for next note (or we may be at the end)
        }
        /* now play the note */
        if(note){
            BuzzerPlayTone(notes[(scale - 4) * 12 + note], duration);
        }
        else{
            DelayMs(duration);
        }
    }
}

void BuzzerDeinit(void){
    
}
/*==================[end of file]============================================*/
