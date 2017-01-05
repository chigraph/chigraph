/*
    This file is part of KDE.

    Copyright (C) 2009 Marco Martin <notmart@gmail.com>

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

#ifndef ATTICA_KNOWLEDGEBASEENTRY_H
#define ATTICA_KNOWLEDGEBASEENTRY_H

#include "attica_export.h"

#include <QtCore/QDateTime>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QMap>

#include <QtCore/QUrl>

namespace Attica
{

class ATTICA_EXPORT KnowledgeBaseEntry
{
public:
    typedef QList<KnowledgeBaseEntry> List;
    class Parser;

    KnowledgeBaseEntry();
    KnowledgeBaseEntry(const KnowledgeBaseEntry &other);
    KnowledgeBaseEntry &operator=(const KnowledgeBaseEntry &other);
    ~KnowledgeBaseEntry();

    void setId(QString id);
    QString id() const;

    void setContentId(int id);
    int contentId() const;

    void setUser(const QString &user);
    QString user() const;

    void setStatus(const QString &status);
    QString status() const;

    void setChanged(const QDateTime &changed);
    QDateTime changed() const;

    void setName(const QString &name);
    QString name() const;

    void setDescription(const QString &description);
    QString description() const;

    void setAnswer(const QString &answer);
    QString answer() const;

    void setComments(int comments);
    int comments() const;

    void setDetailPage(const QUrl &detailPage);
    QUrl detailPage() const;

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

