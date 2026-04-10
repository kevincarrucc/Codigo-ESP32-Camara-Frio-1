#ifndef ENTRADAS_H
#define ENTRADAS_H

#include <Arduino.h>

// Prototipos de funciones de hardware y red
void inicializarHardware();
void inicializarComunicaciones();
void loop_I2C();
void procesarLogicaControl();
void setup_I2C();
void setup_wifi();
void reconnect();

// Prototipo del callback de MQTT (necesario si se usa en otros lados)
void callback(char* topic, byte* payload, unsigned int length);

#endif