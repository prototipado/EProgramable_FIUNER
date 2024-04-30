/**
 * @file rtc_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-04-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "rtc_mcu.h"
#include <stdint.h>
#include "sys/time.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
bool RtcConfig(rtc_t * rtc){
    struct tm tm;
    tm.tm_year = rtc->year;
    if(rtc->month > MAX_MONTH) return false;
    if(rtc->mday > MAX_MDAY) return false;
    if(rtc->wday > MAX_WDAY) return false;
    if(rtc->hour > MAX_HOUR) return false;
    if(rtc->min > MAX_MIN) return false;
    if(rtc->sec > MAX_SEC) return false;
    tm.tm_mon = rtc->month;
    tm.tm_mday = rtc->mday;
    tm.tm_wday = rtc->wday;
    tm.tm_hour = rtc->hour;
    tm.tm_min = rtc->min;
    tm.tm_sec = rtc->sec;
    time_t t = mktime(&tm);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);

    return true;
}

void RtcRead(rtc_t * rtc){
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    rtc->year = timeinfo.tm_year;
    rtc->month = timeinfo.tm_mon;
    rtc->mday = timeinfo.tm_mday;
    rtc->wday = timeinfo.tm_wday;
    rtc->hour = timeinfo.tm_hour;
    rtc->min = timeinfo.tm_min;
    rtc->sec = timeinfo.tm_sec;
}
/*==================[end of file]============================================*/
