/**
 ******************************************************************************
 *
 * @file       generalsettings.cpp
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

#include "generalsettings.h"

#include <utils/stylehelper.h>
#include <utils/qtcolorbutton.h>
#include <utils/consoleprocess.h>
#include <coreplugin/icore.h>
#include <QtGui/QMessageBox>
#include <QtCore/QDir>

#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>

#include "ui_generalsettings.h"

using namespace Utils;
using namespace Core::Internal;

GeneralSettings::GeneralSettings():
    m_dialog(0)
{
}

QString GeneralSettings::id() const
{
    return QLatin1String("General");
}

QString GeneralSettings::trName() const
{
    return tr("General");
}

QString GeneralSettings::category() const
{
    return QLatin1String("Environment");
}

QString GeneralSettings::trCategory() const
{
    return tr("Environment");
}

static bool hasQmFilesForLocale(const QString &locale, const QString &creatorTrPath)
{
    static const QString qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

    const QString trFile = QLatin1String("qt_") + locale + QLatin1String(".qm");
    return QFile::exists(qtTrPath+'/'+trFile) || QFile::exists(creatorTrPath+'/'+trFile);
}

void GeneralSettings::fillLanguageBox() const
{
    const QString currentLocale = language();

    m_page->languageBox->addItem(tr("<System Language>"), QString());
    // need to add this explicitly, since there is no qm file for English
    m_page->languageBox->addItem(QLatin1String("English"), QLatin1String("C"));
    if (currentLocale == QLatin1String("C"))
        m_page->languageBox->setCurrentIndex(m_page->languageBox->count() - 1);

    const QString creatorTrPath =
            Core::ICore::instance()->resourcePath() + QLatin1String("/translations");
    const QStringList languageFiles = QDir(creatorTrPath).entryList(QStringList(QLatin1String("openpilotgcs*.qm")));

    Q_FOREACH(const QString &languageFile, languageFiles)
    {
        int start = languageFile.indexOf(QLatin1Char('_'))+1;
        int end = languageFile.lastIndexOf(QLatin1Char('.'));
        const QString locale = languageFile.mid(start, end-start);
        // no need to show a language that creator will not load anyway
        if (hasQmFilesForLocale(locale, creatorTrPath)) {
            m_page->languageBox->addItem(QLocale::languageToString(QLocale(locale).language()), locale);
            if (locale == currentLocale)
                m_page->languageBox->setCurrentIndex(m_page->languageBox->count() - 1);
        }
    }
}


QWidget *GeneralSettings::createPage(QWidget *parent)
{
    m_page = new Ui::GeneralSettings();
    QWidget *w = new QWidget(parent);
    m_page->setupUi(w);

    fillLanguageBox();

    m_page->colorButton->setColor(StyleHelper::baseColor());
#ifdef Q_OS_UNIX
    m_page->terminalEdit->setText(ConsoleProcess::terminalEmulator(Core::ICore::instance()->settings()));
#else
    m_page->terminalLabel->hide();
    m_page->terminalEdit->hide();
    m_page->resetTerminalButton->hide();
#endif

    connect(m_page->resetButton, SIGNAL(clicked()),
            this, SLOT(resetInterfaceColor()));
    connect(m_page->resetEditorButton, SIGNAL(clicked()),
            this, SLOT(resetExternalEditor()));
    connect(m_page->helpExternalEditorButton, SIGNAL(clicked()),
            this, SLOT(showHelpForExternalEditor()));
#ifdef Q_OS_UNIX
    connect(m_page->resetTerminalButton, SIGNAL(clicked()),
            this, SLOT(resetTerminal()));
#endif

    return w;
}

void GeneralSettings::apply()
{
    int currentIndex = m_page->languageBox->currentIndex();
    setLanguage(m_page->languageBox->itemData(currentIndex, Qt::UserRole).toString());
    // Apply the new base color if accepted
    StyleHelper::setBaseColor(m_page->colorButton->color());
#ifdef Q_OS_UNIX
	ConsoleProcess::setTerminalEmulator(Core::ICore::instance()->settings(),
                                        m_page->terminalEdit->text());
#endif

}

void GeneralSettings::finish()
{
    delete m_page;
}

void GeneralSettings::readSettings(QSettings* qs)
{
    qs->beginGroup(QLatin1String("General"));
    m_language = qs->value(QLatin1String("OverrideLanguage"),QLocale::system().name()).toString();
    qs->endGroup();

}

void GeneralSettings::saveSettings(QSettings* qs)
{
    qs->beginGroup(QLatin1String("General"));

    if (m_language.isEmpty())
        qs->remove(QLatin1String("OverrideLanguage"));
    else
        qs->setValue(QLatin1String("OverrideLanguage"), m_language);

    qs->endGroup();
}

void GeneralSettings::resetInterfaceColor()
{
    m_page->colorButton->setColor(0x666666);
}

void GeneralSettings::resetExternalEditor()
{
}

#ifdef Q_OS_UNIX
void GeneralSettings::resetTerminal()
{
    m_page->terminalEdit->setText(ConsoleProcess::defaultTerminalEmulator() + QLatin1String(" -e"));
}
#endif

void GeneralSettings::showHelpForExternalEditor()
{
    if (m_dialog) {
        m_dialog->show();
        m_dialog->raise();
        m_dialog->activateWindow();
        return;
    }
#if 0
    QMessageBox *mb = new QMessageBox(QMessageBox::Information,
                                  tr("Variables"),
                                  EditorManager::instance()->externalEditorHelpText(),
                                  QMessageBox::Cancel,
                                  m_page->helpExternalEditorButton);
    mb->setWindowModality(Qt::NonModal);
    m_dialog = mb;
    mb->show();
#endif
}

void GeneralSettings::resetLanguage()
{
    // system language is default
    m_page->languageBox->setCurrentIndex(0);
}

QString GeneralSettings::language() const
{
    return m_language;
}

void GeneralSettings::setLanguage(const QString &locale)
{
    if (m_language != locale)
    {
        QMessageBox::information((QWidget*)Core::ICore::instance()->mainWindow(), tr("Restart required"),
                                 tr("The language change will take effect after a restart of the OpenPilot GCS."));
        m_language = locale;
    }
}
