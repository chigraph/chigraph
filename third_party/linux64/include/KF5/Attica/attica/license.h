/*
    Copyright (c) 2010 Frederik Gladhorn <gladhorn@kde.org>

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

#ifndef ATTICA_LICENSE_H
#define ATTICA_LICENSE_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{
/**
    The License class contains information about one license that the server offers.
    It consists of an integer id, a name and a link to a webpage describing the license.
 */
class ATTICA_EXPORT License
{
public:
    typedef QList<License> List;
    class Parser;

    /**
    * Creates an empty License
    */
    License();

    /**
    * Copy constructor.
    * @param other the License to copy from
    */
    License(const License &other);

    /**
    * Assignment operator.
    * @param other the License to assign from
    * @return pointer to this License
    */
    License &operator=(const License &other);

    /**
    * Destructor.
    */
    ~License();

    /*
    <id>3</id>
    <name>Artistic 2.0</name>
    <link>http://dev.perl.org/perl6/rfc/346.html</link>
    */

    uint id() const;
    void setId(uint id);

    QString name() const;
    void setName(const QString &name);

    QUrl url() const;
    void setUrl(const QUrl &url);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
