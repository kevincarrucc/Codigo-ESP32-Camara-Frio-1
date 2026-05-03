#include "Planta.h"
#include "Variables.h"
#include <PubSubClient.h>

extern PubSubClient client;
extern const char* TOPIC_SET_POINT_TEMPERATURA;
extern const char* TOPIC_SET_POINT_HISTERESIS;
extern const char* TOPIC_ALARMA_TEMPERATURA_ALTA;
extern const char* TOPIC_ALARMA_TEMPERATURA_BAJA;
extern const char* TOPIC_AVISO_TEMPERATURA_ALTA;
extern const char* TOPIC_AVISO_TEMPERATURA_BAJA;
extern bool avisoReconocido;
extern bool alertaReconocida;

Planta::Planta(Teclado &t, Display &d) : teclado(t), display(d) {}

void Planta::begin()
{
    actual = PANTALLA_PRINCIPAL;
    anterior = (Pantalla)-1;
}

void Planta::update()
{

    char tecla = teclado.leer();

    if (tecla != '\0')
    {
        ultimaActividad = millis();
        manejarTecla(tecla);
    }

    verificarAlertas();
    verificarAvisos();
    verificarTimeout();
    actualizarPantalla();

    if (millis() - ultimoUpdate >= 2000)
    {
        ultimoUpdate = millis();
        refrescarPantalla();
    }
}

void Planta::limpiarBuffer() {
    buffer = "";
    editando = false;
}
void Planta::verificarAlertas() {
    // 1. Si hay una alarma activa y no ha sido reconocida
    if (Estado_Alarma && !alertaReconocida) {
        if (!enPantallaAlerta) {
            pantallaAnteriorAlerta = actual; 
            actual = PANTALLA_ALERTAS;
            enPantallaAlerta = true;
            anterior = (Pantalla)-1; 
        }
    }
    // 2. SEGURIDAD: Solo volvemos a la principal si ESTÁBAMOS viendo el cartel
    else if (!Estado_Alarma && enPantallaAlerta) {
        enPantallaAlerta = false;
        // Solo forzamos PANTALLA_PRINCIPAL si el cartel estaba bloqueando la vista
        if (actual == PANTALLA_ALERTAS) {
            actual = PANTALLA_PRINCIPAL;
        }
        anterior = (Pantalla)-1;
    }
}

void Planta::verificarAvisos() {
    if (Estado_Aviso && !avisoReconocido && !enPantallaAviso && !(Estado_Alarma && !alertaReconocida)) {
        if (!enPantallaAviso) {
            pantallaAnteriorAviso = actual;
            actual = PANTALLA_AVISOS;
            enPantallaAviso = true;
            anterior = (Pantalla)-1;
        }
    }
    // SEGURIDAD: Solo volvemos a la principal si ESTÁBAMOS viendo el cartel
    else if (!Estado_Aviso && enPantallaAviso) {
        enPantallaAviso = false;
        if (actual == PANTALLA_AVISOS) {
            actual = PANTALLA_PRINCIPAL;
        }
        anterior = (Pantalla)-1;
    }
}

void Planta::verificarTimeout() {

    if (logueado) {

        unsigned long ahora = millis();

        if (ahora - ultimaActividad > TIMEOUT_LOGIN) {

            logueado = false;

            actual = PANTALLA_PRINCIPAL;

            anterior = (Pantalla)-1;
        }
    }
}

// ------------------ EDICIÓN ------------------

void Planta::editarFloat(char tecla, float &variable, Pantalla pantallaVolver, float min, float max)
{

    if (!editando)
    {
        editando = true;
        buffer = "";
    }

    if (tecla >= '0' && tecla <= '9')
    {
        if (buffer.length() < 2)
        {
            buffer += tecla;
        }
    }

    if (tecla == '#')
    {
        if (buffer.length() > 0)
        {
            float nuevo = buffer.toFloat();


            if (nuevo < min) nuevo = min;
            if (nuevo > max) nuevo = max;

            variable = nuevo;
            limpiarBuffer();

            if (client.connected())
            {
                char publishBuffer[16];
                dtostrf(variable, 4, 1, publishBuffer);

                if (actual == PANTALLA_CONFIG_TEMPERATURA)
                {
                    client.publish(TOPIC_SET_POINT_TEMPERATURA, publishBuffer);
                }
                else if (actual == PANTALLA_CONFIG_HISTERESIS)
                {
                    client.publish(TOPIC_SET_POINT_HISTERESIS, publishBuffer);
                }
                else if (actual == PANTALLA_CONFIG_ALARMA_BAJO)
                {
                    client.publish(TOPIC_ALARMA_TEMPERATURA_BAJA, publishBuffer);
                }
                else if (actual == PANTALLA_CONFIG_ALARMA_ALTO)
                {
                    client.publish(TOPIC_ALARMA_TEMPERATURA_ALTA, publishBuffer);
                }
                else if (actual == PANTALLA_CONFIG_AVISO_BAJO)
                {
                    client.publish(TOPIC_AVISO_TEMPERATURA_BAJA, publishBuffer);
                }
                else if (actual == PANTALLA_CONFIG_AVISO_ALTO)
                {
                    client.publish(TOPIC_AVISO_TEMPERATURA_ALTA, publishBuffer);
                }
            }
        }
        editando = false;
        actual = pantallaVolver;
    }

    if (tecla == 'C')
    {
        editando = false;
        limpiarBuffer();
        actual = pantallaVolver;
    }
}

