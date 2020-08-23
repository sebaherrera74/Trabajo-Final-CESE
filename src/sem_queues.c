
#include "sem_queues.h"

#include "sapi.h"


int sem_queues_init(void)
{
	//----Creacion de Colas----

   valorLO_queue     =xQueueCreate( 1, sizeof(int16_t) );   //cola para pasar valor de longitud de onda
   valorLOselec_queue=xQueueCreate( 1, sizeof(int16_t) );	//cola para pasar valor de longitud de onda seleccionada
   valorAnLeido      =xQueueCreate( 1, sizeof(char[10]) );
   txQueue           =xQueueCreate(128, sizeof(char));      //cola para enviar datos por la UART

   if((valorLO_queue==NULL)||(valorLOselec_queue==NULL)||(valorAnLeido==NULL)||(txQueue==NULL) )//chequeo que se hallan creado bien las colas
	{
	  printf ("Error al crear colas");
	  return 1;
	}

   mutex_impresion	=  xSemaphoreCreateMutex();
   mutex_impresion_1=  xSemaphoreCreateMutex();

   if((mutex_impresion==NULL)||(mutex_impresion_1==NULL) )
     {
      printf ("Error al crear Semaforos");
      return 1;
     }
   return 0;
}


