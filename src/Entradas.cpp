#include "Variables.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Claves.h"
//firebase
#include <Arduino.h>
#include <Firebase_ESP_Client.h> // <--- ESTA LÍNEA ES CRUCIAL
#include "Variables.h"
#include "Clases.h"


// Esto le avisa al compilador que las variables existen en el main
// y que NO las tiene que crear de nuevo acá.
extern bool Estado_Web_Luz;
extern bool Estado_Luz;
extern int lastButtonState;
extern unsigned long ultimoEnvio;
extern unsigned long Arranque_M1;
extern unsigned long Arranque_M2;
extern WiFiClientSecure espClient; 
extern PubSubClient client;

extern Parpadeo Coneccion_Wifi; // Instancia para el parpadeo de la conexión WiFi

// Instancia para el ADS1115
Adafruit_ADS1115 ads; 

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

// Alias para usar en el código
const char* TOPIC_ESTADO_PUERTA = ESTADO_PUERTA;

const char* TOPIC_TEMPERATURA_1 = TEMPERATURA_1;
const char* TOPIC_TEMPERATURA_2 = TEMPERATURA_2;
const char* TOPIC_TEMPERATURA_EXTERIOR = TEMPERATURA_EXTERIOR;

const char* TOPIC_TENSION_L1 = TENSION_L1;
const char* TOPIC_TENSION_L2 = TENSION_L2;
const char* TOPIC_TENSION_L3 = TENSION_L3;

const char* TOPIC_CORRIENTE_L1 = CORRIENTE_L1;
const char* TOPIC_CORRIENTE_L2 = CORRIENTE_L2;
const char* TOPIC_CORRIENTE_L3 = CORRIENTE_L3;

const char* TOPIC_ESTADO_MOTOR_1 = ESTADO_MOTOR_1;
const char* TOPIC_ESTADO_MOTOR_2 = ESTADO_MOTOR_2;
const char* TOPIC_ACT_LUZ_CAMARA = ACT_LUZ_CAMARA;
const char* TOPIC_ACT_WEB_LUZ_CAMARA = ACT_WEB_LUZ_CAMARA;

// --- Alarmas y Seguridad (Crucial para el monitoreo remoto) ---
const char* TOPIC_ESTADO_PRESENCIA_FASE = ESTADO_PRESENCIA_FASE;        // Aviso_Ausencia
const char* TOPIC_ALERTA_ANOMALIA = ESTADO_ANOMALIA;  // Anomalia (30 min)
const char* TOPIC_ALERTA_PUERTA = ESTADO_ALERTA_PUERTA;      // Alerta_Puerta_Abierta

//variables de control
const char* TOPIC_SET_POINT_TEMPERATURA = SET_POINT_TEMPERATURA;
const char* TOPIC_SET_POINT_HISTERESIS = SET_POINT_HISTERESIS;

// Variables para el control de temperatura
const char* TOPIC_ESTADO_ALARMA_TEMPERATURA_ALTA = ESTADO_ALARMA_TEMPERATURA_ALTA;
const char* TOPIC_ESTADO_ALARMA_TEMPERATURA_BAJA = ESTADO_ALARMA_TEMPERATURA_BAJA;
const char* TOPIC_ESTADO_AVISO_TEMPERATURA_ALTA = ESTADO_AVISO_TEMPERATURA_ALTA;
const char* TOPIC_ESTADO_AVISO_TEMPERATURA_BAJA = ESTADO_AVISO_TEMPERATURA_BAJA;

const char* TOPIC_ALARMA_TEMPERATURA_ALTA = ALARMA_TEMPERATURA_ALTA;
const char* TOPIC_ALARMA_TEMPERATURA_BAJA = ALARMA_TEMPERATURA_BAJA;      
const char* TOPIC_AVISO_TEMPERATURA_ALTA = AVISO_TEMPERATURA_ALTA;
const char* TOPIC_AVISO_TEMPERATURA_BAJA = AVISO_TEMPERATURA_BAJA;


//unsigned long ultimoEnvio = 0;
//
static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";
/*
WiFiClientSecure espClient;
PubSubClient client(espClient);
*/
unsigned long lastMsg = 0;
char msg[50];

// Prototipos (Avisos para el compilador)
void loop_I2C();
void setup_I2C();
void procesarLogicaControl();
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);


