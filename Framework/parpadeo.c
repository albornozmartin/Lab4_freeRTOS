/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include "parpadeo.h"

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */



/** 
  @Function
    void parpadeo(void *p_param )

  @Summary
 Función que controla el parpadeo del led rojo.

  @Description
 Controla con delays de freeRTOS el tiempo en el que el led permanece encendido y apagado, de manera no bloqueante.  
 
 */
void parpadeo(void *p_param ){
    while (1){
        LEDA_SetHigh();
        vTaskDelay(tiempoEncendido1);
        LEDA_SetLow();
        vTaskDelay(tiempoApagado1);
    }
}

/* *****************************************************************************
 End of File
 */
