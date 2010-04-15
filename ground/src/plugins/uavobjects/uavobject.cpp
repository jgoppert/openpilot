/**
 ******************************************************************************
 *
 * @file       uavobject.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 *             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
 * @brief      
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   uavobjects_plugin
 * @{
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
#include "uavobject.h"
#include <QtEndian>

/**
 * Constructor
 * @param objID The object ID
 * @param isSingleInst True if this object can only have a single instance
 * @param name Object name
 */
UAVObject::UAVObject(quint32 objID, bool isSingleInst, const QString& name)
{
    this->objID = objID;
    this->instID = 0;
    this->isSingleInst = isSingleInst;
    this->name = name;
    this->mutex = new QMutex(QMutex::Recursive);
}

/**
 * Initialize object with its instance ID
 */
void UAVObject::initialize(quint32 instID)
{
    QMutexLocker locker(mutex);
    this->instID = instID;
}

/**
 * Initialize objects' data fields
 * @param fields List of fields held by the object
 * @param data Pointer to that actual object data, this is needed by the fields to access the data
 * @param numBytes Number of bytes in the object (total, including all fields)
 */
void UAVObject::initializeFields(QList<UAVObjectField*>& fields, quint8* data, quint32 numBytes)
{
    QMutexLocker locker(mutex);
    this->numBytes = numBytes;
    this->data = data;
    this->fields = fields;
    // Initialize fields
    quint32 offset = 0;
    for (int n = 0; n < fields.length(); ++n)
    {
        fields[n]->initialize(data, offset, this);
        offset += fields[n]->getNumBytes();
        connect(fields[n], SIGNAL(fieldUpdated(UAVObjectField*)), this, SLOT(fieldUpdated(UAVObjectField*)));
    }
}

/**
 * Called from the fields each time they are updated
 */
void UAVObject::fieldUpdated(UAVObjectField* field)
{
//    emit objectUpdatedAuto(this); // trigger object updated event
//    emit objectUpdated(this);
}

/**
 * Get the object ID
 */
quint32 UAVObject::getObjID()
{
    return objID;
}

/**
 * Get the instance ID
 */
quint32 UAVObject::getInstID()
{
    return instID;
}

/**
 * Returns true if this is a single instance object
 */
bool UAVObject::isSingleInstance()
{
    return isSingleInst;
}

/**
 * Get the name of the object
 */
QString UAVObject::getName()
{
    return name;
}

/**
 * Get the total number of bytes of the object's data
 */
quint32 UAVObject::getNumBytes()
{
    return numBytes;
}

/**
 * Request that this object is updated with the latest values from the autopilot
 */
void UAVObject::requestUpdate()
{
    emit updateRequested(this);
}

/**
 * Signal that the object has been updated
 */
void UAVObject::updated()
{
    emit objectUpdatedManual(this);
    emit objectUpdated(this);
}

/**
 * Lock mutex of this object
 */
void UAVObject::lock()
{
    mutex->lock();
}

/**
 * Lock mutex of this object
 */
void UAVObject::lock(int timeoutMs)
{
    mutex->tryLock(timeoutMs);
}

/**
 * Unlock mutex of this object
 */
void UAVObject::unlock()
{
    mutex->unlock();
}

/**
 * Get object's mutex
 */
QMutex* UAVObject::getMutex()
{
    return mutex;
}

/**
 * Get the number of fields held by this object
 */
qint32 UAVObject::getNumFields()
{
    QMutexLocker locker(mutex);
    return fields.count();
}

/**
 * Get the object's fields
 */
QList<UAVObjectField*> UAVObject::getFields()
{
    QMutexLocker locker(mutex);
    return fields;
}

/**
 * Get a specific field
 * @returns The field or NULL if not found
 */
UAVObjectField* UAVObject::getField(QString& name)
{
    QMutexLocker locker(mutex);
    // Look for field
    for (int n = 0; n < fields.length(); ++n)
    {
        if (name.compare(fields[n]->getName()) == 0)
        {
            return fields[n];
        }
    }
    // If this point is reached then the field was not found
    return NULL;
}

/**
 * Pack the object data into a byte array
 * @returns The number of bytes copied
 */
qint32 UAVObject::pack(quint8* dataOut)
{
    QMutexLocker locker(mutex);
    qint32 offset = 0;
    for (int n = 0; n < fields.length(); ++n)
    {
        fields[n]->pack(&dataOut[offset]);
        offset += fields[n]->getNumBytes();
    }
    return numBytes;
}

