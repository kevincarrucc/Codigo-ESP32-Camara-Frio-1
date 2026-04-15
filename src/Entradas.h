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
void callback(char* topic, unsigned char* payload, unsigned int length);



#endif