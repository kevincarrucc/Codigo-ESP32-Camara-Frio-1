#ifndef TECLADO_H
#define TECLADO_H

#include <Arduino.h>

class Teclado {

private:
    char ultimaTecla = '\0';
    unsigned long ultimoTiempo = 0;
    const unsigned long debounce = 150;
    bool teclaPresionada = false;

public:
    Teclado(uint8_t direccion);

    void begin();

    char leer();  // devuelve tecla presionada (o '\0')

};

#endif