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
#ifndef ATTICA_CATEGORY_H
#define ATTICA_CATEGORY_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QList>

#include "attica_export.h"

namespace Attica
{

/**
 * Represents a single content category
 */
class ATTICA_EXPORT Category
{
public:
    typedef QList<Category> List;
    class Parser;

    /**
     * Creates an empty Category
     */
    Category();

    /**
     * Copy constructor.
     * @param other the Category to copy from
     */
    Category(const Category &other);

    /**
     * Assignment operator.
     * @param other the Category to assign from
     * @return pointer to this Category
     */
    Category &operator=(const Category &other);

    /**
     * Destructor.
     */
    ~Category();

    /**
     * Sets the id of the Category.
     * The id uniquely identifies a Category with the OCS API.
     * @param id the new id
     */
    void setId(const QString &);

    /**
     * Gets the id of the Category.
     * The id uniquely identifies a Category with the OCS API.
     * @return the id
     */
    QString id() const;

    /**
     * Sets the name of the Category.
     * @param name the new name
     */
    void setName(const QString &name);

    /**
     * Gets the name of the Category.
     * @return the name
     */
    QString name() const;

    /**
     * Checks whether this Category has an id
     * @return @c true if an id has been set, @c false otherwise
     */
    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

QDebug operator<<(QDebug s, const Attica::Category& cat);

#endif
