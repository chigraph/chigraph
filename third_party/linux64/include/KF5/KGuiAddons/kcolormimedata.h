/* This file is part of the KDE libraries
   Copyright (C) 1999 Steffen Hansen (hansen@kde.org)
   Copyright (C) 2005 Joseph Wenninger (jowenn@kde.org)

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

#ifndef _KCOLORMIMEDATA_H
#define _KCOLORMIMEDATA_H

#include <kguiaddons_export.h>

class QColor;
class QDrag;
class QMimeData;
class QObject;

/**
 * Drag-and-drop and clipboard mimedata manipulation for QColor objects. The according MIME type
 * is set to application/x-color.
 *
 * See the Qt drag'n'drop documentation.
 */
namespace KColorMimeData
{
/**
 * Sets the color and text representation fields for the specified color in the mimedata object:
 * application/x-color and text/plain types are set
 */
KGUIADDONS_EXPORT void populateMimeData(QMimeData *mimeData, const QColor &color);

/**
 * Returns true if the MIME data @p mimeData contains a color object.
 * First checks for application/x-color and if that fails, for a text/plain entry, which
 * represents a color in the format \#hexnumbers
 */
KGUIADDONS_EXPORT bool canDecode(const QMimeData *mimeData);

/**
 * Decodes the MIME data @p mimeData and returns the resulting color.
 * First tries application/x-color and if that fails, a text/plain entry, which
 * represents a color in the format \#hexnumbers. If this fails too,
 * an invalid QColor object is returned, use QColor::isValid() to test it.
 */
KGUIADDONS_EXPORT QColor fromMimeData(const QMimeData *mimeData);

/**
 * Creates a color drag object. Either you have to start this drag or delete it
 * The drag object's mime data has the application/x-color and text/plain type set and a pixmap
 * filled with the specified color, which is going to be displayed next to the mouse cursor
 */
KGUIADDONS_EXPORT QDrag *createDrag(const QColor &color, QObject *dragsource);
}

#endif // _KCOLORMIMEDATA_H
