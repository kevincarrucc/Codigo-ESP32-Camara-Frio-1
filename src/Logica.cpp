#include "EntradaSalida.h"
#include "arduino.h"

const unsigned long Retardo_Motor2 = 5000; // Retardo de 5 segundos para encender el motor 2 después de encender el motor 1

const unsigned long Espera_Respuesta_M1 = 300000; // Espera de 5 minutos para la respuesta del motor 1

const unsigned long Arranque_Periodico_M2 = 86400000; // Arranque periódico del motor 2 cada 24 horas

const unsigned long Espera_Respuesta_M2 = 300000; // Espera de 5 minutos para la respuesta del motor 2


const unsigned long Retardo_Alerta_Puerta = 60000; // Retardo de 1 minuto para la alerta de puerta abierta
unsigned long High_Puerta = 0; // Variable para almacenar el tiempo en que se activó la alerta de puerta abierta
const unsigned long Intermitencia_Alerta_Puerta = 1000; // Intermitencia de la alerta de puerta abierta (1 segundo)
unsigned long Ultimo_Alerta_Puerta = 0; // Variable para almacenar el tiempo del último parpadeo de la alerta de puerta abierta


/*Funciones*/
void Control_Temperatura(void);
void Alerta_Puerta (void);
void Luz_Interior (void);
void Funcionamiento_Periodico_M2 (void);
void SR_Arranque_M1 (void);
void SR_Arranque_M2 (void);
void SR_Parada_M1 (void);
void SR_Parada_M2 (void);




