/**
 ******************************************************************************
 *
 * @file       uavobjectmanager.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Object manager library. This library holds a collection of all objects.
 *             It can be used by all modules/libraries to find an object reference.
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

// Constants

// Private types

/**
 * List of event queues and the eventmask associated with the queue.
 */
struct ObjectQueueListStruct {
	xQueueHandle queue;
	UAVObjEventCallback cb;
	int32_t eventMask;
    struct ObjectQueueListStruct* next;
};
typedef struct ObjectQueueListStruct ObjectQueueList;

/**
 * List of object instances, holds the actual data structure and instance ID
 */
struct ObjectInstListStruct {
	void* data;
	uint16_t instId;
    struct ObjectInstListStruct* next;
};
typedef struct ObjectInstListStruct ObjectInstList;

/**
 * List of objects registered in the object manager
 */
struct ObjectListStruct {
	uint32_t id; /** The object ID */
	const char* name; /** The object name */
	int8_t isMetaobject; /** Set to 1 if this is a metaobject */
	int8_t isSingleInstance; /** Set to 1 if this object has a single instance */
	int8_t isSettings; /** Set to 1 if this object is a settings object */
	uint16_t numBytes; /** Number of data bytes contained in the object (for a single instance) */
	uint16_t numInstances; /** Number of instances */
	struct ObjectListStruct* linkedObj; /** Linked object, for regular objects this is the metaobject and for metaobjects it is the parent object */
	ObjectInstList* instances; /** List of object instances, instance 0 always exists */
	ObjectQueueList* queues; /** Event queues registered on the object */
    struct ObjectListStruct* next; /** Needed by linked list library (utlist.h) */
};
typedef struct ObjectListStruct ObjectList;

// Private functions
int32_t sendEvent(ObjectList* obj, uint16_t instId, UAVObjEventType event);
ObjectInstList* createInstance(ObjectList* obj, uint16_t instId);
ObjectInstList* getInstance(ObjectList* obj, uint16_t instId);
int32_t connectObj(UAVObjHandle obj, xQueueHandle queue, UAVObjEventCallback cb, int32_t eventMask);
int32_t disconnectObj(UAVObjHandle obj, xQueueHandle queue, UAVObjEventCallback cb);

// Private variables
ObjectList* objList;
xSemaphoreHandle mutex;
UAVObjMetadata defMetadata;

/**
 * Initialize the object manager
 * \return 0 Success
 * \return -1 Failure
 */
int32_t UAVObjInitialize()
{
	// Initialize object list
	objList = NULL;

	// Create mutex
	mutex = xSemaphoreCreateRecursiveMutex();
	if (mutex == NULL)
		return -1;

	// Initialize default metadata structure (metadata of metaobjects)
	defMetadata.telemetryAcked = 1;
	defMetadata.telemetryUpdateMode = UPDATEMODE_ONCHANGE;
	defMetadata.telemetryUpdatePeriod = 0;
	defMetadata.gcsTelemetryAcked = 1;
	defMetadata.gcsTelemetryUpdateMode = UPDATEMODE_ONCHANGE;
	defMetadata.gcsTelemetryUpdatePeriod = 0;
	defMetadata.loggingUpdateMode = UPDATEMODE_ONCHANGE;
	defMetadata.loggingUpdatePeriod = 0;

	// Done
	return 0;
}

/**
 * Register and new object in the object manager.
 * \param[in] id Unique object ID
 * \param[in] name Object name
 * \param[in] isMetaobject Is this a metaobject (1:true, 0:false)
 * \param[in] isSingleInstance Is this a single instance or multi-instance object
 * \param[in] isSettings Is this a settings object
 * \param[in] numBytes Number of bytes of object data (for one instance)
 * \return Object handle, or 0 if failure.
 * \return
 */
