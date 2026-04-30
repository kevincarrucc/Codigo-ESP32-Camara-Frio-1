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
#include "Teclado.h"
#include "Display.h"
#include "Planta.h"
//firebase
#include <Firebase_ESP_Client.h> // Asegúrate de tener esta librería instalada
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

FirebaseData fbdo_stream; // Objeto para el flujo de datos en tiempo real
FirebaseAuth auth;
FirebaseConfig config_fb;

Teclado teclado(0x26);
Display display(0x27);
Planta planta(teclado, display);

bool Estado_Luz = false;
bool Estado_Web_Luz = false;
int ultimoEstadoTeclaFisica = -1;
int ultimoEstadoWeb = -1; // Para detectar cambios en el callback de la web

// --- 1. DEFINICIÓN DE VARIABLES GLOBALES ---
float Tension_L1=0, Tension_L2=0, Tension_L3=0;
float Corriente_L1=0, Corriente_L2=0, Corriente_L3=0;
float Temperatura_C1=0, Temperatura_C2=0, Temperatura_Exterior=0;
bool Ausencia_Fase1=false, Ausencia_Fase2=false, Ausencia_Fase3=false;
int lastButtonState = HIGH;
unsigned long ultimoEnvio = 0;
unsigned long Arranque_M1 = 0, Arranque_M2 = 0;

// Definiciones adicionales
int16_t Corriente_L1_Lectura = 0;
int16_t Corriente_L2_Lectura = 0;
int16_t Corriente_L3_Lectura = 0;

float Referencia_Temperatura = 3.0; 
float Ventana_Temperatura = 2.0;      
float Temperatura_Promedio = 0.0;
bool Realizar_Arranque_Periodico = false;
bool Ultimo_Estado_M2 = false;
bool Ultimo_Estado_Luz = false;
bool Anomalia = false, Aviso_Ausencia = false, Alerta_Puerta_Abierta = false;
bool M1_Encendido = false, M2_Encendido = false, Puerta_Abierta = false;
bool P_Pulsador_Luz = false; 



// Valores de umbrales (puedes ajustar los números)
float Alarma_Temperatura_Alta = 10.0;
float Alarma_Temperatura_Baja = -2.0;
float Aviso_Temperatura_Alta = 8.0;
float Aviso_Temperatura_Baja = 0.0;

// Estados de las alarmas (Banderas)
bool Estado_Alarma_Temperatura_Alta = false;
bool Estado_Alarma_Temperatura_Baja = false;
bool Estado_Aviso_Temperatura_Alta = false;
bool Estado_Aviso_Temperatura_Baja = false;

// Definiciones de variables globales faltantes
unsigned long intervaloEnvio = 10000;
String claveSistema = "1234";
String bufferClave = "";

// --- 2. OBJETOS DE RED ---
WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- 3. INSTANCIAS DE CLASES ---
Motor Motor_1(Actuador_M1);
Motor Motor_2(Actuador_M2);
Parpadeo Puerta(Actuador_Alerta_Puerta, 1000); // 1000ms de intermitencia
Parpadeo Coneccion_Wifi(Actuatuador_coneccion_wifi, 150);
Parpadeo anomalia(Actuador_Alerta_Anomalia, 100);
Fase Fase_1(Actuador_F1, Ausencia_Fase1, Tension_L1);
Fase Fase_2(Actuador_F2, Ausencia_Fase2, Tension_L2);
Fase Fase_3(Actuador_F3, Ausencia_Fase3, Tension_L3);

// --- 4. SETUP ---
void setup() {
  Serial.begin(115200);
  delay(1000);

    setup_I2C(); // Configura el bus I2C (en Entradas.cpp)
    teclado.begin();
    display.begin();
    planta.begin();

void streamTimeoutCallback(bool timeout);
  // Configuración de pines extra y comunicaciones
  inicializarHardware();      // Configura INPUT_PULLUP y otros pines en Entradas.cpp
  inicializarComunicaciones(); // Conecta WiFi y MQTT en Red.cpp

  Serial.println(">>> HELADERA INDUSTRIAL ONLINE <<<");

}

