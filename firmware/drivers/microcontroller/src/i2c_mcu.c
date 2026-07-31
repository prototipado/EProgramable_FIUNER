/**
 * @file i2c_mcu.c
 * @author Juan Cerrudo (juan.cerrudo@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-01-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <stdlib.h>
#include "driver/i2c_master.h"

#include "i2c_mcu.h"
/*==================[macros and definitions]=================================*/

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); /*assert(0 && #x);*/} } while(0);

/*==================[internal data definition]===============================*/
static i2c_master_bus_handle_t s_i2c_bus_handle = NULL;
static uint32_t s_i2c_clock_rate_hz = I2C_MASTER_FREQ_HZ;

/*==================[internal functions declaration]=========================*/
static esp_err_t i2c_mcu_get_device_handle(uint8_t devAddr, i2c_master_dev_handle_t *ret_handle)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = devAddr,
        .scl_speed_hz = s_i2c_clock_rate_hz,
        .scl_wait_us = 0,
        .flags.disable_ack_check = 0,
    };

    if (s_i2c_bus_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    return i2c_master_bus_add_device(s_i2c_bus_handle, &dev_cfg, ret_handle);
}

static void i2c_mcu_release_device_handle(i2c_master_dev_handle_t dev_handle)
{
    if (dev_handle != NULL) {
        i2c_master_bus_rm_device(dev_handle);
    }
}

static int i2c_mcu_timeout_ms(uint16_t timeout)
{
    return (timeout > 0) ? (int)timeout : I2C_MASTER_TIMEOUT_MS;
}

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/** Initialize I2C0
 */
bool I2C_initialize( uint32_t clockRateHz )
{
    if (s_i2c_bus_handle != NULL) {
        s_i2c_clock_rate_hz = clockRateHz;
        return true;
    }

    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 0,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags.enable_internal_pullup = 1,
    };

    esp_err_t ret = i2c_new_master_bus(&bus_config, &s_i2c_bus_handle);
    s_i2c_clock_rate_hz = (ret == ESP_OK) ? clockRateHz : I2C_MASTER_FREQ_HZ;
    return (ret == ESP_OK);
}



/** Enable or disable I2C
 * @param isEnabled true = enable, false = disable
 */
void I2C_enable(bool isEnabled) {
  
}

/** Default timeout value for read operations.
 */
//uint16_t I2C_readTimeout = I2C_MASTER_TIMEOUT_MS;
/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout) {


	uint8_t b;
    uint8_t count = I2C_readByte(devAddr, regAddr, &b, timeout);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = I2C_readByte(devAddr, regAddr, &b, timeout)) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout) {
    return I2C_readBytes(devAddr, regAddr, 1, data, timeout);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2C_readTimeout)
 * @return I2C_TransferReturn_TypeDef http://downloads.energymicro.com/documentation/doxygen/group__I2C.html
 */
int8_t I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {
    i2c_master_dev_handle_t dev_handle = NULL;
    uint8_t register_addr = regAddr;
    esp_err_t ret = ESP_FAIL;

    if (data == NULL || length == 0) {
        return 0;
    }

    ret = i2c_mcu_get_device_handle(devAddr, &dev_handle);
    if (ret == ESP_OK) {
        ret = i2c_master_transmit_receive(dev_handle, &register_addr, 1, data, length, i2c_mcu_timeout_ms(timeout));
    }

    i2c_mcu_release_device_handle(dev_handle);
    return (ret == ESP_OK) ? length : 0;
}

bool I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data){

	uint8_t data1[] = {(uint8_t)(data>>8), (uint8_t)(data & 0xff)};
	I2C_writeBytes(devAddr, regAddr, 2, data1);
	return true;
}

void I2C_SelectRegister(uint8_t devAddr, uint8_t reg){
    i2c_master_dev_handle_t dev_handle = NULL;
    esp_err_t ret = i2c_mcu_get_device_handle(devAddr, &dev_handle);

    if (ret == ESP_OK) {
        ret = i2c_master_transmit(dev_handle, &reg, 1, i2c_mcu_timeout_ms(0));
    }

    i2c_mcu_release_device_handle(dev_handle);
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2C_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    I2C_readByte(devAddr, regAddr, &b, 0);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return I2C_writeByte(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b = 0;
    if (I2C_readByte(devAddr, regAddr, &b, 0) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return I2C_writeByte(devAddr, regAddr, b);
    } else {
        return false;
    }
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2C_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    uint8_t buffer[2] = {regAddr, data};
    i2c_master_dev_handle_t dev_handle = NULL;
    esp_err_t ret = i2c_mcu_get_device_handle(devAddr, &dev_handle);

    if (ret == ESP_OK) {
        ret = i2c_master_transmit(dev_handle, buffer, sizeof(buffer), i2c_mcu_timeout_ms(0));
    }

    i2c_mcu_release_device_handle(dev_handle);
    return (ret == ESP_OK);
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param length Number of bytes to write
 * @param data Array of bytes to write
 * @return Status of operation (true = success)
 */
bool I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data){
    uint8_t *buffer = NULL;
    i2c_master_dev_handle_t dev_handle = NULL;
    esp_err_t ret = ESP_FAIL;

    if (data == NULL || length == 0) {
        return false;
    }

    buffer = (uint8_t *)malloc(length + 1);
    if (buffer == NULL) {
        return false;
    }

    buffer[0] = regAddr;
    memcpy(&buffer[1], data, length);

    ret = i2c_mcu_get_device_handle(devAddr, &dev_handle);
    if (ret == ESP_OK) {
        ret = i2c_master_transmit(dev_handle, buffer, length + 1, i2c_mcu_timeout_ms(0));
    }

    i2c_mcu_release_device_handle(dev_handle);
    free(buffer);
    return (ret == ESP_OK);
}


/**
 * read word
 * @param devAddr
 * @param regAddr
 * @param data
 * @param timeout
 * @return
 */
int8_t I2C_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout){
	uint8_t msb[2] = {0,0};
	I2C_readBytes(devAddr, regAddr, 2, msb, 0);
	*data = (int16_t)((msb[0] << 8) | msb[1]);
	return 0;
}

/*==================[end of file]============================================*/
