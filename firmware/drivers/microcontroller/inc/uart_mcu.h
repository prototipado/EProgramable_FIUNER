#ifndef UART_MCU_H
#define UART_MCU_H

/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup UART UART
 ** @{ */

/** \brief UART driver for the ESP-EDU Board.
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 02/07/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include "stdint.h"
/*==================[macros]=================================================*/
#define UART_NO_INT	0		/*!< Flag used when no reading interruption is required */
/*==================[typedef]================================================*/
/**
 * @brief List of UART ports available in ESP-EDU
 */
typedef enum uart_ports{
	UART_PC,				/*!< UART connected PC through USB port (indicated with UART) (also maped to TX: GPIO16, RX: GPIO17) */
	UART_CONNECTOR,			/*!< UART connected to J2 connector (TX: GPIO18, RX: GPIO19) */
} uart_mcu_port_t;
/**
 * @brief Serial port configuration struct
 */
typedef struct {			
	uart_mcu_port_t port;	/*!< port */
	uint32_t baud_rate;		/*!< baudrate (bits per second) */
	void *func_p;			/*!< Pointer to callback function to call when receiving data (= UART_NO_INT if not requiered)*/
	void *param_p;			/*!< Pointer to callback function parameters */
} serial_config_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Serial port initialization
 * 
 * @param port_config 
 */
void UartInit(serial_config_t *port_config);

/**
 * @brief Read a single byte from serial port
 * 
 * @param port Port to read from
 * @param data Pointer to variable where data will be stored
 * @return uint8_t 
 */
uint8_t UartReadByte(uart_mcu_port_t port, uint8_t *data);

/**
 * @brief Read multiple bytes from serial port
 * 
 * @param port Port to read from
 * @param data Pointer to array where data will be stored
 * @param nbytes Number of bytes to be readed
 * @return uint8_t 
 */
uint8_t UartReadBuffer(uart_mcu_port_t port, uint8_t *data, uint16_t nbytes);

/**
 * @brief Send a single byte trough serial port
 * 
 * @param port Port for sending data
 * @param data Pointer to variable with data to be transmitted
 */
void UartSendByte(uart_mcu_port_t port, const char *data);

/**
 * @brief Send a String trough serial port
 * 
 * @note Sends data untill finding the '\0' character (used to indicate a String end).
 * 
 * @param port Port for sending data
 * @param msg Pointer to string to be transmitted
 */
void UartSendString(uart_mcu_port_t port, const char *msg);

/**
 * @brief Send multiple bytes through serial port
 * 
 * @param port Port for sending data
 * @param data Pointer to array of data to be transmitted
 * @param nbytes Number of bytes to be sended
 */
void UartSendBuffer(uart_mcu_port_t port, const char *data, uint8_t nbytes);

/**
 * @brief Convert a number to a String (char array ended with '\0')
 * 
 * @param val Number to be converted
 * @param base Base of the converted number (2: binary, 10: decimal, 16: hexadecimal)
 * @return uint8_t* 
 */
uint8_t* UartItoa(uint32_t val, uint8_t base);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif

/*==================[end of file]============================================*/
