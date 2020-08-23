/*
 * Copyright Sebastian Herrera <sebaherrera152@gmail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
         (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 * Version: 0.0.1
 * Fecha de creacion: 2019/10/10*/

/*=====[Inclusion de su propia cabecera]=====================================*/
#include "Tareas.h"

#include "ADS1015.h"
#include "debouncetecla.h"
#include "debouncetecla.h"
#include "secuencia_giro.h"
#include "sem_queues.h"
#include "ADS1015.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"


DEBUG_PRINT_ENABLE;

int16_t longitudOnda=0;
uint8_t velocidad=10;

/*=====Implementations of public functions]=================================*/
// ------------Implementacion de Tareas----------------

// Implementacion de la tarea update teclas
void tarea_tecla( void* taskParmPtr ){
	/*taskENTER_CRITICAL();
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	fflush( stdout );
	taskEXIT_CRITICAL();*/

	tTecla* config = (tTecla*) taskParmPtr;

	//inicializacion teclas
	int i = 0;
	for (i = 0; i < 4; i++)
		mefbotonInit(&config[i]);

	// Tarea periodica cada 10 ms
	portTickType xPeriodicity =  40/ portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	// ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE) {
		for (i = 0; i < 4; i++)
			actualizacionTecla(&config[i]);  //update de tareas teclas 1 ,2
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

// Tarea de incrementar o decrementar valor longitud onda
void tarea_seteo_LO ( void* taskParmPtr )
{
	/*taskENTER_CRITICAL();
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	fflush( stdout );
	taskEXIT_CRITICAL();*/

	tTecla* config = (tTecla*) taskParmPtr;
	// ---------- REPETIR POR SIEMPRE --------------------------
	while(TRUE) {

		if(xSemaphoreTake(tecla_config[0].sem_tec_pulsada ,0)){
			if (tecla_config[0].tiempo_diff>TIEMPO_UP){
				longitudOnda=longitudOnda+CAMBIO_ESCALA;
			}
			longitudOnda++;

			if  (longitudOnda>LONGITUD_FINAL ){
				longitudOnda=LONGITUD_FINAL ;
			}
			impresion_LO();
		}

		if (xSemaphoreTake(tecla_config[1].sem_tec_pulsada ,0)){
			if (tecla_config[1].tiempo_diff>TIEMPO_UP){
				longitudOnda=longitudOnda-CAMBIO_ESCALA;
			}
			longitudOnda--;
			if (longitudOnda<LONGITUD_INICIAL){
				longitudOnda=LONGITUD_INICIAL;
			}
			impresion_LO();
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

// Tarea inicio de ensayo
void tarea_inicio ( void* taskParmPtr ){
	/*taskENTER_CRITICAL();
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	fflush( stdout );
	taskEXIT_CRITICAL();*/

	tTecla* config = (tTecla*) taskParmPtr;
	uint32_t longitud_onda_seleccionada=0;
	static char uartBuff_ie[5]; //valor de longitud de onda seleccionado para posible inicio solo para impresion
	portTickType xPeriodicity =  100/ portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	while(TRUE) {
		gpioToggle(LEDB);
		if (xSemaphoreTake(tecla_config[2].sem_tec_pulsada ,portMAX_DELAY)){    // presiono tecla 3 para iniciar el ensayo

			longitud_onda_seleccionada=longitudOnda;   // Asigno la longitud de onda a otra varible este valor enviara por uart

			itoa( longitud_onda_seleccionada, uartBuff_ie, 10 ); // base 10 significa decimal

			terminal_println(MSG_IN_ENS);  //encolo msje posible inicio y envio a  tarea de impresion
			terminal_puts(uartBuff_ie);    //encolo longitud de onda seleccionada envio a  tarea de impresion

			if(xSemaphoreTake(tecla_config[2].sem_tec_pulsada  ,5000/ portTICK_RATE_MS)){   //Presiono nuevamente TEC3 para comenzar ensayo
				//gpioToggle(LED2);
				xQueueSend(valorLOselec_queue, &longitud_onda_seleccionada, portMAX_DELAY);
			}

		}
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity ); //revisar esto me parece que no tiene
		//que ser periodica
	}
}

// Tarea posicionamiento de motor de acuerdo a la longitud de onda
void tarea_posicionamiento (void* taskParmPtr ){
	/*taskENTER_CRITICAL();
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	fflush( stdout );
	taskEXIT_CRITICAL();*/
	int cantidadPasos=0;
	int cantidadPasosSP=0;
	int cantidadPasosActual=0;
	uint32_t aux=0;
	static char uartBuff_pos[5];

	inicializar_bobinas();                    //inicializo GPIO como salidas

	while(TRUE){
		if(xQueueReceive(valorLOselec_queue, &aux, portMAX_DELAY)){   //bloqueo hasta que me llegue inicio del ensayo

			cantidadPasosSP=RELACION_LOXCP*aux;                       //longitud de onda convierto a cantidad de pasos

			cantidadPasosActual=cantidadPasosSP-cantidadPasos;

			if (cantidadPasosActual>0){
				cantidadPasos=cantidadPasosActual;
				rotarBobinasCW(velocidad,cantidadPasos);
				cantidadPasos=cantidadPasosSP;
			}

			else {
				cantidadPasos=cantidadPasos-cantidadPasosSP;
				rotarBobinasCCW(velocidad,cantidadPasos);
				cantidadPasos=cantidadPasosSP;
			}
			detenerMotor();           //freno motor

			//creo tarea de lectura ADC one-shot

  BaseType_t res =xTaskCreate(tarea_lectura_ADC1015,                // Funcion de la tarea a ejecutar
					(const char *)"lectura ADC ",   // Nombre de la tarea como String amigable para el usuario
					configMINIMAL_STACK_SIZE*2,      // Cantidad de stack de la tarea
					0,                              // Parametros de tarea
					tskIDLE_PRIORITY+3,              // Prioridad de la tarea
					0);                              // Puntero a la tarea creada en el sistema
                  if (res == pdFAIL) {
                	  ERROR_CREACION_TAREAS;
                   }


            itoa( aux, uartBuff_pos, 10 );
			xSemaphoreTake(mutex_impresion_1,portMAX_DELAY);  //coloco semaforo mutex
			terminal_println(MSG_log_Posic); //envio a tarea de impresion valor de longitud de
			terminal_puts(uartBuff_pos);           //posicionado
			xSemaphoreGive( mutex_impresion_1);

		}

		vTaskDelay(10/portTICK_RATE_MS);
	}
}

// Tarea que envia mensaje por UART0
void msjexuart( void* taskParmPtr )
{
	// ---------- CONFIGURACIONES ------------------------------

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		char c;

		if (xQueueReceive(txQueue, &c, portMAX_DELAY)) {
			uartWriteByte(UART_USB, c);
		}

		vTaskDelay(10/portTICK_RATE_MS);
	}
}

// Tarea one-shot para lectura de conversor ADC
void tarea_lectura_ADC1015(void* taskParmPtr) {

	i2cInit( I2C0, 100000 );
	int16_t adc0, adc1, adc2, adc3=0;
	float voltage_0=0;
	static char result[10];

	//  configuro resolucion

	setGain(GAIN_TWOTHIRDS);    // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)

	adc0 = readADC_SingleEnded(0);
	//tomo lectura de un solo canal por ahora
	//adc1 = readADC_SingleEnded(1);
	//adc2 = readADC_SingleEnded(2);
	//adc3 = readADC_SingleEnded(3);
	//printf("AIN0: %i \r\n ",adc0);

	voltage_0=(adc0*0.003);   //multiplico valor leido por la resolucion seleccionada arriba


	floatToString( voltage_0, result, 8 );  //convierto valor flotante en string para visualizar

	terminal_println(MSG_Val_Analg); //envio a tarea de impresion valor adc capturado
	terminal_puts(result);

	//solo muestro el AIN0 por ahora solo a los fines practicos

	//printf("Valor de voltaje leido es : %s \r\n ",result);
	//printf("AIN1: %i \r\n ",adc1);
	//printf("AIN2: %i \r\n ",adc2);
	//printf("AIN3: %i \r\n ",adc3);

	vTaskDelete(NULL);
}


// funciones auxiliares
void terminal_putc(const char c) {
	xQueueSend(txQueue, &c, portMAX_DELAY);
}

void terminal_puts(const char s[]) {
	xSemaphoreTake(mutex_impresion,portMAX_DELAY);  //coloco semaforo mutex
	for (; *s; s++) {
		terminal_putc(*s);
	}
	terminal_putc('\r');
	terminal_putc('\n');
	xSemaphoreGive( mutex_impresion);
}

void terminal_println(const char s[]) {
	terminal_puts(s);
}

void impresion_LO(void){
	static char uartBuff_id[5];    //valor de longitud de onda seteado incrementado o decrementado con teclas 1 y 2

	itoa( longitudOnda, uartBuff_id, 10 );     // base 10 significa decimal
	terminal_println(MSG_LO_SET);              //encolo msje y envio a impresion
	terminal_puts(uartBuff_id);                //encolo valor y envio a impresion

}

char* itoa(int value, char* result, int base) {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}


