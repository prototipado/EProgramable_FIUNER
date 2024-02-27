#ifndef I2C_MCU_H
#define I2C_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup I2C I2C
 ** @{
 */

/** @brief I2C driver for ESP-EDU board
 * 
 * @note SDA: GPIO_6, SCL: GPIO_7.
 * 
 * @note ESP-EDU have 4 I2C connector in the board (J4, J5, J6 and J8), but all of them are routed to the same I2C port.
 *
 * @author Juan Ignacio Cerrudo
 * 
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 30/01/2024 | Document creation		                         |
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <stdbool.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "gpio_mcu.h"
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
#define I2C_MASTER_SCL_IO           GPIO_7      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_6      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0           /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000      /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @fn I2C_initialize( uint32_t clockRateHz )
 * @brief Initialize I2C0
 */
bool I2C_initialize( uint32_t clockRateHz );

/** @fn I2C_enable(bool isEnabled)
 * @brief Enable or disable I2C
 * @param isEnabled true = enable, false = disable
 */
void I2C_enable(bool isEnabled);

/** @fn I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout)
 * @brief Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout);

/** @fn I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout);
 * @brief Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout);

/** @fn I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout);
 * @brief Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout);

/** @fn I2C_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout)
 * @brief Read word from an 16-bit device register.
 * @param devAddr
 * @param regAddr
 * @param data
 * @param timeout
 * @return
 */
int8_t I2C_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout);

/** @fn I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout)
 * @brief Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return I2C_TransferReturn_TypeDef
 */
int8_t I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout);

/** @fn I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
 * @brief write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);

/** @fn I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
 * @brief Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);

/** @fn I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
 * @brief Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);

/** @fn bool I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
 * @brief Write word from an 16-bit device register.
 * @param devAddr
 * @param regAddr
 * @param data
 * @return Status of operation (true = success)
 */
bool I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);

/** @fn I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
 * @brief Write multiple bytes to device.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param length Number of bytes to write
 * @param data Array of bytes to write
 * @return Status of operation (true = success)
 */
bool I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

/** @fn I2C_SelectRegister(uint8_t dev, uint8_t reg)
 * @brief Select a register
 * @param devAddr I2C slave device address
 * @param reg Register address to select
 */
void I2C_SelectRegister(uint8_t devAddr, uint8_t reg);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* #ifndef I2C_MCU_H */

/*==================[end of file]============================================*/