// ------------------ TECLAS ------------------

void Planta::manejarTecla(char tecla)
{
    Serial.print("Tecla: ");
    Serial.println(tecla);

    // Verificar pantallas de aviso/alerta ANTES del switch
    if (enPantallaAviso) {
        if (tecla == '#') {
            avisoReconocido = true;
            enPantallaAviso = false;
            actual = pantallaAnteriorAviso;
        }
        return;
    }

    if (enPantallaAlerta) {
        if (tecla == '#') {
            alertaReconocida = true;
            enPantallaAlerta = false;
            actual = pantallaAnteriorAlerta;
        }
        return;
    }

    switch (actual)
    {

    case PANTALLA_PRINCIPAL:
        if (tecla == 'A')
            actual = PANTALLA_FASES;
        if (tecla == 'D')
            {
            if (logueado)
                actual = PANTALLA_CONFIGURACION;
            else
                actual = PANTALLA_LOGIN;
        }
        break;

    case PANTALLA_FASES:
        if (tecla == 'C')
            actual = PANTALLA_PRINCIPAL;
        if (tecla == 'D')
        {
            if (logueado)
                actual = PANTALLA_CONFIGURACION;
            else
                actual = PANTALLA_LOGIN;
        }
        break;

    case PANTALLA_LOGIN:

        // escribir clave
        if (tecla >= '0' && tecla <= '9')
        {
            if (buffer.length() < 6)
            {
                buffer += tecla;
            }
        }

        // borrar (opcional con *)
        if (tecla == '*')
        {
            if (buffer.length() > 0)
            {
                buffer.remove(buffer.length() - 1);
            }
        }

        //confirmar
        if (tecla == '#') {

        if (buffer == claveSistema) {

        logueado = true;

        tiempoLogin = millis();
        ultimaActividad = millis();
        actual = PANTALLA_CONFIGURACION;
        } 
        else 
        {
        buffer = "";
        }
        }

        // salir
        if (tecla == 'C')
        {
            buffer = "";
            actual = PANTALLA_PRINCIPAL;
            
        }

        break;

    case PANTALLA_CONFIGURACION:
        if (!logueado) {
        actual = PANTALLA_LOGIN;
        break;
        }
        if (tecla == 'C') {
            actual = PANTALLA_PRINCIPAL;
            limpiarBuffer();
        }
        if (tecla == '1')
            actual = PANTALLA_CONFIG_TEMPERATURA;
        if (tecla == '2')
            actual = PANTALLA_CONFIG_HISTERESIS;
        if (tecla == '3')
            actual = PANTALLA_MENU_CONFIG_AVISOS;
        if (tecla == '4')
            actual = PANTALLA_MENU_CONFIG_ALARMAS;
        break;

    case PANTALLA_CONFIG_TEMPERATURA:
        editarFloat(tecla, Referencia_Temperatura, PANTALLA_CONFIGURACION,3, 7);
        break;

    case PANTALLA_CONFIG_HISTERESIS:
        editarFloat(tecla, Ventana_Temperatura, PANTALLA_CONFIGURACION, 1, 5);
        break;

    case PANTALLA_MENU_CONFIG_AVISOS:
        if (tecla == 'C')
            actual = PANTALLA_CONFIGURACION;
        if (tecla == '1')
            actual = PANTALLA_CONFIG_AVISO_BAJO;
        if (tecla == '2')
            actual = PANTALLA_CONFIG_AVISO_ALTO;
        break;

    case PANTALLA_MENU_CONFIG_ALARMAS:
        if (tecla == 'C')
            actual = PANTALLA_CONFIGURACION;
        if (tecla == '1')
            actual = PANTALLA_CONFIG_ALARMA_BAJO;
        if (tecla == '2')
            actual = PANTALLA_CONFIG_ALARMA_ALTO;
        break;

    case PANTALLA_CONFIG_AVISO_BAJO:
        editarFloat(tecla, Aviso_Temperatura_Baja, PANTALLA_CONFIGURACION, 0, 3);
        break;

    case PANTALLA_CONFIG_AVISO_ALTO:
        editarFloat(tecla, Aviso_Temperatura_Alta, PANTALLA_CONFIGURACION, 10, 15);
        break;

    case PANTALLA_CONFIG_ALARMA_BAJO:
        editarFloat(tecla, Alarma_Temperatura_Baja, PANTALLA_CONFIGURACION, 0, 3);
        break;

    case PANTALLA_CONFIG_ALARMA_ALTO:
        editarFloat(tecla, Alarma_Temperatura_Alta, PANTALLA_CONFIGURACION, 10, 18);
        break;

    case PANTALLA_AVISOS:
        if (tecla == 'C')
            actual = PANTALLA_PRINCIPAL;
        break;

    case PANTALLA_ALERTAS:
        if (tecla == 'C')
            actual = PANTALLA_PRINCIPAL;
        break;
    }
}

