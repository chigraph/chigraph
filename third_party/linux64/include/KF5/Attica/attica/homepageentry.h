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

#ifndef ATTICA_HOMEPAGEENTRY_H
#define ATTICA_HOMEPAGEENTRY_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{
/**
    The HomePageEntry class contains information about one home page entry.
    It consists of a type and a home page url.
 */
class ATTICA_EXPORT HomePageEntry
{
public:
    typedef QList<HomePageEntry> List;

    /**
    * Creates an empty HomePageEntry
    */
    HomePageEntry();

    /**
    * Copy constructor.
    * @param other the HomePageEntry to copy from
    */
    HomePageEntry(const HomePageEntry &other);

    /**
    * Assignment operator.
    * @param other the HomePageEntry to assign from
    * @return pointer to this HomePageEntry
    */
    HomePageEntry &operator=(const HomePageEntry &other);

    /**
    * Destructor.
    */
    ~HomePageEntry();

    QString type() const;
    void setType(const QString &type);

    QUrl url() const;
    void setUrl(const QUrl &url);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
