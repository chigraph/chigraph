/*
    This file is part of KDE.

    Copyright (c) 2011 Laszlo Papp <djszapi@archlinux.us>

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

#ifndef ATTICA_TOPIC_H
#define ATTICA_TOPIC_H

#include "attica_export.h"

#include <QtCore/QDateTime>
#include <QtCore/QSharedDataPointer>

namespace Attica
{

class ATTICA_EXPORT Topic
{
public:
    typedef QList<Topic> List;
    class Parser;

    Topic();
    Topic(const Topic &other);
    Topic &operator=(const Topic &other);
    ~Topic();

    void setId(const QString &id);
    QString id() const;

    void setForumId(const QString &forumId);
    QString forumId() const;

    void setUser(const QString &user);
    QString user() const;

    void setDate(const QDateTime &date);
    QDateTime date() const;

    void setSubject(const QString &subject);
    QString subject() const;

    void setContent(const QString &content);
    QString content() const;

    void setComments(const int comments);
    int comments() const;

    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