// --- 5. LOOP PRINCIPAL ---
void loop() {
  loop_I2C(); // Mantiene el bus I2C activo (en Entradas.cpp)
  Escalar(); // Lee y escala sensores (en Escalado.cpp)
  // A. Gestión de Red y Mensajes MQTT (en Entradas.cpp)
  //procesarLogicaControl(); 
  Control_Fases(); // Verifica fases y actualiza Ausencia_FaseX
  // B. Tu Lógica Real (de Logica.cpp)
  Control_Avisos_Temperatura(); // Control de avisos y alarmas de temperatura
  Control_Temperatura();           // Control de frío y motores
  Alerta_Puerta();                 // Manejo del sensor de puerta
  Luz_Interior();                  // Pulsador físico y web
  Funcionamiento_Periodico_M2();   // Rotación de motores
  Control_Anomalias();             // Seguridad por tiempo de encendido
  procesarLogicaControl();           // Procesa la lógica de control de motores y alertas (en Logica.cpp)
  planta.update();                 // Actualiza la pantalla y maneja el teclado (en Planta.cpp)
  
  static unsigned long ultimoMonitor = 0;
  if (millis() - ultimoMonitor >= 2000) { // Cada 2 segundos
    ultimoMonitor = millis();
    /*
    Serial.println("---------- ESTADO HELADERA ----------");
    Serial.print("Temp 1: "); Serial.print(Temperatura_C1); Serial.println(" °C");
    Serial.print("Temp 2: "); Serial.print(Temperatura_C2); Serial.println(" °C");
    Serial.print("PROMEDIO: "); Serial.print(Temperatura_Promedio); Serial.println(" °C");
    Serial.print("REFERENCIA: "); Serial.print(Referencia_Temperatura); Serial.println(" °C");
    Serial.print("Ventana (+/-): "); Serial.print(Ventana_Temperatura); Serial.println(" °C");
    Serial.print("MOTOR 1: "); Serial.println(digitalRead(Actuador_M1) ? "ENCENDIDO" : "APAGADO");
    Serial.print("MOTOR 2: "); Serial.println(digitalRead(Actuador_M2) ? "ENCENDIDO" : "APAGADO");
    Serial.println("---------------corriente----------------------");
    
   Serial.println("---------------corriente----------------------");
    Serial.print("Corriente L1: "); Serial.print(Corriente_L1_Lectura); Serial.println(" A");
    Serial.print("Corriente L2: "); Serial.print(Corriente_L2); Serial.println(" A");
    Serial.print("Corriente L3: "); Serial.print(Corriente_L3); Serial.println(" A");
    static unsigned long ultimoMonitor = 0;
    */
    //Serial.println("---------- ESTADOS DE ILUMINACIÓN ----------");
    char tecla = teclado.leer();

    if (tecla != '\0') {
        Serial.println(tecla);
    }
    // 1. Lectura física del pin (GPIO 2)
    int estadoPinFisico = digitalRead(Pulsador_Luz_Pin);
    /*
    Serial.print("Pulsador Físico (Pin 2): ");
    Serial.println(estadoPinFisico == HIGH ? "1 (ALTO)" : "0 (BAJO)");

    // 2. Estado de la variable unificada (La que cambia por MQTT/Firebase)
    Serial.print("Estado Variable Web/Red (Estado_Luz): ");
    Serial.println(Estado_Luz ? "TRUE (ENCENDER)" : "FALSE (APAGAR)");

    // 3. Memoria del ciclo anterior (Para detectar flancos)
    Serial.print("Memoria Estado Anterior (Ultimo_Estado_Luz): ");
    Serial.println(Ultimo_Estado_Luz ? "ENCENDIDO" : "APAGADO");

    // 4. Verificación real del Relé (GPIO 23)
    int estadoRele = digitalRead(Actuador_Luz_Camara);
    Serial.print("Estado REAL Relé/Luz (Pin 23): ");
    Serial.println(estadoRele == HIGH ? "FÍSICAMENTE ON" : "FÍSICAMENTE OFF");
    
    Serial.println("--------------------------------------------");  

    Serial.println("\n===== ⚙️ PARÁMETROS DE CONTROL =====");
        
        // 1. Parámetros de Operación Normal
        Serial.printf("📍 Set Point: %.1f °C\n", Referencia_Temperatura);
        Serial.printf("📏 Histéresis: %.1f °C\n", Ventana_Temperatura);
        
        Serial.println("----- 🚨 UMBRALES DE ALARMA -----");
        
        // 2. Umbrales de Aviso (Preventivos)
        Serial.printf("⚠️ Aviso Alta:  > %.1f °C\n", Aviso_Temperatura_Alta);
        Serial.printf("⚠️ Aviso Baja:  < %.1f °C\n", Aviso_Temperatura_Baja);
        
        // 3. Umbrales de Alarma (Críticos)
        Serial.printf("🛑 Alarma Alta: > %.1f °C\n", Alarma_Temperatura_Alta);
        Serial.printf("🛑 Alarma Baja: < %.1f °C\n", Alarma_Temperatura_Baja);
        
        Serial.println("==================================\n");
        Serial.printf("anomalia detectada: %s\n", Anomalia ? "SÍ" : "NO");
        */
  }
}