/**
 * Unpack the object data from a byte array
 * @returns The number of bytes copied
 */
qint32 UAVObject::unpack(const quint8* dataIn)
{
    QMutexLocker locker(mutex);
    qint32 offset = 0;
    for (int n = 0; n < fields.length(); ++n)
    {
        fields[n]->unpack(&dataIn[offset]);
        offset += fields[n]->getNumBytes();
    }
    emit objectUnpacked(this); // trigger object updated event
    emit objectUpdated(this);
    return numBytes;
}

/**
 * Save the object data to the file.
 * The file will be created in the current directory
 * and its name will be the same as the object with
 * the .uavobj extension.
 * @returns True on success, false on failure
 */
bool UAVObject::save()
{
    QMutexLocker locker(mutex);

    // Open file
    QFile file(name + ".uavobj");
    if (!file.open(QFile::WriteOnly))
    {
        return false;
    }

    // Write object
    if ( !save(file) )
    {
        return false;
    }

    // Close file
    file.close();
    return true;
}

/**
 * Save the object data to the file.
 * The file is expected to be already open for writting.
 * The data will be appended and the file will not be closed.
 * @returns True on success, false on failure
 */
bool UAVObject::save(QFile& file)
{
    QMutexLocker locker(mutex);
    quint8 buffer[numBytes];
    quint8 tmpId[4];

    // Write the object ID
    qToBigEndian<quint32>(objID, tmpId);
    if ( file.write((const char*)tmpId, 4) == -1 )
    {
        return false;
    }

    // Write the instance ID
    if (!isSingleInst)
    {
        qToBigEndian<quint16>(instID, tmpId);
        if ( file.write((const char*)tmpId, 2) == -1 )
        {
            return false;
        }
    }

    // Write the data
    pack(buffer);
    if ( file.write((const char*)buffer, numBytes) == -1 )
    {
        return false;
    }

    // Done
    return true;
}

/**
 * Load the object data from a file.
 * The file will be openned in the current directory
 * and its name will be the same as the object with
 * the .uavobj extension.
 * @returns True on success, false on failure
 */
bool UAVObject::load()
{
    QMutexLocker locker(mutex);

    // Open file
    QFile file(name + ".uavobj");
    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }

    // Load object
    if ( !load(file) )
    {
        return false;
    }

    // Close file
    file.close();
    return true;
}

/**
 * Load the object data from file.
 * The file is expected to be already open for reading.
 * The data will be read and the file will not be closed.
 * @returns True on success, false on failure
 */
bool UAVObject::load(QFile& file)
{
    QMutexLocker locker(mutex);
    quint8 buffer[numBytes];
    quint8 tmpId[4];

    // Read the object ID
    if ( file.read((char*)tmpId, 4) != 4 )
    {
        return false;
    }

    // Check that the IDs match
    if (qFromBigEndian<quint32>(tmpId) != objID)
    {
        return false;
    }

    // Read the instance ID
    if ( file.read((char*)tmpId, 2) != 2 )
    {
        return false;
    }

    // Check that the IDs match
    if (qFromBigEndian<quint16>(tmpId) != instID)
    {
        return false;
    }

    // Read and unpack the data
    if ( file.read((char*)buffer, numBytes) != numBytes )
    {
        return false;
    }
    unpack(buffer);

    // Done
    return true;
}

/**
 * Return a string with the object information
 */
QString UAVObject::toString()
{
    QString sout;
    sout.append( toStringBrief() );
    sout.append( toStringData() );
    return sout;
}

/**
 * Return a string with the object information (only the header)
 */
QString UAVObject::toStringBrief()
{
    QString sout;
    sout.append( QString("%1 (ID: %2, InstID: %3, NumBytes: %4, SInst: %5)\n")
                 .arg(getName())
                 .arg(getObjID())
                 .arg(getInstID())
                 .arg(getNumBytes())
                 .arg(isSingleInstance()) );
    return sout;
}

/**
 * Return a string with the object information (only the data)
 */
QString UAVObject::toStringData()
{
    QString sout;
    sout.append("Data:\n");
    for (int n = 0; n < fields.length(); ++n)
    {
        sout.append( QString("\t%1").arg(fields[n]->toString()) );
    }
    return sout;
}
