/*
    This file is part of KDE.

    Copyright (c) 2011 Dan Leinir Turthra Jensen <admin@leinir.dk>

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

#ifndef PUBLISHERFIELD_H
#define PUBLISHERFIELD_H

#include "buildservice.h"
#include "attica_export.h"

namespace Attica
{

class ATTICA_EXPORT PublisherField
{
public:
    typedef QList<PublisherField> List;
    class Parser;

    PublisherField();
    PublisherField(const PublisherField &other);
    PublisherField &operator=(const PublisherField &other);
    ~PublisherField();

    void setName(const QString &value);
    QString name() const;

    void setType(const QString &value);
    QString type() const;

    void setData(const QString &value);
    QString data() const;

    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Attica

#endif // PUBLISHERFIELD_H
