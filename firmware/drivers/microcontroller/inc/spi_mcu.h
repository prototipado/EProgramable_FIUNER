#ifndef SPI_MCU_H
#define SPI_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup SPI SPI
 ** @{ */

/** \brief SPI driver for the ESP-EDU Board.
 *
 * @note ESP-EDU only have 1 SPI port, than can be connected up to 3 diferent devices using 
 * chip select pins (CS1, CS2, CS3). The SPI_0, SPI_1 and SPI_2 defines refer to this.
 * 
 * @note MISO: GPIO_22, MOSI: GPIO_21, SCLK: GPIO_20, CS1: GPIO_19, CS2: GPIO_18, CS3: GPIO_9
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 09/02/2024 | Document creation		                         						|
 * 
 **/
/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/**
 * @brief ESP-EDU only have 1 SPI port, than can be connected up to 3 diferent devices using 
 * chip select pins (CS1, CS2, CS3). The SPI_0, SPI_1 and SPI_2 defines refer to this.
 */
typedef enum spi_devices {
	SPI_1,			/*!<  CS_1: GPIO_19 */
	SPI_2,			/*!<  CS_2: GPIO_18 */
	SPI_3			/*!<  CS_3: GPIO_9 */
} spi_dev_t;

/**
 * @brief SPI phase and polarity
 */
typedef enum {
	MODE0 = 0,	/*!< phase = 0 and polarity = 0 */
	MODE1 = 1,	/*!< phase = 1 and polarity = 0 */
	MODE2 = 2,	/*!< phase = 0 and polarity = 1 */
	MODE3 = 3,	/*!< phase = 1 and polarity = 1 */
} clk_mode_t;

/**
 * @brief SPI transfer modes
 */
typedef enum {
	SPI_POLLING,		/*!< Polling */
	SPI_INTERRUPT,		/*!< Interrupción */
} transfer_mode_t;

/**
 * @brief SPI configuration structure
 */
typedef struct{
	spi_dev_t device;				/*!< SPI device number */
	clk_mode_t clk_mode;			/*!< Mode: phase and polarity */
	uint32_t bitrate;				/*!< Transfer speed (up to 26MHz) */
	transfer_mode_t transfer_mode;	/*!< Transfer mode */
	void *func_p;					/*!< Pointer to callback function for transaction end */
	void *param_p;					/*!< Pointer to callback parameter */
} spi_mcu_config_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Initialize SPI module with the corresponding configuration
 * 
 * @param spi Structure with the module configuration
 * @return uint8_t 
 */
uint8_t SpiInit(spi_mcu_config_t* spi);

/**
 * @brief Read data from SPI port
 * 
 * @param device SPI device to read from
 * @param rx_buffer pointer to buffer where data is stored
 * @param rx_buffer_size numbers of bytes to read
 */
void SpiRead(spi_dev_t device, uint8_t * rx_buffer, uint32_t rx_buffer_size);

/**
 * @brief Write data from SPI port
 * 
 * @param device SPI device to read from
 * @param tx_buffer pointer to buffer where data is stored
 * @param tx_buffer_size numbers of bytes to write
 */
void SpiWrite(spi_dev_t device, uint8_t * tx_buffer, uint32_t tx_buffer_size);

/**
 * @brief Write and Read data simultaneous from SPI port
 * 
 * @param device SPI device to read from
 * @param tx_buffer pointer to buffer where data to write is stored
 * @param rx_buffer pointer to buffer where data read is stored
 * @param buffer_size numbers of bytes to read or write
 */
void SpiReadWrite(spi_dev_t device, uint8_t * tx_buffer, uint8_t * rx_buffer, uint32_t buffer_size);

/**
 * @brief De-Initialize SPI module with the corresponding configuration
 * 
 * @param device SPI device 
 * @return uint8_t 
 */
uint8_t SpiDeInit(spi_dev_t device);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif 

/*==================[end of file]============================================*/
