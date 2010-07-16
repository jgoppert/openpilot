/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup PIOS_STM32 STM32 HAL
 * @brief STM32 specific global data structures 
 * @{
 *
 * @file       pios_stm32.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Types that are specific to the STM32 peripherals
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

#ifndef PIOS_STM32_H
#define PIOS_STM32_H

struct stm32_irq {
  void               (*handler)(void);
  uint32_t           flags;
  NVIC_InitTypeDef   init;
};

struct stm32_dma_chan {
  DMA_Channel_TypeDef   * channel;
  DMA_InitTypeDef         init;
};

struct stm32_dma {
  uint32_t                ahb_clk;
  struct stm32_irq        irq;
  struct stm32_dma_chan   rx;
  struct stm32_dma_chan   tx;
};

struct stm32_gpio {
  GPIO_TypeDef          * gpio;
  GPIO_InitTypeDef        init;
};

/**
  * @}
  * @}
  */

#endif /* PIOS_STM32_H */
