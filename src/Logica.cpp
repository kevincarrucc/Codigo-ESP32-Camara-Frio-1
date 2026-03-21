#include "EntradaSalida.h"
#include "arduino.h"

const unsigned long Retardo_Motor2 = 5000; // Retardo de 5 segundos para encender el motor 2 después de encender el motor 1
unsigned long High_M1 = 0; // Variable para almacenar el tiempo en que se activó el motor 1
const unsigned long Retardo_Alerta_Puerta = 60000; // Retardo de 1 minuto para la alerta de puerta abierta
unsigned long High_Puerta = 0; // Variable para almacenar el tiempo en que se activó la alerta de puerta abierta
const unsigned long Intermitencia_Alerta_Puerta = 1000; // Intermitencia de la alerta de puerta abierta (1 segundo)
unsigned long Ultimo_Alerta_Puerta = 0; // Variable para almacenar el tiempo del último parpadeo de la alerta de puerta abierta
const unsigned long Arranque_Periodico_M2 = 86400000; // Arranque periódico del motor 2 cada 24 horas
unsigned long Ultimo_Arranque_M2 = 0; // Variable para almacenar el tiempo del último arranque periódico del motor 2
/*Funciones*/


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
        if (Estado_M1 == LOW)
            {
                High_M1 = millis();
                digitalWrite(Actuador_M1, HIGH);
            }  
        
        if (Referencia_Temperatura + Ventana_Temperatura *2 < Temperatura_Promedio && millis() - High_M1 >= Retardo_Motor2)
        {
            digitalWrite(Actuador_M2, HIGH); 
        }
    }
    else if (Referencia_Temperatura >= Temperatura_Promedio)
    {
        digitalWrite(Actuador_M2, LOW);
    }
    else if (Referencia_Temperatura - Ventana_Temperatura >= Temperatura_Promedio)
    {
     digitalWrite(Actuador_M1, LOW);  
    }
    /*En resumen
• 	Ventilador M1: siempre encendido (fuera de este bloque).
• 	Motor M1: se enciende si la temperatura supera referencia + ventana y estaba apagado; se apaga si baja por debajo de referencia − ventana.
• 	Motor M2: se enciende si la temperatura supera referencia + 2×ventana y ya pasó el retardo desde que se encendió M1; se apaga si la temperatura baja a la referencia o menos.
• 	La condición inicial con  asegura que solo se intente encender motores si al menos uno está apagado.*/
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
        
}