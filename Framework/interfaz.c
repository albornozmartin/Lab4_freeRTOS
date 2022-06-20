/* ************************************************************************** */
/** Archivo donde est�n las funciones relacionadas a la Tarea B

  @File Name
 interfaz.c

  @Summary
 Archivo que contiene las funciones relacionadas a la interfaz.

  @Description
 Separamos todas las funciones de la interfaz, tambi�n se inicializa el sem�foro y el LED el cual es el recurso compartido.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include "interfaz.h"



/* ************************************************************************** */
/* ************************************************************************** */
/*Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */


app_register_t ledConsultaEstado;

/* Semaforo para utilizar el LED, se usa tanto en la tarea B como la tarea C.*/
SemaphoreHandle_t semaforo;

/* Handler del timer one-shot.*/
xTimerHandle handlerDelTimer;
/* ************************************************************************** */

static const TickType_t tiempoms = (10 / portTICK_PERIOD_MS);

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */


/** 
  @Function
 static void crearSemaforo(void)

  @Summary
 Crea el sem�foro.

  @Description
 Crea el sem�foro y lo permite utilizarlo a trav�s de su handler. Se ejecuta una sola vez.
    
 */
static void crearSemaforo() {
    semaforo = xSemaphoreCreateMutex();
    if (semaforo != NULL) {
    }
}

/** 
  @Function
 static void usbListo2()

  @Summary
 Retorna un booleano segun el estado del USB.

  @Description
 Retorna verdadero si el USB est� configurado y no est� en estado suspendido, en caso contrario retornar� falso.
    
 */
static bool usbListo2() {
    if ((USBGetDeviceState() < CONFIGURED_STATE) || (USBIsDeviceSuspended() == true)) {
        return false;
    } else {
        return true;
    }
}


/** 
  @Function
 static void enviarUnDato(uint8_t unDato)

  @Summary
 Envia un dato al USB.

  @Description
 Envia datos al USB, revisando si est� listo, en caso contrario espera y realiza la funci�n CDCTxService(); 
 * hasta que logre enviar los datos. 
 * Luego retorna True.
    
 */
static bool enviarUnDato(uint8_t* unDato) {
    while (1) {
        if (USBUSARTIsTxTrfReady()) {
            putsUSBUSART(unDato);
            return true;
        } else {
            vTaskDelay(tiempoms);
            CDCTxService();
        }
    }
}

/** 
  @Function
 static uint8_t recibirDeUSB()

  @Summary
 Recibe los datos del USB.

  @Description
 * Realiza un memset para limpiar el buffer y luego carga en el buffer los datos obtenidos del USB.
 * Retorna el largo de los elementos que recibe del USB.
 */
static uint8_t recibirDeUSB() {
    memset(buffer, 0, sizeof (buffer)); //utilizo memset para limpiar el buffer antes de utilizarlo, as� me aseguro que est� vac�o antes
    uint8_t num = getsUSBUSART(buffer, sizeof (buffer));
    return num;
}

/** 
  @Function
 static bool validarFecha(int dia,int mes, int anio)

  @Summary
 Valida la fecha pasada por par�metro.

  @Description
 Retorna verdadero si la fecha pasada por par�metro es correcta, en caso contrario retorna falso.
    
 */
static bool validarFecha(int dia, int mes, int anio) {
    if (mes > 0 && mes < 13) {
        if (anio > 1899 && anio <= 2022) {
            if ((mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12) && dia <= 31 && dia > 0) { //Si los meses son de 31 dias entonces retorna verdadero
                return true;
            } else if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia <= 30 && dia > 0) { // si los meses son de 30 dias retorno verdadero
                return true;
            } else if (mes == 2 && dia <= 29 && dia > 0) { // si febrero tiene hasta 29 dias retorno verdadero 
                return true;
            }
        }
    }
    return false;
}

/** 
  @Function
 static bool validarHora()

  @Summary
 Valida la hora pasada por par�metro.

  @Description
  Retorna verdadero si la hora pasada por par�metro es correcta, en caso contrario retorna falso.
    
 */
static bool validarHora(int hora, int min) {
    if (hora >= 0 && hora <= 23) {
        if (min >= 0 && min <= 59) {
            return true;
        }
    }
    return false;
}

/** 
  @Function
 static uint8_t esperandoEntrada()

  @Summary
 Queda esperando hasta que recibe algo del USB y lo retorna.

  @Description
 Hace un delay del freeRTOS mientras no recibe nada, cuando recibe algo lo retorna.
    
 */
uint8_t esperandoEntrada(void) {
    uint8_t respuesta = 0;
    while (1) {
        respuesta = recibirDeUSB();
        while (respuesta == 0) {
            vTaskDelay(tiempoms);
            CDCTxService();
            respuesta = recibirDeUSB();
        }
        return buffer;
        break;
    }
}

