/*=====[motor paso-paso]===================================================
 * Copyright Sebastian Herrera <sebaherrera152@gmail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
         (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 * Version: 0.0.1
 * Fecha de creacion: 2020/06/10/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef _DEBOUNCETECLA_H_
#define _DEBOUNCETECLA_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "sapi.h"
#include "semphr.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define CANT_TECLAS	4
#define tiempo_antirebote  40      // tiempo de 40 milisegundos


typedef enum {
	estado_up,
	estado_raising,
	estado_down,
	estado_falling,
} mef_tec_estado;

typedef struct
{
	gpioMap_t tecla;			//config
	mef_tec_estado estadoTecla;//variables
	TickType_t tiempo_medido;	//variables
	TickType_t tiempo_down;		//variables
	TickType_t tiempo_up;		//variables
	TickType_t  tiempo_diff;
	uint8_t contfalling  ;		//variables
	uint8_t contrising  ;		//variables
	SemaphoreHandle_t sem_tec_pulsada;  //semaforo para cada tecla pulsada

} tTecla;


tTecla  tecla_config[CANT_TECLAS];


void botonPresionado(tTecla* config);
void botonLiberado(tTecla* config);
void actualizacionTecla(tTecla* config);
void mefbotonError(tTecla* config);
void mefbotonInit(tTecla* config);




/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* _MODULE_NAME_H_ */
