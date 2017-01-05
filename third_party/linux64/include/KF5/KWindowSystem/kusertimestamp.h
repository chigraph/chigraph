/* This file is part of the KDE libraries
    Copyright (c) 2003 Luboš Luňák <l.lunak@kde.org>

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

#ifndef KUSERTIMESTAMP_H
#define KUSERTIMESTAMP_H

#include <kwindowsystem_export.h>

namespace KUserTimestamp
{
/**
 * Returns the last user action timestamp or 0 if no user activity has taken place yet.
 * @see updateuserTimestamp
 */
KWINDOWSYSTEM_EXPORT unsigned long userTimestamp();

/**
 * Updates the last user action timestamp to the given time, or to the current time,
 * if 0 is given. Do not use unless you're really sure what you're doing.
 * Consult focus stealing prevention section in kdebase/kwin/README.
 */
KWINDOWSYSTEM_EXPORT void updateUserTimestamp(unsigned long time = 0);
}

#endif

