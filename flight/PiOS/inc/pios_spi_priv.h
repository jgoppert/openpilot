/**
 ******************************************************************************
 *
 * @file       pios_spi_priv.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * 	       Parts by Thorsten Klose (tk@midibox.org)
 * @brief      SPI private definitions.
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

#ifndef PIOS_SPI_PRIV_H
#define PIOS_SPI_PRIV_H

#include <pios.h>
#include <pios_stm32.h>

struct pios_spi_cfg {
  SPI_TypeDef           * regs;
  SPI_InitTypeDef         init;
  bool                    use_crc;
  struct stm32_dma        dma;
  struct stm32_gpio       ssel;
  struct stm32_gpio       sclk;
  struct stm32_gpio       miso;
  struct stm32_gpio       mosi;
};

struct pios_spi_dev {
  const struct pios_spi_cfg * const cfg;
  void                        (*callback)(uint8_t, uint8_t);
  uint8_t                     tx_dummy_byte;
  uint8_t                     rx_dummy_byte;
};

extern struct pios_spi_dev pios_spi_devs[];
extern uint8_t             pios_spi_num_devices;

#endif /* PIOS_SPI_PRIV_H */
