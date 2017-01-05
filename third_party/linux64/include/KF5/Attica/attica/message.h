/*
    This file is part of KDE.

    Copyright (c) 2008 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef ATTICA_MESSAGE_H
#define ATTICA_MESSAGE_H

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>

#include "attica_export.h"

namespace Attica
{

class ATTICA_EXPORT Message
{
public:
    typedef QList<Message> List;
    class Parser;

    enum Status { Unread = 0, Read = 1, Answered = 2};

    Message();
    Message(const Message &other);
    Message &operator=(const Message &other);
    ~Message();

    void setId(const QString &);
    QString id() const;

    void setFrom(const QString &);
    QString from() const;

    void setTo(const QString &);
    QString to() const;

    void setSent(const QDateTime &);
    QDateTime sent() const;

    void setStatus(Status);
    Status status() const;

    void setSubject(const QString &);
    QString subject() const;

    void setBody(const QString &);
    QString body() const;

    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
