#include "Variables.h"
#include "arduino.h"
#include "Clases.h"
#include "Claves.h"
#include <Firebase_ESP_Client.h> 
#include <PubSubClient.h>
#include "Planta.h"
#include "Logica.h"



extern Planta planta; 

// ... el resto de tus externs (Estado_Luz, etc.)

extern bool avisoReconocido;
extern bool alertaReconocida;

extern const char* TOPIC_ACT_LUZ_CAMARA;
extern const char* TOPIC_ACT_WEB_LUZ_CAMARA;

// Esto le avisa al compilador que las variables existen en el main
// DECLARACIONES EXTERN
extern bool Estado_Luz;
extern bool Estado_Web_Luz;
extern int ultimoEstadoTeclaFisica;
extern int ultimoEstadoWeb; 
bool ultimoProcesadoWeb = false;


extern unsigned long ultimoEnvio;
extern unsigned long Arranque_M1;
extern unsigned long Arranque_M2;
extern PubSubClient client;

const unsigned long Retardo_Motor2 = 5000; // Retardo de 5 segundos para encender el motor 2 después de encender el motor 1
const unsigned long Espera_Respuesta_M1 = 650000; // Espera de 15 minutos para la respuesta del motor 1
//unsigned long Arranque_M1 = 0; // Variable para almacenar el tiempo en que se activó el motor 1
const unsigned long Arranque_Periodico_M2 = 86400000; // Arranque periódico del motor 2 cada 24 horas
unsigned long Ultimo_Arranque_M2 = 0; // Variable para almacenar el tiempo del último arranque del motor 2
const unsigned long Espera_Respuesta_M2 = 650000; // Espera de 5 minutos para la respuesta del motor 2
//unsigned long Arranque_M2 = 0; // Variable para almacenar el tiempo en que se activó el motor 2
const unsigned long Retardo_Alerta_Puerta = 60000; // Retardo de 1 minuto para la alerta de puerta abierta
unsigned long High_Puerta = 0; // Variable para almacenar el tiempo en que se activó la alerta de puerta abierta
const unsigned long Intermitencia_Alerta = 1000; // Intermitencia de la alerta de puerta abierta (1 segundo)
unsigned long Ultimo_Alerta_Puerta = 0; // Variable para almacenar el tiempo del último parpadeo de la alerta de puerta abierta
const unsigned long Espera_Anomalia = 600000;  //1800000;   Espera de 10 minutos para detectar una anomalía en el funcionamiento de los motores, si alguno de los motores lleva encendido más de este tiempo, se detecta una anomalía

//unsigned long ultimoTiempoCambioLuz = 0;
//const unsigned long tiempoDebounce = 200; // 200 milisegundos de "bloqueo"
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms
int lastReading = HIGH;
// Esto le dice a Logica.cpp: "Busca estos objetos en el main"
extern Motor Motor_1;
extern Motor Motor_2;
extern Parpadeo Puerta;
extern Parpadeo anomalia;
extern Fase Fase_1;
extern Fase Fase_2;
extern Fase Fase_3;

/*Funciones*/
void Control_Temperatura(void);
void Alerta_Puerta (void);
void Luz_Interior (void);
void Funcionamiento_Periodico_M2 (void);
void Control_Anomalias (void);
void Control_Fases (void);

