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

#include "funcionesaux.h"
#include "sapi.h"


#define CICLOS 100000

/*=====[Inclusiones de dependencias de funciones privadas]===================*/

//#include "dependencia.h"
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



/*=====[Definiciones de Variables globales privadas]=========================*/



/*=====[Prototipos de funciones privadas]====================================*/
static void rotarMotor_cero(void);


/*=====[Implementaciones de funciones publicas]==============================*/

//Funcion de mensaje de inicio en UART
void msjUart(void){
	uartWriteString(UART_USB,"Driver de Espectrofotometro \r\n" );
	uartWriteString(UART_USB,"Ensayo de Seleccion de Longitud presione TEC1 \r\n");
	uartWriteString(UART_USB,"Ensayo de Barrido de Longitud de onda presione TEC4 \r\n ");
}

bool_t estadoSwitch (void){
	if (!gpioRead(GPIO5)){
		return TRUE;
	}
	return FALSE;
}


//Funcion para inicialziar pines de GPIO de la edu-ciia
void inicializacion_gpio(void){
   gpioConfig( GPIO1, GPIO_OUTPUT );
   gpioConfig( GPIO2, GPIO_OUTPUT );
   gpioConfig( GPIO3, GPIO_OUTPUT );
   gpioConfig( GPIO4, GPIO_OUTPUT );
   gpioConfig( GPIO5, GPIO_INPUT_PULLDOWN ); //Inicializo Pin de gpio08 , como entrada digital para switch
}


//funcion inicial , en caso de falo de energia que me posiciona el motor en la longitud de onda cero
//llega a cero una vez que el switch(GPIO5) se encuentre cerrrado
void posicion_cero(void){

	while(true){
		if (estadoSwitch()){
			gpioWrite(LEDB,ON);
			gpioWrite(LED1,OFF);
            rotarMotor_cero();
		}
		else{
			gpioWrite(LED1,ON);
			gpioWrite(LEDB,OFF);
			//detenerMotor();
			break;
	   }
	}
}


/*=====[Implementaciones de funciones de interrupcion publicas]==============*/


/*=====[Implementaciones de funciones privadas]==============================*/
static void rotarMotor_cero(void){
	int i;

	gpioWrite(GPIO1,1);
	gpioWrite(GPIO2,0);
	gpioWrite(GPIO3,0);
	gpioWrite(GPIO4,0);
	for (i=0;i<CICLOS;i++){
	};

	gpioWrite(GPIO1,0);
	gpioWrite(GPIO2,1);
	gpioWrite(GPIO3,0);
	gpioWrite(GPIO4,0);
	for (i=0;i<CICLOS;i++){
	};

	gpioWrite(GPIO1,0);
	gpioWrite(GPIO2,0);
	gpioWrite(GPIO3,1);
	gpioWrite(GPIO4,0);
	for (i=0;i<CICLOS;i++){
	};

	gpioWrite(GPIO1,0);
	gpioWrite(GPIO2,0);
	gpioWrite(GPIO3,0);
	gpioWrite(GPIO4,1);
	for (i=0;i<CICLOS;i++){
	};
}

