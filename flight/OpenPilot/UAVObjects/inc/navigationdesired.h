/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup NavigationDesired NavigationDesired 
 * @brief Currently unused
 *
 * Autogenerated files and functions for NavigationDesired Object
 
 * @{ 
 *
 * @file       navigationdesired.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the NavigationDesired object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: navigationdesired.xml. 
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

#ifndef NAVIGATIONDESIRED_H
#define NAVIGATIONDESIRED_H

// Object constants
#define NAVIGATIONDESIRED_OBJID 2357718118U
#define NAVIGATIONDESIRED_NAME "NavigationDesired"
#define NAVIGATIONDESIRED_METANAME "NavigationDesiredMeta"
#define NAVIGATIONDESIRED_ISSINGLEINST 1
#define NAVIGATIONDESIRED_ISSETTINGS 0
#define NAVIGATIONDESIRED_NUMBYTES sizeof(NavigationDesiredData)

// Object access macros
/**
 * @function NavigationDesiredGet(dataOut)
 * @brief Populate a NavigationDesiredData object
 * @param[out] dataOut 
 */
#define NavigationDesiredGet(dataOut) UAVObjGetData(NavigationDesiredHandle(), dataOut)
#define NavigationDesiredSet(dataIn) UAVObjSetData(NavigationDesiredHandle(), dataIn)
#define NavigationDesiredInstGet(instId, dataOut) UAVObjGetInstanceData(NavigationDesiredHandle(), instId, dataOut)
#define NavigationDesiredInstSet(instId, dataIn) UAVObjSetInstanceData(NavigationDesiredHandle(), instId, dataIn)
#define NavigationDesiredConnectQueue(queue) UAVObjConnectQueue(NavigationDesiredHandle(), queue, EV_MASK_ALL_UPDATES)
#define NavigationDesiredConnectCallback(cb) UAVObjConnectCallback(NavigationDesiredHandle(), cb, EV_MASK_ALL_UPDATES)
#define NavigationDesiredCreateInstance() UAVObjCreateInstance(NavigationDesiredHandle())
#define NavigationDesiredRequestUpdate() UAVObjRequestUpdate(NavigationDesiredHandle())
#define NavigationDesiredRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(NavigationDesiredHandle(), instId)
#define NavigationDesiredUpdated() UAVObjUpdated(NavigationDesiredHandle())
#define NavigationDesiredInstUpdated(instId) UAVObjUpdated(NavigationDesiredHandle(), instId)
#define NavigationDesiredGetMetadata(dataOut) UAVObjGetMetadata(NavigationDesiredHandle(), dataOut)
#define NavigationDesiredSetMetadata(dataIn) UAVObjSetMetadata(NavigationDesiredHandle(), dataIn)
#define NavigationDesiredReadOnly(dataIn) UAVObjReadOnly(NavigationDesiredHandle(), dataIn)

// Object data
typedef struct {
    float Latitude;
    float Longitude;
    float Altitude;
    float Speed;

} __attribute__((packed)) NavigationDesiredData;

// Field information
// Field Latitude information
// Field Longitude information
// Field Altitude information
// Field Speed information


// Generic interface functions
int32_t NavigationDesiredInitialize();
UAVObjHandle NavigationDesiredHandle();

#endif // NAVIGATIONDESIRED_H

/**
 * @}
 * @}
 */