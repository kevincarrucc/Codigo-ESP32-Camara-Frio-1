
#ifndef CLASES_H
#define CLASES_H

#include <Arduino.h>
#include "Variables.h" 

// --- CLASE PARPADEO ---
class Parpadeo {
    private:
        int Pin_Parpadeo;
        unsigned long Intervalo_Parpadeo;
        unsigned long Ultimo_Parpadeo;
        bool Estado; 
    public:
        Parpadeo(int Pin, unsigned long Intervalo);
        void parpadear(void);
};

// --- CLASE MOTOR ---
class Motor {
    private:
        int Pin_Motor;
    public:
        Motor (int Pin);
        void arranque(void);
        void parada(void);
}; 

// --- CLASE FASE ---
class Fase {
    private:
        int pin;
        bool* pAlarma;
        float* pTension;
        
        // Definimos los umbrales de la ventana
        const float UMBRAL_BAJO = 197.5f; // Se activa la alarma
        const float UMBRAL_ALTO = 202.5f; // Se desactiva la alarma (recuperación)

    public:
        Fase(int _pin, bool &_alarma, float &_tension) {
            pin = _pin;
            pAlarma = &_alarma; 
            pTension = &_tension;
        }

        void presencia() {
            // Lógica de Histéresis:
            
            // 1. Si la tensión cae por debajo del límite crítico
            if (*pTension <= UMBRAL_BAJO) {
                *pAlarma = true;
                digitalWrite(pin, LOW); // Activa alerta física
            } 
            
            // 2. Si la tensión sube por encima del límite de recuperación
            else if (*pTension >= UMBRAL_ALTO) {
                *pAlarma = false;
                digitalWrite(pin, HIGH); // Desactiva alerta (Fase OK)
            }
            
            // 3. Si está entre 195 y 205, NO HACE NADA. 
            // Mantiene el estado anterior (esto evita el "zapateo").
        }
};

// --- IMPLEMENTACIÓN DE MÉTODOS ---

inline Parpadeo::Parpadeo(int Pin, unsigned long Intervalo) {
    Pin_Parpadeo = Pin;
    Intervalo_Parpadeo = Intervalo;
    Ultimo_Parpadeo = millis();
    Estado = LOW; 
}

inline void Parpadeo::parpadear(void) {
    if (millis() - Ultimo_Parpadeo >= Intervalo_Parpadeo) {
        Estado = !Estado; 
        digitalWrite(Pin_Parpadeo, Estado);
        Ultimo_Parpadeo = millis();
    }
}

inline Motor::Motor(int _Pin) {
    Pin_Motor = _Pin;
}
// Nota: He usado extern para Arranque_M1/M2 si no están en EntradaSalida.h
extern unsigned long Arranque_M1; 
extern unsigned long Arranque_M2;

inline void Motor::arranque(void){
    if (digitalRead(Pin_Motor) == LOW){
        digitalWrite(Pin_Motor, HIGH);
    }
    if (Pin_Motor == Actuador_M1){
        Arranque_M1 = millis();
        M1_Encendido = true;
    }
    else if (Pin_Motor == Actuador_M2){
        Arranque_M2 = millis();
        M2_Encendido = true;
    }
}

inline void Motor::parada(void){
    if (digitalRead(Pin_Motor) == HIGH){
        digitalWrite(Pin_Motor, LOW);
    }
    if (Pin_Motor == Actuador_M2 ){
        Ultimo_Estado_M2 = false;
        M2_Encendido = false;
        if (Realizar_Arranque_Periodico == true){
        Realizar_Arranque_Periodico = false;
        }
    }
    else if (Pin_Motor == Actuador_M1){
        M1_Encendido = false;
    }
    if (Anomalia == true){
        Anomalia = false;
        digitalWrite(Actuador_Alerta_Anomalia, LOW); // Apagamos la alerta física de anomalía
    }
}


#endif