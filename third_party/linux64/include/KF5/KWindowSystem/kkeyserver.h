/*
    Copyright (C) 2001 Ellis Whitehead <ellis@kde.org>

    Win32 port:
    Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _KKEYSERVER_H
#define _KKEYSERVER_H

#include <kwindowsystem_export.h>

#include <qglobal.h>

#include <config-kwindowsystem.h>
#if KWINDOWSYSTEM_HAVE_X11 /*or defined Q_OS_WIN*/
#include "kkeyserver_x11.h"
#elif defined Q_OS_MAC
#include "kkeyserver_mac.h"
#elif defined Q_OS_WIN
#include "kkeyserver_win.h"
#endif

class QString;

/**
 * A collection of functions for the conversion of key presses and
 * their modifiers from the window system specific format
 * to the generic format and vice-versa.
 */
namespace KKeyServer
{
/**
 * Converts the mask of ORed KKey::ModFlag modifiers to a
 * user-readable string.
 * @param mod the mask of ORed KKey::ModFlag modifiers
 * @return the user-readable string
 */
KWINDOWSYSTEM_EXPORT QString modToStringUser(uint mod);

/**
 * Converts the modifier given as user-readable string
 * to KKey::ModFlag modifier, or 0.
 * @internal
 */
KWINDOWSYSTEM_EXPORT uint stringUserToMod(const QString &mod);

/**
* Test if the shift modifier should be recorded for a given key.
*
* For example, if shift+5 produces '%' Qt wants ctrl+shift+5 recorded as ctrl+% and
* in that case this function would return false.
*
* @since 4.7.1
*/
KWINDOWSYSTEM_EXPORT bool isShiftAsModifierAllowed(int keyQt);

} // namespace KKeyServer

#endif // !_KKEYSERVER_H
