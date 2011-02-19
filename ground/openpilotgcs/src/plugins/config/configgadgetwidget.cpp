/**
 ******************************************************************************
 *
 * @file       configgadgetwidget.cpp
 * @author     E. Lafargue & The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup ConfigPlugin Config Plugin
 * @{
 * @brief The Configuration Gadget used to update settings in the firmware
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
#include "configgadgetwidget.h"

#include "fancytabwidget.h"
#include "configservowidget.h"
#include "configairframewidget.h"
#include "configtelemetrywidget.h"
#include "configahrswidget.h"

#include <QDebug>
#include <QStringList>
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>



ConfigGadgetWidget::ConfigGadgetWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    FancyTabWidget *ftw = new FancyTabWidget(this, true);

    ftw->setIconSize(64);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ftw);
    setLayout(layout);

    // *********************

    QWidget *qwd;

    qwd = new ConfigServoWidget(this);
    ftw->insertTab(0, qwd, QIcon(":/configgadget/images/Servo.png"), QString("RC Input/Output"));

    qwd = new ConfigAirframeWidget(this);
    ftw->insertTab(1, qwd, QIcon(":/configgadget/images/Airframe.png"), QString("Aircraft"));

    qwd = new ConfigTelemetryWidget(this);
    ftw->insertTab(2, qwd, QIcon(":/configgadget/images/XBee.svg"), QString("Telemetry"));

    qwd = new ConfigAHRSWidget(this);
    ftw->insertTab(3, qwd, QIcon(":/configgadget/images/AHRS-v1.3.png"), QString("INS"));

//    qwd = new ConfigPipXtremeWidget(this);
//    ftw->insertTab(4, qwd, QIcon(":/configgadget/images/PipXtreme.png"), QString("PipXtreme"));

    // *********************
    // Listen to autopilot connection events

    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    TelemetryManager* telMngr = pm->getObject<TelemetryManager>();
    connect(telMngr, SIGNAL(connected()), this, SLOT(onAutopilotConnect()));

    // *********************
}

ConfigGadgetWidget::~ConfigGadgetWidget()
{
   // Do nothing
}

void ConfigGadgetWidget::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
}

void ConfigGadgetWidget::onAutopilotConnect() {
    emit autopilotConnected();
}

