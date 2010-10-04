/**
 ******************************************************************************
 *
 * @file       ahrs_comm_objects.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      AHRS comms AUVObjects. This file defined teh objects to be used.
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

#include "ahrs_spi_comm.h"
#include "pios_debug.h"

static AttitudeRawData AttitudeRaw;
static AttitudeActualData AttitudeActual;
static AHRSCalibrationData AHRSCalibration;
static AhrsStatusData AhrsStatus;
static BaroAltitudeData BaroAltitude;
static GPSPositionData GPSPosition;
static PositionActualData PositionActual;
static HomeLocationData HomeLocation;
static AHRSSettingsData AHRSSettings;

AhrsSharedObject objectHandles[MAX_AHRS_OBJECTS];

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef IN_AHRS

//slightly hacky - implement our own version of the xxxHandle() functions
#define CREATEHANDLE(n,obj) \
	void * obj##Handle() {return (&objectHandles[n]);}

CREATEHANDLE(0, AttitudeRaw);
CREATEHANDLE(1, AttitudeActual);
CREATEHANDLE(2, AHRSCalibration);
CREATEHANDLE(3, AhrsStatus);
CREATEHANDLE(4, BaroAltitude);
CREATEHANDLE(5, GPSPosition);
CREATEHANDLE(6, PositionActual);
CREATEHANDLE(7, HomeLocation);
CREATEHANDLE(8, AHRSSettings);

#if 9 != MAX_AHRS_OBJECTS	//sanity check
#error We did not create the correct number of xxxHandle() functions
#endif

#define ADDHANDLE(idx,obj) {\
		int n = idx;\
		objectHandles[n].data = &obj;\
		objectHandles[n].size = sizeof(obj);\
		objectHandles[n].index = n;\
		}

#else

static void ObjectUpdatedCb(UAVObjEvent * ev);

#define ADDHANDLE(idx,obj) {\
		int n = idx;\
		objectHandles[n].data = &obj;\
		objectHandles[n].uavHandle = obj##Handle();\
		objectHandles[n].size = sizeof(obj);\
		objectHandles[n].index = n;\
		}

#endif

void AhrsInitHandles(void)
{
	int idx = 0;
//Note: the first item in this list has the highest priority
//the last has the lowest priority
	ADDHANDLE(idx++, AttitudeRaw);
	ADDHANDLE(idx++, AttitudeActual);
	ADDHANDLE(idx++, AHRSCalibration);
	ADDHANDLE(idx++, AhrsStatus);
	ADDHANDLE(idx++, BaroAltitude);
	ADDHANDLE(idx++, GPSPosition);
	ADDHANDLE(idx++, PositionActual);
	ADDHANDLE(idx++, HomeLocation);
	ADDHANDLE(idx++, AHRSSettings);
	if (idx != MAX_AHRS_OBJECTS) {
		PIOS_DEBUG_Assert(0);
	}
//Note: Only connect objects that the AHRS needs to read
//When the AHRS writes to these the data does a round trip
//AHRS->OP->AHRS due to these events
#ifndef IN_AHRS
	AHRSSettingsConnectCallback(ObjectUpdatedCb);
	BaroAltitudeConnectCallback(ObjectUpdatedCb);
	GPSPositionConnectCallback(ObjectUpdatedCb);
	HomeLocationConnectCallback(ObjectUpdatedCb);
	AHRSCalibrationConnectCallback(ObjectUpdatedCb);

#endif
}

AhrsObjHandle AhrsFromIndex(uint8_t index)
{
	if (index >= MAX_AHRS_OBJECTS) {
		return (NULL);
	}
	return (&objectHandles[index]);
}

#ifndef IN_AHRS

AhrsObjHandle AhrsFromUAV(UAVObjHandle obj)
{
	if (objectHandles[0].uavHandle == NULL) {	//Oops - we haven't been initialised!
		PIOS_DEBUG_Assert(0);
	}
	for (int ct = 0; ct < MAX_AHRS_OBJECTS; ct++) {
		if (objectHandles[ct].uavHandle == obj) {
			return (&objectHandles[ct]);
		}
	}
	return (NULL);
}

/** Callback to update AHRS from UAVObjects
*/
static void ObjectUpdatedCb(UAVObjEvent * ev)
{
	if (!(ev->event & EV_MASK_ALL_UPDATES)) {
		return;
	}
	AhrsObjHandle hdl = AhrsFromUAV(ev->obj);
	if (hdl) {
		AhrsSharedData data;	//this is guaranteed to be big enough
		UAVObjGetData(ev->obj, &data);
		AhrsSetData(hdl, &data);
	}
}

#endif
