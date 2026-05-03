#ifndef ESCALADO_H
#define ESCALADO_H

#include <Arduino.h>

// Funciones para convertir lecturas crudas a valores reales
float escalarTemperatura(int bits);
float escalarTension(int bits);
float escalarCorrienteADS(uint16_t bitsADS); // Para el ADS1015 (divisor 2047)

//Prueba
// Convierte bits de ADC interno (0-4095) a Voltios (0-3.3V)
float calcularVoltiosESP32(int bits);

// Convierte bits de ADS1015 (0-2047) a Voltios (0-4.096V)
float calcularVoltiosADS(int16_t bitsADS);

// En Escalado.h, antes del #endif
void procesarTemperaturas(int bitsC1, int bitsC2, int bitsExt, float &t1, float &t2, float &tExt);

float escalarTension(int bits);
void procesarTensiones(int bitsL1, int bitsL2, int bitsL3, float &vL1, float &vL2, float &vL3);

void Escalar(void);
float Calcular_Potencia_Aparente(float tensionL1, float tensionL2, float tensionL3,
                              float corrienteL1, float corrienteL2, float corrienteL3);

#endif