void Control_Temperatura(void) {
    /*variables locales*/
    int Estado_M1 = digitalRead(Actuador_M1);
    int Estado_M2 = digitalRead(Actuador_M2);
    

    /*Promedio de temperaturas*/
    Temperatura_Promedio = (Temperatura_C1 + Temperatura_C2) / 2;

if (Aviso_Ausencia == false){
    /*Accion sobre el motor 1-2 y el ventilador*/
   digitalWrite(Actuador_Ventilador_M1, HIGH);// El ventilador del motor 1 siempre está encendido

    if (Referencia_Temperatura + Ventana_Temperatura <= Temperatura_Promedio && (Estado_M1 == LOW || Estado_M2 == LOW))
     {  
        if (Realizar_Arranque_Periodico == true) {
            Motor_2.arranque(); // Realiza el arranque periódico del motor 2 si la bandera está activa
            }
        else if (millis() - Arranque_M2 >= Espera_Respuesta_M2 && Temperatura_Promedio > Referencia_Temperatura && Estado_M1 == LOW && Estado_M2 == HIGH) {
            Motor_1.arranque(); // Enciende el motor 1 si el motor 2 está encendido, el motor 1 está apagado, la temperatura sigue siendo alta, y ya pasó la espera de respuesta del motor 2
            }
        else if (Realizar_Arranque_Periodico == false && Estado_M1 == LOW) {
            Motor_1.arranque(); // Realiza el arranque del motor 1 si no hay arranque periódico y el motor 1 está apagado
            }
        else if (Realizar_Arranque_Periodico == false && Estado_M2 == LOW && millis() - Arranque_M1>= Espera_Respuesta_M1) {
            Motor_2.arranque(); // Enciende el motor 2 después del retardo si el motor 1 ya está encendido
            }
        else if (Referencia_Temperatura + Ventana_Temperatura *2 < Temperatura_Promedio && millis() - Arranque_M1 >= Retardo_Motor2 && Estado_M2 == LOW){
            Motor_2.arranque(); // Enciende el motor 2 si la temperatura supera aún más la referencia y el motor 1 lleva encendido suficiente tiempo 
            }
    }
    if (Referencia_Temperatura >= Temperatura_Promedio && (Realizar_Arranque_Periodico == false || (Realizar_Arranque_Periodico == true && Estado_M1 == HIGH))){
        
        Motor_2.parada(); // Apaga el motor 2 si la temperatura está por debajo de la referencia, y no hay arranque periódico activo o sí lo hay pero el motor 1 está encendido
    }
    if (Referencia_Temperatura - Ventana_Temperatura >= Temperatura_Promedio){

        Motor_1.parada(); // Apaga el motor 1 si la temperatura está por debajo de la referencia menos la ventana
        if (Realizar_Arranque_Periodico == true){
        
            Motor_2.parada(); // Si el motor 2 estaba encendido por arranque periódico, también se apaga y se actualiza el tiempo del último arranque del motor 2
        }
    }
} else if (Aviso_Ausencia == true){
    if (digitalRead(Actuador_Ventilador_M1) == HIGH) {
        digitalWrite(Actuador_Ventilador_M1, LOW);
    }
    if (M1_Encendido == true || M2_Encendido == true){
    digitalWrite(Actuador_Ventilador_M1, LOW); // Apagamos el ventilador del motor 1
    Motor_1.parada(); // Apagamos el motor 1
    Motor_2.parada(); // Apagamos el motor 2
    }
}

/*//////Lectura inicial
            - Se leen estados de M1 y M2.
            - Se calcula la temperatura promedio.
            - El ventilador de M1 se enciende siempre.
- //////Encendido de motores (cuando temperatura > referencia + ventana)
            - Si hay arranque periódico activo → se enciende M2 inmediatamente.
            - Si M1 está apagado y M2 encendido, y pasó la espera → se enciende M1.
            - Si no hay arranque periódico y M1 está apagado → se enciende M1.
            - Si no hay arranque periódico y M2 está apagado, y pasó la espera desde M1 → se enciende M2.
            - Si la temperatura supera aún más (referencia + 2× ventana) y M1 lleva encendido suficiente tiempo → se enciende M2.
-////// Apagado de M2 (cuando temperatura ≤ referencia)
            - Se apaga M2 si:
            - No hay arranque periódico activo.
            - O sí lo hay, pero M1 está encendido.
            - Al apagarse, se actualiza Ultimo_Arranque_M2 y se reinicia la bandera de arranque periódico.
//////- Apagado de M1 y M2 (cuando temperatura ≤ referencia – ventana)
            - Se apaga M1 si estaba encendido.
            - Si M2 estaba encendido por arranque periódico, también se apaga y se actualiza Ultimo_Arranque_M2.*/
}

