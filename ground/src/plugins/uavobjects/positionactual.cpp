/**
 ******************************************************************************
 *
 * @file       positionactual.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @see        The GNU Public License (GPL) Version 3
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVObjectsPlugin UAVObjects Plugin
 * @{
 *   
 * @note       Object definition file: positionactual.xml. 
 *             This is an automatically generated file.
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
#include "positionactual.h"
#include "uavobjectfield.h"

const QString PositionActual::NAME = QString("PositionActual");

/**
 * Constructor
 */
PositionActual::PositionActual(): UAVDataObject(OBJID, ISSINGLEINST, ISSETTINGS, NAME)
{
    // Create fields
    QList<UAVObjectField*> fields;
    QStringList StatusElemNames;
    StatusElemNames.append("0");
    QStringList StatusEnumOptions;
    StatusEnumOptions.append("NoGPS");
    StatusEnumOptions.append("NoFix");
    StatusEnumOptions.append("Fix2D");
    StatusEnumOptions.append("Fix3D");
    fields.append( new UAVObjectField(QString("Status"), QString(""), UAVObjectField::ENUM, StatusElemNames, StatusEnumOptions) );
    QStringList LatitudeElemNames;
    LatitudeElemNames.append("0");
    fields.append( new UAVObjectField(QString("Latitude"), QString("degrees"), UAVObjectField::FLOAT32, LatitudeElemNames, QStringList()) );
    QStringList LongitudeElemNames;
    LongitudeElemNames.append("0");
    fields.append( new UAVObjectField(QString("Longitude"), QString("degrees"), UAVObjectField::FLOAT32, LongitudeElemNames, QStringList()) );
    QStringList AltitudeElemNames;
    AltitudeElemNames.append("0");
    fields.append( new UAVObjectField(QString("Altitude"), QString("meters"), UAVObjectField::FLOAT32, AltitudeElemNames, QStringList()) );
    QStringList GeoidSeparationElemNames;
    GeoidSeparationElemNames.append("0");
    fields.append( new UAVObjectField(QString("GeoidSeparation"), QString("meters"), UAVObjectField::FLOAT32, GeoidSeparationElemNames, QStringList()) );
    QStringList HeadingElemNames;
    HeadingElemNames.append("0");
    fields.append( new UAVObjectField(QString("Heading"), QString("degrees"), UAVObjectField::FLOAT32, HeadingElemNames, QStringList()) );
    QStringList GroundspeedElemNames;
    GroundspeedElemNames.append("0");
    fields.append( new UAVObjectField(QString("Groundspeed"), QString("m/s"), UAVObjectField::FLOAT32, GroundspeedElemNames, QStringList()) );
    QStringList SatellitesElemNames;
    SatellitesElemNames.append("0");
    fields.append( new UAVObjectField(QString("Satellites"), QString(""), UAVObjectField::INT8, SatellitesElemNames, QStringList()) );
    QStringList PDOPElemNames;
    PDOPElemNames.append("0");
    fields.append( new UAVObjectField(QString("PDOP"), QString(""), UAVObjectField::FLOAT32, PDOPElemNames, QStringList()) );
    QStringList HDOPElemNames;
    HDOPElemNames.append("0");
    fields.append( new UAVObjectField(QString("HDOP"), QString(""), UAVObjectField::FLOAT32, HDOPElemNames, QStringList()) );
    QStringList VDOPElemNames;
    VDOPElemNames.append("0");
    fields.append( new UAVObjectField(QString("VDOP"), QString(""), UAVObjectField::FLOAT32, VDOPElemNames, QStringList()) );

    // Initialize object
    initializeFields(fields, (quint8*)&data, NUMBYTES);
    // Set the default field values
    setDefaultFieldValues();
}

/**
 * Get the default metadata for this object
 */
UAVObject::Metadata PositionActual::getDefaultMetadata()
{
    UAVObject::Metadata metadata;
    metadata.flightAccess = ACCESS_READWRITE;
    metadata.gcsAccess = ACCESS_READWRITE;
    metadata.gcsTelemetryAcked = 0;
    metadata.gcsTelemetryUpdateMode = UAVObject::UPDATEMODE_MANUAL;
    metadata.gcsTelemetryUpdatePeriod = 0;
    metadata.flightTelemetryAcked = 0;
    metadata.flightTelemetryUpdateMode = UAVObject::UPDATEMODE_PERIODIC;
    metadata.flightTelemetryUpdatePeriod = 1000;
    metadata.loggingUpdateMode = UAVObject::UPDATEMODE_PERIODIC;
    metadata.loggingUpdatePeriod = 1000;
    return metadata;
}

/**
 * Initialize object fields with the default values.
 * If a default value is not specified the object fields
 * will be initialized to zero.
 */
void PositionActual::setDefaultFieldValues()
{

}

/**
 * Get the object data fields
 */
PositionActual::DataFields PositionActual::getData()
{
    QMutexLocker locker(mutex);
    return data;
}

/**
 * Set the object data fields
 */
void PositionActual::setData(const DataFields& data)
{
    QMutexLocker locker(mutex);
    // Get metadata
    Metadata mdata = getMetadata();
    // Update object if the access mode permits
    if ( mdata.gcsAccess == ACCESS_READWRITE )
    {
        this->data = data;
        emit objectUpdatedAuto(this); // trigger object updated event
        emit objectUpdated(this);
    }
}

/**
 * Create a clone of this object, a new instance ID must be specified.
 * Do not use this function directly to create new instances, the
 * UAVObjectManager should be used instead.
 */
UAVDataObject* PositionActual::clone(quint32 instID)
{
    PositionActual* obj = new PositionActual();
    obj->initialize(instID, this->getMetaObject());
    return obj;
}

/**
 * Static function to retrieve an instance of the object.
 */
PositionActual* PositionActual::GetInstance(UAVObjectManager* objMngr, quint32 instID)
{
    return dynamic_cast<PositionActual*>(objMngr->getObject(PositionActual::OBJID, instID));
}
