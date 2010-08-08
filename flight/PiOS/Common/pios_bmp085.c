/**
  ******************************************************************************
  * @addtogroup PIOS PIOS Core hardware abstraction layer
  * @{
  * @addtogroup PIOS_BMP085 BMP085 Functions
  * @brief Hardware functions to deal with the altitude pressure sensor
  * @{
  *
  * @file       pios_bmp085.c  
  * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
  * @brief      BMP085 Pressure Sensor Routines
  * @see        The GNU Public License (GPL) Version 3
  *
  ******************************************************************************/
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* Project Includes */
#include "pios.h"

#if defined(PIOS_INCLUDE_BMP085)
#if !defined(PIOS_INCLUDE_EXTI)
#error PIOS_EXTI Must be included in the project!
#endif

/* Glocal Variables */
ConversionTypeTypeDef CurrentRead;
xSemaphoreHandle PIOS_BMP085_EOC;

/* Local Variables */
static BMP085CalibDataTypeDef CalibData;

/* Straight from the datasheet */
static int32_t X1, X2, X3, B3, B5, B6, P;
static uint32_t B4, B7;
static volatile uint16_t RawTemperature;
static volatile uint32_t RawPressure;
static volatile uint32_t Pressure;
static volatile uint16_t Temperature;


/**
* Initialise the BMP085 sensor
*/
void PIOS_BMP085_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Semaphore used by ISR to signal End-Of-Conversion */
	vSemaphoreCreateBinary(PIOS_BMP085_EOC);
	/* Must start off empty so that first transfer waits for EOC */
	xSemaphoreTake(PIOS_BMP085_EOC, portMAX_DELAY);

	/* Enable EOC GPIO clock */
	RCC_APB2PeriphClockCmd(PIOS_BMP085_EOC_CLK | RCC_APB2Periph_AFIO, ENABLE);

	/* Configure EOC pin as input floating */
	GPIO_InitStructure.GPIO_Pin = PIOS_BMP085_EOC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PIOS_BMP085_EOC_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure the End Of Conversion (EOC) interrupt */
	GPIO_EXTILineConfig(PIOS_BMP085_EOC_PORT_SOURCE, PIOS_BMP085_EOC_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = PIOS_BMP085_EOC_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable and set EOC EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = PIOS_BMP085_EOC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PIOS_BMP085_EOC_PRIO;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Read all 22 bytes of calibration data in one transfer, this is a very optimised way of doing things */
	uint8_t Data[BMP085_CALIB_LEN];
	PIOS_BMP085_Read(BMP085_CALIB_ADDR, Data, BMP085_CALIB_LEN);
	
	/* Parameters AC1-AC6 */
	CalibData.AC1 = (Data[0] << 8) | Data[1];
	CalibData.AC2 = (Data[2] << 8) | Data[3];
	CalibData.AC3 = (Data[4] << 8) | Data[5];
	CalibData.AC4 = (Data[6] << 8) | Data[7];
	CalibData.AC5 = (Data[8] << 8) | Data[9];
	CalibData.AC6 = (Data[10] << 8) | Data[11];

	/* Parameters B1, B2 */
	CalibData.B1 =  (Data[12] << 8) | Data[13];
	CalibData.B2 =  (Data[14] << 8) | Data[15];

	/* Parameters MB, MC, MD */
	CalibData.MB =  (Data[16] << 8) | Data[17];
	CalibData.MC =  (Data[18] << 8) | Data[19];
	CalibData.MD =  (Data[20] << 8) | Data[21];
}


/**
* Start the ADC conversion
* \param[in] PresOrTemp BMP085_PRES_ADDR or BMP085_TEMP_ADDR
* \return Raw ADC value
*/
void PIOS_BMP085_StartADC(ConversionTypeTypeDef Type)
{
	/* Start the conversion */
	if(Type == TemperatureConv) {
		PIOS_BMP085_Write(BMP085_CTRL_ADDR, BMP085_TEMP_ADDR);
	} else if(Type == PressureConv) {
		PIOS_BMP085_Write(BMP085_CTRL_ADDR, BMP085_PRES_ADDR);
	}
	
	CurrentRead = Type;
}


