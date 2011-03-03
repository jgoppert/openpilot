/**
 ******************************************************************************
 *
 * @file       usbmonitor.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup RawHIDPlugin Raw HID Plugin
 * @{
 * @brief Monitors the USB bus for devince insertion/removal
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

#ifndef USBMONITOR_H
#define USBMONITOR_H

//#include "rawhid_global.h"

#include <QThread>

//TODO: temporary, will have to remove
#include "pjrc_rawhid.h"

// Depending on the OS, we'll need different things:
#if defined( Q_OS_MAC)

// TODO

#elif defined(Q_OS_UNIX)

#include <libudev.h>
#include <QSocketNotifier>

#elif defined (Q_OS_WIN32)
//TODO
#endif

/*
struct USBPortInfo {
    QString friendName; ///< Friendly name.
    QString physName;
    QString enumName;   ///< It seems its the only one with meaning
    int vendorID;       ///< Vendor ID.
    int productID;      ///< Product ID
};
*/


/**
*   A monitoring thread which will wait for device events.
*/
class USBMonitor : public QThread
{
	Q_OBJECT


public:
    USBMonitor(QObject *parent = 0);
//    USBMonitor(int vid, int pid);
    ~USBMonitor();
    QList<USBPortInfo> availableDevices();
    QList<USBPortInfo> availableDevices(int vid, int pid, int bcdDevice);

signals:
    /*!
      A new device has been connected to the system.

      setUpNotifications() must be called first to enable event-driven device notifications.
      Currently only implemented on Windows and OS X.
      \param info The device that has been discovered.
    */
    void deviceDiscovered( const USBPortInfo & info );
    /*!
      A device has been disconnected from the system.

      setUpNotifications() must be called first to enable event-driven device notifications.
      Currently only implemented on Windows and OS X.
      \param info The device that was disconnected.
    */
    void deviceRemoved( const USBPortInfo & info );

private slots:
    /**
     Callback available for whenever the system that is put in place gets
     an event
     */
    void deviceEventReceived();

private:

    // Depending on the OS, we'll need different things:
#if defined( Q_OS_MAC)

// TODO

#elif defined(Q_OS_UNIX)
    struct udev *context;
    struct udev_monitor *monitor;
    QSocketNotifier *monitorNotifier;
    USBPortInfo makePortInfo(struct udev_device *dev);
#elif defined (Q_OS_WIN32)
    //TODO
#endif

};

#endif // USBMONITOR_H
