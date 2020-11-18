
/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "debouncetecla.h"
#include "funcionesaux.h"
#include "sem_queues.h"
#include "Tareas.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "sapi.h"

DEBUG_PRINT_ENABLE;
void teclas_config(void);

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void)
{
	int Error_creacion_Colas=0;
	// ---------- CONFIGURACIONES ------------------------------
	// Inicializar y configurar la plataforma
	boardConfig();
	uartConfig( UART_USB, 115200 );
	msjUart();
	// Led para dar señal de vida
	gpioWrite( LED3, ON );
	teclas_config();          // configura teclas para debounce
	inicializacion_gpio();    //Inicializo los pines GPIO a utilizar
	delayInaccurateMs(100);   //Puse este delay porque sino no me tomaba la funcion de posicion cero
	posicion_cero();          //funcion bloqueante, posiciona el motor al arranque en la longitud de onda inicial

	/* funcion que crea semaforos y colas a utilizar */
	Error_creacion_Colas=sem_queues_init();

	//CREACION DE TAREAS EN  freeRTOS

	//-------------Tarea update de tecla 1,2 para cambiar valor de longitud de onda--------------

	BaseType_t res1 =xTaskCreate(tarea_tecla,                     // Funcion de la tarea a ejecutar
			(const char *)"update Teclas",   // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,      // Cantidad de stack de la tarea
			tecla_config,                    // Parametros de tarea
			tskIDLE_PRIORITY+1,              // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema


	//----------Tarea que incrementara o decrementara el valor de longitud de onda para el ensayo
	BaseType_t res2 =xTaskCreate(tarea_seteo_LO,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea setea LO ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			tecla_config,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema


	//----------Tarea inicio de ensayo en longitud de onda determinada-----------------------------
	BaseType_t res3 =xTaskCreate(tarea_inicio,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea Inicio Ensayo ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			tecla_config,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema


	//-------------Tarea de informe de situacion por uart  -----------
	BaseType_t res4 =xTaskCreate(msjexuart,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea Msje x Uart ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			0,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema


	//-------------Tarea de posicionar el motor de acuerdo a la longitud seleccionada-----------
	BaseType_t res5 =xTaskCreate(tarea_posicionamiento,                  // Funcion de la tarea a ejecutar
			(const char *)"tarea Posicionar ",    // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
			0,                                // Parametros de tarea
			tskIDLE_PRIORITY+1,               // Prioridad de la tarea
			0);                              // Puntero a la tarea creada en el sistema

	//-------------Tarea de Ensayo de barrido de longitud seleccionada-----------
	BaseType_t res6 =xTaskCreate(tarea_barrido,    // Funcion de la tarea a ejecutar
				(const char *)"tarea Barrido ",    // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE*2,        // Cantidad de stack de la tarea
				0,                                // Parametros de tarea
				tskIDLE_PRIORITY+1,               // Prioridad de la tarea
				0);

	//-------------Tarea de lectura del ADC-----------
	BaseType_t res7 =xTaskCreate(tarea_lectura_ADC1015,                // Funcion de la tarea a ejecutar
				(const char *)"lectura ADC ",   // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE*2,      // Cantidad de stack de la tarea
				0,                              // Parametros de tarea
				tskIDLE_PRIORITY+3,              // Prioridad de la tarea
				0);                              // Puntero a la tarea creada en el sistema






	/*Chequeo de errores e la creacion de las tareas */

	if(res1 == pdFAIL || res2 == pdFAIL || res3 == pdFAIL || res4 == pdFAIL || res5==pdFAIL || res6==pdFAIL ||res7==pdFAIL)
	{
		//gpioWrite( LED1, ON );
		printf( "Error al crear las tareas." );
		while(TRUE)                              //Entro en un lazo cerrado
		{
		}
	}

	//Inicia Scheduler y chequeo para activar el scheduler que las colas y semaforos se hayan
	//creado correctamente

	if (Error_creacion_Colas==0)
	{
	  vTaskStartScheduler();
	}else
	  {
	    printf("Error al iniciar el sistema !!!");
	   }


	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE ) {

		// Si cae en este while 1 significa que no pudo iniciar el scheduler
	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}

void teclas_config(void)
{
	tecla_config[0].tecla= TEC2;
	tecla_config[0].sem_tec_pulsada	= xSemaphoreCreateBinary();

	tecla_config[1].tecla= TEC3;
	tecla_config[1].sem_tec_pulsada	= xSemaphoreCreateBinary();

	tecla_config[2].tecla= TEC1;
	tecla_config[2].sem_tec_pulsada	= xSemaphoreCreateBinary();

	tecla_config[3].tecla= TEC4;
	tecla_config[3].sem_tec_pulsada = xSemaphoreCreateBinary();
}