void callback(char* topic, unsigned char* payload, unsigned int length) {

    char buffer[length + 1];
    memcpy(buffer, payload, length);
    buffer[length] = '\0'; // Asegura que el buffer sea una cadena válida

   if (strcmp(topic, TOPIC_ACT_WEB_LUZ_CAMARA) == 0) {

    Estado_Luz = payload[0] - '0';

    digitalWrite(Actuador_Luz_Camara, Estado_Luz);
    client.publish(TOPIC_ACT_LUZ_CAMARA, Estado_Luz ? "1" : "0");
   }
    if (strcmp(topic, TOPIC_SET_POINT_TEMPERATURA) == 0) {

        Referencia_Temperatura = atof(buffer);
    }
    if (strcmp(topic, TOPIC_SET_POINT_HISTERESIS) == 0) {

        Ventana_Temperatura = atof(buffer);
    }
    if (strcmp(topic, TOPIC_ALARMA_TEMPERATURA_ALTA) == 0) {

        Alarma_Temperatura_Alta = atof(buffer);
    }
    if (strcmp(topic, TOPIC_ALARMA_TEMPERATURA_BAJA) == 0) {

        Alarma_Temperatura_Baja = atof(buffer);
    }
    if (strcmp(topic, TOPIC_AVISO_TEMPERATURA_ALTA) == 0) {

        Aviso_Temperatura_Alta = atof(buffer);
    }
    if (strcmp(topic, TOPIC_AVISO_TEMPERATURA_BAJA) == 0) {

        Aviso_Temperatura_Baja = atof(buffer);
    }
}
 
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Mientras NO esté conectado...
  while (WiFi.status() != WL_CONNECTED) {
    
    // Ejecutamos el método parpadear constantemente.
    // Como tu clase usa millis(), ella sola decidirá 
    // cuándo cambiar el estado del LED sin detener el código.
    Coneccion_Wifi.parpadear(); 
    
    // Opcional: un mini delay para no saturar el Serial, 
    // pero MUCHO menor que el intervalo de parpadeo.
    delay(1); 
    
    // Para ver el progreso en el monitor serie sin llenarlo de puntos:
    static unsigned long ultimoPunto = 0;
    if (millis() - ultimoPunto > 500) {
        Serial.print(".");
        ultimoPunto = millis();
    }
  }

  Serial.println("\nWiFi connected");
  // Al conectar, dejamos el LED fijo (HIGH o LOW según tu circuito)
  digitalWrite(Actuatuador_coneccion_wifi, HIGH); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(TOPIC_ACT_LUZ_CAMARA);/*
      client.subscribe(TOPIC_TEMPERATURA_1);
      client.subscribe(TOPIC_TEMPERATURA_2);
      client.subscribe(TOPIC_TEMPERATURA_EXTERIOR);
      client.subscribe(TOPIC_TENSION_L1);
      client.subscribe(TOPIC_CORRIENTE_L1);
      client.subscribe(TOPIC_ESTADO_PUERTA);
      client.subscribe(TOPIC_ESTADO_MOTOR_1);
      client.subscribe(TOPIC_ESTADO_MOTOR_2);
      client.subscribe(TOPIC_ESTADO_PRESENCIA_FASE);
      client.subscribe(TOPIC_ALERTA_ANOMALIA);
      client.subscribe(TOPIC_ALERTA_PUERTA);*/
      client.subscribe(TOPIC_ACT_WEB_LUZ_CAMARA);
      client.subscribe(TOPIC_SET_POINT_TEMPERATURA);
      client.subscribe(TOPIC_SET_POINT_HISTERESIS);
      client.subscribe(TOPIC_ALARMA_TEMPERATURA_ALTA);
      client.subscribe(TOPIC_ALARMA_TEMPERATURA_BAJA);
      client.subscribe(TOPIC_AVISO_TEMPERATURA_ALTA);
      client.subscribe(TOPIC_AVISO_TEMPERATURA_BAJA);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void procesarLogicaControl() {

  // 1. Mantener conexión MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // 3. Envío periódico de datos a la web (MQTT)
  if (millis() - ultimoEnvio >= intervaloEnvio) {
    ultimoEnvio = millis();
    char buffer[10];

    //client.publish(TOPIC_ACT_LUZ_CAMARA, Estado_Luz ? "1" : "0");
    //client.publish(TOPIC_ACT_WEB_LUZ_CAMARA, Estado_Luz ? "1" : "0"); estas dos estan en LOGICA > funcion luzinterior, ubicarlas aca hace que responda mas lento 

  // 4. Envío de Temperaturas Escaladas 
    // dtostrf(variable, ancho_total, decimales, buffer)
    
    dtostrf(Temperatura_C1, 4, 1, buffer); 
    client.publish(TOPIC_TEMPERATURA_1, buffer);

    dtostrf(Temperatura_C2, 4, 1, buffer); 
    client.publish(TOPIC_TEMPERATURA_2, buffer);

    dtostrf(Temperatura_Exterior, 4, 1, buffer); 
    client.publish(TOPIC_TEMPERATURA_EXTERIOR, buffer);

    // --- Envío de Tensiones Escaladas ---
    dtostrf(Tension_L1, 5, 1, buffer); 
    client.publish(TOPIC_TENSION_L1, buffer);
    
    dtostrf(Tension_L2, 5, 1, buffer); 
    client.publish(TOPIC_TENSION_L2, buffer);
    
    dtostrf(Tension_L3, 5, 1, buffer); 
    client.publish(TOPIC_TENSION_L3, buffer);

    // --- Envío de Corrientes Escaladas (Desde el ADS1015) ---
    // Usamos 2 decimales para mayor precisión en el consumo
    
    dtostrf(Corriente_L1, 5, 2, buffer); 
    client.publish(TOPIC_CORRIENTE_L1, buffer);

    dtostrf(Corriente_L2, 5, 2, buffer); 
    client.publish(TOPIC_CORRIENTE_L2, buffer);

    dtostrf(Corriente_L3, 5, 2, buffer); 
    client.publish(TOPIC_CORRIENTE_L3, buffer);

    // --- Estado de la Puerta  ---
    // Si Puerta_Abierta es true enviamos "1", si es false enviamos "0"
    client.publish(TOPIC_ESTADO_PUERTA, Puerta_Abierta ? "1" : "0");

    // --- ESTADOS DE MOTORES (0 o 1) ---
    client.publish(TOPIC_ESTADO_MOTOR_1, M1_Encendido ? "1" : "0");
    client.publish(TOPIC_ESTADO_MOTOR_2, M2_Encendido ? "1" : "0");

    // --- ALARMAS CRÍTICAS ---
    client.publish(TOPIC_ESTADO_PRESENCIA_FASE, Aviso_Ausencia ? "1" : "0");
    
    // Si un motor falló o quedó encendido de más (Anomalía)
    client.publish(TOPIC_ALERTA_ANOMALIA, Anomalia ? "1" : "0");

    // Si la puerta quedó abierta más tiempo del debido
    client.publish(TOPIC_ALERTA_PUERTA, Alerta_Puerta_Abierta ? "1" : "0");

    //Enviso de avisos y alarmas de temperatura
    client.publish(TOPIC_ESTADO_ALARMA_TEMPERATURA_ALTA, Estado_Alarma_Temperatura_Alta ? "1" : "0");
    client.publish(TOPIC_ESTADO_ALARMA_TEMPERATURA_BAJA, Estado_Alarma_Temperatura_Baja ? "1" : "0");
    client.publish(TOPIC_ESTADO_AVISO_TEMPERATURA_ALTA, Estado_Aviso_Temperatura_Alta ? "1" : "0");
    client.publish(TOPIC_ESTADO_AVISO_TEMPERATURA_BAJA, Estado_Aviso_Temperatura_Baja ? "1" : "0");
  }
}

