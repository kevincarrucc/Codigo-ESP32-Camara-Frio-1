#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Claves.h"         
#include "Variables.h"  
#include "Clases.h"         
#include "Logica.h"
#include "Entradas.h"
#include "Escalado.h"
//firebase
#include <Firebase_ESP_Client.h> // Asegúrate de tener esta librería instalada
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

FirebaseData fbdo_stream; // Objeto para el flujo de datos en tiempo real
FirebaseAuth auth;
FirebaseConfig config_fb;

// --- 1. DEFINICIÓN DE VARIABLES GLOBALES ---
float Tension_L1=0, Tension_L2=0, Tension_L3=0;
float Corriente_L1=0, Corriente_L2=0, Corriente_L3=0;
float Temperatura_C1=0, Temperatura_C2=0, Temperatura_Exterior=0;
bool Ausencia_Fase1=false, Ausencia_Fase2=false, Ausencia_Fase3=false;
bool Estado_Luz = false;
int lastButtonState = HIGH;
unsigned long ultimoEnvio = 0;
unsigned long Arranque_M1 = 0, Arranque_M2 = 0;

int Referencia_Temperatura = 3; 
int Ventana_Temperatura = 2;      
int Temperatura_Promedio = 0;
bool Realizar_Arranque_Periodico = false;
bool Ultimo_Estado_M2 = false;
bool Ultimo_Estado_Luz = false;
bool Anomalia = false, Aviso_Ausencia = false, Alerta_Puerta_Abierta = false;
bool M1_Encendido = false, M2_Encendido = false, Puerta_Abierta = false;
bool P_Pulsador_Luz = false; 

// --- 2. OBJETOS DE RED ---
WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- 3. INSTANCIAS DE CLASES ---
Motor Motor_1(Actuador_M1);
Motor Motor_2(Actuador_M2);
Parpadeo Puerta(Actuador_Alerta_Puerta, 1000); // 1000ms de intermitencia
Fase Fase_1(Actuador_F1, Ausencia_Fase1, Tension_L1);
Fase Fase_2(Actuador_F2, Ausencia_Fase2, Tension_L2);
Fase Fase_3(Actuador_F3, Ausencia_Fase3, Tension_L3);

// --- 4. SETUP ---
void setup() {
  Serial.begin(115200);
  delay(1000);
// --- PROTOTIPOS DE FIREBASE (Agrégalos aquí) ---
void streamCallback(FirebaseStream data);
void streamTimeoutCallback(bool timeout);
  // Configuración de pines extra y comunicaciones
  inicializarHardware();      // Configura INPUT_PULLUP y otros pines en Entradas.cpp
  inicializarComunicaciones(); // Conecta WiFi y MQTT en Red.cpp


  // En tu setup() o inicializarComunicaciones()
if (!Firebase.RTDB.beginStream(&fbdo_stream, "/camara/luz/P_Pulsador_Luz")) {
    Serial.printf("Error en Stream: %s\n", fbdo_stream.errorReason().c_str());
}

// Definimos la función que se ejecuta cuando llega un cambio
Firebase.RTDB.setStreamCallback(&fbdo_stream, streamCallback, streamTimeoutCallback);


  Serial.println(">>> HELADERA INDUSTRIAL ONLINE <<<");
}

// --- 5. LOOP PRINCIPAL ---
void loop() {
  Escalar(); // Lee y escala sensores (en Escalado.cpp)
  // A. Gestión de Red y Mensajes MQTT (en Entradas.cpp)
  //procesarLogicaControl(); 
  Control_Fases(); // Verifica fases y actualiza Ausencia_FaseX
  // B. Tu Lógica Real (de Logica.cpp)
  Control_Temperatura();           // Control de frío y motores
  Alerta_Puerta();                 // Manejo del sensor de puerta
  Luz_Interior();                  // Pulsador físico y web
  Funcionamiento_Periodico_M2();   // Rotación de motores
  Control_Anomalias();             // Seguridad por tiempo de encendido
  
  static unsigned long ultimoMonitor = 0;
  if (millis() - ultimoMonitor >= 2000) { // Cada 2 segundos
    ultimoMonitor = millis();
    Serial.println("---------- ESTADO HELADERA ----------");
    Serial.print("Temp 1: "); Serial.print(Temperatura_C1); Serial.println(" °C");
    Serial.print("Temp 2: "); Serial.print(Temperatura_C2); Serial.println(" °C");
    Serial.print("PROMEDIO: "); Serial.print(Temperatura_Promedio); Serial.println(" °C");
    Serial.print("REFERENCIA: "); Serial.print(Referencia_Temperatura); Serial.println(" °C");
    Serial.print("Ventana (+/-): "); Serial.print(Ventana_Temperatura); Serial.println(" °C");
    Serial.print("MOTOR 1: "); Serial.println(digitalRead(Actuador_M1) ? "ENCENDIDO" : "APAGADO");
    Serial.print("MOTOR 2: "); Serial.println(digitalRead(Actuador_M2) ? "ENCENDIDO" : "APAGADO");
    Serial.println("-------------------------------------");
    Serial.println("--- ESTADOS DE ILUMINACIÓN ---");
        
        // Leemos el pin 2 directamente para ver qué llega
        int estadoPinFisico = digitalRead(Pulsador_Luz_Pin);
        Serial.print("Lectura Real Pin 2 (Físico): ");
        Serial.println(estadoPinFisico == HIGH ? "1 (ALTO)" : "0 (BAJO)");

        // Mostramos tus variables globales
        Serial.print("Pulsador Web (P_Pulsador_Luz): ");
        Serial.println(P_Pulsador_Luz ? "TRUE" : "FALSE");

        Serial.print("Último Estado Guardado: ");
        Serial.println(Ultimo_Estado_Luz ? "ENCENDIDO" : "APAGADO");

        // Leemos la salida para ver si el ESP32 envió la orden al relé
        Serial.print("Estado Salida Lámpara (Pin 23): ");
        Serial.println(digitalRead(Actuador_Luz_Camara) == HIGH ? "ON" : "OFF");
        
        Serial.println("------------------------------");  
  }
}