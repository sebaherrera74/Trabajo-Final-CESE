/*
 * Copyright Sebastian Herrera <sebastianh74@hotmail.com.ar.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
         (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 * Version: 0.0.1
 * Fecha de creacion: 2019/10/10*/

/*=====[Inclusion de su propia cabecera]=====================================*/
#include "debouncetecla.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
DEBUG_PRINT_ENABLE;


/*=====[Macros de definicion de constantes privadas]=========================*/


/*=====[Prototipos de funciones privadas]====================================*/


// accion de el evento de tecla pulsada
void botonPresionado(tTecla* config){
	config->tiempo_down=xTaskGetTickCount();

}

void botonLiberado(tTecla* config ){
	config->tiempo_up=xTaskGetTickCount();
	config->tiempo_diff=config->tiempo_up-config->tiempo_down;
	if (config->tiempo_diff > 0)
			xSemaphoreGive( config->sem_tec_pulsada );
}

void mefbotonInit(tTecla* config) {
	config->contfalling = 0;
	config->contrising = 0;
	config->estadoTecla=estado_up; //inicializo el estado de teclas**/


}

void mefbotonError(tTecla* config){
	config->estadoTecla=estado_up;
}


void actualizacionTecla(tTecla* config){

	switch (config->estadoTecla){

	case estado_up:

		if (!gpioRead(config->tecla)){
			config->estadoTecla=estado_falling;
		}
		break;

	case estado_falling :
		if (config->contfalling <=TIEMPO_ANTIREBOTE){
			if (!gpioRead(config->tecla)){
				config->estadoTecla=estado_down;
				/*accion del evento*/

				botonPresionado(config);
			}
			else{
				config->estadoTecla=estado_up;
			}
			config->contfalling=0;
		}
		config->contfalling++;
		break;

	case estado_down :
		if (gpioRead(config->tecla)){
			config->estadoTecla=estado_raising;

		}
		break;

	case estado_raising:
		if (config->contrising<=TIEMPO_ANTIREBOTE){
			if (gpioRead(config->tecla)){
				config->estadoTecla=estado_up;
				/*accion del evento*/
				botonLiberado(config);
			}
			else{
				config->estadoTecla=estado_down;
			}
			config->contrising=0;
		}
		config->contrising++;
		break;

	default:
		mefbotonError(config);

	}
}
