/*
This file is part of KDE.

Copyright (c) 2010 Martin Sandsmark <martin.sandsmark@kde.org>

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
#ifndef ATTICA_PRIVATEDATA_H
#define ATTICA_PRIVATEDATA_H

#include "provider.h"

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{

class ATTICA_EXPORT PrivateData
{
public:
    class Parser;

    typedef QList<PrivateData> List; // nonsense

    PrivateData();
    PrivateData(const PrivateData &other);
    PrivateData &operator=(const PrivateData &other);
    ~PrivateData();

    /**
     * Sets an attribute referenced by \key to \value.
     */
    void setAttribute(const QString &key, const QString &value);

    /**
     * Returns an attribute referenced by \key.
     */
    QString attribute(const QString &key) const;

    /**
     * Sets when an attribute last was changed (mostly for internal use).
     */
    void setTimestamp(const QString &key, const QDateTime &when);

    /**
     * Returns the date and time an attribute last was changed.
     */
    QDateTime timestamp(const QString &key) const;

    /**
     * Returns a list of fetched keys.
     */
    QStringList keys() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif//ATTICA_ATTRIBUTES_H
