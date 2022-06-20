/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _interfaz_h    /* Guard against multiple inclusion */
#define _interfaz_h


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */


#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#include "../mcc_generated_files/usb/usb_device.h"
#include "../mcc_generated_files/usb/usb.h"
#include "../mcc_generated_files/usb/usb_device_cdc.h"
#include "../mcc_generated_files/rtcc.h"
#include "../Platform/WS2812.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../freeRTOS/include/FreeRTOS.h"
#include "../freeRTOS/include/task.h"
#include"../freeRTOS/include/timers.h"
#include "../freeRTOS/include/semphr.h"  
#include"ejecutarEventos.h"



/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

    
// buffer donde se almacena los datos obtenidos del USB.
static uint8_t buffer[255];

//Semáforo que se utiliza para controlar el acceso al último led modificado.
extern SemaphoreHandle_t semaforo;


        /* ************************************************************************** */
    /** Menú

      @Summary
     Constante con el menú a mostrar.
     */

static char menu[] = "MENU:\n -Para fijar fecha y hora presiona 1\n -Para encender y apagar un LED presiona 2 \n -Para consultar el estado del último LED presiona 3\n";



    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
 
typedef struct {
   /* Estructura para controlar los datos de los LED */
uint8_t led;
/* Entero que corresponde al número del LED, entre 0 y 7. */
uint8_t color;     
/* Entero que corresponde al número del color del LED, 0-Blanco 1-Rojo 2-Azul 3-Verde 4-Negro. */
uint32_t time;
/* Entero que corresponde a la fecha en que fue modificado. */
bool ledModificado;
/* Booleano que indica si el LED fue modificado o no.*/
  uint32_t tiempoEncendido; 
  /* Indica el tiempo que debe pasar antes de que el LED se encienda..*/
} app_register_t;
  

//LED utilizado en tarea B y tarea C
extern  app_register_t ledConsultaEstado;



typedef enum {
    /*  */
    IngresarFecha,
    /* Estado que muestra el cartel de ingreso de fecha. */
    ErrorFecha,
    /* Estado que espera por una fecha. */
    IngresarHora,
    /* Estado que muestra el cartel para ingresar una hora.. */
    ErrorHora,
    /* Estado que espera por una hora. */
    ConfigurarRTCC,
} ESTADOS_CONFIG_FECHA;



typedef enum {
    /*  */
    NumeroLED,
    /* Estado que pide que LED debe encenderse. */
    ErrorNumero,
    /* Estado cuando se recibe un LED no válido. */
    ColorLED,
    /* Estado que espera por el color del LED a encender. */
    ErrorColor,
    /* Estado cuando se recibe un color no válido. */
    TiempoEncendidoLED,
    /* Estado que espera por el tiempo que se debe esperar antes de enceder el LED. */
    ErrorTiempoEncendido,
    /* Estado cuando se recibe un tiempo no válido. */
    NuevoEvento,
    /* Estado donde se configura el evento nuevo. */
} ESTADOS_EVENTO;

typedef enum {
    /*  */
    Inicial,
            /* Estado inicial de la interfaz, donde espera algun caracter para mostrar el menú. */
    MostrarMenu,
    /* Estado que muestra el menú. */
} ESTADOS_INTERFAZ;
    // *****************************************************************************



    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************


/** 
  @Function
 static bool nuevoEvento()

  @Summary
 Configura un nuevo Evento.

  @Description
 Pide y valida los parámetros para encender un LED, configura el timer para enviar el evento luego de expirado el tiempo.
    
 */
static bool nuevoEvento(void);


   /** 
  @Function
 void consultarEstado()

  @Summary
 Consulta el estado del último LED modificado.

  @Description
 Si no se modificó ningun LED, muestra eso en el Hercules, en caso contrario, utilizando un semáforo, accede al recurso compartido
 * y muestra los datos del último LED modificado.
    
 */
void consultarEstado(void);

   
    /**
      @Function
        void interfaz2(void)

      @Summary
      Función principal de la interfaz. 

      @Description
        Controla el menú y redirige a la función correcta según el parámetro pasado.
 
     */
void interfaz2 (void *p_param);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
