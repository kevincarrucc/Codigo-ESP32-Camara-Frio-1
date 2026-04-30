#ifndef PLANTA_H
#define PLANTA_H

#include "Teclado.h"
#include "Display.h"

enum Pantalla {
    PANTALLA_PRINCIPAL,
    PANTALLA_FASES,
    PANTALLA_LOGIN,
    PANTALLA_NUEVA_CLAVE,
    PANTALLA_CONFIGURACION,
    PANTALLA_CONFIG_TEMPERATURA,
    PANTALLA_CONFIG_HISTERESIS,
    PANTALLA_MENU_CONFIG_AVISOS,
    PANTALLA_MENU_CONFIG_ALARMAS,
    PANTALLA_CONFIG_AVISO_BAJO,
    PANTALLA_CONFIG_AVISO_ALTO,
    PANTALLA_CONFIG_ALARMA_BAJO,  
    PANTALLA_CONFIG_ALARMA_ALTO,
    PANTALLA_ALERTAS
};

class Planta {

private:
    Teclado& teclado;
    Display& display;

    Pantalla actual;
    Pantalla anterior;

    // -------- UI --------
    bool editando = false;
    String buffer = "";
    bool logueado = false;
    String nuevaClave1 = "";
    String nuevaClave2 = "";
    bool repitiendoClave = false;

    // -------- CONTROL --------
    unsigned long ultimoUpdate = 0;
    unsigned long ultimoRedibujoPantalla = 0;

public:
    Planta(Teclado& t, Display& d);

    void begin();
    void update();

private:
    void manejarTecla(char tecla);
    void actualizarPantalla();
    void refrescarPantalla();

    // Edición genérica
    void editarFloat(char tecla, float &variable, Pantalla pantallaVolver);

    // (Opcional pero recomendado)
    void limpiarBuffer();
};

#endif