/** 
  @Function
 static bool configurarHoraSwitch

  @Summary
 Configura la fecha y hora del RTCC.

  @Description
 Funci�n que es parte de la interfaz, chequea que los datos que vaya obteniendo del USB sean correctos, y configura el RTCC.
 * retorna True cuando finaliza la operacion correctamente.
    
 */
static bool configurarHoraSwitch() {
    static ESTADOS_CONFIG_FECHA estado = IngresarFecha;
    static struct tm fecha;
    while (1) {
        switch (estado) {
            case(IngresarFecha):
                enviarUnDato("\n\n -Ingresa una fecha dd/mm/aaaa:");
                esperandoEntrada();
                if (sscanf(buffer, "%u/%u/%u", &(fecha.tm_mday), &(fecha.tm_mon), &(fecha.tm_year)) == 3) {
                    if (validarFecha(fecha.tm_mday, fecha.tm_mon, fecha.tm_year)) {
                        estado = IngresarHora;
                        break;
                    } else {
                        estado = ErrorFecha;
                        break;
                    }
                } else {
                    estado = ErrorFecha;
                    break;
                }
                break;
            case(ErrorFecha):
                enviarUnDato("\n -Lo siento, Fecha ingresada NO v�lida. \n");
                estado = IngresarFecha;
                break;

            case(IngresarHora):
                enviarUnDato("\n\n -Ingresa una hora en forma HH:MM :");
                esperandoEntrada();
                if (sscanf(buffer, "%u:%u", &(fecha.tm_hour), &(fecha.tm_min)) == 2) {
                    if (validarHora(fecha.tm_hour, fecha.tm_min)) {
                        estado = ConfigurarRTCC;
                    } else {
                        estado = ErrorHora;
                        break;
                    }
                } else {
                    estado = ErrorHora;
                }
                break;
            case(ErrorHora):
                enviarUnDato("\n -Lo siento, Hora ingresada NO v�lida. \n");
                estado = IngresarHora;
                break;
            case(ConfigurarRTCC):
                fecha.tm_mon = fecha.tm_mon - 1; //mes de 0 a 11
                fecha.tm_year = fecha.tm_year - 1900; //a�o a partir de 1900 
                RTCC_TimeSet(&fecha);
                enviarUnDato("\n\n Fecha y hora configurada correctamente!!.\n\n");
                estado = IngresarFecha;
                return true;
        }
    }
}



/** 
  @Function
 static timerCallBack

  @Summary
 Callback del timer utilizado.

  @Description
 Esta es la funci�n que se ejucta luego de que expira el tiempo del timer. Env�a el evento a la cola para ejecutarse.
    
 */
void timerCallBack(xTimerHandle handler) {
    app_register_t *ledEncender = (app_register_t*) pvTimerGetTimerID(handler);
    agregarEvento(*ledEncender);
    free(ledEncender);
}



/* ************************************************************************** */



/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/** 
  @Function
 static bool nuevoEvento()

  @Summary
 Configura un nuevo Evento.

  @Description
 Pide y valida los par�metros para encender un LED, configura el timer para enviar el evento luego de expirado el tiempo.
    
 */
static bool nuevoEvento() {

    static ESTADOS_EVENTO estadoLED = NumeroLED;

    int nroLED;
    int colorLED;
    int tEncendidoLED;
    while (1) {
        switch (estadoLED) {
            case(NumeroLED):
                enviarUnDato("\n\n -Elegir LED a encender (0-7):\n");
                esperandoEntrada();
                nroLED = atoi(buffer);
                if (nroLED >= 0 && nroLED <= 7) {
                    estadoLED = ColorLED;
                } else {
                    estadoLED = ErrorNumero;
                }
                break;
            case (ErrorNumero):
                enviarUnDato("\n -Lo siento, LED indicado NO v�lido.");
                estadoLED = NumeroLED;
                break;
            case(ColorLED):
                enviarUnDato("\n -Elegir el color del LED:\n0-Blanco\n1-Rojo\n2-Azul\n3-Verde\n4-Negro");
                esperandoEntrada();
                colorLED = atoi(buffer);
                if (colorLED >= 0 && colorLED <= 4) {
                    estadoLED = TiempoEncendidoLED;
                } else {
                    estadoLED = ErrorColor;
                }
                break;
            case(ErrorColor):
                enviarUnDato("\n -Lo siento, el color indicado NO es v�lido.\n ");
                estadoLED = ColorLED;
                break;
            case(TiempoEncendidoLED):
                enviarUnDato("\n -Elegir en cuanto tiempo se modificar� el LED, en segundos:\n");
                esperandoEntrada(); 
                tEncendidoLED = atoi(buffer);
                if (tEncendidoLED > 0) {
                    estadoLED = NuevoEvento;
                } else {
                    enviarUnDato("\n -Lo siento, el tiempo indicado NO es v�lido.\n ");
                    estadoLED = ErrorTiempoEncendido;
                }
                break;
            case(NuevoEvento):
                ;
                app_register_t *ledNuevo = (app_register_t*) malloc(sizeof (app_register_t));
                ledNuevo->led = nroLED;
                ledNuevo->color = colorLED;
                ledNuevo->tiempoEncendido = tEncendidoLED;
                ledNuevo->ledModificado = true;
                handlerDelTimer = xTimerCreate("timerLED", pdMS_TO_TICKS(ledNuevo->tiempoEncendido * 1000), pdFALSE, (void*) ledNuevo, timerCallBack);
                xTimerStart(handlerDelTimer, 20);
                xTimerReset(handlerDelTimer, (TickType_t) 20);
                enviarUnDato("\n -Nuevo evento configurado.\n ");
                estadoLED = NumeroLED;
                return true;
        }
    }
}



