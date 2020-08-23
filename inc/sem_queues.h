
#include "FreeRTOS.h"
#include "semphr.h"
#include "sapi.h"


#ifndef _SEM_QUEUES_H_
#define _SEM_QUEUES_H_



//SemaphoreHandle_t fin_posicion_motor;

SemaphoreHandle_t mutex_impresion;
SemaphoreHandle_t mutex_impresion_1;


QueueHandle_t valorLO_queue;
QueueHandle_t valorLOselec_queue;
QueueHandle_t valorAnLeido;
QueueHandle_t txQueue;

void sem_queues_init(void);





#endif
