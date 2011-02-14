/**
 ******************************************************************************
 *
 * @file       configccpmtwidget.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup ConfigPlugin Config Plugin
 * @{
 * @brief ccpm configuration panel
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
#ifndef CONFIGccpmWIDGET_H
#define CONFIGccpmWIDGET_H

#include "ui_ccpm.h"
#include "configtaskwidget.h"
#include "extensionsystem/pluginmanager.h"
#include "uavobjectmanager.h"
#include "uavobject.h"
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QGraphicsSvgItem>
#include <QtGui/QWidget>
#include <QList>

#define CCPM_MAX_SWASH_SERVOS 4

class Ui_Widget;

typedef struct {
    int ServoChannels[CCPM_MAX_SWASH_SERVOS];
    int Used[CCPM_MAX_SWASH_SERVOS];
    int Max[CCPM_MAX_SWASH_SERVOS];
    int Neutral[CCPM_MAX_SWASH_SERVOS];
    int Min[CCPM_MAX_SWASH_SERVOS];
} SwashplateServoSettingsStruct;

class ConfigccpmWidget: public ConfigTaskWidget
{
    Q_OBJECT

public:
    ConfigccpmWidget(QWidget *parent = 0);
    ~ConfigccpmWidget();

private:
        Ui_ccpmWidget *m_ccpm;
        QGraphicsSvgItem *SwashplateImg;
        QGraphicsSvgItem *CurveImg;
        //QGraphicsSvgItem *ServoW;
        //QGraphicsSvgItem *ServoX;
        //QGraphicsSvgItem *ServoY;
        //QGraphicsSvgItem *ServoZ;
        //QGraphicsTextItem *ServoWText;
        //QGraphicsTextItem *ServoXText;
        //QGraphicsTextItem *ServoYText;
        //QGraphicsTextItem *ServoZText;
        QGraphicsSvgItem *Servos[CCPM_MAX_SWASH_SERVOS];
        QGraphicsTextItem *ServosText[CCPM_MAX_SWASH_SERVOS];
        QGraphicsLineItem *ServoLines[CCPM_MAX_SWASH_SERVOS];
        QSpinBox *SwashLvlSpinBoxes[CCPM_MAX_SWASH_SERVOS];

        bool SwashLvlConfigurationInProgress;
        UAVObject::Metadata SwashLvlaccInitialData;
        int SwashLvlState;
        int SwashLvlServoInterlock;

        SwashplateServoSettingsStruct oldSwashLvlConfiguration;
        SwashplateServoSettingsStruct newSwashLvlConfiguration;


        int MixerChannelData[6];
        int ShowDisclaimer(int messageID);

    private slots:
        void ccpmSwashplateUpdate();
        void ccpmSwashplateRedraw();
        void UpdateCurveSettings();
        void GenerateCurve();
        void UpdateMixer();
        void UpdateType();
        void resetMixer(MixerCurveWidget *mixer, int numElements);
        void UpdateCurveWidgets();
        void updatePitchCurveValue(QList<double>,double);
        void updateThrottleCurveValue(QList<double>,double);

        void SwashLvlStartButtonPressed();
        void SwashLvlNextButtonPressed();
        void SwashLvlCancelButtonPressed();
        void SwashLvlFinishButtonPressed();

        void enableSwashplateLevellingControl(bool state);
        void setSwashplateLevel(int percent);
        void SwashLvlSpinBoxChanged(int value);
        void FocusChanged(QWidget *oldFocus, QWidget *newFocus);
    public slots:
        void requestccpmUpdate();
        void sendccpmUpdate();
        void saveccpmUpdate();
protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

};

#endif // CONFIGccpmWIDGET_H
