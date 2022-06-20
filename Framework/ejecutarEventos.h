/* ************************************************************************** */
/** 

  @File Name
 ejecutarEventos.h

  @Summary
 Funciones relacionadas al encendido de LEDs.

  @Description
 Están contenidas las funciones que inician la queue, agregan los eventos y los ejecutan.
 */
/* ************************************************************************** */

#ifndef _ejecutar_Eventos_h    /* Guard against multiple inclusion */
#define _ejecutar_Eventos_h


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

#include "../mcc_generated_files/pin_manager.h"
#include "../freeRTOS/include/FreeRTOS.h"
#include "../freeRTOS/include/task.h"
#include"../freeRTOS/include/semphr.h"
#include"../freeRTOS/include/queue.h"

#include"../Framework/interfaz.h" 
#include "../Platform/WS2812.h"


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


  

    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************
    
   
 void ejecutarEvento(void *p_param);
 
 
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif 

/* *****************************************************************************
 End of File
 */