/**
* Read the ADC conversion value (once ADC conversion has completed)
* \param[in] PresOrTemp BMP085_PRES_ADDR or BMP085_TEMP_ADDR
* \return Raw ADC value
*/
void PIOS_BMP085_ReadADC(void)
{
	uint8_t Data[3];
	Data[0] = 0;
	Data[1] = 0;
	Data[2] = 0;
	
	/* Read and store the 16bit result */
	if(CurrentRead == TemperatureConv) {
		/* Read the temperature conversion */
		PIOS_BMP085_Read(BMP085_ADC_MSB, Data, 3); /* Read 3 since it is more reliable on i2c than the proper read of 2 bytes. */
		RawTemperature = ((Data[0] << 8) | Data[1]);

		X1 = (RawTemperature - CalibData.AC6) * CalibData.AC5 >> 15;
		X2 = ((int32_t)CalibData.MC << 11) / (X1 + CalibData.MD);
		B5 = X1 + X2;
		Temperature = (B5 + 8) >> 4;
	} else {
		/* Read the pressure conversion */
		PIOS_BMP085_Read(BMP085_ADC_MSB, Data, 3);
		RawPressure = ((Data[0] << 16) | (Data[1] << 8) | Data[2]) >> (8 - BMP085_OVERSAMPLING);

		B6 = B5 - 4000;
		X1 = (CalibData.B2 * (B6 * B6 >> 12)) >> 11;
		X2 = CalibData.AC2 * B6 >> 11;
		X3 = X1 + X2;
		B3 = ((((int32_t)CalibData.AC1 * 4 + X3) << BMP085_OVERSAMPLING) + 2) >> 2;
		X1 = CalibData.AC3 * B6 >> 13;
		X2 = (CalibData.B1 * (B6 * B6 >> 12)) >> 16;
		X3 = ((X1 + X2) + 2) >> 2;
		B4 = (CalibData.AC4 * (uint32_t) (X3 + 32768)) >> 15;
		B7 = ((uint32_t)RawPressure - B3) * (50000 >> BMP085_OVERSAMPLING);
		P = B7 < 0x80000000 ? (B7 * 2) / B4 : (B7 / B4) * 2;

		X1 = (P >> 8) * (P >> 8);
		X1 = (X1 * 3038) >> 16;
		X2 = (-7357 * P) >> 16;
		Pressure = P + ((X1 + X2 + 3791) >> 4);
	}
}

int16_t PIOS_BMP085_GetTemperature(void)
{
	return Temperature;
}

int32_t PIOS_BMP085_GetPressure(void)
{
	return Pressure;
}

/**
* Reads one or more bytes into a buffer
* \param[in] address BMP085 register address (depends on size)
* \param[out] buffer destination buffer
* \param[in] len number of bytes which should be read
* \return 0 if operation was successful
* \return -1 if error during I2C transfer
* \return -2 if BMP085 blocked by another task (retry it!)
* \return -4 if invalid length
*/
bool PIOS_BMP085_Read(uint8_t address, uint8_t *buffer, uint8_t len)
{
  uint8_t addr_buffer[] = {
    address,
  };

  const struct pios_i2c_txn txn_list[] = {
    {
      .addr = BMP085_I2C_ADDR,
      .rw   = PIOS_I2C_TXN_WRITE,
      .len  = sizeof(addr_buffer),
      .buf  = addr_buffer,
    },
    {
      .addr = BMP085_I2C_ADDR,
      .rw   = PIOS_I2C_TXN_READ,
      .len  = len,
      .buf  = buffer,
    }
  };

  return PIOS_I2C_Transfer(PIOS_I2C_MAIN_ADAPTER, txn_list, NELEMENTS(txn_list));
}


/**
* Writes one or more bytes to the BMP085
* \param[in] address Register address
* \param[in] buffer source buffer
* \return 0 if operation was successful
* \return -1 if error during I2C transfer
* \return -2 if BMP085 blocked by another task (retry it!)
*/
bool PIOS_BMP085_Write(uint8_t address, uint8_t buffer)
{
  uint8_t data[] = {
    address,
    buffer,
  };

  const struct pios_i2c_txn txn_list[] = {
    {
      .addr = BMP085_I2C_ADDR,
      .rw   = PIOS_I2C_TXN_WRITE,
      .len  = sizeof(data),
      .buf  = data,
    },
  };

  return PIOS_I2C_Transfer(PIOS_I2C_MAIN_ADAPTER, txn_list, NELEMENTS(txn_list));
}


#endif
