/**
 ******************************************************************************
 *
 * @file       uavobjectsinit.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       This is an automatically generated file.
 *             DO NOT modify manually. 
 *
 * @brief      The UAVUObjects GCS plugin 
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
#include "uavobjectsinit.h"
#include "actuatorcommand.h"
#include "actuatordesired.h"
#include "actuatorsettings.h"
#include "ahrscalibration.h"
#include "ahrssettings.h"
#include "ahrsstatus.h"
#include "attitudeactual.h"
#include "attitudedesired.h"
#include "attituderaw.h"
#include "baroaltitude.h"
#include "batterysettings.h"
#include "firmwareiapobj.h"
#include "flightbatterystate.h"
#include "flightplancontrol.h"
#include "flightplansettings.h"
#include "flightplanstatus.h"
#include "flighttelemetrystats.h"
#include "gcstelemetrystats.h"
#include "gpsposition.h"
#include "gpssatellites.h"
#include "gpstime.h"
#include "guidancesettings.h"
#include "homelocation.h"
#include "i2cstats.h"
#include "manualcontrolcommand.h"
#include "manualcontrolsettings.h"
#include "mixersettings.h"
#include "mixerstatus.h"
#include "objectpersistence.h"
#include "pipxtrememodemsettings.h"
#include "pipxtrememodemstatus.h"
#include "positionactual.h"
#include "positiondesired.h"
#include "ratedesired.h"
#include "stabilizationsettings.h"
#include "systemalarms.h"
#include "systemsettings.h"
#include "systemstats.h"
#include "taskinfo.h"
#include "telemetrysettings.h"
#include "velocityactual.h"
#include "velocitydesired.h"


/**
 * Function used to initialize the first instance of each object.
 * This file is automatically updated by the UAVObjectGenerator.
 */
void UAVObjectsInitialize(UAVObjectManager* objMngr)
{
    objMngr->registerObject( new ActuatorCommand() );
    objMngr->registerObject( new ActuatorDesired() );
    objMngr->registerObject( new ActuatorSettings() );
    objMngr->registerObject( new AHRSCalibration() );
    objMngr->registerObject( new AHRSSettings() );
    objMngr->registerObject( new AhrsStatus() );
    objMngr->registerObject( new AttitudeActual() );
    objMngr->registerObject( new AttitudeDesired() );
    objMngr->registerObject( new AttitudeRaw() );
    objMngr->registerObject( new BaroAltitude() );
    objMngr->registerObject( new BatterySettings() );
    objMngr->registerObject( new FirmwareIAPObj() );
    objMngr->registerObject( new FlightBatteryState() );
    objMngr->registerObject( new FlightPlanControl() );
    objMngr->registerObject( new FlightPlanSettings() );
    objMngr->registerObject( new FlightPlanStatus() );
    objMngr->registerObject( new FlightTelemetryStats() );
    objMngr->registerObject( new GCSTelemetryStats() );
    objMngr->registerObject( new GPSPosition() );
    objMngr->registerObject( new GPSSatellites() );
    objMngr->registerObject( new GPSTime() );
    objMngr->registerObject( new GuidanceSettings() );
    objMngr->registerObject( new HomeLocation() );
    objMngr->registerObject( new I2CStats() );
    objMngr->registerObject( new ManualControlCommand() );
    objMngr->registerObject( new ManualControlSettings() );
    objMngr->registerObject( new MixerSettings() );
    objMngr->registerObject( new MixerStatus() );
    objMngr->registerObject( new ObjectPersistence() );
    objMngr->registerObject( new PipXtremeModemSettings() );
    objMngr->registerObject( new PipXtremeModemStatus() );
    objMngr->registerObject( new PositionActual() );
    objMngr->registerObject( new PositionDesired() );
    objMngr->registerObject( new RateDesired() );
    objMngr->registerObject( new StabilizationSettings() );
    objMngr->registerObject( new SystemAlarms() );
    objMngr->registerObject( new SystemSettings() );
    objMngr->registerObject( new SystemStats() );
    objMngr->registerObject( new TaskInfo() );
    objMngr->registerObject( new TelemetrySettings() );
    objMngr->registerObject( new VelocityActual() );
    objMngr->registerObject( new VelocityDesired() );

}
