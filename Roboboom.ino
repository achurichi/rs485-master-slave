// #include "./Librerias/Asminit/AsmInit.h"
// #include "./Librerias/NivelDeBateria/NivelDeBateria.h"
// #include "./Librerias/Movimiento/Movimiento.h"
#include <Movimiento.h>
#include <NivelDeBateria.h>
#include <SensorDeGas.h>
#include <ArduinoEEPROM.h>
#include <Datatype.h>

//Sensor de Gas
#define MQ2_ADC 6
#define DHT_PIN 5
#define VCC 5
#define RO 1455

//Nivel de Batería
#define BAT_ADC 2   // Pin analógico de lectura
#define BAT_MIN 3   // Tensión mínima (0%)
#define BAT_MAX 4.5 // Tensión máxima (100%)
#define V_REF 5     // Tensión de referencia

extern "C"
{
    void asm_init();
}

datos_resource d_resource;
datos_usuario_resource d_usuario_resource;

void setup()
{
    asm_init();                                                  // Inicializa el Atmega328p, salvo los timers.
    nivelDeBateriaInit((uint8_t)BAT_ADC, BAT_MIN, BAT_MAX, VCC); // Inicialización necesaria para usar las funciones de la libería NivelDeBateria
    sensorDeGasInit((uint8_t)MQ2_ADC, DHT_PIN, VCC, RO);         // Inicialización necesaria para usar las funciones de la libería SensorDeGas
}

void loop()
{
    movimientoInit(&d_resource, &d_usuario_resource); //No llamarla antes de tener los valores               // Inicialización necesaria para poder utilizar las funciones de la libreria Movimiento
}

ISR(TIMER1_OVF_vect)
{
    asm volatile ("cli\n");


    TCNT1H = 0x85;  
    TCNT1L = 0xED;          // Reinicio el timer en 84285 para obtener la interrupcion nuevamente en 500 ms.
    TCCR1B = 0x04;          // Activo nuevamente el timer 1.
    asm volatile ("sei\n");
}
