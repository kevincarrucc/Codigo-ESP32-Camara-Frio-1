#include "Display.h"



Display::Display(uint8_t addr) : lcd(addr, 20, 4) {}

void Display::begin() {
    lcd.init();
    lcd.backlight();
}

void mostrarEdicion(LiquidCrystal_I2C &lcd, float actual, String buffer, bool editando) {

    lcd.setCursor(0,1);
    lcd.print("Valor actual:");
    lcd.setCursor(13,1);
    lcd.print(actual,0);

    lcd.setCursor(0,2);
    lcd.print("Nuevo valor:");
    lcd.setCursor(13,2);

    if (editando) {
        lcd.setCursor(13,2);
        lcd.print(buffer);
        lcd.setCursor(13 + buffer.length(), 2);
    } 

    lcd.setCursor(0,3);
    lcd.print("C-Atras   #-Confirm.");
}

void Display::pantallaPrincipal(float Temperatura_Promedio, float Referencia_Temperatura, float Ventana_Temperatura) { 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Temperatura:"); 
    
    lcd.setCursor(0, 1); 
    lcd.print("Actual:"); 
    lcd.setCursor(7, 1); 
    lcd.print(Temperatura_Promedio,0);
    lcd.setCursor(10, 1); 
    lcd.print("Min:"); 
    lcd.setCursor(14, 1); 
    lcd.print(Referencia_Temperatura-Ventana_Temperatura,0); 
    lcd.setCursor(0, 2); 
    lcd.print("SP:");
    lcd.setCursor(3, 2); 
    lcd.print(Referencia_Temperatura,0);
    lcd.setCursor(10, 2); 
    lcd.print("Max:"); 
    lcd.setCursor(14, 2); 
    lcd.print(Referencia_Temperatura+Ventana_Temperatura,0); 
    lcd.setCursor(0, 3); 
    lcd.print("A-Fases    D-Config."); }

void Display::pantallaFases(bool Ausencia_Fase1, bool Ausencia_Fase2, bool Ausencia_Fase3) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Estado de fases:");

    lcd.setCursor(0,1);
    lcd.print("     F1  F2  F3");

    lcd.setCursor(5,2);
    if (Ausencia_Fase1) {
        lcd.print("--");
    } else {
        lcd.print("OK");
    }
    lcd.setCursor(9,2);
    if (Ausencia_Fase2) {
        lcd.print("--");
    } else {
        lcd.print("OK");
    }
    lcd.setCursor(13,2);
    if (Ausencia_Fase3) {
        lcd.print("--");
    } else {
        lcd.print("OK");
    }

    lcd.setCursor(0,3);
    lcd.print("C-Atras   D-Config.");
}

void Display::pantallaLogin(const String &buffer) {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Ingresar");

    lcd.setCursor(0,1);
    lcd.print("Clave:");

    lcd.setCursor(7,1);

    for (int i = 0; i < buffer.length(); i++) {
        lcd.print(buffer[i]);
    }

    lcd.setCursor(0,3);
    lcd.print("C-Atras         #-OK");
}

void Display::pantallaConfig() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Configuraciones:");

    lcd.setCursor(0,1);
    lcd.print("1-Temp. 2-Histeresis");

    lcd.setCursor(0,2);
    lcd.print("3-Avisos 4-Alarmas");

    lcd.setCursor(0,3);
    lcd.print("C-Atras   #-Confirm.");
}

void Display::pantallaConfigTemp(float val, String buffer, bool editando) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Config. temp.(C)");

    mostrarEdicion(lcd, val, buffer, editando);
}

void Display::pantallaConfigHisteresis(float val, String buffer, bool editando) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Config. hist.(+/-C)");

    mostrarEdicion(lcd, val, buffer, editando);
}

void Display::pantallaMenuConfigAvisos() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Config. avisos:");

    lcd.setCursor(0,1);
    lcd.print("1-Temp. baja");

    lcd.setCursor(0,2);
    lcd.print("2-Temp. alta");

    lcd.setCursor(0,3);
    lcd.print("C-Atras   #-Confirm.");
}

void Display::pantallaMenuConfigAlarmas() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Config. alarmas:");

    lcd.setCursor(0,1);
    lcd.print("1-Temp. baja");

    lcd.setCursor(0,2);
    lcd.print("2-Temp. alta");

    lcd.setCursor(0,3);
    lcd.print("C-Atras   #-Confirm.");
}

void Display::pantallaConfigAvisoBajo(float val, String buffer, bool editando) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Aviso baja temp.(C)");

    mostrarEdicion(lcd, val, buffer, editando);
}
void Display::pantallaConfigAvisoAlto(float val, String buffer, bool editando) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Aviso alta temp.(C)");

    mostrarEdicion(lcd, val, buffer, editando);
}

void Display::pantallaConfigAlarmaBajo(float val, String buffer, bool editando) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alar. baja temp.(C)");

    mostrarEdicion(lcd, val, buffer, editando);
}

void Display::pantallaConfigAlarmaAlto(float val, String buffer, bool editando) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Alar. alta temp.(C)");

    mostrarEdicion(lcd, val, buffer, editando);
}

void Display::pantallaAvisos() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   ADVERTENCIA!!!");

    lcd.setCursor(0,1);
    lcd.print(" Hay avisos activos");

    lcd.setCursor(0,2);
    lcd.print("Verificar pagina web");

    lcd.setCursor(0,3);
    lcd.print("          #-Confirm.");
}


void Display::pantallaAlertas() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("      ALERTA!!!");

    lcd.setCursor(0,1);
    lcd.print("Hay alarmas activas");

    lcd.setCursor(0,2);
    lcd.print("Verificar pagina web");

    lcd.setCursor(0,3);
    lcd.print("          #-Confirm.");
}
