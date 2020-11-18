/*=====[Nombre del modulo]=====================================================
 * Copyright YYYY Nombre completo del autor <author@mail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
         (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.0
 * Fecha de creacion: YYYY/MM/DD
 */

/*=====[Inclusion de su propia cabecera]=====================================*/
#include "secuencia_giro.h"
#include "FreeRTOSConfig.h"
#include "Tareas.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"
#include "task.h"
#include "sem_queues.h"
//#include <dependencia.h>

/*=====[Macros de definicion de constantes privadas]=========================*/



/*=====[Macros estilo funcion privadas]======================================*/



/*=====[Definiciones de tipos de datos privados]=============================*/

// Tipo de datos que renombra un tipo basico
// Tipo de datos de puntero a funcion

// Tipo de datos enumerado

// Tipo de datos estructua, union o campo de bits

/*=====[Definiciones de Variables globales publicas externas]================*/

/*=====[Definiciones de Variables globales publicas]=========================*/

//Secuencia de paso simple
uint8_t secuenciaPasosSimple[4][4]={
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
};
//Seceuncia de paso normal
uint8_t secuenciaPasosNormal[4][4]={
		{1,1,0,0},
		{0,1,1,0},
		{0,0,1,1},
		{1,0,0,1}
};
//Secuencia de medio paso
uint8_t secuenciaPasosMediopaso[8][4]={
		{1,0,0,0},
		{1,1,0,0},
		{0,1,0,0},
		{0,1,1,0},
		{0,0,1,0},
		{0,0,1,1},
		{0,0,0,1},
		{1,0,0,1}
};

/*=====[Definiciones de Variables globales privadas]=========================*/

/*=====[Prototipos de funciones privadas]====================================*/
//static void setSentidoGiro(void);

/*=====[Implementaciones de funciones publicas]==============================*/
void inicializar_bobinas(){
	gpioConfig( GPIO1, GPIO_OUTPUT );
	gpioConfig( GPIO2, GPIO_OUTPUT );
	gpioConfig( GPIO3, GPIO_OUTPUT );
	gpioConfig( GPIO4, GPIO_OUTPUT );

}


//Secuencia en sentido de las agujas del reloj
void rotarBobinasCW(uint8_t velocidad,uint16_t cantPasos){

	uint8_t aux=0;
	uint16_t i;
	for (i=cantPasos;i>0;i--){
		aux=(i+4)%4;
		gpioWrite(GPIO1,secuenciaPasosSimple[aux][0]);
		gpioWrite(GPIO2,secuenciaPasosSimple[aux][1]);
		gpioWrite(GPIO3,secuenciaPasosSimple[aux][2]);
		gpioWrite(GPIO4,secuenciaPasosSimple[aux][3]);
		vTaskDelay(velocidad);

	}
}

//Seceuncia en sentido contrario a las agujas del reloj
void rotarBobinasCCW(uint8_t velocidad,uint16_t cantPasos){
	uint8_t aux=0;
	uint16_t i;

	for (i=0;i<cantPasos;i++){
		aux=(i+4)%4;

		gpioWrite(GPIO1,secuenciaPasosSimple[aux][0]);
		gpioWrite(GPIO2,secuenciaPasosSimple[aux][1]);
		gpioWrite(GPIO3,secuenciaPasosSimple[aux][2]);
		gpioWrite(GPIO4,secuenciaPasosSimple[aux][3]);
		vTaskDelay(velocidad);
	}
}


//Secuencia de giro con un semaforo para disparar adquisicion de datos adc
void rotarBobinasCW_barrido(uint8_t velocidad,uint16_t cantPasos){
	uint8_t aux=0;
	uint16_t i;
	for (i=cantPasos;i>0;i--){
		aux=(i+4)%4;
		gpioWrite(GPIO1,secuenciaPasosSimple[aux][0]);
		gpioWrite(GPIO2,secuenciaPasosSimple[aux][1]);
		gpioWrite(GPIO3,secuenciaPasosSimple[aux][2]);
		gpioWrite(GPIO4,secuenciaPasosSimple[aux][3]);
		vTaskDelay(velocidad);
		xSemaphoreGive(sem_inicio);
	}
}

void detenerMotor(void){
	gpioWrite(GPIO1,0);
	gpioWrite(GPIO2,0);
	gpioWrite(GPIO3,0);
	gpioWrite(GPIO4,0);

}