void inicializarHardware() {
    // --- SALIDAS (Actuadores y LEDs) ---
    pinMode(Actuador_M1, OUTPUT);                 // D19
    pinMode(Actuador_M2, OUTPUT);                 // D18
    pinMode(Actuador_Ventilador_M1, OUTPUT);      // D5
    pinMode(Actuador_Luz_Camara, OUTPUT);         // D23
    pinMode(Actuador_Alerta_Puerta, OUTPUT);      // D4
    pinMode(Actuador_Alerta_Anomalia, OUTPUT);    // D15
    pinMode(Actuatuador_coneccion_wifi, OUTPUT);  // D14
    pinMode(Actuador_F1, OUTPUT);                 // D25
    pinMode(Actuador_F2, OUTPUT);                 // D26
    pinMode(Actuador_F3, OUTPUT);                 // D27

    // --- ENTRADAS DIGITALES (Con Pull-up para evitar ruido) ---
    pinMode(Final_Puerta_Pin, INPUT_PULLUP);      // D4
    pinMode(Pulsador_Luz_Pin, INPUT_PULLUP);      // D17

    // --- ENTRADAS ANALÓGICAS ---
    // En ESP32, pinMode(ANALOG) no es estrictamente necesario para analogRead,
    // pero dejarlo ayuda a documentar el código.
    pinMode(Temperatura_C1_Pin, INPUT);           // D33
    pinMode(Temperatura_C2_Pin, INPUT);           // D32
    pinMode(Temperatura_Ext_Pin, INPUT);          // D35
    pinMode(Tension_L1_Pin, INPUT);               // D36
    pinMode(Tension_L2_Pin, INPUT);               // D39
    pinMode(Tension_L3_Pin, INPUT);               // D34

    // Estado inicial de seguridad (Todo apagado al arrancar)
    digitalWrite(Actuador_M1, LOW);
    digitalWrite(Actuador_M2, LOW);
}

void setup_I2C() {
  // Inicializar I2C en los pines de tu diagrama
  Wire.begin(21, 22);

  // Iniciar ADS1015, si falla no continúa
  if (!ads.begin()) {
    while (1); 
  }

  // Configurar Ganancia para rango +/- 4.096V
  ads.setGain(GAIN_ONE);
}

void loop_I2C() {
  // USA LA VARIABLE 'Corriente_L1' (que es float), NO EL PIN.
  Corriente_L1_Lectura = ads.readADC_SingleEnded(1);
  Corriente_L2_Lectura = ads.readADC_SingleEnded(0);
  Corriente_L3_Lectura = ads.readADC_SingleEnded(2);
}

void inicializarComunicaciones() {
    setup_wifi();
    espClient.setCACert(root_ca);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}
