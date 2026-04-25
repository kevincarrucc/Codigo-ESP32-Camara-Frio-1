#ifndef TECLADO_H
#define TECLADO_H

#include <Arduino.h>
#include <PCF8574.h>

class Teclado {

private:
    PCF8574 pcf;

    // Mapa de teclas
    char teclas[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    uint8_t filas[4] = {0, 1, 2, 3};
    uint8_t columnas[4] = {4, 5, 6, 7};

    char ultimaTecla = '\0';
    unsigned long ultimoTiempo = 0;
    const unsigned long debounce = 150;

public:
    Teclado(uint8_t direccion);

    void begin();

    char leer();  // devuelve tecla presionada (o '\0')

};

#endif