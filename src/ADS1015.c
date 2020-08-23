/*==============================================================================
 * Program: ADS1015
 * Date: 2020/05
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "ADS1015.h"

#include "FreeRTOSConfig.h"
#include "Tareas.h"
#include "sapi.h"


/*----- Funcion de seleccion de ganancia------ */
void setGain(adsGain_t gain){
	m_gain=gain;
}


/*-------Medicion analogica de un canal ADC conectado a GND---------*/
//parametro de la funcion Canal analogico de entrada
//retorna el valor ADC leido
uint16_t readADC_SingleEnded(uint8_t channel) {

	bool_t retVal = TRUE; // True if OK

	if (channel > 3) {
		return 0;
	}

	// Start with default values
	uint16_t config =0;
	config=
			ADS1015_REG_CONFIG_CQUE_NONE |     // Disable the comparator (default val)
			ADS1015_REG_CONFIG_CLAT_NONLAT |   // Non-latching (default val)
			ADS1015_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
			ADS1015_REG_CONFIG_CMODE_TRAD |    // Traditional comparator (default val)
			ADS1015_REG_CONFIG_DR_1600SPS |    // 1600 samples per second (default)
			ADS1015_REG_CONFIG_MODE_SINGLE;    // Single-shot mode (default)
	//hasta aqui config=0x0183 de acuerdo a lo calculado anteriormente

	// Set PGA/voltage range
	config |= m_gain;

	//de acuerdo a lo que seleccion aqui m_gain me parametriza el valor de config
	//si selecciono m_gain=0x0000 entonces config =0x0183

	// Set single-ended input channel
	switch (channel) {
	case (0):
    		config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;  //0x4183
	break;
	case (1):
    		config |= ADS1015_REG_CONFIG_MUX_SINGLE_1; //0x5183
	break;
	case (2):
    		config |= ADS1015_REG_CONFIG_MUX_SINGLE_2; //0x6183
	break;
	case (3):
    		config |= ADS1015_REG_CONFIG_MUX_SINGLE_3; //0x7183
	break;
	}

	//canal 0 -> config =0x4183
	//canal 1 -> config=0x5183
	//canal 2 -> config=0x6183
	//canal 3 -> config=0x7183

	// Set 'start single-conversion' bit
	config |= ADS1015_REG_CONFIG_OS_SINGLE;

	// canal 0 -> config =0xC183
	// canal 1  0xD183
	// canal 2   0xE183
	// canal3    0xF183

	uint8_t transmitDataBuffer[3]={ADS1015_REG_POINTER_CONFIG,config>>8,config & 0xFF};  //config>>8 me da la parte alta de config,config&0XFF la parte baja
	uint8_t dataToReadBuffer[1]={ ADS1015_REG_POINTER_CONVERT};
	uint8_t receiveDataBuffer[2]={ADS1015_REG_POINTER_CONVERT,ADS1015_REG_POINTER_CONVERT};

	retVal=i2cWrite(I2C0,ADS1015_ADDRESS,transmitDataBuffer,3,TRUE);
	vTaskDelay(ADS1015_CONVERSIONDELAY);

	retVal=i2cRead(I2C0,ADS1015_ADDRESS,dataToReadBuffer,1,1,receiveDataBuffer,2,1) ;
	vTaskDelay(ADS1015_CONVERSIONDELAY);
	if (retVal==TRUE){

		//me devuelve la lectura del registro de conversion pero hay que hacerle un desplazamiento a la
		//derecha 4 bits porque los 3 primeros son reservados

		return ((receiveDataBuffer[0]<<8) |(receiveDataBuffer[1]))>>4;
	}

	else return 0;  //devuelve 0 si no logra leer el i2c
}

/*-----------Funcion que lee la diferencia entre entrada 0 y 1 ----------*/
//Lee los resultados de conversión, midiendo la diferencia de voltaje entre
//la entrada P (AIN0) y N (AIN1). Genera un valor con signo ya que la diferencia
//puede ser positiva o negativa.

int16_t readADC_Differential_0_1() {
	uint16_t valorLeido;
	bool_t retVal = TRUE; // True if OK


  // Start with default values
  uint16_t config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1; // AIN0 = P, AIN1 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  uint8_t transmitDataBuffer[3]={ADS1015_REG_POINTER_CONFIG,config>>8,config & 0xFF};  //config>>8 me da la parte alta de config,config&0XFF la parte baja
  uint8_t dataToReadBuffer[1]={ ADS1015_REG_POINTER_CONVERT};
  uint8_t receiveDataBuffer[2]={ADS1015_REG_POINTER_CONVERT,ADS1015_REG_POINTER_CONVERT};

  // Write config register to the ADC
  retVal=i2cWrite(I2C0,ADS1015_ADDRESS,transmitDataBuffer,3,TRUE);

  // Wait for the conversion to complete
  delay(ADS1015_CONVERSIONDELAY);

  // Read the conversion results
  retVal=i2cRead(I2C0,ADS1015_ADDRESS,dataToReadBuffer,1,1,receiveDataBuffer,2,1) ;
  	delay(ADS1015_CONVERSIONDELAY);
  	if (retVal==TRUE){
  		//me devuelve la lectura del registro de conversion pero hay que hacerle un desplazamiento a la
  		//derecha 4 bits porque los 3 primeros son reservados

  		valorLeido=((receiveDataBuffer[0]<<8) |(receiveDataBuffer[1]))>>4;

  		if (valorLeido>0x07FF){
  			valorLeido|=0xF000;
  		}

 		return (int16_t) valorLeido;
  	}


}
