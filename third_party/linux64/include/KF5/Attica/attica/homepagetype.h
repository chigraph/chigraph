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

#ifndef ATTICA_HOMEPAGETYPE_H
#define ATTICA_HOMEPAGETYPE_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{
/**
    The HomePageType class contains information about one home page type.
    It consists of an integer id and a home page type name.
 */
class ATTICA_EXPORT HomePageType
{
public:
    typedef QList<HomePageType> List;
    class Parser;

    /**
    * Creates an empty HomePageType
    */
    HomePageType();

    /**
    * Copy constructor.
    * @param other the HomePageType to copy from
    */
    HomePageType(const HomePageType &other);

    /**
    * Assignment operator.
    * @param other the HomePageType to assign from
    * @return pointer to this HomePageType
    */
    HomePageType &operator=(const HomePageType &other);

    /**
    * Destructor.
    */
    ~HomePageType();

    /*
    <id>10</id>
    <name>Blog</name>
    */

    uint id() const;
    void setId(uint id);

    QString name() const;
    void setName(const QString &name);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
