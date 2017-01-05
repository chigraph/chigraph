/*  This file is part of the KDE Frameworks

    Copyright 2014 Montel Laurent <montel@kde.org>

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

#ifndef KDELIBS4CONFIGMIGRATOR_H
#define KDELIBS4CONFIGMIGRATOR_H

#include <kcoreaddons_export.h>
#include <QStringList>

/**
 * Kdelibs4ConfigMigrator migrates selected config files and ui files
 * from the kdelibs 4.x location ($KDEHOME, as used by KStandardDirs)
 * to the Qt 5.x location ($XDG_*_HOME, as used by QStandardPaths).
 *
 * @short Class for migration of config files and ui file from kdelibs4.
 * @since 5.2
 */
class KCOREADDONS_EXPORT Kdelibs4ConfigMigrator
{
public:
    /**
     * Constructs a Kdelibs4ConfigMigrator
     *
     * @param appName The application name, which is used for the directory
     * containing the .ui files.
     */
    explicit Kdelibs4ConfigMigrator(const QString &appName);

    /**
     * Destructor
     */
    ~Kdelibs4ConfigMigrator();

    /**
     * Migrate the files, if any.
     *
     * Returns true if the migration happened.
     * It will return false if there was nothing to migrate (no KDEHOME).
     * This return value is unrelated to error handling. It is just a way to skip anything else
     * related to migration on a clean system, by writing
     * @code
     * if (migrate()) {
     *    look for old data to migrate as well
     * }
     * @endcode
     */
    bool migrate();

    /**
     * Set the list of config files that need to be migrated.
     * @param configFileNameList list of config files
     */
    void setConfigFiles(const QStringList &configFileNameList);

    /**
     * Set the list of ui files to migrate.
     * @param uiFileNameList list of ui files
     */
    void setUiFiles(const QStringList &uiFileNameList);

private:
    class Private;
    friend class Private;
    Private *const d;
};

#endif // KDELIBS4CONFIGMIGRATOR_H
