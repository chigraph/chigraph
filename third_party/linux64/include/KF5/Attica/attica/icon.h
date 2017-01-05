/*
    This file is part of KDE.

    Copyright (c) 2010 Intel Corporation
    Author: Mateu Batle Sastre <mbatle@collabora.co.uk>

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

#ifndef ATTICA_ICON_H
#define ATTICA_ICON_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{
/**
    The Icon class contains information about an icon.
    It consists of a Url and icon size information.
 */
class ATTICA_EXPORT Icon
{
public:
    typedef QList<Icon> List;

    /**
    * Creates an empty Icon
    */
    Icon();

    /**
    * Copy constructor.
    * @param other the Icon to copy from
    */
    Icon(const Icon &other);

    /**
    * Assignment operator.
    * @param other the Icon to assign from
    * @return pointer to this Icon
    */
    Icon &operator=(const Icon &other);

    /**
    * Destructor.
    */
    ~Icon();

    QUrl url() const;
    void setUrl(const QUrl &url);

    uint width() const;
    void setWidth(uint width);

    uint height() const;
    void setHeight(uint height);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
