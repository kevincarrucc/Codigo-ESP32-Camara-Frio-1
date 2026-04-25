#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>

class Display {
private:
    LiquidCrystal_I2C lcd;

public:
    Display(uint8_t addr);
    void begin();

    void pantallaPrincipal(float Temperatura_Promedio, float Referencia_Temperatura, float Ventana_Temperatura);
    void pantallaFases(bool Ausencia_Fase1, bool Ausencia_Fase2, bool Ausencia_Fase3);
    void pantallaLogin(const String &buffer);
    void pantallaNuevaClave(const String &buffer, bool repitiendo);
    void pantallaConfig();
    void pantallaConfigTemp(float temperatura, String buffer, bool editando);
    void pantallaConfigHisteresis(float histeresis, String buffer, bool editando);
    void pantallaMenuConfigAvisos();
    void pantallaMenuConfigAlarmas();
    void pantallaConfigAvisoBajo(float avisoBajo, String buffer, bool editando);
    void pantallaConfigAvisoAlto(float avisoAlto, String buffer, bool editando);
    void pantallaConfigAlarmaBajo(float alarmaBaja, String buffer, bool editando);
    void pantallaConfigAlarmaAlto(float alarmaAlta, String buffer, bool editando);
    void pantallaAlertas();
};

#endif