#include "EntradaSalida.h"
#include "arduino.h"

unsigned long referenciaAccion = 0;   // guarda el instante de inicio
bool accionActiva = false;            // bandera de control



/*Funciones*/
// contador universal: millis() corre siempre


// función para activar el temporizador
void activarTemporizador(unsigned long intervalo) {
    referenciaAccion = millis();   // cargo el valor inicial
    accionActiva = true;           // levanto bandera
}

// función para verificar si ya pasó el tiempo
bool temporizadorCumplido(unsigned long intervalo) {
    if (accionActiva && (millis() - referenciaAccion >= intervalo)) {
        accionActiva = false;      // reseteo bandera
        return true;               // ✔ tiempo cumplido
    }
    return false;                  // todavía no
}

void Control_Temperatura(void) {
    /*Promedio de temperaturas*/
    Temperatura_Promedio = (Temperatura_C1 + Temperatura_C2) / 2;
    /*Accion sobre el motor 1-2 y el ventilador*/
   digitalWrite(Actuador_Ventilador_M1, HIGH);
    if (Referencia_Temperatura + Ventana_Temperatura < Temperatura_Promedio) {
        digitalWrite(Actuador_M1, HIGH);
    }
    else if (Referencia_Temperatura - Ventana_Temperatura > Temperatura_Promedio)
    {
       
    }
    
}


