/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * Este programa implementa una aplicación de adquisición y transmisión de señales analógicas
 * utilizando un microcontrolador con FreeRTOS.
 *
 * - Se toma una entrada analógica mediante el canal CH1.
 * - La señal adquirida se envía a la PC por UART (puerto serie) para su visualización.
 * - En paralelo, una segunda tarea genera una señal ECG simulada y la envía por una salida D/A.
 * - Ambas tareas se sincronizan mediante temporizadores independientes (TIMER_A y TIMER_B).
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * | Peripheral | ESP32 Pin |
 * |:-----------:|:---------:|
 * | Entrada A/D (CH1) | GPIO correspondiente al canal CH1 |
 * | Salida D/A        | GPIO configurado para DAC         |
 * | UART_TX (PC)      | GPIO conectado al puerto serie de PC |
 *
 * @section changelog Changelog
 *
 * | Date | Description |
 * |:-----:|:------------|
 * | 08/10/2025 | Document creation |
 *
 * @author
 * Angeles Perez (maria.perez@ingenieria.uner.edu.ar)
 */

/*==================[inclusiones]============================================*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/

/**
 * @brief Tamaño del buffer que almacena una señal ECG simulada.
 *
 * Se trata de un conjunto de 231 muestras que simulan un ciclo de una señal ECG.
 */
#define BUFFER_SIZE 231

/*==================[internal data definition]===============================*/

/**
 * @brief Manejadores (handles) de las tareas creadas.
 *
 * Se utilizan para enviar notificaciones desde las interrupciones de los temporizadores.
 */
TaskHandle_t cambio = NULL;           ///< Handle de la tarea encargada de leer el ADC y enviar por UART.
TaskHandle_t main_task_handle = NULL; ///< Handle de la tarea que genera y muestra el ECG simulado por DAC.

/**
 * @brief Arreglo con los valores de la señal ECG simulada (de 8 bits).
 *
 * Estos valores se envían secuencialmente al DAC para reconstruir la forma de onda.
 */
const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

/*==================[internal functions declaration]=========================*/

/**
 * @brief Callback de interrupción del temporizador A.
 *
 * Envía una notificación a la tarea encargada de leer la entrada analógica.
 * Esta función **no bloquea** y debe ser lo más breve posible.
 */
void FuncTimerA(void* param) {
    vTaskNotifyGiveFromISR(cambio, pdFALSE); 
}

/**
 * @brief Callback de interrupción del temporizador B.
 *
 * Envía una notificación a la tarea encargada de mostrar la señal ECG simulada.
 */
void FuncTimerB(void* param) {
    vTaskNotifyGiveFromISR(main_task_handle, pdFALSE); 
}

/*==================[internal tasks definition]==============================*/

/**
 * @brief Tarea de adquisición y transmisión serie.
 *
 * Esta tarea se bloquea hasta recibir una notificación del temporizador A.
 * Cada vez que se activa:
 * - Lee un valor analógico desde CH1 mediante el ADC.
 * - Convierte ese valor a texto (itoa).
 * - Lo envía a través del puerto serie UART hacia la PC.
 */
void Funcion(void* param)
{
    uint16_t guardado; // Variable donde se almacena la lectura del ADC
    while (true)
    { 
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Espera notificación del timer A
        AnalogInputReadSingle(CH1, &guardado);   // Lectura analógica
        UartSendString(UART_PC, (char*)UartItoa(guardado, 10)); // Conversión y envío
        UartSendString(UART_PC, "\r"); // Retorno de carro (para graficadores tipo serial plotter)
    }
}

/**
 * @brief Tarea que genera la señal ECG simulada en la salida analógica.
 *
 * Se bloquea hasta recibir una notificación del temporizador B.
 * Luego:
 * - Toma el siguiente valor del arreglo `ecg[]`
 * - Lo escribe en el DAC (salida analógica)
 * - Cuando llega al final del arreglo, vuelve al inicio (señal periódica)
 */
void mostrarecg()
{
    int i = 0;
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Espera notificación del timer B
        if (i < BUFFER_SIZE)
        {
            AnalogOutputWrite(ecg[i]); // Escribe el valor en la salida D/A
        }
        else
        {
            i = 0; // Reinicia el índice para repetir la señal
        }
        i++;
    }
}

/*==================[external functions definition]==========================*/

/**
 * @brief Función principal de la aplicación (punto de entrada de FreeRTOS).
 *
 * Aquí se inicializan los periféricos, temporizadores y tareas.
 */
void app_main(void)
{
    // --- Configuración de entrada analógica (ADC) ---
    analog_input_config_t config = {
        .input = CH1,          // Canal de entrada
        .mode = ADC_SINGLE,    // Modo de lectura única
        .func_p = NULL,        // Sin callback (lectura manual)
        .param_p = NULL,
        .sample_frec = 0       // Frecuencia de muestreo no utilizada en modo single
    };
    AnalogInputInit(&config);
    
    // --- Configuración del temporizador A (para la lectura A/D) ---
    timer_config_t timer_led_1 = {
        .timer = TIMER_A,
        .period = 2000,        // Periodo en microsegundos (ajusta la frecuencia de muestreo)
        .func_p = FuncTimerA,  // Callback asociado
        .param_p = NULL
    };
    TimerInit(&timer_led_1);

    // --- Configuración del temporizador B (para la salida ECG) ---
    timer_config_t timer_led_2 = {
        .timer = TIMER_B,
        .period = 4000,        // Periodo (controla la velocidad de reproducción del ECG)
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_led_2);

    // --- Configuración de la comunicación serie (UART) ---
    serial_config_t pantalla = {
        .port = UART_PC,
        .baud_rate = 115200,   // Velocidad típica para terminal o graficador
        .func_p = NULL,
        .param_p = NULL
    };
    UartInit(&pantalla);

    // --- Creación de las tareas de FreeRTOS ---
    // Prioridad media-alta (5), stack de 2048 bytes
    xTaskCreate(&Funcion, "OSCILOSCOPIO", 2048, NULL, 5, &cambio);
    xTaskCreate(&mostrarecg, "ECG_SIMULADO", 2048, NULL, 5, &main_task_handle);
    
    // --- Inicialización y arranque de temporizadores ---
    TimerStart(timer_led_1.timer);
    TimerStart(timer_led_2.timer);
    
    // --- Inicialización de la salida analógica (DAC) ---
    AnalogOutputInit();
}

/*==================[end of file]============================================*/
