/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup SystemAlarms SystemAlarms 
 * @brief Alarms from OpenPilot to indicate failure conditions or warnings.  Set by various modules.
 *
 * Autogenerated files and functions for SystemAlarms Object
 
 * @{ 
 *
 * @file       systemalarms.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the SystemAlarms object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: systemalarms.xml. 
 *             This is an automatically generated file.
 *             DO NOT modify manually.
 *
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

#ifndef SYSTEMALARMS_H
#define SYSTEMALARMS_H

// Object constants
#define SYSTEMALARMS_OBJID 2311311584U
#define SYSTEMALARMS_NAME "SystemAlarms"
#define SYSTEMALARMS_METANAME "SystemAlarmsMeta"
#define SYSTEMALARMS_ISSINGLEINST 1
#define SYSTEMALARMS_ISSETTINGS 0
#define SYSTEMALARMS_NUMBYTES sizeof(SystemAlarmsData)

// Object access macros
/**
 * @function SystemAlarmsGet(dataOut)
 * @brief Populate a SystemAlarmsData object
 * @param[out] dataOut 
 */
#define SystemAlarmsGet(dataOut) UAVObjGetData(SystemAlarmsHandle(), dataOut)
#define SystemAlarmsSet(dataIn) UAVObjSetData(SystemAlarmsHandle(), dataIn)
#define SystemAlarmsInstGet(instId, dataOut) UAVObjGetInstanceData(SystemAlarmsHandle(), instId, dataOut)
#define SystemAlarmsInstSet(instId, dataIn) UAVObjSetInstanceData(SystemAlarmsHandle(), instId, dataIn)
#define SystemAlarmsConnectQueue(queue) UAVObjConnectQueue(SystemAlarmsHandle(), queue, EV_MASK_ALL_UPDATES)
#define SystemAlarmsConnectCallback(cb) UAVObjConnectCallback(SystemAlarmsHandle(), cb, EV_MASK_ALL_UPDATES)
#define SystemAlarmsCreateInstance() UAVObjCreateInstance(SystemAlarmsHandle())
#define SystemAlarmsRequestUpdate() UAVObjRequestUpdate(SystemAlarmsHandle())
#define SystemAlarmsRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(SystemAlarmsHandle(), instId)
#define SystemAlarmsUpdated() UAVObjUpdated(SystemAlarmsHandle())
#define SystemAlarmsInstUpdated(instId) UAVObjUpdated(SystemAlarmsHandle(), instId)
#define SystemAlarmsGetMetadata(dataOut) UAVObjGetMetadata(SystemAlarmsHandle(), dataOut)
#define SystemAlarmsSetMetadata(dataIn) UAVObjSetMetadata(SystemAlarmsHandle(), dataIn)
#define SystemAlarmsReadOnly(dataIn) UAVObjReadOnly(SystemAlarmsHandle(), dataIn)

// Object data
typedef struct {
    uint8_t Alarm[10];

} __attribute__((packed)) SystemAlarmsData;

// Field information
// Field Alarm information
/* Enumeration options for field Alarm */
typedef enum { SYSTEMALARMS_ALARM_OK=0, SYSTEMALARMS_ALARM_WARNING=1, SYSTEMALARMS_ALARM_ERROR=2, SYSTEMALARMS_ALARM_CRITICAL=3 } SystemAlarmsAlarmOptions;
/* Array element names for field Alarm */
typedef enum { SYSTEMALARMS_ALARM_OUTOFMEMORY=0, SYSTEMALARMS_ALARM_STACKOVERFLOW=1, SYSTEMALARMS_ALARM_CPUOVERLOAD=2, SYSTEMALARMS_ALARM_EVENTSYSTEM=3, SYSTEMALARMS_ALARM_SDCARD=4, SYSTEMALARMS_ALARM_TELEMETRY=5, SYSTEMALARMS_ALARM_MANUALCONTROL=6, SYSTEMALARMS_ALARM_ACTUATOR=7, SYSTEMALARMS_ALARM_STABILIZATION=8, SYSTEMALARMS_ALARM_AHRSCOMMS=9 } SystemAlarmsAlarmElem;
/* Number of elements for field Alarm */
#define SYSTEMALARMS_ALARM_NUMELEM 10


// Generic interface functions
int32_t SystemAlarmsInitialize();
UAVObjHandle SystemAlarmsHandle();

#endif // SYSTEMALARMS_H

/**
 * @}
 * @}
 */