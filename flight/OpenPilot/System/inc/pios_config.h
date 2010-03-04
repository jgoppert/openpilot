/**
 ******************************************************************************
 *
 * @file       pios_config.h  
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      PiOS configuration header. 
 *                 - Central compile time config for the project.
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


#ifndef PIOS_CONFIG_H
#define PIOS_CONFIG_H


/* Enable/Disable PiOS Modules */
//#define PIOS_DONT_USE_ADC
//#define PIOS_DONT_USE_DELAY
//#define PIOS_DONT_USE_I2C
//#define PIOS_DONT_USE_IRQ
//#define PIOS_DONT_USE_LED
//#define PIOS_DONT_USE_PWM
//#define PIOS_DONT_USE_SERVO
//#define PIOS_DONT_USE_SPI
//#define PIOS_DONT_USE_SYS
//#define PIOS_DONT_USE_USART
#define PIOS_DONT_USE_USB_COM
//#define PIOS_DONT_USE_USB_HID
//#define PIOS_DONT_USE_USB
//#define PIOS_DONT_USE_BMP085
//#define PIOS_DONT_USE_COM
//#define PIOS_DONT_USE_SDCARD
//#define PIOS_DONT_USE_SETTINGS
//#define PIOS_DONT_USE_ESC_I2C


/* Defaults for Logging */
#define LOG_FILENAME 			"PIOS.LOG"
#define STARTUP_LOG_ENABLED		1


/* Defaults for MinIni */
#define SETTINGS_FILE			"SETTINGS.INI"

/* COM Module */
#define GPS_BAUDRATE			19200
#define TELEM_BAUDRATE			19200
#define AUXUART_ENABLED			0
#define AUXUART_BAUDRATE		19200

/* Servos */
#define SERVOS_POSITION_MIN		800
#define SERVOS_POSITION_MAX		2200


#endif /* PIOS_CONFIG_H */
