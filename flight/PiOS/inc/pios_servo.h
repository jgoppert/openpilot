/**
 ******************************************************************************
 *
 * @file       pios_servo.h  
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      RC Servo functions header.
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

#ifndef PIOS_SERVO_H
#define PIOS_SERVO_H

/* Public Functions */
extern void PIOS_Servo_Init(void);
extern void PIOS_Servo_SetHz(uint16_t onetofour, uint16_t fivetoeight);
extern void PIOS_Servo_Set(uint8_t Servo, uint16_t Position);

#endif /* PIOS_SERVO_H */
