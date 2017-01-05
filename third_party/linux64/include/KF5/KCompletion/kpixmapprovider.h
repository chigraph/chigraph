/* This file is part of the KDE libraries

   Copyright (c) 2000 Carsten Pfeiffer <pfeiffer@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License (LGPL) as published by the Free Software Foundation; either
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

#ifndef KPIXMAPPROVIDER_H
#define KPIXMAPPROVIDER_H

#include <kcompletion_export.h>
#include <QPixmap>

/**
 * A tiny abstract class with just one method:
 * pixmapFor()
 *
 * It will be called whenever an icon is searched for @p text.
 *
 * Used e.g. by KHistoryComboBox
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 * @short an abstract interface for looking up icons
 */
class KCOMPLETION_EXPORT KPixmapProvider
{
public:
    virtual ~KPixmapProvider();
    /**
     * You may subclass this and return a pixmap of size @p size for @p text.
     * @param text the text that is associated with the pixmap
     * @param size the size of the icon in pixels, 0 for defaylt size.
     *             See KIconLoader::StdSize.
     * @return the pixmap for the arguments, or null if there is none
     */
    virtual QPixmap pixmapFor(const QString &text, int size = 0) = 0;
protected:
    /** Virtual hook, used to add new "virtual" functions while maintaining
    binary compatibility. Unused in this class.
    */
    virtual void virtual_hook(int id, void *data);
};

#endif // KPIXMAPPROVIDER_H
