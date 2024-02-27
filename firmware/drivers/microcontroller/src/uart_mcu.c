/**
 * @file uart_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-02-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "uart_mcu.h"
#include "gpio_mcu.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
/*==================[macros and definitions]=================================*/
#define UART_CONN_TX        GPIO_18         /*!<  */
#define UART_CONN_RX        GPIO_19         /*!<  */
#define TX_BUFFER_SIZE      256             /*!<  */
#define RX_BUFFER_SIZE      256             /*!<  */
#define EVENT_QUEUE_SIZE    16              /*!<  */
#define READ_TIMEOUT        100             /*!<  */
/*==================[internal data declaration]==============================*/
void (*uart_pc_isr_p)(void*);	            /*!<  */
void (*uart_conn_isr_p)(void*);	            /*!<  */
void *uart_pc_user_data;	                /*!<  */
void *uart_conn_user_data;	                /*!<  */
static QueueHandle_t uart_pc_queue;         /*!<  */
static QueueHandle_t uart_conn_queue;       /*!<  */
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
static void uart_pc_event_task(void *pvParameters){
    uart_event_t event;
    uart_driver_install(UART_NUM_0, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 16, &uart_pc_queue, 0);
    while(1){
        //Waiting for UART event.
        if (xQueueReceive(uart_pc_queue, (void *)&event, (TickType_t)portMAX_DELAY)){
            switch(event.type) {
                case UART_DATA:
                    uart_pc_isr_p(uart_pc_user_data);
                    break;
                case UART_BREAK:
                    break;
                case UART_BUFFER_FULL:
                    break;
                case UART_FIFO_OVF:
                    break;
                case UART_FRAME_ERR:
                    break;
                case UART_PARITY_ERR:
                    break;
                case UART_DATA_BREAK:
                    break;
                case UART_PATTERN_DET:
                    break;
                case UART_WAKEUP:
                    break;
                case UART_EVENT_MAX:
                    break;
            }
        }
    }
}

static void uart_conn_event_task(void *pvParameters){
    uart_event_t event;
    uart_driver_install(UART_NUM_1, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 16, &uart_conn_queue, 0);
    while(1){
        //Waiting for UART event.
        if(xQueueReceive(uart_conn_queue, (void *)&event, (TickType_t)portMAX_DELAY)){
            switch(event.type) {
                case UART_DATA:
                    uart_conn_isr_p(uart_conn_user_data);
                    break;
                case UART_BREAK:
                    break;
                case UART_BUFFER_FULL:
                    break;
                case UART_FIFO_OVF:
                    break;
                case UART_FRAME_ERR:
                    break;
                case UART_PARITY_ERR:
                    break;
                case UART_DATA_BREAK:
                    break;
                case UART_PATTERN_DET:
                    break;
                case UART_WAKEUP:
                    break;
                case UART_EVENT_MAX:
                    break;
            }
        }
    }
}
/*==================[external functions definition]==========================*/

void UartInit(serial_config_t *port_config){
    uart_config_t uart_config = {
        .baud_rate = port_config->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    switch(port_config->port){
        case UART_PC:
            uart_param_config(UART_NUM_0, &uart_config);
            uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
            if(port_config->func_p != UART_NO_INT){
                uart_pc_isr_p = port_config->func_p;
                uart_pc_queue = port_config->param_p;
                xTaskCreate(uart_pc_event_task, "uart_pc_event_task", 2048, NULL, 12, 0);
            }else{
                uart_driver_install(UART_NUM_0, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 0, NULL, 0);
            }
            break;
        case UART_CONNECTOR:
            uart_param_config(UART_NUM_1, &uart_config);
            uart_set_pin(UART_NUM_1, UART_CONN_TX, UART_CONN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
            if(port_config->func_p != UART_NO_INT){
                uart_conn_isr_p = port_config->func_p;
                uart_conn_queue = port_config->param_p;
                xTaskCreate(uart_conn_event_task, "uart_conn_event_task", 2048, NULL, 12, NULL);
            }else{
                uart_driver_install(UART_NUM_1, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 0, NULL, 0);
            }
            break;
    }
}

uint8_t UartReadByte(uart_mcu_port_t port, uint8_t* data){
    uart_port_t uart_num = UART_NUM_0;
    uint16_t length = 0;
    switch(port){
        case UART_PC:
                uart_num = UART_NUM_0;
            break;
        case UART_CONNECTOR:
                uart_num = UART_NUM_1;
            break;
    }
    length = uart_read_bytes(uart_num, data, 1, READ_TIMEOUT);
    if(length > 0){
        return true;
    } else{
        return false;
    }
}

uint8_t UartReadBuffer(uart_mcu_port_t port, uint8_t* data, uint16_t nbytes){
    uart_port_t uart_num = UART_NUM_0;
    uint16_t length = 0;
    switch(port){
        case UART_PC:
                uart_num = UART_NUM_0;
            break;
        case UART_CONNECTOR:
                uart_num = UART_NUM_1;
            break;
    }
    length = uart_read_bytes(uart_num, data, nbytes, READ_TIMEOUT);
    if(length > 0){
        return true;
    } else{
        return false;
    }
}

void UartSendByte(uart_mcu_port_t port, const char *data){
    uart_port_t uart_num = UART_NUM_0;
    switch(port){
        case UART_PC:
                uart_num = UART_NUM_0;
            break;
        case UART_CONNECTOR:
                uart_num = UART_NUM_1;
            break;
    }
    uart_tx_chars(uart_num, data, 1);
}

void UartSendString(uart_mcu_port_t port, const char *msg){
    uart_port_t uart_num = UART_NUM_0;
    switch(port){
        case UART_PC:
                uart_num = UART_NUM_0;
            break;
        case UART_CONNECTOR:
                uart_num = UART_NUM_1;
            break;
    }
	while(*msg != 0){
        uart_tx_chars(uart_num, msg, 1);
		msg++;
	}
}

void UartSendBuffer(uart_mcu_port_t port, const char *data, uint8_t nbytes){
    uart_port_t uart_num = UART_NUM_0;
    switch(port){
        case UART_PC:
                uart_num = UART_NUM_0;
            break;
        case UART_CONNECTOR:
                uart_num = UART_NUM_1;
            break;
    }
    uart_tx_chars(uart_num, data, nbytes);
}

uint8_t* UartItoa(uint32_t val, uint8_t base){
	static uint8_t buf[32] = {0};
	uint32_t i = 30;
    if(val == 0){
        return (uint8_t*)"0";
    }else{
        for(; val && i ; --i, val /= base){
            buf[i] = "0123456789abcdef"[val % base];
        }
        return &buf[i+1];
    }
}

/*==================[end of file]============================================*/