/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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

#ifndef KICONUTILS_H
#define KICONUTILS_H

#include <kguiaddons_export.h>

#include <QIcon>

namespace KIconUtils
{

/**
 * Adds the \a overlay over the \a icon in the specified \a position
 *
 * The \a overlay icon is scaled down approx. to 1/3 or 1/4 (depending on the icon size)
 * and placed in one of the corners of the base icon.
 */
KGUIADDONS_EXPORT QIcon addOverlay(const QIcon &icon, const QIcon &overlay, Qt::Corner position);

/**
 * Adds \a overlays over the \a icon
 *
 * The \a overlays is a QHash of Qt::Corner and QIcon. The Qt::Corner value
 * decides where the overlay icon will be painted, the QIcon value
 * is the overlay icon to be painted.
 *
 * The overlay icon is scaled down to 1/3 or 1/4 (depending on the icon size)
 * and placed in one of the corners of the base icon.
 */
KGUIADDONS_EXPORT QIcon addOverlays(const QIcon &icon, const QHash<Qt::Corner, QIcon> &overlays);
}

#endif // KICONUTILS_H
