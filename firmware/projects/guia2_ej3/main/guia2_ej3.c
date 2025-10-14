/*! @mainpage Sensor de distancia ultrasónico con FreeRTOS
 *
 * @section genDesc Descripción general
 *
 * Este programa utiliza FreeRTOS para gestionar tareas relacionadas con la
 * medición de distancia mediante un sensor ultrasónico **HC-SR04**, junto con
 * el control de **LEDs indicadores**, una **pantalla LCD** y comunicación **UART**.
 *
 * El sistema emplea temporizadores e interrupciones para sincronizar las tareas,
 * mientras que los **switches** y la **UART** permiten modificar el modo de funcionamiento:
 * - **Encendido/Apagado** del sistema.
 * - **Hold** para mantener el valor actual de distancia en pantalla.
 *
 * <a href="https://drive.google.com/...">Ejemplo de operación</a>
 *
 * @section hardConn Conexiones de hardware
 *
 * | Periférico   | ESP32 (GPIO) |
 * |:--------------|:-------------|
 * | SENSOR_TRIG   | GPIO_3       |
 * | SENSOR_ECHO   | GPIO_2       |
 * | SWITCH_1      | GPIO_X       |
 * | SWITCH_2      | GPIO_Y       |
 * | LED_1         | GPIO_A       |
 * | LED_2         | GPIO_B       |
 * | LED_3         | GPIO_C       |
 * | LCD           | GPIO_D       |
 *
 * @section changelog Historial de cambios
 *
 * | Fecha       | Descripción                                |
 * |:------------|:-------------------------------------------|
 * | 1/10/2025   | Creación del documento para el ejercicio 3 |
 *
 * @section author Autor
 *
 * Angeles Perez (maria.perez@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "math.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "timer_mcu.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/
/**
 * @brief Periodo de la interrupción del temporizador A (en microsegundos).
 */
#define CONFIG_BLINK_PERIOD1 1000000

/**
 * @brief Periodo de la interrupción del temporizador B (en microsegundos).
 */
#define CONFIG_BLINK_PERIOD 500000

/*==================[internal data definition]===============================*/
/**
 * @brief Variable que indica si el sistema está encendido.
 */
bool encendido = true;

/**
 * @brief Variable que indica si el valor de la distancia debe mantenerse fijo (modo hold).
 */
bool hold = false;

/**
 * @brief Valor de la distancia medida por el sensor HC-SR04.
 */
uint32_t distancia = 0;

/**
 * @brief Handle de la tarea encargada de medir la distancia.
 */
TaskHandle_t Mido = NULL;

/**
 * @brief Handle de la tarea encargada de mostrar la distancia.
 */
TaskHandle_t Muestro = NULL;

/*==================[internal functions declaration]=========================*/
void FuncTimerA(void* param);
void FuncTimerB(void* param);
void medirdistancia(void *pvParameter);
void mostrarDistancia(void *pvParameter);
void UartTask(void *pvParameter);
void CambioEncendido(void);
void CambioHold(void);
void cambioEstado(void *param);
void Leds(void);

/*==================[internal functions definition]==========================*/
/**
 * @brief Interrupción del temporizador A: notifica a la tarea de medición.
 */
void FuncTimerA(void* param) {
    vTaskNotifyGiveFromISR(Mido, pdFALSE);
}

/**
 * @brief Interrupción del temporizador B: notifica a la tarea de visualización.
 */
void FuncTimerB(void* param) {
    vTaskNotifyGiveFromISR(Muestro, pdFALSE);
}

/**
 * @brief Tarea de FreeRTOS que mide la distancia utilizando el sensor HC-SR04.
 */
void medirdistancia(void *pvParameter) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (encendido) {
            distancia = HcSr04ReadDistanceInCentimeters();
        }
    }
}

/**
 * @brief Cambia el estado de encendido/apagado del sistema.
 */
void CambioEncendido() {
    encendido = !encendido;
}

/**
 * @brief Cambia el estado del modo hold.
 */
void CambioHold() {
    hold = !hold;
}

/**
 * @brief Controla el encendido de LEDs según la distancia medida.
 */
void Leds() {
    if (distancia < 10) {
        LedOff(LED_1);
        LedOff(LED_2);
        LedOff(LED_3);
    } else if (distancia < 20) {
        LedOn(LED_1);
        LedOff(LED_2);
        LedOff(LED_3);
    } else if (distancia < 30) {
        LedOn(LED_1);
        LedOn(LED_2);
        LedOff(LED_3);
    } else {
        LedOn(LED_1);
        LedOn(LED_2);
        LedOn(LED_3);
    }
}

/**
 * @brief Tarea de FreeRTOS que actualiza la pantalla LCD y los LEDs.
 */
void mostrarDistancia(void *pvParameter) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (encendido) {
            Leds();
            if (!hold) {
                LcdItsE0803Write(distancia);
            }
        } else {
            LcdItsE0803Off();
            LedsOffAll();
        }
    }
}

/**
 * @brief Tarea que envía por UART la distancia medida.
 */
void UartTask(void *pvParameter) {
    while (true) {
        UartSendString(UART_PC, "La distancia es: ");
        UartSendString(UART_PC, (char*)UartItoa(distancia, 10));
        UartSendString(UART_PC, " cm\r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Callback UART: permite cambiar el estado de encendido o hold con comandos.
 */
void cambioEstado(void *param) {
    uint8_t caracter;
    UartReadByte(UART_PC, &caracter);
    if (caracter == 'o') encendido = !encendido;
    if (caracter == 'h') hold = !hold;
}

/*==================[external functions definition]==========================*/
/**
 * @brief Función principal: inicializa periféricos y crea tareas de FreeRTOS.
 */
void app_main(void) {

    /* Inicialización de LEDs, switches y periféricos */
    LedsInit();
    SwitchesInit();
    LcdItsE0803Init();
    HcSr04Init(GPIO_3, GPIO_2);

    /* Configuración de interrupciones por switch */
    SwitchActivInt(SWITCH_1, CambioEncendido, NULL);
    SwitchActivInt(SWITCH_2, CambioHold, NULL);

    /* Configuración de temporizadores */
    timer_config_t timer_led_1 = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_led_1);

    timer_config_t timer_led_2 = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD1,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
    TimerInit(&timer_led_2);

    /* Configuración del puerto UART */
    serial_config_t pantalla = {
        .port = UART_PC,
        .baud_rate = 9600,
        .func_p = cambioEstado,
        .param_p = NULL,
    };
    UartInit(&pantalla);

    /* Creación de tareas */
    xTaskCreate(&medirdistancia, "MideDistancia", 2048, NULL, 5, &Mido);
    xTaskCreate(&mostrarDistancia, "MuestraDistancia", 512, NULL, 5, &Muestro);
    xTaskCreate(&UartTask, "UART", 512, &pantalla, 5, NULL);

    /* Inicio de los temporizadores */
    TimerStart(timer_led_1.timer);
    TimerStart(timer_led_2.timer);
}

/*==================[end of file]============================================*/