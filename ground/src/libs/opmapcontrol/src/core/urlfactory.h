/**
******************************************************************************
*
* @file       urlfactory.h
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
#ifndef URLFACTORY_H
#define URLFACTORY_H

#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>
#include <QCoreApplication>
#include "providerstrings.h"
#include "pureimagecache.h"
#include "../internals/pointlatlng.h"
#include "geodecoderstatus.h"
#include <QTime>
#include "cache.h"
#include "placemark.h"
#include <QTextCodec>
#include "cmath"

using namespace internals;

namespace core {
    class UrlFactory: public QObject,public ProviderStrings
    {
        Q_OBJECT
    public:
        /// <summary>
        /// Gets or sets the value of the User-agent HTTP header.
        /// </summary>
        QByteArray UserAgent;
        QNetworkProxy Proxy;
        UrlFactory();
        ~UrlFactory();
        QString MakeImageUrl(const MapType::Types &type,const Point &pos,const int &zoom,const QString &language);
        PointLatLng GetLatLngFromGeodecoder(const QString &keywords,GeoCoderStatusCode::Types &status);
        Placemark GetPlacemarkFromGeocoder(PointLatLng location);
        int Timeout;
    private:
        void GetSecGoogleWords(const Point &pos,  QString &sec1, QString &sec2);
        int GetServerNum(const Point &pos,const int &max) const;
        void TryCorrectGoogleVersions();
        bool isCorrectedGoogleVersions;
        QString TileXYToQuadKey(const int &tileX,const int &tileY,const int &levelOfDetail) const;
        bool CorrectGoogleVersions;
        bool UseGeocoderCache; //TODO GetSet
        bool UsePlacemarkCache;//TODO GetSet
        static const double EarthRadiusKm = 6378.137; // WGS-84
        double GetDistance(PointLatLng p1,PointLatLng p2);

    protected:
        static short timelapse;
        QString LanguageStr;
        bool IsCorrectGoogleVersions();
        void setIsCorrectGoogleVersions(bool value);
        QString MakeGeocoderUrl(QString keywords);
        QString MakeReverseGeocoderUrl(PointLatLng &pt,const QString &language);
        PointLatLng GetLatLngFromGeocoderUrl(const QString &url,const bool &useCache, GeoCoderStatusCode::Types &status);
        Placemark GetPlacemarkFromReverseGeocoderUrl(const QString &url,const bool &useCache);
    };

}
#endif // URLFACTORY_H