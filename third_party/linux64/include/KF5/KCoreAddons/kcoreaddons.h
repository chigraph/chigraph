/*
 * This file is part of the KDE Libraries
 * Copyright (C) 2016 David Edmundson <davidedmundson@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef KCOREADDONS_H
#define KCOREADDONS_H

#include <kcoreaddons_export.h>
#include <QString>

namespace KCoreAddons
{
    /*
     * Returns the version number of KCoreAddons at run-time as a string (for example, "5.19.0").
     * This may be a different version than the version the application was compiled against.
     * @since 5.20
     */
    KCOREADDONS_EXPORT QString versionString();

    /*
     * Returns a numerical version number of KCoreAddons at run-time in the form 0xMMNNPP
     * (MM = major, NN = minor, PP = patch)
     * This can be compared using the macro QT_VERSION_CHECK.
     *
     * For example:
     * \code
     * if (KCoreAddons::version() < QT_VERSION_CHECK(5,19,0) )
     * \endcode
     *
     * This may be a different version than the version the application was compiled against.
     * @since 5.20
     */
    KCOREADDONS_EXPORT unsigned int version();
}

#endif
