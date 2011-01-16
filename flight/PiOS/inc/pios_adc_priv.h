/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup   PIOS_ADC ADC Functions
 * @brief PIOS interface for USART port
 * @{
 *
 * @file       pios_adc_priv.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      ADC private definitions.
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
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

#ifndef PIOS_ADC_PRIV_H
#define PIOS_ADC_PRIV_H

#include <pios.h>
#include <pios_stm32.h>
#include <pios_adc.h>
#include <fifo_buffer.h>

struct pios_adc_cfg {
	struct stm32_dma dma;
	uint32_t half_flag;
	uint32_t full_flag;
};

struct pios_adc_dev {
	const struct pios_adc_cfg *const cfg;	
	ADCCallback callback_function;
	volatile int16_t *valid_data_buffer;
	volatile uint8_t adc_oversample;
	int16_t fir_coeffs[PIOS_ADC_MAX_SAMPLES+1]  __attribute__ ((aligned(4)));
	volatile int16_t raw_data_buffer[PIOS_ADC_MAX_SAMPLES]  __attribute__ ((aligned(4)));	// Double buffer that DMA just used
	float downsampled_buffer[PIOS_ADC_NUM_CHANNELS]  __attribute__ ((aligned(4)));
};

extern struct pios_adc_dev pios_adc_devs[];
extern uint8_t pios_adc_num_devices;

#endif /* PIOS_ADC_PRIV_H */

/**
 * @}
 * @}
 */

