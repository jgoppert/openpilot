/**
 ******************************************************************************
 *
 * @file       notifyplugin.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   notifyplugin
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
#ifndef SOUNDNOTIFYPLUGIN_H
#define SOUNDNOTIFYPLUGIN_H

#include <extensionsystem/iplugin.h> 
#include "uavtalk/telemetrymanager.h"
#include "uavobjects/uavobjectmanager.h"
#include "uavobjects/uavobject.h"

#include <QSettings>
#include <phonon>

class NotifyPluginOptionsPage;
class NotifyPluginConfiguration;

typedef struct {
	Phonon::MediaObject* mo;
	QList<Phonon::MediaSource>* ms;
	NotifyPluginConfiguration* notify;
        bool firstPlay;
} PhononObject, *pPhononObject;

class SoundNotifyPlugin : public ExtensionSystem::IPlugin
{ 
	Q_OBJECT
public: 
   SoundNotifyPlugin();
   ~SoundNotifyPlugin();

   void extensionsInitialized(); 
   bool initialize(const QStringList & arguments, QString * errorString); 
   void shutdown();


   QList<NotifyPluginConfiguration*> getListNotifications() { return lstNotifications; }
   //void setListNotifications(QList<NotifyPluginConfiguration*>& list_notify) {  }

   bool getEnableSound() const { return enableSound; }
   void setEnableSound(bool value) {enableSound = value; }



private:
   bool enableSound;
   QList< QList<Phonon::MediaSource>* > lstMediaSource;
   QStringList mediaSource;
   //QMap<QString, Phonon::MediaObject*> mapMediaObjects;
   QMultiMap<QString, PhononObject> mapMediaObjects;

   QSettings* settings;

   QList<UAVDataObject*> lstNotifiedUAVObjects;

   QList<NotifyPluginConfiguration*> lstNotifications;
   QList<NotifyPluginConfiguration*> pendingNotifications;
   QList<NotifyPluginConfiguration*> removedNotifies;

   NotifyPluginConfiguration* nowPlayingConfiguration;

   QString m_field;
   PhononObject phonon;
   NotifyPluginOptionsPage *mop;
   TelemetryManager* telMngr;

   bool playNotification(NotifyPluginConfiguration* notification);
   void checkNotificationRule(NotifyPluginConfiguration* notification, UAVObject* object);

private slots:
   void onTelemetryManagerAdded(QObject* obj);
   void onAutopilotDisconnect();
   void connectNotifications();
   void updateNotificationList(QList<NotifyPluginConfiguration*> list);
   void resetNotification(void);
   void appendNotification(UAVObject *object);
   void repeatTimerHandler(void);
   void expireTimerHandler(void);
   void stateChanged(Phonon::State newstate, Phonon::State oldstate);
}; 

#endif // SOUNDNOTIFYPLUGIN_H
