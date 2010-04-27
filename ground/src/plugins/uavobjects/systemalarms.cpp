/**
 ******************************************************************************
 *
 * @file       systemalarms.cpp
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
#include "systemalarms.h"
#include "uavobjectfields.h"

const QString SystemAlarms::NAME = QString("SystemAlarms");

SystemAlarms::SystemAlarms(): UAVDataObject(OBJID, ISSINGLEINST, ISSETTINGS, NAME)
{
    // Create fields
    QList<UAVObjectField*> fields;
    QStringList AlarmElemNames;
    AlarmElemNames.append("OutOfMemory");
    AlarmElemNames.append("StackOverflow");
    AlarmElemNames.append("CPUOverload");
    AlarmElemNames.append("EventSystem");
    AlarmElemNames.append("SDCard");
    AlarmElemNames.append("Telemetry");
    QStringList AlarmEnumOptions;
    AlarmEnumOptions.append("OK");
    AlarmEnumOptions.append("Warning");
    AlarmEnumOptions.append("Error");
    AlarmEnumOptions.append("Critical");
    fields.append(new UAVObjectFieldEnum(QString("Alarm"), QString(""), AlarmElemNames, AlarmEnumOptions));

    // Initialize object
    initializeFields(fields, (quint8*)&data, NUMBYTES);
}

UAVObject::Metadata SystemAlarms::getDefaultMetadata()
{
    UAVObject::Metadata metadata;
    metadata.gcsTelemetryAcked = 1;
    metadata.gcsTelemetryUpdateMode = UAVObject::UPDATEMODE_ONCHANGE;
    metadata.gcsTelemetryUpdatePeriod = 0;
    metadata.flightTelemetryAcked = 1;
    metadata.flightTelemetryUpdateMode = UAVObject::UPDATEMODE_PERIODIC;
    metadata.flightTelemetryUpdatePeriod = 4000;
    metadata.loggingUpdateMode = UAVObject::UPDATEMODE_PERIODIC;
    metadata.loggingUpdatePeriod = 1000;
    return metadata;
}

SystemAlarms::DataFields SystemAlarms::getData()
{
    QMutexLocker locker(mutex);
    return data;
}

void SystemAlarms::setData(DataFields& data)
{
    QMutexLocker locker(mutex);
    this->data = data;
    emit objectUpdatedAuto(this); // trigger object updated event
    emit objectUpdated(this);
}

UAVDataObject* SystemAlarms::clone(quint32 instID)
{
    SystemAlarms* obj = new SystemAlarms();
    obj->initialize(instID, this->getMetaObject());
    return obj;
}
