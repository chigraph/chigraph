/* This file is part of libkdbusaddons

   Copyright (c) 2013 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KDEINIT_IFACE_H
#define KDEINIT_IFACE_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <kdbusaddons_export.h>

/**
 * The KDEInitInterface namespace contains:
 *
 * <ul>
 *   <li>A function to ensure kdeinit and klauncher are running.
 *      This is necessary before making D-Bus calls to org.kde.klauncher5.
 * </ul>
 *
 * @since 5.0
 */
namespace KDEInitInterface
{
KDBUSADDONS_EXPORT void ensureKdeinitRunning();
}

#endif
