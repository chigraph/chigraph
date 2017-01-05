/*
    This file is part of KDE.

    Copyright (c) 2009 Frederik Gladhorn <gladhorn@kde.org>

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
#ifndef ATTICA_METADATA_H
#define ATTICA_METADATA_H

#include <QtCore/QString>

#include <QtCore/QSharedDataPointer>

#include "attica_export.h"

namespace Attica
{
class BaseJob;

/**
    *Status messages from the server
    */
class ATTICA_EXPORT Metadata
{
public:
    Metadata();
    Metadata(const Metadata &other);
    ~Metadata();
    Metadata &operator=(const Metadata &other);

    enum Error {
        NoError = 0,
        NetworkError,
        OcsError
    };

    /**
     * Check if the job was successful.
     * @return the error state enum returns the type of error (network or ocs)
     */
    Error error() const;
    void setError(Error error);

    /**
     * The status as integer.
     * If the error is an OCS error, refer to http://www.freedesktop.org/wiki/Specifications/open-collaboration-services
     * in any other case it is the network return code.
     */
    int statusCode() const;
    void setStatusCode(int code);

    /**
     * The status of the job, for example "Ok"
     */
    QString statusString() const;
    void setStatusString(const QString &status);

    /// An optional additional message from the server
    QString message();
    void setMessage(const QString &message);

    /// The number of items returned by this job (only relevant for list jobs)
    int totalItems();
    void setTotalItems(int items);

    /// The number of items per page the server was asked for
    int itemsPerPage();
    void setItemsPerPage(int itemsPerPage);

    /// The resulting ID when a PostJob created a new item.
    QString resultingId();
    void setResultingId(const QString &id);

private:
    class Private;
    QSharedDataPointer<Private> d;

    friend class Attica::BaseJob;
};

}

#endif
