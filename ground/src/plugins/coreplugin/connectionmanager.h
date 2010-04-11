/**
 ******************************************************************************
 *
 * @file       connectionmanager.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 *             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
 * @brief
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   coreplugin
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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QWidget>

#include <QtCore/QVector>
#include <QtCore/QIODevice>
#include <QtCore/QLinkedList>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>

#include "core_global.h"

namespace Core {

    class IConnection;

namespace Internal {
    class FancyTabWidget;
    class FancyActionBar;
    class MainWindow;
} // namespace Internal


struct devListItem
{
    IConnection *connection;
    QString devName;
    QString displayedName;
};


class CORE_EXPORT ConnectionManager : public QWidget
{
    Q_OBJECT

public:
    ConnectionManager(Internal::MainWindow *mainWindow, Internal::FancyTabWidget *modeStack);
    virtual ~ConnectionManager();

    void init();

    QIODevice *getCurrentConnection();

protected:
    void unregisterAll(IConnection *connection);
    void registerDevice(IConnection *conn, const QString &devN, const QString &disp);
    devListItem findDevice(const QString &displayedName);

signals:
    void deviceConnected(QIODevice *dev);
    void deviceDisconnected();

private slots:
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);

    void onConnectPressed();
    void devChanged(IConnection *connection);

protected:
    QComboBox *m_availableDevList;
    QPushButton *m_connectBtn;
    QLinkedList<devListItem> m_devList;

    //currently connected connection plugin
    devListItem m_connectionDevice;

    //currently connected QIODevice
    QIODevice *m_ioDev;
};

} //namespace Core

#endif // CONNECTIONMANAGER_H
