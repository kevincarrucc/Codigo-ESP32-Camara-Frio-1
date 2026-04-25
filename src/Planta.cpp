#include "Planta.h"
#include "Variables.h"

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
        manejarTecla(tecla);
    }

    actualizarPantalla();
}

// ------------------ EDICIÓN ------------------

void Planta::editarFloat(char tecla, float &variable, Pantalla pantallaVolver)
{

    if (!editando)
    {
        editando = true;
        buffer = "";
    }

    if (tecla >= '0' && tecla <= '9')
    {
        if (buffer.length() < 6)
        {
            buffer += tecla;
        }
    }

    if (tecla == '#')
    {
        if (buffer.length() > 0)
        {
            variable = buffer.toFloat();
        }
        editando = false;
        actual = pantallaVolver;
    }

    if (tecla == 'C')
    {
        editando = false;
        actual = pantallaVolver;
    }
}

// ------------------ TECLAS ------------------

void Planta::manejarTecla(char tecla)
{

    switch (actual)
    {

    case PANTALLA_PRINCIPAL:
        if (tecla == 'A')
            actual = PANTALLA_FASES;
        if (tecla == 'D')
            actual = PANTALLA_CONFIGURACION;
        break;

    case PANTALLA_FASES:
        if (tecla == 'B')
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

        // confirmar
        if (tecla == '#')
        {

            if (buffer == claveSistema)
            {
                logueado = true;
                actual = PANTALLA_CONFIGURACION;
            }
            else
            {
                // clave incorrecta → limpiar
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

    case PANTALLA_NUEVA_CLAVE:

        if (tecla >= '0' && tecla <= '9')
        {
            if (buffer.length() < 6)
            {
                buffer += tecla;
            }
        }

        if (tecla == '*')
        {
            if (buffer.length() > 0)
            {
                buffer.remove(buffer.length() - 1);
            }
        }

        if (tecla == '#')
        {

            if (!repitiendoClave)
            {
                nuevaClave1 = buffer;
                buffer = "";
                repitiendoClave = true;
            }
            else
            {
                nuevaClave2 = buffer;

                if (nuevaClave1 == nuevaClave2)
                {
                    claveSistema = nuevaClave1;
                }

                // reset
                buffer = "";
                nuevaClave1 = "";
                nuevaClave2 = "";
                repitiendoClave = false;

                actual = PANTALLA_LOGIN;
            }
        }

        if (tecla == 'C')
        {
            buffer = "";
            repitiendoClave = false;
            actual = PANTALLA_LOGIN;
        }

        break;

    case PANTALLA_CONFIGURACION:
        if (tecla == 'C')
            actual = PANTALLA_PRINCIPAL;
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
        editarFloat(tecla, Referencia_Temperatura, PANTALLA_CONFIGURACION);
        break;

    case PANTALLA_CONFIG_HISTERESIS:
        editarFloat(tecla, Ventana_Temperatura, PANTALLA_CONFIGURACION);
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
        editarFloat(tecla, Aviso_Temperatura_Baja, PANTALLA_CONFIGURACION);
        break;

    case PANTALLA_CONFIG_AVISO_ALTO:
        editarFloat(tecla, Aviso_Temperatura_Alta, PANTALLA_CONFIGURACION);
        break;

    case PANTALLA_CONFIG_ALARMA_BAJO:
        editarFloat(tecla, Alarma_Temperatura_Baja, PANTALLA_CONFIGURACION);
        break;

    case PANTALLA_CONFIG_ALARMA_ALTO:
        editarFloat(tecla, Alarma_Temperatura_Alta, PANTALLA_CONFIGURACION);
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

        if (actual != anterior || editando)
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

            case PANTALLA_NUEVA_CLAVE:
                display.pantallaNuevaClave(buffer, repitiendoClave);
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

            case PANTALLA_ALERTAS:
                display.pantallaAlertas();
                break;
            }

            anterior = actual;
        }
    }
