#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Claves.h"

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

const char* TOPIC_ACT_MOTOR_1 = ACT_MOTOR_1;
const char* TOPIC_ACT_VENTILADOR_MOTOR_1 = ACT_VENTILADOR_MOTOR_1;
const char* TOPIC_ACT_MOTOR_2 = ACT_MOTOR_2;
const char* TOPIC_ACT_LUZ_CAMARA = ACT_LUZ_CAMARA;

// Definición de pines a utilizar
//output pines
const int Motor1Pin = 19;
const int Motor2Pin = 18;
const int VentiladorPin = 5;
const int LuzCamaraPin = 23;
//input pines
const int BttnPuertaPin = 4;
const int BttnLuzPin = 15;
//adc pines
const int Temperatura1Pin = 33;
const int Temperatura2Pin = 32;
const int TemperaturaExtPin = 35;
const int TensionL1Pin = 34;
const int CorrienteL1Pin = 39;
//variables globales
bool estadoLuz = false;
int lastButtonState = HIGH;
int valorAnalogico = 0;
float temperatura = 0.0;
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

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
char msg[50];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_ACT_LUZ_CAMARA) == 0) {

    estadoLuz = payload[0] - '0';

    digitalWrite(LuzCamaraPin, estadoLuz);

    Serial.print("Luz camara MQTT: ");
    Serial.println(estadoLuz);
  }

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(TOPIC_ACT_LUZ_CAMARA);

      client.subscribe(TOPIC_TEMPERATURA_1);
      client.subscribe(TOPIC_TEMPERATURA_2);
      client.subscribe(TOPIC_TEMPERATURA_EXTERIOR);
      client.subscribe(TOPIC_TENSION_L1);
      client.subscribe(TOPIC_CORRIENTE_L1);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LuzCamaraPin,      OUTPUT);
  pinMode(VentiladorPin,     OUTPUT);
  pinMode(Motor1Pin,         OUTPUT);
  pinMode(Motor2Pin,         OUTPUT);
  pinMode(BttnPuertaPin,     INPUT);
  pinMode(BttnLuzPin,        INPUT_PULLUP);
  pinMode(Temperatura1Pin,   ANALOG);
  pinMode(Temperatura2Pin,   ANALOG); 
  pinMode(TemperaturaExtPin, ANALOG);
  pinMode(TensionL1Pin,      ANALOG);
  pinMode(CorrienteL1Pin,    ANALOG);

  Serial.begin(9600);
  setup_wifi();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  char buffer[10];

  int buttonState = digitalRead(BttnLuzPin);

  if(buttonState == LOW && lastButtonState == HIGH){

    estadoLuz = !estadoLuz;

    digitalWrite(LuzCamaraPin, estadoLuz);

    client.publish(TOPIC_ACT_LUZ_CAMARA, estadoLuz ? "1" : "0");

    delay(200);
  }

lastButtonState = buttonState;

  valorAnalogico = analogRead(Temperatura1Pin);
  
  itoa(valorAnalogico, buffer, 10);
  client.publish(TOPIC_TEMPERATURA_1, buffer);

  valorAnalogico = analogRead(Temperatura2Pin);
  
  itoa(valorAnalogico, buffer, 10);
  client.publish(TOPIC_TEMPERATURA_2, buffer);

  valorAnalogico = analogRead(TemperaturaExtPin);
  
  itoa(valorAnalogico, buffer, 10);
  client.publish(TOPIC_TEMPERATURA_EXTERIOR, buffer);

  valorAnalogico = analogRead(TensionL1Pin);
  
  itoa(valorAnalogico, buffer, 10);
  client.publish(TOPIC_TENSION_L1, buffer);

  valorAnalogico = analogRead(CorrienteL1Pin);
  
  itoa(valorAnalogico, buffer, 10);
  client.publish(TOPIC_CORRIENTE_L1, buffer);

  int puerta = digitalRead(BttnPuertaPin);
  client.publish(TOPIC_ESTADO_PUERTA, puerta ? "1" : "0");

  delay(9000);
}