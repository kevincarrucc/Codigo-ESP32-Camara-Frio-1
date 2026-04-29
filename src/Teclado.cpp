#include "Teclado.h"
#include <Arduino.h>

Teclado::Teclado(uint8_t direccion) : pcf(direccion) {}

void Teclado::begin() {
    pcf.begin();

    // Filas como salida
    for (int i = 0; i < 4; i++) {
        pcf.pinMode(filas[i], OUTPUT);
        pcf.digitalWrite(filas[i], HIGH);
    }

    // Columnas como entrada
    for (int i = 0; i < 4; i++) {
        pcf.pinMode(columnas[i], INPUT);
    }
}

char Teclado::leer() {

    char teclaDetectada = '\0';

    for (int fila = 0; fila < 4; fila++) {

        // Poner todas las filas en HIGH
        for (int i = 0; i < 4; i++) {
            pcf.digitalWrite(filas[i], HIGH);
        }

        // Activar fila actual
        pcf.digitalWrite(filas[fila], LOW);

        // Leer columnas
        for (int col = 0; col < 4; col++) {

            if (pcf.digitalRead(columnas[col]) == LOW) {
                teclaDetectada = teclas[fila][col];
            }
        }
    }

    // ------------------ ANTIRREBOTE ------------------

    unsigned long ahora = millis();

    if (teclaDetectada != '\0') {

        if (teclaDetectada != ultimaTecla || (ahora - ultimoTiempo > debounce)) {

            ultimaTecla = teclaDetectada;
            ultimoTiempo = ahora;

            return teclaDetectada;
        }

    } else {
        // Reset cuando se suelta
        ultimaTecla = '\0';
    }

    return '\0';
}