void Control_Temperatura(void) {
    /*variables locales*/
    int Estado_M1 = digitalRead(Actuador_M1);
    int Estado_M2 = digitalRead(Actuador_M2);
    

    /*Promedio de temperaturas*/
    Temperatura_Promedio = (Temperatura_C1 + Temperatura_C2) / 2;

    /*Accion sobre el motor 1-2 y el ventilador*/
   digitalWrite(Actuador_Ventilador_M1, HIGH);// El ventilador del motor 1 siempre está encendido

    if (Referencia_Temperatura + Ventana_Temperatura <= Temperatura_Promedio && (Estado_M1 == LOW || Estado_M2 == LOW))
     {
    
            
        if (Realizar_Arranque_Periodico == true) {
            SR_Arranque_M2(); // Realiza el arranque del motor 2 de forma obligatoria por arranque periódico
            }
        else if (millis() - Arranque_M2 >= Espera_Respuesta_M2 && Temperatura_Promedio > Referencia_Temperatura && Estado_M1 == LOW && Estado_M2 == HIGH) {
            SR_Arranque_M1(); // Realiza el arranque del motor 1 si el motor 2 está encendido y ha pasado la espera de respuesta
            }
        else if (Realizar_Arranque_Periodico == false && Estado_M1 == LOW) {
            SR_Arranque_M1(); // Realiza el arranque del motor 1 si no hay arranque periódico y el motor 1 está apagado
            }
         else if (Realizar_Arranque_Periodico == false && Estado_M2 == LOW && millis() - Arranque_M1>= Espera_Respuesta_M1) {
            SR_Arranque_M2(); // Enciende el motor 2 después del retardo si el motor 1 ya está encendido
            }
        if (Referencia_Temperatura + Ventana_Temperatura *2 < Temperatura_Promedio && millis() - High_M1 >= Retardo_Motor2 && Estado_M2 == LOW){
            SR_Arranque_M2(); // Enciende el motor 2 si la temperatura supera aún más la referencia y el motor 1 lleva encendido suficiente tiempo 
            }
    }
    else if (Referencia_Temperatura >= Temperatura_Promedio && (Realizar_Arranque_Periodico == false || (Realizar_Arranque_Periodico == true && Estado_M1 == HIGH))){
        
        SR_Parada_M2(); // Apaga el motor 2 si la temperatura está por debajo o igual a la referencia y no hay arranque periódico activo, o sí lo hay pero el motor 1 está encendido
    }
    else if (Referencia_Temperatura - Ventana_Temperatura >= Temperatura_Promedio){

        SR_Parada_M1(); // Apaga el motor 1 si la temperatura está por debajo de la referencia menos la ventana
   
        if (Realizar_Arranque_Periodico == true){
        
            SR_Parada_M2(); // Si el motor 2 estaba encendido por arranque periódico, también se apaga y se actualiza el tiempo del último arranque del motor 2
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

class Motor {
    private: //atributos
        int Pin_Motor; //Pin del motor
        unsigned long High_M1 = 0; // Variable para almacenar el tiempo en que se activó el motor 1
        unsigned long Arranque_M1 = 0; // Variable para almacenar el tiempo en que se activó el motor 1
        unsigned long Ultimo_Arranque_M2 = 0; // Variable para almacenar el tiempo del último arranque periódico del motor 2
        unsigned long Arranque_M2 = 0; // Variable para almacenar el tiempo en que se activó el motor 2
    public: //métodos
        void Arranque_M1 (void);
        void Arranque_M2 (void);
        void Parada_M1 (void);
        void Parada_M2 (void);

} 

void Alerta_Puerta (void) {

    /*variable local*/
    bool Alerta_Puerta_Abierta = false; // Alerta de puerta abierta, true si la puerta esta abierta, false si esta cerrada

    if (digitalRead(Final_Puerta_Pin) == HIGH)
    {
        if (Alerta_Puerta_Abierta == false)
        {
            Alerta_Puerta_Abierta = true;
            High_Puerta = millis();
        }
        else if (Alerta_Puerta_Abierta == true && millis() - High_Puerta >= Retardo_Alerta_Puerta)
        {
            // Aquí se podría agregar código para enviar una alerta, como publicar en MQTT o activar una alarma
            // Por ejemplo: client.publish("Alerta/Puerta", "La puerta ha estado abierta por más de 1 minuto");
            if (Actuador_Alerta_Puerta == LOW && millis() - Ultimo_Alerta_Puerta >= Intermitencia_Alerta_Puerta) // Si la alerta de puerta abierta está apagada, se enciende
            {
                digitalWrite(Actuador_Alerta_Puerta, HIGH); // Enciende la alerta de puerta abierta y verifica la intermitencia
                Ultimo_Alerta_Puerta = millis();
            }
            else if (Actuador_Alerta_Puerta == HIGH && millis() - Ultimo_Alerta_Puerta >= Intermitencia_Alerta_Puerta) // Si la alerta de puerta abierta está encendida y ha pasado el tiempo de intermitencia, se apaga
            {
                digitalWrite(Actuador_Alerta_Puerta, LOW);
                Ultimo_Alerta_Puerta = millis();
            }
        }
    }
     /*Si la puerta esta abierta se enciende el ventilador del motor 1 y se llama a la funcion de control de temperatura*/
    else if (digitalRead(Final_Puerta_Pin) == LOW && Alerta_Puerta_Abierta == true)
    {
        Alerta_Puerta_Abierta = false;
    }
}

void Luz_Interior (void){

    if (Pulsador_Luz_Pin == HIGH)
    {
        digitalWrite(Actuador_Luz_Camara, HIGH);
    }
    else if (Pulsador_Luz_Pin == LOW)
    {
        digitalWrite(Actuador_Luz_Camara, LOW);
    }
    /*Si el pulsador esta HIGH se enciende las luz y si esta LOW se apaga*/

}

void Funcionamiento_Periodico_M2 (void) {
    int estado_M2 = digitalRead(Actuador_M2);
    if (estado_M2 == HIGH) {
        Ultimo_Arranque_M2 = millis(); // Actualiza el tiempo del último arranque si el motor 2 está encendido
    } else if (estado_M2 == LOW && millis() - Ultimo_Arranque_M2 >= Arranque_Periodico_M2) {
        Realizar_Arranque_Periodico = true; // Indica que se debe realizar el arranque periódico del motor 2
    } 
    /*En resumen
        - Esta función vigila el tiempo de inactividad de M2.
        - Si M2 lleva apagado más de 24 horas, activa la bandera de arranque periódico.
        - Esa bandera luego es interpretada en Control_Temperatura() para encender M2 de forma obligatoria.
        - Así garantizás que M2 se encienda al menos una vez al día, evitando bloqueos mecánicos o falta de circulación de refrigerante.*/
        
}

void SR_Arranque_M1 (void) {
    int Estado_M1 = digitalRead(Actuador_M1);
    if (Estado_M1 == LOW) {
        digitalWrite(Actuador_M1, HIGH); // Enciende el motor 1 después del retardo si el motor 1 ya está encendido
        Arranque_M1 = millis(); // Actualiza el tiempo en que se activó el motor 1
        High_M1 = millis(); // Actualiza el tiempo en que se activó el motor 1
    } 
}

void SR_Arranque_M2 (void) {
    int Estado_M2 = digitalRead(Actuador_M2);
    if (Estado_M2 == LOW) {
        digitalWrite(Actuador_M2, HIGH); // Enciende el motor 2 después del retardo si el motor 2 ya está encendido
        Arranque_M2 = millis(); // Actualiza el tiempo en que se activó el motor 2
    } 
}

void SR_Parada_M1 (void) {
      int Estado_M1 = digitalRead(Actuador_M1);
    if (Estado_M1 == HIGH) {
        digitalWrite(Actuador_M1, LOW); // Apaga el motor 1
    } 
}

void SR_Parada_M2 (void) {
      int Estado_M2 = digitalRead(Actuador_M2);
    if (Estado_M2 == HIGH) {
        digitalWrite(Actuador_M2, LOW); // Apaga el motor 2
        Ultimo_Arranque_M2 = millis(); // Actualiza el tiempo del último arranque del motor 2 al apagarlo
        Realizar_Arranque_Periodico = false; // Reinicia la variable de arranque periódico al apagar el motor 2
    } 
}