/** 
  @Function
 void consultarEstado()

  @Summary
 Consulta el estado del �ltimo LED modificado.

  @Description
 Si no se modific� ningun LED, muestra eso en el Hercules, en caso contrario, utilizando un sem�foro, accede al recurso compartido
 * y muestra los datos del �ltimo LED modificado.
    
 */
void consultarEstado() {
    static char salida[100]; 
    static char color [35]; 
    xSemaphoreTake(semaforo, 20);
    if (ledConsultaEstado.ledModificado) {
        time_t fechaTimeT = (time_t) ledConsultaEstado.time;
        struct tm *fechaDeCambio;
        fechaDeCambio = gmtime(&fechaTimeT); 
        if (fechaDeCambio->tm_year == 206 && fechaDeCambio->tm_mon == 1 && fechaDeCambio->tm_mday == 7) //fecha de default
        {
            fechaDeCambio->tm_year = 0;
            fechaDeCambio->tm_mday = 1;
            fechaDeCambio->tm_mon = 0;
            fechaDeCambio->tm_hour = 0;
            fechaDeCambio->tm_min = 0;
            fechaDeCambio->tm_sec = 0;
        }
        strftime(salida, sizeof (salida), "\n\n -Fecha de modificaci�n: %Ec", fechaDeCambio);
        if (ledConsultaEstado.color == 0) {
            sprintf(color, "\n -Led nro: %u, color: Blanco\n\n", ledConsultaEstado.led, ledConsultaEstado.color);
        }
        if (ledConsultaEstado.color == 1) {
            sprintf(color, "\n -Led nro: %u, color: Rojo\n\n", ledConsultaEstado.led, ledConsultaEstado.color);
        }
        if (ledConsultaEstado.color == 2) {
            sprintf(color, "\n -Led nro: %u, color: Azul\n\n", ledConsultaEstado.led, ledConsultaEstado.color);
        }
        if (ledConsultaEstado.color == 3) {
            sprintf(color, "\n -Led nro: %u, color: Verde\n\n", ledConsultaEstado.led, ledConsultaEstado.color);
        }
        if (ledConsultaEstado.color == 4) {
            sprintf(color, "\n -Led nro: %u, color: Negro\n\n", ledConsultaEstado.led, ledConsultaEstado.color);
        }
        strcat(salida, color);
        xSemaphoreGive(semaforo);
        enviarUnDato(salida);
    } else {
        enviarUnDato("\n -Ningun led fue modificado a�n.\n\n ");
    }
}


/** 
  @Function
 void interfaz2()

  @Summary
 Funci�n principal de la interfaz. 

  @Description
 * Controla el men� y redirige a la funci�n correcta seg�n el par�metro pasado.
    
 */
void interfaz2(void *p_param) {
    static ESTADOS_INTERFAZ estadoIN = Inicial;
    int numMenu;
    crearSemaforo(); //me aseguro que se cree antes de cuaqluier otra task
    while (1) {
        while (!usbListo2()) {
            vTaskDelay(tiempoms);
        }
        switch (estadoIN) {
            case(Inicial):
                esperandoEntrada();
                estadoIN = MostrarMenu;
                break;
            case(MostrarMenu):
                enviarUnDato(menu);
                esperandoEntrada();
                numMenu = atoi(buffer);
                if (numMenu == 1) {
                    configurarHoraSwitch();
                    estadoIN = MostrarMenu;
                    break;
                }

                if (numMenu == 2) {
                    nuevoEvento();
                    estadoIN = MostrarMenu;
                    break;
                }
                if (numMenu == 3) {
                    consultarEstado();
                } else {
                    estadoIN = MostrarMenu;
                }
                break;
        }
    }
}


/* *****************************************************************************
 End of File
 */
