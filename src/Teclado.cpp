#include "Teclado.h"
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

extern Adafruit_ADS1115 ads;

static const uint8_t TECLADO_ADS_CHANNEL = 3; // Canal AIN3 del ADS1115: pin 4 en la placa ADS

struct TeclaRango {
    char tecla;
    int16_t minValor;
    int16_t maxValor;
};

static const TeclaRango TECLADO_RANGOS[16] = {
    {'1', 22390, 32767},
    {'4', 20800, 22390},
    {'2', 20223, 20800},
    {'7', 19238, 20223},
    {'5', 18669, 19238},
    {'3', 17903, 18669},
    {'8', 17470, 17903},
    {'*', 17236, 17470},
    {'6', 16443, 17236},
    {'0', 15929, 16443},
    {'A', 15300, 15929},
    {'9', 15104, 15300},
    {'B', 14579, 15104},
    {'#', 14149, 14579},
    {'C', 13344, 14149},
    {'D', 12000, 13344}
};

static const int TECLADO_ADC_TOLERANCIA = 120; // Ajusta según el ruido y las tolerancias reales de las resistencias
static const int TECLADO_ADC_NO_KEY_LIMITE = 80; // Ajustar si la señal de "no tecla" no queda cerca de 0

Teclado::Teclado(uint8_t direccion) {
    (void)direccion; // Conservamos el constructor compatible con la inicialización existente
}

char Teclado::leer() {
    char teclaDetectada = '\0';

    int16_t lectura = ads.readADC_SingleEnded(TECLADO_ADS_CHANNEL);
    if (lectura < 0) {
        return '\0';
    }

    if (lectura <= TECLADO_ADC_NO_KEY_LIMITE) {
        teclaPresionada = false;
        return '\0';
    }

    for (uint8_t i = 0; i < 16; i++) {
        if (lectura >= TECLADO_RANGOS[i].minValor && lectura < TECLADO_RANGOS[i].maxValor) {
            teclaDetectada = TECLADO_RANGOS[i].tecla;
            break;
        }
    }

    if (teclaDetectada == '\0') {
        Serial.print("ADC raw no coincide: ");
        Serial.println(lectura);
    }
    unsigned long ahora = millis();
    if (teclaDetectada != '\0') {
        if (!teclaPresionada && (ahora - ultimoTiempo > debounce)) {
            teclaPresionada = true;
            ultimaTecla = teclaDetectada;
            ultimoTiempo = ahora;
            Serial.print("Tecla detectada: ");
            Serial.println(teclaDetectada);
            Serial.print("ADC raw: ");
            Serial.println(lectura);
            return teclaDetectada;
        }
    } else {
        teclaPresionada = false;
    }

    return '\0';
}
