/**
******************************************************************************
*
* @file       languagetype.h
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
*             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
* @brief      
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
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
#ifndef LANGUAGETYPE_H
#define LANGUAGETYPE_H

#include <QString>
#include <QStringList>



namespace core {
    class LanguageType
    {
    public:

        enum Types
        {
            Arabic,
            Bulgarian,
            Bengali,
            Catalan,
            Czech,
            Danish,
            German,
            Greek,
            English,
            EnglishAustralian,
            EnglishGreatBritain,
            Spanish,
            Basque,
            Finnish,
            Filipino,
            French,
            Galician,
            Gujarati,
            Hindi,
            Croatian,
            Hungarian,
            Indonesian,
            Italian,
            Hebrew,
            Japanese,
            Kannada,
            Korean,
            Lithuanian,
            Latvian,
            Malayalam,
            Marathi,
            Dutch,
            NorwegianNynorsk,
            Norwegian,
            Oriya,
            Polish,
            Portuguese,
            PortugueseBrazil,
            PortuguesePortugal,
            Romansch,
            Romanian,
            Russian,
            Slovak,
            Slovenian,
            Serbian,
            Swedish,
            Tamil,
            Telugu,
            Thai,
            Turkish,
            Ukrainian,
            Vietnamese,
            ChineseSimplified,
            ChineseTraditional,
        };


        QString toString(Types type);
        LanguageType();
        ~LanguageType();
    private:
        QStringList list;
    };

}
#endif // LANGUAGETYPE_H