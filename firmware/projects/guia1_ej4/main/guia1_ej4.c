/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Angeles Perez (maria.perez@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>          // Para funciones de entrada/salida como printf
#include <stdint.h>         // Para tipos de datos enteros de tamaño fijo (uint8_t, uint32_t, etc.)
#include <stdbool.h>        // Para tipo de dato booleano
#include "freertos/FreeRTOS.h"  // Sistema operativo en tiempo real FreeRTOS
#include "freertos/task.h"      // Manejo de tareas de FreeRTOS
#include "led.h"            // Librería para manejo de LEDs
#include "switch.h"         // Librería para manejo de switches/botones
#include "math.h"           // Funciones matemáticas
#include "gpio_mcu.h"       // Librería para control de pines GPIO del microcontrolador

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/**
 * @brief Convierte un número decimal a un arreglo BCD (Binary Coded Decimal)
 * 
 * Esta función toma un número decimal y lo convierte dígito por dígito,
 * almacenando cada dígito en formato BCD en un arreglo.
 * 
 * @param data Número decimal a convertir (32 bits)
 * @param digits Cantidad de dígitos a extraer
 * @param bcd_number Puntero al arreglo donde se almacenarán los dígitos BCD
 * @return int8_t Código de retorno (0 = éxito)
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
    // Bucle para extraer cada dígito del número
    for (int i = 0; i < digits; i++)
    {
        bcd_number[i] = data % 10;  // Obtiene el dígito menos significativo (resto de dividir por 10)
        data = data / 10;           // Elimina el dígito menos significativo (división entera por 10)
    }   
    return 0;  // Retorna 0 indicando operación exitosa
}

/**
 * @brief Estructura para configuración de pines GPIO
 * 
 * Esta estructura define las propiedades de un pin GPIO:
 * - El número del pin
 * - La dirección (entrada o salida)
 */
typedef struct
{
    gpio_t pin;     /*!< Número del pin GPIO */
    io_t dir;       /*!< Dirección del GPIO: '0' = ENTRADA; '1' = SALIDA */
} gpioConf_t;

/**
 * @brief Mapea los bits de un dígito BCD a los pines GPIO correspondientes
 * 
 * Esta función toma un dígito BCD (4 bits) y activa/desactiva los pines GPIO
 * según el valor de cada bit para controlar un display de 7 segmentos.
 * 
 * @param arreglo Puntero al arreglo de configuración de pines GPIO
 * @param digits Dígito BCD a mapear (4 bits menos significativos)
 */
void mapearbits(gpioConf_t *arreglo, uint8_t digits)
{   
    
    // Bucle para revisar cada uno de los 4 bits del dígito BCD
    for(int i = 0; i < 4; i++)
    {
        // Verifica si el bit i-ésimo está en 1 usando máscara de bits (1<<i)
        if(digits & 1<<i)
        {
            GPIOOn(arreglo[i].pin);     // Si el bit es 1, pone el pin en estado ALTO
        }
        else
        {
            GPIOOff(arreglo[i].pin);    // Si el bit es 0, pone el pin en estado BAJO
        }
    }
}

/**
 * @brief Controla un display multiplexado de 3 dígitos usando BCD y decodificador 7447
 * 
 * Esta función toma un número de 3 dígitos, lo convierte a BCD y lo muestra
 * en un display de 7 segmentos multiplexado. Cada dígito se muestra secuencialmente
 * activando el pin correspondiente del multiplexor.
 * 
 * @param numero Número a mostrar (máximo 3 dígitos)
 * @param digito Cantidad de dígitos del número
 * @param arreglo Puntero al arreglo de pines para las líneas BCD (A,B,C,D)
 * @param mult Puntero al arreglo de pines para control del multiplexor
 */
void multipelxor(uint32_t numero, uint8_t digito, gpioConf_t *arreglo, gpioConf_t *mult)
{
    uint8_t vector[3];  // Arreglo para almacenar los 3 dígitos BCD
    
    // Convierte el número decimal a formato BCD
    convertToBcdArray(numero, digito, vector);
    
    // Bucle para mostrar cada dígito en el display multiplexado
    for (int i = 0; i < 3; i++)
    {
        // Mapea el dígito BCD actual a los pines de salida
        mapearbits(arreglo, vector[i]);
        
        printf("digito; %d", vector[i]);  // Debug: muestra el dígito actual
        
        // Activa el display correspondiente (multiplexor)
        GPIOOn(mult[i].pin);   // Selecciona el display i-ésimo
        GPIOOff(mult[i].pin);  // Desactiva el display (ciclo muy rápido para multiplexado)
    }
}

/*==================[external functions definition]==========================*/

/**
 * @brief Función principal del programa
 * 
 * Configura los pines GPIO y demuestra el funcionamiento del sistema
 * de display BCD multiplexado mostrando el número 138.
 */
void app_main(void)
{
    printf("Hello world!\n");  // Mensaje de inicio

    // Datos de prueba
    uint32_t data = 138;        // Número a mostrar en el display
    uint8_t digits = 3;         // Cantidad de dígitos del número
    uint8_t bcd_number[digits]; // Arreglo para almacenar dígitos BCD

    /*==================[Código comentado del ejercicio 4]==================
    convertToBcdArray (data,digits,bcd_number);
    {
        for (int i=0; i<digits;i++)
        {
            printf("el valor de cada digito es:%d ",bcd_number[i]);
        }   
    }  ejercicio 4
    =========================================================================*/

    // Configuración de pines GPIO para las líneas BCD (A, B, C, D del decodificador 7447)
    gpioConf_t arreglo[] = {
        {GPIO_20, GPIO_OUTPUT},  // Línea A del BCD (bit menos significativo)
        {GPIO_21, GPIO_OUTPUT},  // Línea B del BCD
        {GPIO_22, GPIO_OUTPUT},  // Línea C del BCD  
        {GPIO_23, GPIO_OUTPUT},  // Línea D del BCD (bit más significativo)
    };

    // Configuración de pines GPIO para control del multiplexor (selección de display)
    gpioConf_t mult[] = {
        {GPIO_9,  GPIO_OUTPUT},  // Control para el primer display (unidades)
        {GPIO_18, GPIO_OUTPUT},  // Control para el segundo display (decenas)
        {GPIO_19, GPIO_OUTPUT},  // Control para el tercer display (centenas)
    };

    // Inicialización de los pines BCD como salidas
    for (int i = 0; i < 4; i++)
    {
        GPIOInit(arreglo[i].pin, arreglo[i].dir);
    }
    
    // Inicialización de los pines del multiplexor como salidas
    for (int i = 0; i < 3; i++)
    {
        GPIOInit(mult[i].pin, mult[i].dir);
    }
    
    // Ejecuta la función de multiplexado para mostrar el número en el display
    multipelxor(data, digits, arreglo, mult);
}