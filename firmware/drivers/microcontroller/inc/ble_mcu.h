#ifndef BLE_MCU_H
#define BLE_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup BLE Bluetooth Low Energy
 ** @{ */

/** \brief BLE driver for the ESP-EDU Board.
 *
 * @note This driver emulates HM-10 functionalities (same services and characteristics),
 * so it can be used to communicate with common Android apps, like "Bluetooth Electronics"
 * (https://play.google.com/store/apps/details?id=com.keuwl.arduinobluetooth)
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 22/03/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
/*==================[macros]=================================================*/
#define BLE_NO_INT	0		/*!< Flag used when no reading interruption is required */
/*==================[typedef]================================================*/
/**
 * @brief Prototype of callback function for reading received data 
 * 
 * @param data      pointer to received data array
 * @param length    number of bytes of received data
 */
typedef void (*read_func) (uint8_t * data, uint8_t length);

/**
 * @brief BLE configuration struct
 */
typedef struct {			
	char * device_name;		/*!< BLE device name */
	read_func func_p;		/*!< Pointer to callback function to call when receiving data (= BLE_NO_INT if not requiered) */
} ble_config_t;

/**
 * @brief BLE connection status
 */
typedef enum ble_status {
	BLE_OFF,				/*!< BLE inactive */
	BLE_DISCONNECTED,		/*!< BLE device disconnected */
	BLE_CONNECTED			/*!< BLE device connected */
} ble_status_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Bluetooth initialization
 * 
 * @note It'll start advertising automatically after initialization.
 * 
 * @param ble_device BLE configuration struct
 */
void BleInit(ble_config_t * ble_device);

/**
 * @brief Gets BLE connection status
 * 
 * @return ble_status_t Connection status
 */
ble_status_t BleStatus(void);

/**
 * @brief Send a single byte trough BLE (if connected)
 * 
 * @param data Pointer to variable with data to be transmitted
 */
void BleSendByte(const char *data);

/**
 * @brief Send a string trough BLE (if connected)
 * 
 * @param msg Pointer to string to be transmitted
 */
void BleSendString(const char *msg);

/**
 * @brief Send multiple bytes through serial port
 * 
 * @param data Pointer to array of data to be transmitted
 * @param nbytes Number of bytes to be sended
 */
void BleSendBuffer(const char *data, uint8_t nbytes);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* BLE_MCU_H */

/*==================[end of file]============================================*/