UAVObjHandle UAVObjRegister(uint32_t id, const char* name, int32_t isMetaobject,
		int32_t isSingleInstance, int32_t isSettings, uint32_t numBytes)
{
	ObjectList* objEntry;
	ObjectInstList* instEntry;
	ObjectList* metaObj;

	// Get lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Check that the object is not already registered
	LL_FOREACH(objList, objEntry)
	{
		if (objEntry->id == id)
		{
			// Already registered, ignore
			xSemaphoreGiveRecursive(mutex);
			return -1;
		}
	}

	// Create and append entry
	objEntry = (ObjectList*)pvPortMalloc(sizeof(ObjectList));
	if (objEntry == NULL)
	{
		xSemaphoreGiveRecursive(mutex);
		return 0;
	}
	objEntry->id = id;
	objEntry->name = name;
	objEntry->isMetaobject = (int8_t)isMetaobject;
	objEntry->isSingleInstance = (int8_t)isSingleInstance;
	objEntry->isSettings = (int8_t)isSettings;
	objEntry->numBytes = numBytes;
	objEntry->queues = NULL;
	objEntry->numInstances = 0;
	objEntry->instances = NULL;
	objEntry->linkedObj = NULL; // will be set later
	LL_APPEND(objList, objEntry);

	// Create instance zero
	instEntry = createInstance(objEntry, objEntry->numInstances);
	if ( instEntry == NULL )
	{
		xSemaphoreGiveRecursive(mutex);
		return 0;
	}

	// Create metaobject and update linkedObj
	if (isMetaobject)
	{
		objEntry->linkedObj = NULL; // will be set later
	}
	else
	{
		// Create metaobject
		metaObj = (ObjectList*)UAVObjRegister(id+1, NULL, 1, 1, 0, sizeof(UAVObjMetadata));
		// Link two objects
		objEntry->linkedObj = metaObj;
		metaObj->linkedObj = objEntry;
	}

	// If this is a settings object attempt to load its data from the file system
	if ( objEntry->isSettings )
	{
		UAVObjLoad( (UAVObjHandle)objEntry, 0 );
	}

	// Release lock
	xSemaphoreGiveRecursive(mutex);
	return (UAVObjHandle)objEntry;
}

/**
 * Retrieve an object from the list given its id
 * \param[in] The object ID
 * \return The object or 0 if not found.
 */
