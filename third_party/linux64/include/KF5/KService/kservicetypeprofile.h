/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 2007 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __kservicetypeprofile_h__
#define __kservicetypeprofile_h__

#include <QtCore/QString>
#include <QtCore/QList>

#include <kservicetypetrader.h>

/**
 * KServiceTypeProfile represents the user's preferences for services
 * of a service type.
 * It consists of a list of services (service offers) for the service type
 * that is sorted by the user's preference.
 * KServiceTypeTrader uses KServiceTypeProfile to
 * get results sorted according to the user's preference.
 *
 * @see KService
 * @see KServiceType
 * @see KServiceTypeTrader
 * @short Represents the user's preferences for services of a service type
 */
namespace KServiceTypeProfile
{
/**
 * Write the complete profile for a given servicetype.
 * Do not use this for mimetypes.
 * @param serviceType The name of the servicetype.
 * @param services Ordered list of services, from the preferred one to the least preferred one.
 * @param disabledServices List of services which are normally associated with this serviceType,
 * but which should be disabled, i.e. trader queries will not return them.
 */
KSERVICE_EXPORT void writeServiceTypeProfile(const QString &serviceType,
        const KService::List &services,
        const KService::List &disabledServices = KService::List());

/**
 * Delete the complete profile for a given servicetype, reverting to the default
 * preference order (the one specified by InitialPreference in the .desktop files).
 *
 * Do not use this for mimetypes.
 * @param serviceType The name of the servicetype.
 */
KSERVICE_EXPORT void deleteServiceTypeProfile(const QString &serviceType);

/**
 * @internal, for KServiceTypeTrader
 */
KSERVICE_EXPORT bool hasProfile(const QString &serviceType);

/**
 * Clear all cached information
 * @internal, for KServiceTypeFactory
 */
void clearCache();

}

#endif
