/*
    Copyright 2013  Albert Vaca <albertvaka@gmail.com>

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

#ifndef LINEEDITURLDROPEVENTFILTER_H
#define LINEEDITURLDROPEVENTFILTER_H

#include <kwidgetsaddons_export.h>

#include <QObject>

/**
 * This class provides an event filter that can be installed on a QLineEdit
 * or a subclass of it (KLineEdit) to make it handle url drop events so
 * when a url is dropped it replaces the existing content.
 */
class KWIDGETSADDONS_EXPORT LineEditUrlDropEventFilter
    : public QObject
{
    Q_OBJECT

public:
    LineEditUrlDropEventFilter(QObject *parent = 0);
    virtual ~LineEditUrlDropEventFilter();
    bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE;
};

#endif
