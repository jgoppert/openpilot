/**
 ******************************************************************************
 *
 * @file       opmapgadgetoptionspage.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   opmap
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

#include "opmapgadgetoptionspage.h"
#include "opmapgadgetconfiguration.h"
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "ui_opmapgadgetoptionspage.h"


OPMapGadgetOptionsPage::OPMapGadgetOptionsPage(OPMapGadgetConfiguration *config, QObject *parent) :
    IOptionsPage(parent),
    m_config(config)
{
}

QWidget *OPMapGadgetOptionsPage::createPage(QWidget *parent)
{
    m_page = new Ui::OPMapGadgetOptionsPage();
    QWidget *w = new QWidget(parent);
    m_page->setupUi(w);

    int index = m_page->providerComboBox->findText(m_config->mapProvider());
    index = (index >= 0) ? index : 0;
    m_page->providerComboBox->setCurrentIndex(index);
    m_page->zoomSpinBox->setValue(m_config->zoom());
    m_page->latitudeSpinBox->setValue(m_config->latitude());
    m_page->longitudeSpinBox->setValue(m_config->longitude());

    return w;
}

void OPMapGadgetOptionsPage::apply()
{
    m_config->setMapProvider(m_page->providerComboBox->currentText());
    m_config->setZoom(m_page->zoomSpinBox->value());
    m_config->setLatitude(m_page->latitudeSpinBox->value());
    m_config->setLongitude(m_page->longitudeSpinBox->value());
}

void OPMapGadgetOptionsPage::finish()
{
    delete m_page;
}