void Alerta_Puerta(void) {
    // 1. Leer el pin (según tu lógica: LOW = ABIERTA, HIGH = CERRADA)
    int Estado_Puerta = digitalRead(Final_Puerta_Pin);

    // --- CASO: PUERTA ABIERTA (LOW) ---
    if (Estado_Puerta == LOW) {

        if (Puerta_Abierta == false) {// Si la puerta acaba de abrirse, iniciamos el conteo
            Puerta_Abierta = true;
            High_Puerta = millis();
            Serial.println("Puerta detectada: ABIERTA. Iniciando conteo...");
        } 
        
        // Si ya pasó el tiempo de retardo
        if (millis() - High_Puerta >= Retardo_Alerta_Puerta) {
            Puerta.parpadear(); // Activa la alerta física
            Alerta_Puerta_Abierta = true; // Marcamos la alerta como activa
        }
    }
    
    // --- CASO: PUERTA CERRADA (HIGH) ---
    else {
        if (Puerta_Abierta == true) {
            Serial.println("Puerta detectada: CERRADA. Reseteando alertas.");
        }
        // Reseteamos todas las banderas
        Puerta_Abierta = false;
        Alerta_Puerta_Abierta = false;
        digitalWrite(Actuador_Alerta_Puerta, LOW); // Apagamos la alerta física
    }
}

void Luz_Interior(void) {

    int reading = digitalRead(Pulsador_Luz_Pin);

    if (reading != lastReading) {
    lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading == LOW && lastButtonState == HIGH) {

        Estado_Luz = !Estado_Luz;

        digitalWrite(Actuador_Luz_Camara, Estado_Luz);

        client.publish(TOPIC_ACT_LUZ_CAMARA, Estado_Luz ? "1" : "0");
        client.publish(TOPIC_ACT_WEB_LUZ_CAMARA, Estado_Luz ? "1" : "0");
    }

    lastButtonState = reading;
    }

    lastReading = reading;

}

void Funcionamiento_Periodico_M2 (void) {

    if (M2_Encendido == true && Ultimo_Estado_M2 == false) {
        Ultimo_Estado_M2 = true; // Actualiza el último estado del motor 2 a encendido
        Ultimo_Arranque_M2 = millis(); // Actualiza el tiempo del último arranque si el motor 2 está encendido
    } else if (M2_Encendido == false && millis() - Ultimo_Arranque_M2 >= Arranque_Periodico_M2) {
        Realizar_Arranque_Periodico = true; // Indica que se debe realizar el arranque periódico del motor 2
    } 
    /*En resumen
    - Si M2 está encendido y el último estado era apagado → se actualiza el último estado a encendido y se actualiza el tiempo del último arranque.
    - Si M2 está apagado y ya pasó el tiempo del arranque periódico desde el último arranque → se activa la bandera para realizar el arranque periódico del motor 2.*/
}

void Control_Anomalias (void) {

    if ((M1_Encendido == true && millis() - Arranque_M1 >= Espera_Anomalia) || (M2_Encendido == true && millis() - Arranque_M2 >= Espera_Anomalia)) {
        Anomalia = true; // Si alguno de los motores lleva encendido más de 30 minutos, se detecta una anomalía
        anomalia.parpadear(); // Activa la alerta física de anomalía
    }
     /*En resumen
        - Esta función monitorea el tiempo que llevan encendidos M1 y M2.
        - Si alguno de los motores lleva encendido más de 30 minutos, se activa la bandera de anomalía.
        - Esa bandera luego puede ser utilizada para enviar alertas o tomar acciones correctivas, como apagar los motores o activar una alarma.*/
}

