/*
 * This file is part of the KDE Frameworks.
 * Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
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
 */

#ifndef KDBUSCONNECTIONPOOL_H
#define KDBUSCONNECTIONPOOL_H

#include <kdbusaddons_export.h>

#include <QtDBus/QDBusConnection>

namespace KDBusConnectionPool
{
/**
 * The KDBusConnectionPool works around the problem
 * of QDBusConnection not being thread-safe. As soon as that
 * has been fixed (either directly in libdbus or with a work-
 * around in Qt) this method can be dropped in favor of
 * QDBusConnection::sessionBus().
 *
 * Note that this will create a thread-local QDBusConnection
 * object, which means whichever thread this is called
 * from must have both an event loop and be as long-lived as
 * the object using it. If either condition is not met, the
 * returned QDBusConnection will not send or receive DBus
 * events (calls, return values, etc).
 *
 * Using this within libraries can create complexities for
 * application developers working with threads as its use
 * in the library may not be apparent to the application
 * developer, and so functionality may appear to be broken
 * simply due to the nature of the thread from which this
 * ends up being called from. Library developers using
 * this facility are strongly encouraged to note this
 * caveat in the library's documentation.
 */
KDBUSADDONS_EXPORT QDBusConnection threadConnection();
}

#endif

