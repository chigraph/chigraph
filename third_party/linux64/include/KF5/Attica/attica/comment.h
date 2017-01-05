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

#ifndef ATTICA_COMMENT_H
#define ATTICA_COMMENT_H

#include "attica_export.h"

#include <QtCore/QDateTime>
#include <QtCore/QSharedDataPointer>

#include <QtCore/QUrl>

namespace Attica
{

class ATTICA_EXPORT Comment
{
public:
    typedef QList<Comment> List;
    class Parser;

    enum Type {
        ContentComment,
        ForumComment,
        KnowledgeBaseComment,
        EventComment
    };
    static QString commentTypeToString(const Comment::Type type);

    Comment();
    Comment(const Comment &other);
    Comment &operator=(const Comment &other);
    ~Comment();

    void setId(const QString &id);
    QString id() const;

    void setSubject(const QString &subject);
    QString subject() const;

    void setText(const QString &text);
    QString text() const;

    void setChildCount(const int childCount);
    int childCount() const;

    void setUser(const QString &user);
    QString user() const;

    void setDate(const QDateTime &date);
    QDateTime date() const;

    /**
      This is for internal usage, @see Provider::setCommentScore to set scores in comments.
      @param score average comment score in scale from 0 to 100
     */
    void setScore(const int score);
    /**
      Returns score of this comment.
      @param score average comment score in scale from 0 to 100
     */
    int score() const;

    void setChildren(QList<Comment> comments);
    QList<Comment> children() const;

    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