void Control_Fases (void) { // Función para controlar la presencia de las fases
    Fase_1.presencia(); // Verifica la presencia de la fase 1
    Fase_2.presencia(); // Verifica la presencia de la fase 2
    Fase_3.presencia(); // Verifica la presencia de la fase 3

    if (Ausencia_Fase1 == true || Ausencia_Fase2 == true || Ausencia_Fase3 == true) {
        Aviso_Ausencia = true; // Si alguna de las fases está ausente, se activa el aviso de ausencia de fase

    } else if (Ausencia_Fase1 == false && Ausencia_Fase2 == false && Ausencia_Fase3 == false) {
        Aviso_Ausencia = false; // Si todas las fases están presentes, se desactiva el aviso de ausencia de fase
    }
}

void Control_Avisos_Temperatura(void) {
    // --- 1. ALERTAS POR ALTA ---
    // Alarma Alta
    if (Temperatura_Promedio >= Alarma_Temperatura_Alta) {
        Estado_Alarma_Temperatura_Alta = true;
    } else if (Temperatura_Promedio < (Alarma_Temperatura_Alta - 0.5)) { // Pequeña histéresis de 0.5
        Estado_Alarma_Temperatura_Alta = false;
    }

    // Aviso Alta
    if (Temperatura_Promedio >= Aviso_Temperatura_Alta) {
        Estado_Aviso_Temperatura_Alta = true;
    } else if (Temperatura_Promedio < (Aviso_Temperatura_Alta - 0.5)) {
        Estado_Aviso_Temperatura_Alta = false;
    }

    // --- 2. ALERTAS POR BAJA ---
    // Alarma Baja (Si baja de -5, por ejemplo)
    if (Temperatura_Promedio <= Alarma_Temperatura_Baja) {
        Estado_Alarma_Temperatura_Baja = true;
    } else if (Temperatura_Promedio > (Alarma_Temperatura_Baja + 0.5)) {
        Estado_Alarma_Temperatura_Baja = false;
    }

    // Aviso Baja
    if (Temperatura_Promedio <= Aviso_Temperatura_Baja) {
        Estado_Aviso_Temperatura_Baja = true;
    } else if (Temperatura_Promedio > (Aviso_Temperatura_Baja + 0.5)) {
        Estado_Aviso_Temperatura_Baja = false;
    }
  // --- LÓGICA DE AVISOS ---
    if ((Estado_Aviso_Temperatura_Alta && !Estado_Aviso_Alto_Anterior) || 
        (Estado_Aviso_Temperatura_Baja && !Estado_Aviso_Bajo_Anterior)) {
        
        Estado_Aviso = true;
        planta.dispararNuevoAviso(); 

        // CRUCIAL: Actualizamos los estados anteriores AQUÍ MISMO
        // para que en la siguiente vuelta del loop no vuelva a entrar a este IF
        Estado_Aviso_Alto_Anterior = Estado_Aviso_Temperatura_Alta;
        Estado_Aviso_Bajo_Anterior = Estado_Aviso_Temperatura_Baja;
    }

    if (!Estado_Aviso_Temperatura_Alta && !Estado_Aviso_Temperatura_Baja) {
        Estado_Aviso = false;
        // También actualizamos al volver a la normalidad
        Estado_Aviso_Alto_Anterior = false;
        Estado_Aviso_Bajo_Anterior = false;
    }

    // --- LÓGICA DE ALARMAS ---
    if ((Estado_Alarma_Temperatura_Alta && !Estado_Alarma_Alta_Anterior) || 
        (Estado_Alarma_Temperatura_Baja && !Estado_Alarma_Baja_Anterior)) {
        
        Estado_Alarma = true;
        planta.dispararNuevaAlerta();

        Estado_Alarma_Alta_Anterior = Estado_Alarma_Temperatura_Alta;
        Estado_Alarma_Baja_Anterior = Estado_Alarma_Temperatura_Baja;
    }

    if (!Estado_Alarma_Temperatura_Alta && !Estado_Alarma_Temperatura_Baja) {
        Estado_Alarma = false;
        Estado_Alarma_Alta_Anterior = false;
        Estado_Alarma_Baja_Anterior = false;
    }
}
