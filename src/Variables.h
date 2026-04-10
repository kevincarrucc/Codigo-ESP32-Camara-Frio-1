#ifndef ENTRADASALIDA_H
#define ENTRADASALIDA_H

#include <Arduino.h>

// 1. PINES (CONSTANTES)
const int Temperatura_C1_Pin = 33;
const int Temperatura_C2_Pin = 32;
const int Temperatura_Ext_Pin = 35;
const int Tension_L1_Pin = 36;
const int Tension_L2_Pin = 39;
const int Tension_L3_Pin = 34;
const int Pulsador_Luz_Pin = 2;
const int Final_Puerta_Pin = 16;
const int Actuador_M1 = 19;
const int Actuador_M2 = 18;
const int Actuador_Ventilador_M1 = 5;
const int Actuador_Luz_Camara = 23;
const int Actuador_Alerta_Puerta = 4;
const int Actuador_Alerta_Anomalia = 15;
const int Actuador_F1 = 25;
const int Actuador_F2 = 26;
const int Actuador_F3 = 27;

// 2. VARIABLES GLOBALES (EXTERN)
// Estas variables se crean en el main y se usan en los otros archivos
extern float Temperatura_C1, Temperatura_C2, Temperatura_Exterior;
extern float Tension_L1, Tension_L2, Tension_L3;
extern int16_t Corriente_L1_Lectura, Corriente_L2_Lectura, Corriente_L3_Lectura;
extern float Corriente_L1, Corriente_L2, Corriente_L3;
extern int Referencia_Temperatura, Ventana_Temperatura, Temperatura_Promedio;
extern bool Realizar_Arranque_Periodico, Ultimo_Estado_M2;
extern bool Ausencia_Fase1, Ausencia_Fase2, Ausencia_Fase3, Ultimo_Estado_Luz;
extern bool Anomalia, Aviso_Ausencia, Alerta_Puerta_Abierta, M1_Encendido, M2_Encendido, Puerta_Abierta, P_Pulsador_Luz, Estado_Luz;
extern int lastButtonState;
extern unsigned long ultimoEnvio;
extern const unsigned long intervaloEnvio;

#endif