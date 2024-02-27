#ifndef _HX711_H_
#define _HX711_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup HX711 HX711
 */

/** \brief The HX711 amplifier is a breakout board that allows you to easily read load cells to measure weight. It communicates with the EDU-ESP
 * board via I2C.
 * 
 * @author Juan Ignacio Cerrudo
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 30/01/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <gpio_mcu.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/** @fn HX711_Init(uint8_t gain, gpio_t pd_sck, gpio_t dout)
 * @brief Define clock and data pin, channel, and gain factor
 * @param[in] gain Gain
 * @param[in] pd_sck Clock pin
 * @param[in] dout Datapin
 */
void HX711_Init(uint8_t gain, gpio_t pd_sck, gpio_t dout);

/** @fn int HX711_isReady(void)
 * @brief Check if HX711 is ready
 * @return 1 if ready
 */
int HX711_isReady(void);

/** @fn HX711_setGain(uint8_t gain)
 * @brief Set the gain factor; takes effect only after a call to read()
 * channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
 * depending on the parameter, the channel is also set to either A or B
 * @param[in] gain Gain
 */
void HX711_setGain(uint8_t gain);

/** @fn HX711_read(void)
 * @brief Waits for the chip to be ready and returns a reading
 * @return Read value
 */
uint32_t HX711_read(void);

// returns an average reading; times = how many times to read
/** @fn HX711_readAverage(uint8_t times)
 * @brief Returns an average reading
 * @param[in] times How many times to read
 * @return Read value
 */
uint32_t HX711_readAverage(uint8_t times);

/** @fn HX711_get_value(uint8_t times)
 * @brief Returns (read_average() - OFFSET), that is the current value without the tare weight
 * @param[in] times How many times to read
 * @return Read value
 */
double HX711_get_value(uint8_t times);


/** @fn HX711_get_units(uint8_t times)
 * @brief Returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
 * @param[in] times How many readings to do
 * @return Read value
 */
float HX711_get_units(uint8_t times);

/** @fn HX711_tare(uint8_t times)
 * @brief Set the OFFSET value for tare weight
 * @param[in] times How many times to read the tare value
 */
void HX711_tare(uint8_t times);

/** @fn HX711_setScale(float scale)
 * @brief Set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
 * @param[in] scale Scale vlaue
 */
void HX711_setScale(float scale);

/** @fn HX711_getScale(void)
 * @brief Get the current SCALE
 * @return Scale value
 */
float HX711_getScale(void);

/** @fn X711_setOffset(double offset)
 * @brief Set OFFSET, the value that's subtracted from the actual reading (tare weight)
 * @param[in] offset Offset vlaue
 */
void HX711_setOffset(double offset);

/** @fn HX711_getOffset(void)
 * @brief Get the current OFFSET
 * @return Offset value
 */
double HX711_getOffset(void);

/** @fn HX711_powerDown(void)
 * @brief Puts the chip into power down mode
 */
void HX711_powerDown(void);

/** @fn HX711_powerUp(void)
 * @brief Wakes up the chip after power down mode
 */
void HX711_powerUp(void);

/*==================[internal functions declaration]=========================*/
// Sends/receives data. 
uint8_t shiftIn(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* _HX711_H_ */

/*==================[end of file]============================================*/