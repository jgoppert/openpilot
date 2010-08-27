/**
 ******************************************************************************
 * @addtogroup UAVObjects OpenPilot UAVObjects
 * @{ 
 * @addtogroup ActuatorCommand ActuatorCommand
 * @brief Contains the pulse duration sent to each of the channels.  Set by @ref ActuatorModule
 *
 * Autogenerated files and functions for ActuatorCommand Object
 * @{ 
 *
 * @file       actuatorcommand.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the ActuatorCommand object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: actuatorcommand.xml. 
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

#include "openpilot.h"
#include "actuatorcommand.h"

// Private variables
static UAVObjHandle handle;

// Private functions
static void setDefaults(UAVObjHandle obj, uint16_t instId);

/**
 * Initialize object.
 * \return 0 Success
 * \return -1 Failure
 */
int32_t ActuatorCommandInitialize()
{
	// Register object with the object manager
	handle = UAVObjRegister(ACTUATORCOMMAND_OBJID, ACTUATORCOMMAND_NAME, ACTUATORCOMMAND_METANAME, 0,
			ACTUATORCOMMAND_ISSINGLEINST, ACTUATORCOMMAND_ISSETTINGS, ACTUATORCOMMAND_NUMBYTES, &setDefaults);

	// Done
	if (handle != 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
 * Initialize object fields and metadata with the default values.
 * If a default value is not specified the object fields
 * will be initialized to zero.
 */
static void setDefaults(UAVObjHandle obj, uint16_t instId)
{
	ActuatorCommandData data;
	UAVObjMetadata metadata;

	// Initialize object fields to their default values
	UAVObjGetInstanceData(obj, instId, &data);
	memset(&data, 0, sizeof(ActuatorCommandData));

	UAVObjSetInstanceData(obj, instId, &data);

	// Initialize object metadata to their default values
	metadata.access = ACCESS_READWRITE;
	metadata.gcsAccess = ACCESS_READWRITE;
	metadata.telemetryAcked = 0;
	metadata.telemetryUpdateMode = UPDATEMODE_PERIODIC;
	metadata.telemetryUpdatePeriod = 1000;
	metadata.gcsTelemetryAcked = 0;
	metadata.gcsTelemetryUpdateMode = UPDATEMODE_MANUAL;
	metadata.gcsTelemetryUpdatePeriod = 0;
	metadata.loggingUpdateMode = UPDATEMODE_NEVER;
	metadata.loggingUpdatePeriod = 0;
	UAVObjSetMetadata(obj, &metadata);
}

/**
 * Get object handle
 */
UAVObjHandle ActuatorCommandHandle()
{
	return handle;
}

/**
 * @}
 */

