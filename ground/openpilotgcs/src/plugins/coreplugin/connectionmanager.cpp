/**
 ******************************************************************************
 *
 * @file       connectionmanager.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 *             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup CorePlugin Core Plugin
 * @{
 * @brief The Core GCS plugin
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

#include "connectionmanager.h"

#include <aggregation/aggregate.h>
#include <coreplugin/iconnection.h>
#include <extensionsystem/pluginmanager.h>

#include <utils/styledbar.h>

#include "fancytabwidget.h"
#include "fancyactionbar.h"
#include "mainwindow.h"
#include "qextserialport/src/qextserialenumerator.h"
#include "qextserialport/src/qextserialport.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>

namespace Core {


ConnectionManager::ConnectionManager(Internal::MainWindow *mainWindow, Internal::FancyTabWidget *modeStack) :
	QWidget(mainWindow),	// Pip
	m_availableDevList(0),
    m_connectBtn(0),
    m_ioDev(NULL)
{
    Q_UNUSED(mainWindow);

    QVBoxLayout *top = new QVBoxLayout;
    top->setSpacing(0);
    top->setMargin(0);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(5,0,5,0);
    layout->addWidget(new QLabel("Connections: "));

    m_availableDevList = new QComboBox;
    //m_availableDevList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_availableDevList->setMinimumWidth(100);
    m_availableDevList->setMaximumWidth(150);
    m_availableDevList->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_availableDevList);

    m_connectBtn = new QPushButton("Connect");
    m_connectBtn->setEnabled(false);
    layout->addWidget(m_connectBtn);

    Utils::StyledBar *bar = new Utils::StyledBar;
    bar->setLayout(layout);

    top->addWidget(bar);
    setLayout(top);

    modeStack->insertCornerWidget(modeStack->cornerWidgetCount()-1, this);

	QObject::connect(m_connectBtn, SIGNAL(pressed()), this, SLOT(onConnectPressed()));
}

ConnectionManager::~ConnectionManager()
{
	disconnectDevice();	// Pip
	suspendPolling();	// Pip
}

void ConnectionManager::init()
{
    //register to the plugin manager so we can receive
    //new connection object from plugins
	QObject::connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)), this, SLOT(objectAdded(QObject*)));
	QObject::connect(ExtensionSystem::PluginManager::instance(), SIGNAL(aboutToRemoveObject(QObject*)), this, SLOT(aboutToRemoveObject(QObject*)));
}

/**
*   Method called when the user clicks the "Connect" button
*/
bool ConnectionManager::connectDevice()
{
	devListItem connection_device = findDevice(m_availableDevList->currentText());
	if (!connection_device.connection)
		return false;

	QIODevice *io_dev = connection_device.connection->openDevice(connection_device.devName);
	if (!io_dev)
		return false;

	io_dev->open(QIODevice::ReadWrite);

	// check if opening the device worked
	if (!io_dev->isOpen())
	{
		qDebug() << "Error: could not connect to " << connection_device.devName;

		// close the device
		try
		{
			m_connectionDevice.connection->closeDevice(m_connectionDevice.devName);
		}
		catch (...)
		{	// handle exception
			qDebug() << "Exception: m_connectionDevice.connection->closeDevice(" << m_connectionDevice.devName << ")";
		}

		return false;
	}

	// we appear to have connected to the device OK

	// remember the connection/device details
	m_connectionDevice = connection_device;
	m_ioDev = io_dev;

//	connect(m_connectionDevice.connection, SIGNAL(deviceClosed(QObject *)), this, SLOT(onConnectionClosed(QObject *)));
	connect(m_connectionDevice.connection, SIGNAL(destroyed(QObject *)), this, SLOT(onConnectionDestroyed(QObject *)), Qt::QueuedConnection);

	m_connectBtn->setText("Disconnect");
	m_availableDevList->setEnabled(false);

	// signal interested plugins that we connected to the device
	emit deviceConnected(m_ioDev);

	return true;
}

/**
*   Method called by plugins who want to force a disconnection.
*   Used by Uploader gadget for instance.
*/
bool ConnectionManager::disconnectDevice()
{
	if (!m_ioDev)
	{   // apparently we are already disconnected

		m_connectionDevice.connection = NULL;
		m_ioDev = NULL;

		return false;
	}

	// we appear to be connected - disconnect from the device

	try
	{
		if (m_connectionDevice.connection)
			m_connectionDevice.connection->closeDevice(m_connectionDevice.devName);
	}
	catch (...)
	{	// handle exception
		qDebug() << "Exception: m_connectionDevice.connection->closeDevice(" << m_connectionDevice.devName << ")";
	}

	onConnectionClosed(m_connectionDevice.connection);
}