UAVObjHandle UAVObjGetByID(uint32_t id)
{
	ObjectList* objEntry;

	// Get lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Look for object
	LL_FOREACH(objList, objEntry)
	{
		if (objEntry->id == id)
		{
			// Release lock
			xSemaphoreGiveRecursive(mutex);
			// Done, object found
			return (UAVObjHandle)objEntry;
		}
	}

	// Object not found, release lock and return error
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Retrieve an object from the list given its name
 * \param[in] name The name of the object
 * \return The object or 0 if not found.
 */
UAVObjHandle UAVObjGetByName(char* name)
{
	ObjectList* objEntry;

	// Get lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Look for object
	LL_FOREACH(objList, objEntry)
	{
		if (objEntry->name != NULL && strcmp(objEntry->name, name) == 0)
		{
			// Release lock
			xSemaphoreGiveRecursive(mutex);
			// Done, object found
			return (UAVObjHandle)objEntry;
		}
	}

	// Object not found, release lock and return error
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Get the object's ID
 * \param[in] obj The object handle
 * \return The object ID
 */
uint32_t UAVObjGetID(UAVObjHandle obj)
{
	return ((ObjectList*)obj)->id;
}

/**
 * Get the object's name
 * \param[in] obj The object handle
 * \return The object's name
 */
const char* UAVObjGetName(UAVObjHandle obj)
{
	return ((ObjectList*)obj)->name;
}

/**
 * Get the number of bytes of the object's data (for one instance)
 * \param[in] obj The object handle
 * \return The number of bytes
 */
uint32_t UAVObjGetNumBytes(UAVObjHandle obj)
{
	return ((ObjectList*)obj)->numBytes;
}

/**
 * Get the object this object is linked to. For regular objects, the linked object
 * is the metaobject. For metaobjects the linked object is the parent object.
 * This function is normally only needed by the telemetry module.
 * \param[in] obj The object handle
 * \return The object linked object handle
 */
UAVObjHandle UAVObjGetLinkedObj(UAVObjHandle obj)
{
	return (UAVObjHandle)(((ObjectList*)obj)->linkedObj);
}

/**
 * Get the number of instances contained in the object.
 * \param[in] obj The object handle
 * \return The number of instances
 */
uint32_t UAVObjGetNumInstances(UAVObjHandle obj)
{
	uint32_t numInstances;
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	numInstances = ((ObjectList*)obj)->numInstances;
	xSemaphoreGiveRecursive(mutex);
	return numInstances;
}

/**
 * Create a new instance in the object.
 * \param[in] obj The object handle
 * \return The instance ID or -1 if an error
 */
int32_t UAVObjCreateInstance(UAVObjHandle obj)
{
	ObjectList* objEntry;
	ObjectInstList* instEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Create new instance
	objEntry = (ObjectList*)obj;
	instEntry = createInstance(objEntry, objEntry->numInstances);
	if ( instEntry == NULL )
	{
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return instEntry->instId;
}

/**
 * Does this object contains a single instance or multiple instances?
 * \param[in] obj The object handle
 * \return True (1) if this is a single instance object
 */
int32_t UAVObjIsSingleInstance(UAVObjHandle obj)
{
	return ((ObjectList*)obj)->isSingleInstance;
}

/**
 * Is this a metaobject?
 * \param[in] obj The object handle
 * \return True (1) if this is metaobject
 */
int32_t UAVObjIsMetaobject(UAVObjHandle obj)
{
	return ((ObjectList*)obj)->isMetaobject;
}

/**
 * Is this a settings object?
 * \param[in] obj The object handle
 * \return True (1) if this is a settings object
 */
int32_t UAVObjIsSettings(UAVObjHandle obj)
{
	return ((ObjectList*)obj)->isSettings;
}

/**
 * Initialize object data from a string (usually stored as a settings file)
 * \param[in] obj The object handle
 * \param[in] init Text with initialization information (settings file)
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjInitData(UAVObjHandle obj, const char* init)
{
	// TODO: Implement object data initialization from string (settings)
	return -1;
}

/**
 * Unpack an object from a byte array
 * \param[in] obj The object handle
 * \param[in] instId The instance ID
 * \param[in] dataIn The byte array
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjUnpack(UAVObjHandle obj, uint16_t instId, const uint8_t* dataIn)
{
	ObjectList* objEntry;
	ObjectInstList* instEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast handle to object
	objEntry = (ObjectList*)obj;

	// Get the instance
	instEntry = getInstance(objEntry, instId);

	// If the instance does not exist create it and any other instances before it
	if ( instEntry == NULL )
	{
		instEntry = createInstance(objEntry, instId);
		if ( instEntry == NULL )
		{
			// Error, unlock and return
			xSemaphoreGiveRecursive(mutex);
			return -1;
		}
	}

	// Set the data
	memcpy(instEntry->data, dataIn, objEntry->numBytes);

	// Fire event
	sendEvent(objEntry, instId, EV_UNPACKED);

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Pack an object to a byte array
 * \param[in] obj The object handle
 * \param[in] instId The instance ID
 * \param[out] dataOut The byte array
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjPack(UAVObjHandle obj, uint16_t instId, uint8_t* dataOut)
{
	ObjectList* objEntry;
	ObjectInstList* instEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast handle to object
	objEntry = (ObjectList*)obj;

	// Get the instance
	instEntry = getInstance(objEntry, instId);
	if ( instEntry == NULL )
	{
		// Error, unlock and return
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Pack data
	memcpy(dataOut, instEntry->data, objEntry->numBytes);

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Save the data of the specified object instance to the file system (SD card).
 * The object will be appended and the file will not be closed.
 * The object data can be restored using the UAVObjLoad function.
 * @param[in] obj The object handle.
 * @param[in] instId The instance ID
 * @param[in] file File to append to
 * @return 0 if success or -1 if failure
 */
int32_t UAVObjSaveToFile(UAVObjHandle obj, uint16_t instId, FILEINFO* file)
{
	uint32_t bytesWritten;
	ObjectList* objEntry;
	ObjectInstList* instEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast to object
	objEntry = (ObjectList*)obj;

	// Get the instance information
	instEntry = getInstance(objEntry, instId);
	if ( instEntry == NULL )
	{
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Write the object ID
	DFS_WriteFile(file, PIOS_SDCARD_Sector, (uint8_t*)&objEntry->id, &bytesWritten, 4);

	// Write the instance ID
	if (!objEntry->isSingleInstance)
	{
		DFS_WriteFile(file, PIOS_SDCARD_Sector, (uint8_t*)&instEntry->instId, &bytesWritten, 2);
	}

	// Write the data and check that the write was successful
	DFS_WriteFile(file, PIOS_SDCARD_Sector, instEntry->data, &bytesWritten, objEntry->numBytes);
	if ( bytesWritten != objEntry->numBytes )
	{
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Done
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Save the data of the specified object to the file system (SD card).
 * If the object contains multiple instances, all of them will be saved.
 * A new file with the name of the object will be created.
 * The object data can be restored using the UAVObjLoad function.
 * @param[in] obj The object handle.
 * @param[in] instId The instance ID
 * @param[in] file File to append to
 * @return 0 if success or -1 if failure
 */
int32_t UAVObjSave(UAVObjHandle obj, uint16_t instId)
{
	FILEINFO file;
	ObjectList* objEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast to object
	objEntry = (ObjectList*)obj;

	// Open file
	if ( DFS_OpenFile(&PIOS_SDCARD_VolInfo, (uint8_t *)objEntry->name, DFS_WRITE, PIOS_SDCARD_Sector, &file) != DFS_OK )
	{
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Append object
	if ( UAVObjSaveToFile(obj, instId, &file) == -1 )
	{
		DFS_Close(&file);
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Done, close file and unlock
	DFS_Close(&file);
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Load an object from the file system (SD card).
 * @param[in] file File to read from
 * @return The handle of the object loaded or 0 if a failure
 */
UAVObjHandle UAVObjLoadFromFile(FILEINFO* file)
{
	uint32_t bytesRead;
	ObjectList* objEntry;
	ObjectInstList* instEntry;
	uint32_t objId;
	uint16_t instId;
	UAVObjHandle obj;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Read the object ID
	if ( DFS_ReadFile(file, PIOS_SDCARD_Sector, (uint8_t*)&objId, &bytesRead, 4) != DFS_OK )
	{
		xSemaphoreGiveRecursive(mutex);
		return 0;
	}

	// Get the object
	obj = UAVObjGetByID(objId);
	if ( obj == 0 )
	{
		xSemaphoreGiveRecursive(mutex);
		return 0;
	}
	objEntry = (ObjectList*)obj;

	// Get the instance ID
	instId = 0;
	if ( !objEntry->isSingleInstance )
	{
		if ( DFS_ReadFile(file, PIOS_SDCARD_Sector, (uint8_t*)&instId, &bytesRead, 2) != DFS_OK )
		{
			xSemaphoreGiveRecursive(mutex);
			return 0;
		}
	}

	// Get the instance information
	instEntry = getInstance(objEntry, instId);

	// If the instance does not exist create it and any other instances before it
	if ( instEntry == NULL )
	{
		instEntry = createInstance(objEntry, instId);
		if ( instEntry == NULL )
		{
			// Error, unlock and return
			xSemaphoreGiveRecursive(mutex);
			return 0;
		}
	}

	// Read the instance data
	if ( DFS_ReadFile(file, PIOS_SDCARD_Sector, (uint8_t*)instEntry->data, &bytesRead, objEntry->numBytes) != DFS_OK )
	{
		xSemaphoreGiveRecursive(mutex);
		return 0;
	}

	// Fire event
	sendEvent(objEntry, instId, EV_UNPACKED);

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return obj;
}

/**
 * Load an object from the file system (SD card).
 * A file with the name of the object will be opened.
 * The object data can be saved using the UAVObjSave function.
 * @param[in] obj The object handle.
 * @param[in] instId The object instance
 * @return 0 if success or -1 if failure
 */
int32_t UAVObjLoad(UAVObjHandle obj, uint16_t instId)
{
	FILEINFO file;
	ObjectList* objEntry;
	UAVObjHandle loadedObj;
	ObjectList* loadedObjEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast to object
	objEntry = (ObjectList*)obj;

	// Open file
	if ( DFS_OpenFile(&PIOS_SDCARD_VolInfo, (uint8_t *)objEntry->name, DFS_WRITE, PIOS_SDCARD_Sector, &file) != DFS_OK )
	{
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Load object
	loadedObj = UAVObjLoadFromFile(&file);
	if (loadedObj == 0)
	{
		DFS_Close(&file);
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Check that the IDs match
	loadedObjEntry = (ObjectList*)loadedObj;
	if ( loadedObjEntry->id != objEntry->id )
	{
		DFS_Close(&file);
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Done, close file and unlock
	DFS_Close(&file);
	xSemaphoreGiveRecursive(mutex);
	return 0;
}


/**
 * Set the object data
 * \param[in] obj The object handle
 * \param[in] dataIn The object's data structure
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjSetData(UAVObjHandle obj, const void* dataIn)
{
	return UAVObjSetInstanceData(obj, 0, dataIn);
}

/**
 * Get the object data
 * \param[in] obj The object handle
 * \param[out] dataOut The object's data structure
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjGetData(UAVObjHandle obj, void* dataOut)
{
	return UAVObjGetInstanceData(obj, 0, dataOut);
}

/**
 * Set the data of a specific object instance
 * \param[in] obj The object handle
 * \param[in] instId The object instance ID
 * \param[in] dataIn The object's data structure
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjSetInstanceData(UAVObjHandle obj, uint16_t instId, const void* dataIn)
{
	ObjectList* objEntry;
	ObjectInstList* instEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast to object info
	objEntry = (ObjectList*)obj;

	// Get instance information
	instEntry = getInstance(objEntry, instId);
	if ( instEntry == NULL )
	{
		// Error, unlock and return
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Set data
	memcpy(instEntry->data, dataIn, objEntry->numBytes);

	// Fire event
	sendEvent(objEntry, instId, EV_UPDATED);

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Get the data of a specific object instance
 * \param[in] obj The object handle
 * \param[in] instId The object instance ID
 * \param[out] dataOut The object's data structure
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjGetInstanceData(UAVObjHandle obj, uint16_t instId, void* dataOut)
{
	ObjectList* objEntry;
	ObjectInstList* instEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Cast to object info
	objEntry = (ObjectList*)obj;

	// Get instance information
	instEntry = getInstance(objEntry, instId);
	if ( instEntry == NULL )
	{
		// Error, unlock and return
		xSemaphoreGiveRecursive(mutex);
		return -1;
	}

	// Set data
	memcpy(dataOut, instEntry->data, objEntry->numBytes);

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Set the object metadata
 * \param[in] obj The object handle
 * \param[in] dataIn The object's metadata structure
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjSetMetadata(UAVObjHandle obj, const UAVObjMetadata* dataIn)
{
	ObjectList* objEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Set metadata
	objEntry = (ObjectList*)obj;
	if (objEntry->isMetaobject)
	{
		memcpy(&defMetadata, dataIn, sizeof(UAVObjMetadata));
	}
	else
	{
		UAVObjSetData((UAVObjHandle)objEntry->linkedObj, dataIn);
	}

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Get the object metadata
 * \param[in] obj The object handle
 * \param[out] dataOut The object's metadata structure
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjGetMetadata(UAVObjHandle obj, UAVObjMetadata* dataOut)
{
	ObjectList* objEntry;

	// Lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Get metadata
	objEntry = (ObjectList*)obj;
	if (objEntry->isMetaobject)
	{
		memcpy(dataOut, &defMetadata, sizeof(UAVObjMetadata));
	}
	else
	{
		UAVObjGetData((UAVObjHandle)objEntry->linkedObj, dataOut);
	}

	// Unlock
	xSemaphoreGiveRecursive(mutex);
	return 0;
}

/**
 * Connect an event queue to the object, if the queue is already connected then the event mask is only updated.
 * All events matching the event mask will be pushed to the event queue.
 * \param[in] obj The object handle
 * \param[in] queue The event queue
 * \param[in] eventMask The event mask, if EV_MASK_ALL then all events are enabled (e.g. EV_UPDATED | EV_UPDATED_MANUAL)
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjConnectQueue(UAVObjHandle obj, xQueueHandle queue, int32_t eventMask)
{
	int32_t res;
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	res = connectObj(obj, queue, 0, eventMask);
	xSemaphoreGiveRecursive(mutex);
	return res;
}

/**
 * Disconnect an event queue from the object.
 * \param[in] obj The object handle
 * \param[in] queue The event queue
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjDisconnectQueue(UAVObjHandle obj, xQueueHandle queue)
{
	int32_t res;
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	res = disconnectObj(obj, queue, 0);
	xSemaphoreGiveRecursive(mutex);
	return res;
}

/**
 * Connect an event callback to the object, if the callback is already connected then the event mask is only updated.
 * The supplied callback will be invoked on all events matching the event mask.
 * \param[in] obj The object handle
 * \param[in] cb The event callback
 * \param[in] eventMask The event mask, if EV_MASK_ALL then all events are enabled (e.g. EV_UPDATED | EV_UPDATED_MANUAL)
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjConnectCallback(UAVObjHandle obj, UAVObjEventCallback cb, int32_t eventMask)
{
	int32_t res;
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	res = connectObj(obj, 0, cb, eventMask);
	xSemaphoreGiveRecursive(mutex);
	return res;
}

/**
 * Disconnect an event callback from the object.
 * \param[in] obj The object handle
 * \param[in] cb The event callback
 * \return 0 if success or -1 if failure
 */
int32_t UAVObjDisconnectCallback(UAVObjHandle obj, UAVObjEventCallback cb)
{
	int32_t res;
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	res = disconnectObj(obj, 0, cb);
	xSemaphoreGiveRecursive(mutex);
	return res;
}


/**
 * Request an update of the object's data from the GCS. The call will not wait for the response, a EV_UPDATED event
 * will be generated as soon as the object is updated.
 * \param[in] obj The object handle
 */
void UAVObjRequestUpdate(UAVObjHandle obj)
{
	UAVObjRequestInstanceUpdate(obj, UAVOBJ_ALL_INSTANCES);
}

/**
 * Request an update of the object's data from the GCS. The call will not wait for the response, a EV_UPDATED event
 * will be generated as soon as the object is updated.
 * \param[in] obj The object handle
 * \param[in] instId Object instance ID to update
 */
void UAVObjRequestInstanceUpdate(UAVObjHandle obj, uint16_t instId)
{
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	sendEvent((ObjectList*)obj, instId, EV_UPDATE_REQ);
	xSemaphoreGiveRecursive(mutex);
}

/**
 * Send the object's data to the GCS (triggers a EV_UPDATED_MANUAL event on this object).
 * \param[in] obj The object handle
 */
void UAVObjUpdated(UAVObjHandle obj)
{
	UAVObjInstanceUpdated(obj, UAVOBJ_ALL_INSTANCES);
}

/**
 * Send the object's data to the GCS (triggers a EV_UPDATED_MANUAL event on this object).
 * \param[in] obj The object handle
 * \param[in] instId The object instance ID
 */
void UAVObjInstanceUpdated(UAVObjHandle obj, uint16_t instId)
{
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
	sendEvent((ObjectList*)obj, instId, EV_UPDATED);
	xSemaphoreGiveRecursive(mutex);
}

/**
 * Iterate through all objects in the list.
 * \param iterator This function will be called once for each object,
 * the object will be passed as a parameter
 */
void UAVObjIterate(void (*iterator)(UAVObjHandle obj))
{
	ObjectList* objEntry;

	// Get lock
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);

	// Iterate through the list and invoke iterator for each object
	LL_FOREACH(objList, objEntry)
	{
		(*iterator)((UAVObjHandle)objEntry);
	}

	// Release lock
	xSemaphoreGiveRecursive(mutex);
}

/**
 * Send an event to all event queues registered on the object.
 */
int32_t sendEvent(ObjectList* obj, uint16_t instId, UAVObjEventType event)
{
	ObjectQueueList* queueEntry;
	UAVObjEvent msg;

	// Setup event
	msg.obj = (UAVObjHandle)obj;
	msg.event = event;
	msg.instId = instId;

	// Go through each object and push the event message in the queue (if event is activated for the queue)
    LL_FOREACH(obj->queues, queueEntry)
	{
    	if ( queueEntry->eventMask == 0 || (queueEntry->eventMask & event) != 0 )
    	{
    		// Send to queue if a valid queue is registered
    		if (queueEntry->queue != 0)
    		{
    			xQueueSend(queueEntry->queue, &msg, 0); // do not wait if queue is full
    		}
    		// Invoke callback (from event task) if a valid one is registered
    		if (queueEntry->cb != 0)
    		{
    			EventDispatch(&msg, queueEntry->cb); // invoke callback from the event task
    		}
    	}
    }

    // Done
    return 0;
}

/**
 * Create a new object instance, return the instance info or NULL if failure.
 */
ObjectInstList* createInstance(ObjectList* obj, uint16_t instId)
{
	ObjectInstList* instEntry;
	int32_t n;

	// For single instance objects, only instance zero is allowed
	if (obj->isSingleInstance && instId != 0)
	{
		return NULL;
	}

	// Make sure that the instance ID is within limits
	if (instId >= UAVOBJ_MAX_INSTANCES)
	{
		return NULL;
	}

	// Check if the instance already exists
	if ( getInstance(obj, instId) != NULL )
	{
		return NULL;
	}

	// Create any missing instances (all instance IDs must be sequential)
	for (n = obj->numInstances; n < instId; ++n)
	{
		if ( createInstance(obj, n) == NULL )
		{
			return NULL;
		}
	}

	// Create the actual instance
	instEntry = (ObjectInstList*)pvPortMalloc(sizeof(ObjectInstList));
	if (instEntry == NULL) return NULL;
	instEntry->data = pvPortMalloc(obj->numBytes);
	if (instEntry->data == NULL) return NULL;
	memset(instEntry->data, 0, obj->numBytes);
	instEntry->instId = instId;
	LL_APPEND(obj->instances, instEntry);
	++obj->numInstances;

	// Fire event
	UAVObjInstanceUpdated((UAVObjHandle)obj, instId);

	// Done
	return instEntry;
}

/**
 * Get the instance information or NULL if the instance does not exist
 */
ObjectInstList* getInstance(ObjectList* obj, uint16_t instId)
{
	ObjectInstList* instEntry;

	// Look for specified instance ID
	LL_FOREACH(obj->instances, instEntry)
	{
		if (instEntry->instId == instId)
		{
			return instEntry;
		}
	}
	// If this point is reached then instance id was not found
	return NULL;
}

/**
 * Connect an event queue to the object, if the queue is already connected then the event mask is only updated.
 * \param[in] obj The object handle
 * \param[in] queue The event queue
 * \param[in] cb The event callback
 * \param[in] eventMask The event mask, if EV_MASK_ALL then all events are enabled (e.g. EV_UPDATED | EV_UPDATED_MANUAL)
 * \return 0 if success or -1 if failure
 */
int32_t connectObj(UAVObjHandle obj, xQueueHandle queue, UAVObjEventCallback cb, int32_t eventMask)
{
	ObjectQueueList* queueEntry;
	ObjectList* objEntry;

	// Check that the queue is not already connected, if it is simply update event mask
	objEntry = (ObjectList*)obj;
	LL_FOREACH(objEntry->queues, queueEntry)
	{
		if ( queueEntry->queue == queue && queueEntry->cb == cb )
		{
			// Already connected, update event mask and return
			queueEntry->eventMask = eventMask;
			return 0;
		}
	}

	// Add queue to list
	queueEntry = (ObjectQueueList*)pvPortMalloc(sizeof(ObjectQueueList));
	if (queueEntry == NULL)
	{
		return -1;
	}
	queueEntry->queue = queue;
	queueEntry->cb = cb;
	queueEntry->eventMask = eventMask;
	LL_APPEND(objEntry->queues, queueEntry);

	// Done
	return 0;
}

/**
 * Disconnect an event queue from the object
 * \param[in] obj The object handle
 * \param[in] queue The event queue
 * \param[in] cb The event callback
 * \return 0 if success or -1 if failure
 */
int32_t disconnectObj(UAVObjHandle obj, xQueueHandle queue, UAVObjEventCallback cb)
{
	ObjectQueueList* queueEntry;
	ObjectList* objEntry;

	// Find queue and remove it
	objEntry = (ObjectList*)obj;
	LL_FOREACH(objEntry->queues, queueEntry)
	{
		if ( ( queueEntry->queue == queue && queueEntry->cb == cb ) )
		{
			LL_DELETE(objEntry->queues, queueEntry);
			vPortFree(queueEntry);
			return 0;
		}
	}

	// If this point is reached the queue was not found
	return -1;
}
















