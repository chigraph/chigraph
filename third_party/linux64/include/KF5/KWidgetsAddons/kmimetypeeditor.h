/* This file is part of the KDE libraries
   Copyright (C) 2014 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KMIMETYPE_EDITOR_H
#define KMIMETYPE_EDITOR_H

#include <kwidgetsaddons_export.h>
#include <QDialog>
#include <QWidget>

/**
 * This namespace provides a method for starting the file types editor.
 * @since 5.4
 */
namespace KMimeTypeEditor
{
    /**
     * Starts the file types editor for a given MIME type.
     * @param mimeType the mimetype to edit, e.g. "text/plain"
     * @param the parent widget
     * @since 5.4
     */
    KWIDGETSADDONS_EXPORT void editMimeType(const QString &mimeType, QWidget *widget);
}

#endif
