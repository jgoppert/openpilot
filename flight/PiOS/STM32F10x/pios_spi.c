/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup   PIOS_SPI SPI Functions
 * @brief PIOS interface to read and write from SPI ports
 * @{
 *
 * @file       pios_spi.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * 	        Parts by Thorsten Klose (tk@midibox.org) (tk@midibox.org)
 * @brief      Hardware Abstraction Layer for SPI ports of STM32
 * @see        The GNU Public License (GPL) Version 3
 * @notes
 *
 * Note that additional chip select lines can be easily added by using
 * the remaining free GPIOs of the core module. Shared SPI ports should be
 * arbitrated with (FreeRTOS based) Mutexes to avoid collisions!
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

#include <pios.h>

#if defined(PIOS_INCLUDE_SPI)

#include <pios_spi_priv.h>

static struct pios_spi_dev * find_spi_dev_by_id (uint8_t spi)
{
  if (spi >= pios_spi_num_devices) {
    /* Undefined SPI port for this board (see pios_board.c) */
    return NULL;
  }

  /* Get a handle for the device configuration */
  return &(pios_spi_devs[spi]);
}

/**
* Initialises SPI pins
* \param[in] mode currently only mode 0 supported
* \return < 0 if initialisation failed
*/
int32_t PIOS_SPI_Init(void)
{
	struct pios_spi_dev * spi_dev;
	uint8_t               i;

	for (i = 0; i < pios_spi_num_devices; i++) {
	  /* Get a handle for the device configuration */
	  spi_dev = find_spi_dev_by_id(i);
	  PIOS_DEBUG_Assert(spi_dev);

	  /* Disable callback function */
	  spi_dev->callback = NULL;

	  /* Set rx/tx dummy bytes to a known value */
	  spi_dev->rx_dummy_byte = 0xFF;
	  spi_dev->tx_dummy_byte = 0xFF;

	  switch (spi_dev->cfg->init.SPI_NSS) {
	  case SPI_NSS_Soft:
	    if (spi_dev->cfg->init.SPI_Mode == SPI_Mode_Master) {
	      /* We're a master in soft NSS mode, make sure we see NSS high at all times. */
	      SPI_NSSInternalSoftwareConfig(spi_dev->cfg->regs, SPI_NSSInternalSoft_Set);
	      /* Since we're driving the SSEL pin in software, ensure that the slave is deselected */
	      GPIO_SetBits(spi_dev->cfg->ssel.gpio, spi_dev->cfg->ssel.init.GPIO_Pin);
	      GPIO_Init(spi_dev->cfg->ssel.gpio, &(spi_dev->cfg->ssel.init));
	    } else {
	      /* We're a slave in soft NSS mode, make sure we see NSS low at all times. */
	      SPI_NSSInternalSoftwareConfig(spi_dev->cfg->regs, SPI_NSSInternalSoft_Reset);
	    }
	    break;
	  case SPI_NSS_Hard:
	    /* FIXME: Should this also call SPI_SSOutputCmd()? */
	    GPIO_Init(spi_dev->cfg->ssel.gpio, &(spi_dev->cfg->ssel.init));
	    break;
	  default:
	    PIOS_DEBUG_Assert(0);
	  }
	  
	  /* Initialize the GPIO pins */
	  GPIO_Init(spi_dev->cfg->sclk.gpio, &(spi_dev->cfg->sclk.init));
	  GPIO_Init(spi_dev->cfg->mosi.gpio, &(spi_dev->cfg->mosi.init));
	  GPIO_Init(spi_dev->cfg->miso.gpio, &(spi_dev->cfg->miso.init));

	  /* Enable the associated peripheral clock */
	  switch ((uint32_t)spi_dev->cfg->regs) {
	  case (uint32_t)SPI1:
	    /* Enable SPI peripheral clock (APB2 == high speed) */
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	    break;
	  case (uint32_t)SPI2:
	    /* Enable SPI peripheral clock (APB1 == slow speed) */
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	    break;
	  case (uint32_t)SPI3:
	    /* Enable SPI peripheral clock (APB1 == slow speed) */
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	    break;
	  }  
	  
	  /* Enable DMA clock */
	  RCC_AHBPeriphClockCmd(spi_dev->cfg->dma.ahb_clk, ENABLE);
	  
	  /* Configure DMA for SPI Rx */
	  DMA_Cmd(spi_dev->cfg->dma.rx.channel, DISABLE);
	  DMA_Init(spi_dev->cfg->dma.rx.channel, &(spi_dev->cfg->dma.rx.init));
	
	  /* Configure DMA for SPI Tx */
	  DMA_Cmd(spi_dev->cfg->dma.tx.channel, DISABLE);
	  DMA_Init(spi_dev->cfg->dma.tx.channel, &(spi_dev->cfg->dma.tx.init));

	  /* Initialize the SPI block */
	  SPI_Init(spi_dev->cfg->regs, &(spi_dev->cfg->init));

	  /* Configure CRC calculation */
	  if (spi_dev->cfg->use_crc) {
	    SPI_CalculateCRC(spi_dev->cfg->regs, ENABLE);
	  } else {
	    SPI_CalculateCRC(spi_dev->cfg->regs, DISABLE);
	  }

	  /* Enable SPI */
	  SPI_Cmd(spi_dev->cfg->regs, ENABLE);

	  /* Enable SPI interrupts to DMA */
	  SPI_I2S_DMACmd(spi_dev->cfg->regs, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);

	  /* Configure DMA interrupt */
	  NVIC_Init(&(spi_dev->cfg->dma.irq.init));
	}

	return 0;
}


/**
* (Re-)initialises SPI peripheral clock rate
*
* \param[in] spi SPI number (0 or 1)
* \param[in] spi_prescaler configures the SPI speed:
* <UL>
*   <LI>PIOS_SPI_PRESCALER_2: sets clock rate 27.7~ nS @ 72 MHz (36 MBit/s) (only supported for spi==0, spi1 uses 4 instead)
*   <LI>PIOS_SPI_PRESCALER_4: sets clock rate 55.5~ nS @ 72 MHz (18 MBit/s)
*   <LI>PIOS_SPI_PRESCALER_8: sets clock rate 111.1~ nS @ 72 MHz (9 MBit/s)
*   <LI>PIOS_SPI_PRESCALER_16: sets clock rate 222.2~ nS @ 72 MHz (4.5 MBit/s)
*   <LI>PIOS_SPI_PRESCALER_32: sets clock rate 444.4~ nS @ 72 MHz (2.25 MBit/s)
*   <LI>PIOS_SPI_PRESCALER_64: sets clock rate 888.8~ nS @ 72 MHz (1.125 MBit/s)
*   <LI>PIOS_SPI_PRESCALER_128: sets clock rate 1.7~ nS @ 72 MHz (0.562 MBit/s)
*   <LI>PIOS_SPI_PRESCALER_256: sets clock rate 3.5~ nS @ 72 MHz (0.281 MBit/s)
* </UL>
* \return 0 if no error
* \return -1 if disabled SPI port selected
* \return -2 if unsupported SPI port selected
* \return -3 if invalid spi_prescaler selected
*/
int32_t PIOS_SPI_SetClockSpeed(uint8_t spi, SPIPrescalerTypeDef spi_prescaler)
{
  struct pios_spi_dev * spi_dev;
  SPI_InitTypeDef       SPI_InitStructure;

  /* Get a handle for the device configuration */
  spi_dev = find_spi_dev_by_id(spi);

  if (!spi_dev) {
    /* Undefined SPI port for this board (see pios_board.c) */
    return -2;
  }

  if(spi_prescaler >= 8) {
    /* Invalid prescaler selected */
    return -3;
  }

  /* Start with a copy of the default configuration for the peripheral */
  SPI_InitStructure = spi_dev->cfg->init;

  /* Adjust the prescaler for the peripheral's clock */
  SPI_InitStructure.SPI_BaudRatePrescaler = ((uint16_t)spi_prescaler & 7) << 3;

  /* Write back the new configuration */
  SPI_Init(spi_dev->cfg->regs, &SPI_InitStructure);

  PIOS_SPI_TransferByte(spi, 0xFF);
  return 0;
}

/**
* Controls the RC (Register Clock alias Chip Select) pin of a SPI port
* \param[in] spi SPI number (0 or 1)
* \param[in] pin_value 0 or 1
* \return 0 if no error
* \return -1 if disabled SPI port selected
* \return -2 if unsupported SPI port selected
*/
int32_t PIOS_SPI_RC_PinSet(uint8_t spi, uint8_t pin_value)
{
  struct pios_spi_dev * spi_dev;

  /* Get a handle for the device configuration */
  spi_dev = find_spi_dev_by_id(spi);

  if (!spi_dev) {
    /* Undefined SPI port for this board (see pios_board.c) */
    return -2;
  }

  if (pin_value) {
    GPIO_SetBits(spi_dev->cfg->ssel.gpio, spi_dev->cfg->ssel.init.GPIO_Pin);
  } else {
    GPIO_ResetBits(spi_dev->cfg->ssel.gpio, spi_dev->cfg->ssel.init.GPIO_Pin);
  }

  return 0;
}

/**
* Transfers a byte to SPI output and reads back the return value from SPI input
* \param[in] spi SPI number (0 or 1)
* \param[in] b the byte which should be transfered
*/
int32_t PIOS_SPI_TransferByte(uint8_t spi, uint8_t b)
{
  struct pios_spi_dev * spi_dev;
  uint8_t dummy;
  uint8_t rx_byte;

  /* Get a handle for the device configuration */
  spi_dev = find_spi_dev_by_id(spi);
  PIOS_DEBUG_Assert(spi_dev);

  /* 
   * Procedure taken from STM32F10xxx Reference Manual section 23.3.5
   */

  /* Make sure the RXNE flag is cleared by reading the DR register */
  dummy = spi_dev->cfg->regs->DR;

  /* Start the transfer */
  spi_dev->cfg->regs->DR = b;

  /* Wait until there is a byte to read */
  while(!(spi_dev->cfg->regs->SR & SPI_I2S_FLAG_RXNE));

  /* Read the rx'd byte */
  rx_byte = spi_dev->cfg->regs->DR;

  /* Wait until the TXE goes high */
  while (!(spi_dev->cfg->regs->SR & SPI_I2S_FLAG_TXE));

  /* Wait for SPI transfer to have fully completed */
  while (spi_dev->cfg->regs->SR & SPI_I2S_FLAG_BSY);

  /* Return received byte */
  return rx_byte;
}


/**
* Transfers a block of bytes via DMA.
* \param[in] spi SPI number (0 or 1)
* \param[in] send_buffer pointer to buffer which should be sent.<BR>
* If NULL, 0xff (all-one) will be sent.
* \param[in] receive_buffer pointer to buffer which should get the received values.<BR>
* If NULL, received bytes will be discarded.
* \param[in] len number of bytes which should be transfered
* \param[in] callback pointer to callback function which will be executed
* from DMA channel interrupt once the transfer is finished.
* If NULL, no callback function will be used, and PIOS_SPI_TransferBlock() will
* block until the transfer is finished.
* \return >= 0 if no error during transfer
* \return -1 if disabled SPI port selected
* \return -2 if unsupported SPI port selected
* \return -3 if function has been called during an ongoing DMA transfer
*/
int32_t PIOS_SPI_TransferBlock(uint8_t spi, const uint8_t *send_buffer, uint8_t *receive_buffer, uint16_t len, void *callback)
{
  struct pios_spi_dev * spi_dev;
  DMA_InitTypeDef       dma_init;

  /* Get a handle for the device configuration */
  spi_dev = find_spi_dev_by_id(spi);

  if (!spi_dev) {
    /* Undefined SPI port for this board (see pios_board.c) */
    return -2;
  }


  /* Exit if ongoing transfer */
  if (DMA_GetCurrDataCounter(spi_dev->cfg->dma.rx.channel)) {
    return -3;
  }

  /* Disable the SPI peripheral */
  SPI_Cmd(spi_dev->cfg->regs, DISABLE);

  /* Disable the DMA channels */
  DMA_Cmd(spi_dev->cfg->dma.rx.channel, DISABLE);
  DMA_Cmd(spi_dev->cfg->dma.tx.channel, DISABLE);

  /* Set callback function */
  spi_dev->callback = callback;

  /* 
   * Configure Rx channel 
   */

  /* Start with the default configuration for this peripheral */
  dma_init = spi_dev->cfg->dma.rx.init;

  if(receive_buffer != NULL) {
    /* Enable memory addr. increment - bytes written into receive buffer */
    dma_init.DMA_MemoryBaseAddr = (uint32_t)receive_buffer;
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Enable;
  } else {
    /* Disable memory addr. increment - bytes written into dummy buffer */
    spi_dev->rx_dummy_byte = 0xFF;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)&spi_dev->rx_dummy_byte;
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Disable;
  }
  if (spi_dev->cfg->use_crc) {
    /* Make sure the CRC error flag is cleared before we start */
    SPI_I2S_ClearFlag(spi_dev->cfg->regs, SPI_FLAG_CRCERR);
  }

  dma_init.DMA_BufferSize = len;
  DMA_Init(spi_dev->cfg->dma.rx.channel, &(dma_init));

  /* 
   * Configure Tx channel 
   */

  /* Start with the default configuration for this peripheral */
  dma_init = spi_dev->cfg->dma.tx.init;

  if(send_buffer != NULL) {
    /* Enable memory addr. increment - bytes written into receive buffer */
    dma_init.DMA_MemoryBaseAddr = (uint32_t)send_buffer;
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Enable;
  } else {
    /* Disable memory addr. increment - bytes written into dummy buffer */
    spi_dev->tx_dummy_byte = 0xFF;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)&spi_dev->tx_dummy_byte;
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Disable;
  }

  if (spi_dev->cfg->use_crc) {
    /* The last byte of the payload will be replaced with the CRC8 */
    dma_init.DMA_BufferSize = len - 1;
  } else {
    dma_init.DMA_BufferSize = len;
  }

  DMA_Init(spi_dev->cfg->dma.tx.channel, &(dma_init));

  /* Enable DMA interrupt if callback function active */
  DMA_ITConfig(spi_dev->cfg->dma.rx.channel, DMA_IT_TC, (callback != NULL) ? ENABLE : DISABLE);

  /* Flush out the CRC registers */
  SPI_CalculateCRC(spi_dev->cfg->regs, DISABLE);
  (void) SPI_GetCRC(spi_dev->cfg->regs, SPI_CRC_Rx);
  SPI_I2S_ClearFlag(spi_dev->cfg->regs, SPI_FLAG_CRCERR);

  /* Make sure to flush out the receive buffer */
  (void) SPI_I2S_ReceiveData(spi_dev->cfg->regs);

  if (spi_dev->cfg->use_crc) {
    /* Need a 0->1 transition to reset the CRC logic */
    SPI_CalculateCRC(spi_dev->cfg->regs, ENABLE);
  }

  /* Start DMA transfers */
  DMA_Cmd(spi_dev->cfg->dma.rx.channel, ENABLE);
  DMA_Cmd(spi_dev->cfg->dma.tx.channel, ENABLE);

  /* Reenable the SPI device */
  SPI_Cmd(spi_dev->cfg->regs, ENABLE);

  if (callback) {
    /* User has requested a callback, don't wait for the transfer to complete. */
    return 0;
  }

  /* Wait until all bytes have been transmitted/received */
  while(DMA_GetCurrDataCounter(spi_dev->cfg->dma.rx.channel));

  /* Wait for the final bytes of the transfer to complete, including CRC byte(s). */
  while(!(SPI_I2S_GetFlagStatus(spi_dev->cfg->regs, SPI_I2S_FLAG_TXE)));

  /* Wait for the final bytes of the transfer to complete, including CRC byte(s). */
  while(SPI_I2S_GetFlagStatus(spi_dev->cfg->regs, SPI_I2S_FLAG_BSY));

  /* Check the CRC on the transfer if enabled. */
  if (spi_dev->cfg->use_crc) {
    /* Check the SPI CRC error flag */
    if (SPI_I2S_GetFlagStatus(spi_dev->cfg->regs, SPI_FLAG_CRCERR)) {
      return -4;
    }
  }

  /* No error */
  return 0;
}

