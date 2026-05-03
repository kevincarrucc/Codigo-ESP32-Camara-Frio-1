#include "Escalado.h"
#include "Variables.h" 

extern float Potencia_Aparente;
// --- Funciones Matemáticas de Conversión ---

float escalarTemperatura(int bits) {
    return (bits * 55.0f / 4095.0f) - 5.0f; // -5 a 50°C
}

float escalarTension(int bits) {
    return (bits * 300.0f) / 4095.0f;      // 0 a 300V
}

float escalarCorrienteADS(uint16_t bitsADS) {
    return (bitsADS * 100.0f) / 65535.0f; // 0 a 100 A
}


float calcularVoltiosESP32(int bits) {
    return (bits * 3.3f) / 4095.0f;
}

float calcularVoltiosADS(int16_t bitsADS) {
    return (bitsADS * 4.096f) / 2047.0f;
}

// --- Función Maestra (Encapsulada) ---

void Escalar(void) {
    // 1. Procesar Temperaturas
    Temperatura_C1       = escalarTemperatura(analogRead(Temperatura_C1_Pin));
    Temperatura_C2       = escalarTemperatura(analogRead(Temperatura_C2_Pin));
    Temperatura_Exterior = escalarTemperatura(analogRead(Temperatura_Ext_Pin));

    // 2. Procesar Tensiones
    Tension_L1 = escalarTension(analogRead(Tension_L1_Pin));
    Tension_L2 = escalarTension(analogRead(Tension_L2_Pin));
    Tension_L3 = escalarTension(analogRead(Tension_L3_Pin));

    // 3. Procesar Corrientes (Variables ya leídas del ADS)
    Corriente_L1 = escalarCorrienteADS(Corriente_L1_Lectura);
    Corriente_L2 = escalarCorrienteADS(Corriente_L2_Lectura);
    Corriente_L3 = escalarCorrienteADS(Corriente_L3_Lectura);
}

float Calcular_Potencia_Aparente(float tensionL1, float tensionL2, float tensionL3,
                              float corrienteL1, float corrienteL2, float corrienteL3) {
    // Potencia aparente por fase (VA)
    float potenciaL1 = tensionL1 * corrienteL1;
    float potenciaL2 = tensionL2 * corrienteL2;
    float potenciaL3 = tensionL3 * corrienteL3;

    // Potencia total en kVA
    Potencia_Aparente = (potenciaL1 + potenciaL2 + potenciaL3) / 1000.0f;

    return Potencia_Aparente;
}