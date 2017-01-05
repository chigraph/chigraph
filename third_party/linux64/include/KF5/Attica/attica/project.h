/*
    This file is part of KDE.

    Copyright 2010 Sebastian Kügler <sebas@kde.org>

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
#ifndef ATTICA_PROJECT_H
#define ATTICA_PROJECT_H

#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include "attica_export.h"

namespace Attica
{

class ATTICA_EXPORT Project
{
public:
    typedef QList<Project> List;
    class Parser;

    Project();
    Project(const Project &other);
    Project &operator=(const Project &other);
    ~Project();

    void setId(const QString &);
    QString id() const;

    void setName(const QString &);
    QString name() const;

    void setVersion(const QString &);
    QString version() const;

    void setUrl(const QString &);
    QString url() const;

    void setLicense(const QString &);
    QString license() const;

    void setSummary(const QString &);
    QString summary() const;

    void setDescription(const QString &);
    QString description() const;

    void setDevelopers(const QStringList &);
    QStringList developers() const;

    void setRequirements(const QString &);
    QString requirements() const;

    void setSpecFile(const QString &);
    QString specFile() const;

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
