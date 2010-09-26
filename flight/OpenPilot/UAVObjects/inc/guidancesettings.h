/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup GuidanceSettings GuidanceSettings 
 * @brief Settings for the @ref GuidanceModule
 *
 * Autogenerated files and functions for GuidanceSettings Object
 
 * @{ 
 *
 * @file       guidancesettings.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the GuidanceSettings object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: guidancesettings.xml. 
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

#ifndef GUIDANCESETTINGS_H
#define GUIDANCESETTINGS_H

// Object constants
#define GUIDANCESETTINGS_OBJID 2428005802U
#define GUIDANCESETTINGS_NAME "GuidanceSettings"
#define GUIDANCESETTINGS_METANAME "GuidanceSettingsMeta"
#define GUIDANCESETTINGS_ISSINGLEINST 1
#define GUIDANCESETTINGS_ISSETTINGS 1
#define GUIDANCESETTINGS_NUMBYTES sizeof(GuidanceSettingsData)

// Object access macros
/**
 * @function GuidanceSettingsGet(dataOut)
 * @brief Populate a GuidanceSettingsData object
 * @param[out] dataOut 
 */
#define GuidanceSettingsGet(dataOut) UAVObjGetData(GuidanceSettingsHandle(), dataOut)
#define GuidanceSettingsSet(dataIn) UAVObjSetData(GuidanceSettingsHandle(), dataIn)
#define GuidanceSettingsInstGet(instId, dataOut) UAVObjGetInstanceData(GuidanceSettingsHandle(), instId, dataOut)
#define GuidanceSettingsInstSet(instId, dataIn) UAVObjSetInstanceData(GuidanceSettingsHandle(), instId, dataIn)
#define GuidanceSettingsConnectQueue(queue) UAVObjConnectQueue(GuidanceSettingsHandle(), queue, EV_MASK_ALL_UPDATES)
#define GuidanceSettingsConnectCallback(cb) UAVObjConnectCallback(GuidanceSettingsHandle(), cb, EV_MASK_ALL_UPDATES)
#define GuidanceSettingsCreateInstance() UAVObjCreateInstance(GuidanceSettingsHandle())
#define GuidanceSettingsRequestUpdate() UAVObjRequestUpdate(GuidanceSettingsHandle())
#define GuidanceSettingsRequestInstUpdate(instId) UAVObjRequestInstanceUpdate(GuidanceSettingsHandle(), instId)
#define GuidanceSettingsUpdated() UAVObjUpdated(GuidanceSettingsHandle())
#define GuidanceSettingsInstUpdated(instId) UAVObjUpdated(GuidanceSettingsHandle(), instId)
#define GuidanceSettingsGetMetadata(dataOut) UAVObjGetMetadata(GuidanceSettingsHandle(), dataOut)
#define GuidanceSettingsSetMetadata(dataIn) UAVObjSetMetadata(GuidanceSettingsHandle(), dataIn)
#define GuidanceSettingsReadOnly(dataIn) UAVObjReadOnly(GuidanceSettingsHandle())

// Object data
typedef struct {
    int32_t MaxGroundspeed;
    float GroundVelocityP;
    int32_t MaxVerticalSpeed;
    float VertVelocityP;
    float VelP;
    float VelI;
    float VelD;
    float DownP;
    float DownI;
    float DownD;
    float MaxVelIntegral;
    float MaxThrottleIntegral;
    int32_t VelUpdatePeriod;
    int32_t VelPIDUpdatePeriod;

} __attribute__((packed)) GuidanceSettingsData;

// Field information
// Field MaxGroundspeed information
// Field GroundVelocityP information
// Field MaxVerticalSpeed information
// Field VertVelocityP information
// Field VelP information
// Field VelI information
// Field VelD information
// Field DownP information
// Field DownI information
// Field DownD information
// Field MaxVelIntegral information
// Field MaxThrottleIntegral information
// Field VelUpdatePeriod information
// Field VelPIDUpdatePeriod information


// Generic interface functions
int32_t GuidanceSettingsInitialize();
UAVObjHandle GuidanceSettingsHandle();

#endif // GUIDANCESETTINGS_H

/**
 * @}
 * @}
 */