// ------------------ DISPLAY ------------------

void Planta::actualizarPantalla()
{
    unsigned long ahora = millis();
    if (actual != anterior || editando || buffer.length() > 0)
    {
        if (actual == anterior && ahora - ultimoRedibujoPantalla < 500) {
            return;
        }

            switch (actual)
            {

            case PANTALLA_PRINCIPAL:
                display.pantallaPrincipal(
                    Temperatura_Promedio,
                    Referencia_Temperatura,
                    Ventana_Temperatura);
                break;

            case PANTALLA_FASES:
                display.pantallaFases(
                    Ausencia_Fase1,
                    Ausencia_Fase2,
                    Ausencia_Fase3);
                break;

            case PANTALLA_LOGIN:
                display.pantallaLogin(buffer);
                break;

            case PANTALLA_CONFIGURACION:
                display.pantallaConfig();
                break;

            case PANTALLA_CONFIG_TEMPERATURA:
                display.pantallaConfigTemp(
                    Referencia_Temperatura,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_HISTERESIS:
                display.pantallaConfigHisteresis(
                    Ventana_Temperatura,
                    buffer,
                    editando);
                break;

            case PANTALLA_MENU_CONFIG_AVISOS:
                display.pantallaMenuConfigAvisos();
                break;

            case PANTALLA_MENU_CONFIG_ALARMAS:
                display.pantallaMenuConfigAlarmas();
                break;

            case PANTALLA_CONFIG_AVISO_BAJO:
                display.pantallaConfigAvisoBajo(
                    Aviso_Temperatura_Baja,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_AVISO_ALTO:
                display.pantallaConfigAvisoAlto(
                    Aviso_Temperatura_Alta,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_ALARMA_BAJO:
                display.pantallaConfigAlarmaBajo(
                    Alarma_Temperatura_Baja,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_ALARMA_ALTO:
                display.pantallaConfigAlarmaAlto(
                    Alarma_Temperatura_Alta,
                    buffer,
                    editando);
                break;

            case PANTALLA_AVISOS:
                display.pantallaAvisos();
                break;

            case PANTALLA_ALERTAS:
                display.pantallaAlertas();
                break;
            }

            anterior = actual;
            ultimoRedibujoPantalla = ahora;
        }
    }

    void Planta::refrescarPantalla()
    {

        if (!editando && buffer.length() == 0)
        {

            switch (actual)
            {

            case PANTALLA_PRINCIPAL:
                display.pantallaPrincipal(
                    Temperatura_Promedio,
                    Referencia_Temperatura,
                    Ventana_Temperatura);
                break;

            case PANTALLA_FASES:
                display.pantallaFases(
                    Ausencia_Fase1,
                    Ausencia_Fase2,
                    Ausencia_Fase3);
                break;
            
            case PANTALLA_LOGIN:
                display.pantallaLogin(buffer);
                break;

            case PANTALLA_CONFIGURACION:
                display.pantallaConfig();
                break;

            case PANTALLA_CONFIG_TEMPERATURA:
                display.pantallaConfigTemp(
                    Referencia_Temperatura,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_HISTERESIS:
                display.pantallaConfigHisteresis(
                    Ventana_Temperatura,
                    buffer,
                    editando);
                break;

            case PANTALLA_MENU_CONFIG_AVISOS:
                display.pantallaMenuConfigAvisos();
                break;

            case PANTALLA_MENU_CONFIG_ALARMAS:
                display.pantallaMenuConfigAlarmas();
                break;

            case PANTALLA_CONFIG_AVISO_BAJO:
                display.pantallaConfigAvisoBajo(
                    Aviso_Temperatura_Baja,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_AVISO_ALTO:
                display.pantallaConfigAvisoAlto(
                    Aviso_Temperatura_Alta,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_ALARMA_BAJO:
                display.pantallaConfigAlarmaBajo(
                    Alarma_Temperatura_Baja,
                    buffer,
                    editando);
                break;

            case PANTALLA_CONFIG_ALARMA_ALTO:
                display.pantallaConfigAlarmaAlto(
                    Alarma_Temperatura_Alta,
                    buffer,
                    editando);
                break;

            case PANTALLA_AVISOS:
                display.pantallaAvisos();
                break;

            case PANTALLA_ALERTAS:
                display.pantallaAlertas();
                break;
            }
        }
    }