void PIOS_SPI_IRQ_Handler(uint8_t spi)
{
  struct pios_spi_dev * spi_dev;

  spi_dev = find_spi_dev_by_id (spi);
  PIOS_DEBUG_Assert(spi_dev);

  DMA_ClearFlag(spi_dev->cfg->dma.irq.flags);

  /* Wait for the final bytes of the transfer to complete, including CRC byte(s). */
  while(!(SPI_I2S_GetFlagStatus(spi_dev->cfg->regs, SPI_I2S_FLAG_TXE)));

  /* Wait for the final bytes of the transfer to complete, including CRC byte(s). */
  while(SPI_I2S_GetFlagStatus(spi_dev->cfg->regs, SPI_I2S_FLAG_BSY));

  if(spi_dev->callback != NULL) {
    bool crc_ok = TRUE;
    uint8_t crc_val;

    if (SPI_I2S_GetFlagStatus(spi_dev->cfg->regs, SPI_FLAG_CRCERR)) {
      crc_ok = FALSE;
      SPI_I2S_ClearFlag(spi_dev->cfg->regs, SPI_FLAG_CRCERR);
    }
    crc_val = SPI_GetCRC(spi_dev->cfg->regs, SPI_CRC_Rx);
    spi_dev->callback(crc_ok, crc_val);
  }
}

#endif

/**
  * @}
  * @}
  */
