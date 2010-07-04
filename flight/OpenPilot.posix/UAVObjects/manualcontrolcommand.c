/**
 ******************************************************************************
 *
 * @file       manualcontrolcommand.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the ManualControlCommand object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: manualcontrolcommand.xml. 
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
#include "manualcontrolcommand.h"

// Private variables
static UAVObjHandle handle;

// Private functions
static void setDefaults(UAVObjHandle obj, uint16_t instId);

/**
 * Initialize object.
 * \return 0 Success
 * \return -1 Failure
 */
int32_t ManualControlCommandInitialize()
{
	// Register object with the object manager
	handle = UAVObjRegister(MANUALCONTROLCOMMAND_OBJID, MANUALCONTROLCOMMAND_NAME, MANUALCONTROLCOMMAND_METANAME, 0,
			MANUALCONTROLCOMMAND_ISSINGLEINST, MANUALCONTROLCOMMAND_ISSETTINGS, MANUALCONTROLCOMMAND_NUMBYTES, &setDefaults);

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
	ManualControlCommandData data;
	UAVObjMetadata metadata;

	// Initialize object fields to their default values
	UAVObjGetInstanceData(obj, instId, &data);
	memset(&data, 0, sizeof(ManualControlCommandData));

	UAVObjSetInstanceData(obj, instId, &data);

	// Initialize object metadata to their default values
	metadata.access = ACCESS_READWRITE;
	metadata.gcsAccess = ACCESS_READWRITE;
	metadata.telemetryAcked = 0;
	metadata.telemetryUpdateMode = UPDATEMODE_PERIODIC;
	metadata.telemetryUpdatePeriod = 2000;
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
UAVObjHandle ManualControlCommandHandle()
{
	return handle;
}


