/* ************************************************************************** */
/** Descriptive File Name

  @File Name
 ejecutarEventos.c

  @Summary
 Contiene a las funciones correspondientes a la Tarea C.

  @Description
 Contiene lo correspondiente a la Tarea C, esto es, el manejo de la queue, la ejecucion de los eventos y el manejo del ultimo LED modificado.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include"ejecutarEventos.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*Corresponde al arreglo de los LEDS siendo 8 leds del 0 al 7. */
static ws2812_t arrayLEDS[8];
/*Corresponde al handler para el manejo de la queue utilizando las herramientas de freeRTOS. */
QueueHandle_t colaEventos;
/*Este es el recurso compartido controlado por el MUTEX, corresponde a una variable external. */
app_register_t ledRecibido;
/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */


/* ************************************************************************** */

/** 
  @Function
 static void EncenderLED(app_register_t unLED)

  @Summary
 Enciende el LED pasado por parámetro.

  @Description
 Recibe unLed chequea su color y actualiza el arreglo de LEDs con el color correspondiente, hecho esto, llama a la función  WS2812_send(arrayLEDS, 8)
 * que es la que efectivamente enciende el LED.

 */

static void encenderLED(app_register_t unLed) {
    if (unLed.color == 0) {
        arrayLEDS[unLed.led] = WHITE;
    }
    if (unLed.color == 1) {
        arrayLEDS[unLed.led] = RED;
    }
    if (unLed.color == 2) {
        arrayLEDS[unLed.led] = BLUE;
    }
    if (unLed.color == 3) {
        arrayLEDS[unLed.led] = GREEN;
    }
    if (unLed.color == 4) {
        arrayLEDS[unLed.led] = BLACK;
    }
    WS2812_send(arrayLEDS, 8);
}

/** 
  @Function
 void crearCola()

  @Summary
 Crea la queue.

  @Description
 Crea la queue que se utiliza para almacenar los eventos a ejecutarse.
 */
void crearCola() {
    colaEventos = xQueueCreate(10, sizeof ( app_register_t));
    if (colaEventos == NULL) {
        /* Queue was not created and must not be used. */
    }

}
/** 
  @Function
 void agregarEvento (app_register_t eventoNuevo)

  @Summary
 Agrega eventoNuevo a la queue.

 */

void agregarEvento(app_register_t eventoNuevo) {
    if (colaEventos != 0) {
        /* Send an unsigned long.  Wait for 10 ticks for space to become
        available if necessary. */
        if (xQueueSendToBack(colaEventos, &eventoNuevo, (TickType_t) 10) != pdPASS) {
            /* Failed to post the message, even after 10 ticks. */
        }
    }
}


        

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/************************************************************************

/** 
  @Function
 void ejecutarEvento (void *p_param)

  @Summary
 Ejecuta los eventos de la queue.

  @Description
 Si recibe un evento de la cola, entonces lo ejecuta. Luego de esto actualiza el último LED modificado, por tanto, 
 * utiliza el MUTEX para acceder al recurso compartido.
 */

void ejecutarEvento(void *p_param) {
    //lo hace una vez sola.
    static ws2812_t limpiarLEDS[8];
    WS2812_send(limpiarLEDS, 8);
    crearCola();
    
    while (1) {
        if (colaEventos != NULL) {
            if (xQueueReceive(colaEventos,
                    &(ledRecibido),
                    (TickType_t) 10) == pdPASS) {
            } else {
                vTaskDelay(50);
            }
        }
        struct tm fechaConsulta;
    xSemaphoreTake(semaforo, 20);  
                if (ledRecibido.ledModificado) {
                    encenderLED(ledRecibido);
                    ledConsultaEstado.color = ledRecibido.color;
                    ledConsultaEstado.led = ledRecibido.led;
                    RTCC_TimeGet(&fechaConsulta);
                    ledConsultaEstado.time = (uint32_t) mktime(&fechaConsulta);
                    ledConsultaEstado.ledModificado = true;
                    xSemaphoreGive(semaforo);
                }
                vTaskDelay(50);
    }
}

/* *****************************************************************************
 End of File
 */
