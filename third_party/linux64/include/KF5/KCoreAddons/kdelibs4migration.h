/*  This file is part of the KDE Frameworks

    Copyright 2014 David Faure <faure@kde.org>
    Copyright 2014 Ivan Cukic <ivan.cukic(at)kde.org>

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

#ifndef KDELIBS4MIGRATION_H
#define KDELIBS4MIGRATION_H

#include <kcoreaddons_export.h>

#include <QString>

class Kdelibs4MigrationPrivate;

/**
 * \file kdelibs4migration.h
 */

/**
  * Kdelibs4Migration provides support for locating config files
  * and application data files saved by kdelibs 4 in the user's home directory
  * ($KDEHOME, i.e. typically ~/.kde).
  *
  * Distributions that built kdelibs4 with a custom KDE home with
  * the CMake option _KDE_DEFAULT_HOME_POSTFIX should use the same option
  * here with _KDE4_DEFAULT_HOME_POSTFIX
  *
  * The purpose is to be able to let the application migrate these files
  * to the KF5/Qt5 location for these files (QStandardPaths).
  *
  * Files from the "config" resource (as saved by KConfig) should be migrated to
  * QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
  *
  * Files from the "data" resource should be migrated to a subdirectory of
  * QStandardPaths::writableLocation(QStandardPaths::DataLocation)
  *
  * The following resources are supported:
   <ul>
   <li>config</li>
   <li>data</li>
   <li>services</li>
   <li>servicetypes</li>
   <li>wallpaper</li>
   <li>emoticons</li>
   <li>templates</li>
   </ul>
  * Use kdeHome() for anything else.
  *
  * @short Class for migration of config files from kdelibs4
  * @since 5.0
  */
class KCOREADDONS_EXPORT Kdelibs4Migration Q_DECL_FINAL
{
public:
    /**
     * Constructs a Kdelibs4Migration instance.
     * The constructor attempts to locate the user's "kdehome" from kdelibs4.
     */
    explicit Kdelibs4Migration();

    /**
     * Destructor
     */
    ~Kdelibs4Migration();

    /**
     * Returns true if a "kdehome" was found.
     * Otherwise, there is nothing to migrate.
     */
    bool kdeHomeFound() const;

    /**
     * Returns the kdehome that was found.
     * Don't use this method if you can use locateLocal or saveLocation
     * @since 5.13
     */
    QString kdeHome() const;

    /**
     * Finds a local file in a resource.
     * This API is inspired by KStandardDirs::locateLocal for ease of porting.
     * @param type The type of wanted resource.
     * @param filename A relative filename of the resource.
     */
    QString locateLocal(const char *type, const QString &filename) const;

    /**
     * Finds a location to save files into for the given type
     * in the user's home directory.
     * @param type The type of location to return.
     * @param suffix A subdirectory name.
     */
    QString saveLocation(const char *type, const QString &suffix = QString()) const;

private:
    Kdelibs4MigrationPrivate *d;
};

#endif // KFORMAT_H
