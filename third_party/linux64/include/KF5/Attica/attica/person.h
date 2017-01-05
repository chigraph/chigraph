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
#ifndef ATTICA_PERSON_H
#define ATTICA_PERSON_H

#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{

class ATTICA_EXPORT Person
{
public:
    typedef QList<Person> List;
    class Parser;

    Person();
    Person(const Person &other);
    Person &operator=(const Person &other);
    ~Person();

    void setId(const QString &);
    QString id() const;

    void setFirstName(const QString &);
    QString firstName() const;

    void setLastName(const QString &);
    QString lastName() const;

    void setBirthday(const QDate &);
    QDate birthday() const;

    void setCountry(const QString &);
    QString country() const;

    void setLatitude(qreal);
    qreal latitude() const;

    void setLongitude(qreal);
    qreal longitude() const;

    void setAvatarUrl(const QUrl &);
    QUrl avatarUrl() const;

    void setHomepage(const QString &);
    QString homepage() const;

    void setCity(const QString &);
    QString city() const;

    void addExtendedAttribute(const QString &key, const QString &value);
    QString extendedAttribute(const QString &key) const;

    QMap<QString, QString> extendedAttributes() const;

    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
