/**
 ******************************************************************************
 *
 * @file       gcstelemetrystats.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Implementation of the GCSTelemetryStats object. This file has been 
 *             automatically generated by the UAVObjectGenerator.
 * 
 * @note       Object definition file: gcstelemetrystats.xml. 
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
#ifndef GCSTELEMETRYSTATS_H
#define GCSTELEMETRYSTATS_H

#include "uavdataobject.h"

class UAVOBJECTS_EXPORT GCSTelemetryStats: public UAVDataObject
{
    Q_OBJECT

public:
    // Field structure
    typedef struct {
        quint8 Connected;
        float TxDataRate;
        float RxDataRate;
        quint32 TxFailures;
        quint32 RxFailures;
        quint32 TxRetries;

    } __attribute__((packed)) DataFields;

    // Enumeration types
    /* Enumeration options for field Connected */
    typedef enum { CONNECTED_TRUE=0, CONNECTED_FALSE=1,  } ConnectedOptions;

  
    // Constants
    static const quint32 OBJID = 607270704U;
    static const QString NAME;
    static const bool ISSINGLEINST = 1;
    static const bool ISSETTINGS = 0;
    static const quint32 NUMBYTES = sizeof(DataFields);

    // Functions
    GCSTelemetryStats();

    DataFields getData();
    void setData(DataFields& data);
    Metadata getDefaultMetadata();
    UAVDataObject* clone(quint32 instID);
	
private:
    DataFields data;

};

#endif // GCSTELEMETRYSTATS_H
