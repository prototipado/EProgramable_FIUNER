/**
 * @file spi_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-02-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "spi_mcu.h"
#include <stdint.h>
#include <string.h>
#include "driver/spi_master.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define PIN_NUM_MISO	GPIO_22	/*!<  */
#define PIN_NUM_MOSI	GPIO_21	/*!<  */
#define PIN_NUM_CLK		GPIO_20	/*!<  */
#define PIN_NUM_CS1		GPIO_19	/*!<  */
#define PIN_NUM_CS2		GPIO_18	/*!<  */
#define PIN_NUM_CS3		GPIO_9	/*!<  */
/*==================[internal data declaration]==============================*/
spi_device_handle_t spi_1, spi_2, spi_3;
const spi_bus_config_t bus_cfg = {
    .miso_io_num = PIN_NUM_MISO,
    .mosi_io_num = PIN_NUM_MOSI,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 4092
};
transfer_mode_t transfer_mode_1, transfer_mode_2, transfer_mode_3;
void (*spi_1_isr_p)(void*);	/*!<  */
void (*spi_2_isr_p)(void*);	/*!<  */
void (*spi_3_isr_p)(void*);	/*!<  */
void *spi_1_user_data;	    /*!<  */
void *spi_2_user_data;	    /*!<  */
void *spi_3_user_data;	    /*!<  */
/*==================[internal functions declaration]=========================*/
static void IRAM_ATTR spi_1_isr(spi_transaction_t *t){
	spi_1_isr_p(spi_1_user_data);
}
static void IRAM_ATTR spi_2_isr(spi_transaction_t *t){
	spi_2_isr_p(spi_2_user_data);
}
static void IRAM_ATTR spi_3_isr(spi_transaction_t *t){
	spi_3_isr_p(spi_3_user_data);
}
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
uint8_t SpiInit(spi_mcu_config_t* spi){
    static bool spi_initialized = false;
    if(!spi_initialized){
	    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        spi_initialized = true;
    }
	spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = spi->bitrate,     	
        .mode = spi->clk_mode,                  
        .queue_size = 8,                        
    };
    switch(spi->device){
        case SPI_1:
            dev_cfg.spics_io_num = PIN_NUM_CS1;
            transfer_mode_1 = spi->transfer_mode;
            if(transfer_mode_1 == SPI_INTERRUPT){
                dev_cfg.post_cb = spi_1_isr;
            } 
            spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_1);
            spi_1_isr_p = spi->func_p;
            spi_1_user_data = spi->param_p;
            break;
        case SPI_2:
            dev_cfg.spics_io_num = PIN_NUM_CS2;
            if(transfer_mode_2 == SPI_INTERRUPT){
                dev_cfg.post_cb = spi_2_isr;
            } 
            transfer_mode_1 = spi->transfer_mode;
            spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_2);
            spi_2_isr_p = spi->func_p;
            spi_2_user_data = spi->param_p;
            break;
        case SPI_3:
            dev_cfg.spics_io_num = PIN_NUM_CS3;
            if(transfer_mode_3 == SPI_INTERRUPT){
                dev_cfg.post_cb = spi_3_isr;
            } 
            transfer_mode_1 = spi->transfer_mode;
            spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_3);
            spi_3_isr_p = spi->func_p;
            spi_3_user_data = spi->param_p;
            break;
    }
    return 0;
}

void SpiRead(spi_dev_t device, uint8_t * rx_buffer, uint32_t rx_buffer_size){
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       // Zero out the transaction
    t.length = rx_buffer_size * 8;  // tx_buffer_size is in bytes, transaction length is in bits.
    t.rxlength = rx_buffer_size * 8;
    t.rx_buffer = rx_buffer;        // Data
    switch(device){
        case SPI_1:
            switch(transfer_mode_1){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_1, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_1, &t); 
                    break;
            }
            break;
        case SPI_2:
            switch(transfer_mode_2){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_2, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_2, &t); 
                    break;
            }
            break;
        case SPI_3:
            switch(transfer_mode_3){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_3, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_3, &t); 
                    break;
            }
            break;
    }
}

void SpiWrite(spi_dev_t device, uint8_t * tx_buffer, uint32_t tx_buffer_size){
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       // Zero out the transaction
    t.length = tx_buffer_size * 8;  // tx_buffer_size is in bytes, transaction length is in bits.
    t.tx_buffer = tx_buffer;        // Data
    switch(device){
        case SPI_1:
            switch(transfer_mode_1){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_1, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_1, &t); 
                    break;
            }
            break;
        case SPI_2:
            switch(transfer_mode_2){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_2, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_2, &t); 
                    break;
            }
            break;
        case SPI_3:
            switch(transfer_mode_3){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_3, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_3, &t); 
                    break;
            }
            break;
    }
}

void SpiReadWrite(spi_dev_t device, uint8_t * tx_buffer, uint8_t * rx_buffer, uint32_t buffer_size){
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       // Zero out the transaction
    t.length = buffer_size * 8;     // tx_buffer_size is in bytes, transaction length is in bits.
    t.rxlength = buffer_size * 8;
    t.tx_buffer = tx_buffer;        // Data
    t.rx_buffer = rx_buffer;        
    switch(device){
        case SPI_1:
            switch(transfer_mode_1){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_1, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_1, &t); 
                    break;
            }
            break;
        case SPI_2:
            switch(transfer_mode_2){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_2, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_2, &t); 
                    break;
            }
            break;
        case SPI_3:
            switch(transfer_mode_3){
                case SPI_POLLING:
                    spi_device_polling_transmit(spi_3, &t); 
                    break;
                case SPI_INTERRUPT:
                    spi_device_transmit(spi_3, &t); 
                    break;
            }
            break;
    }
}

uint8_t SpiDeInit(spi_dev_t device){
    return 0;
}

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