/**
*   Slot called when a plugin added an object to the core pool
*/
void ConnectionManager::objectAdded(QObject *obj)
{
    //Check if a plugin added a connection object to the pool
    IConnection *connection = Aggregation::query<IConnection>(obj);
	if (!connection) return;

    //qDebug() << "Connection object registered:" << connection->connectionName();
    //qDebug() << connection->availableDevices();

    //register devices and populate CB
    devChanged(connection);

    // Keep track of the registration to be able to tell plugins
    // to do things
    m_connectionsList.append(connection);

	QObject::connect(connection, SIGNAL(availableDevChanged(IConnection *)), this, SLOT(devChanged(IConnection *)));
}

void ConnectionManager::aboutToRemoveObject(QObject *obj)
{
    //Check if a plugin added a connection object to the pool
    IConnection *connection = Aggregation::query<IConnection>(obj);
	if (!connection) return;

	if (m_connectionDevice.connection && m_connectionDevice.connection == connection)	// Pip
	{	// we are currently using the one that is about to be removed
		m_connectionDevice.connection = NULL;
		m_ioDev = NULL;
	}

	if (m_connectionsList.contains(connection))
		m_connectionsList.removeAt(m_connectionsList.indexOf(connection));
}

void ConnectionManager::onConnectionClosed(QObject *obj)	// Pip
{
	if (!m_connectionDevice.connection || m_connectionDevice.connection != obj)
		return;

	m_connectionDevice.connection = NULL;
	m_ioDev = NULL;

	m_connectBtn->setText("Connect");
	m_availableDevList->setEnabled(true);

	// signal interested plugins that user is disconnecting his device
	emit deviceDisconnected();
}

void ConnectionManager::onConnectionDestroyed(QObject *obj)	// Pip
{
	onConnectionClosed(obj);
}

/**
*   Slot called when the user pressed the connect/disconnect button
*/
void ConnectionManager::onConnectPressed()
{
	if (!m_ioDev || !m_connectionDevice.connection)
	{	// connecting
		connectDevice();
    }
    else
	{	// disconnecting
		disconnectDevice();
    }
}

/**
*   Find a device by its displayed (visible on screen) name
*/
devListItem ConnectionManager::findDevice(const QString &displayedName)
{
	foreach (devListItem d, m_devList)
    {
		if (d.displayedName == displayedName)
            return d;
    }

    qDebug() << "findDevice: cannot find " << displayedName << " in device list";

    devListItem d;
    d.connection = NULL;
    return d;
}

/**
*   Tells every connection plugin to stop polling for devices if they
*   are doing that.
*
*/
void ConnectionManager::suspendPolling()
{
	foreach (IConnection *cnx, m_connectionsList)
	{
        cnx->suspendPolling();
    }

	m_connectBtn->setEnabled(false);
	m_availableDevList->setEnabled(false);
}

/**
*   Tells every connection plugin to resume polling for devices if they
*   are doing that.
*/
void ConnectionManager::resumePolling()
{
	foreach (IConnection *cnx, m_connectionsList)
	{
        cnx->resumePolling();
    }

	m_connectBtn->setEnabled(true);
	m_availableDevList->setEnabled(true);
}

/**
*   Unregister all devices from one connection plugin
*   \param[in] connection Connection type that you want to forget about :)
*/
void ConnectionManager::unregisterAll(IConnection *connection)
{
	for (QLinkedList<devListItem>::iterator iter = m_devList.begin(); iter != m_devList.end(); )
	{
		if (iter->connection == connection)
		{
			if (m_connectionDevice.connection && m_connectionDevice.connection == connection)
			{	// we are currently using the one we are about to erase
				onConnectionClosed(m_connectionDevice.connection);
			}

			iter = m_devList.erase(iter);
		}
		else
			++iter;
	}
}

/**
*   Register a device from a specific connection plugin
*/
void ConnectionManager::registerDevice(IConnection *conn, const QString &devN, const QString &disp)
{
    devListItem d;
    d.connection = conn;
    d.devName = devN;
    d.displayedName = disp;

    m_devList.append(d);
}

/**
*   A device plugin notified us that its device list has changed
*   (eg: user plug/unplug a usb device)
*   \param[in] connection Connection type which signaled the update
*/
void ConnectionManager::devChanged(IConnection *connection)
{
    //clear device list combobox
    m_availableDevList->clear();

    //remove registered devices of this IConnection from the list
    unregisterAll(connection);

    //and add them back in the list
    QStringList availableDev = connection->availableDevices();
	foreach (QString dev, availableDev)
    {
        QString cbName = connection->shortName() + ": " + dev;
        registerDevice(connection, dev, cbName);
    }

    //add all the list again to the combobox
	foreach (devListItem d, m_devList)
    {
        m_availableDevList->addItem(d.displayedName);
    }

    //disable connection button if the list is empty
	if (m_availableDevList->count() > 0)
        m_connectBtn->setEnabled(true);
    else
        m_connectBtn->setEnabled(false);
}

} //namespace Core
