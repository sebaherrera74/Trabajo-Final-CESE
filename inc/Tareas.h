/*=====[motor paso-paso]===================================================
 * Copyright Sebastian Herrera <sebastianh74@hotmail.com.ar.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
         (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 * Version: 0.0.1
 * Fecha de creacion: 2019/10/10*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef _TAREAS_H_
#define _TAREAS_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "semphr.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define LONGITUD_INICIAL   0       //longitud inicial de LOngitud de onda
#define LONGITUD_FINAL    1000     //longitud final    de LOngitud de onda
#define TIEMPO_UP         1000    //tiempo de lectura de pulsacion de tecla si presiono por mas de 1 seg
#define CAMBIO_ESCALA      100    // cambia escala de valor a setear de LOngitud de onda

#define RELACION_LOXCP     10     //define relacion entre longitud de onda y cantidad de pasos del motor a girar

#define MSG_LO_SET   "La longitud de onda es :"
#define  MSG_IN_ENS  "\nDESEA INICIAR ENSAYO S/N ? : \r\n" "S -> PRESIONE TEC1  : \r\n" "N-> Seleccione nuevamente\r\n""La longitud de onda seleccionada es :"

#define MSG_Val_Analg "\n\n Valor de Voltaje es : "
#define MSG_log_Posic "\n Longitud de Onda Posicionada es: "
#define ERROR_CREACION_TAREAS    printf("ERROR AL CREAR TAREA")



/*=====[Prototypes (declarations) of public functions]=======================*/
//--funciones auxiliares--

void terminal_putc(const char c);            //cambiar nombre
void terminal_puts(const char s[]) ;
void terminal_println(const char s[]);
void impresion_LO(void);
char* itoa(int value, char* result, int base) ;


//--TAREAS--

void tarea_tecla( void* taskParmPtr );
void tarea_seteo_LO( void* taskParmPtr );
void tarea_inicio ( void* taskParmPtr );
void msjexuart( void* taskParmPtr );
void tarea_posicionamiento (void* taskParmPtr );
void tarea_lectura_ADC1015(void* taskParmPtr);


/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _MODULE_NAME_H_ */
