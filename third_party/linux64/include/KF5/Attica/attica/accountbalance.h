/*
    Copyright (C) 2009 Frederik Gladhorn <gladhorn@kde.org>

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

#ifndef ATTICA_ACCOUNTBALANCE_H
#define ATTICA_ACCOUNTBALANCE_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

#include "attica_export.h"

namespace Attica
{

/**
 * Represents the money in the account of the user
 */
class ATTICA_EXPORT AccountBalance
{
public:
    typedef QList<AccountBalance> List;
    class Parser;

    /**
     * Creates an empty AccountBalance
     */
    AccountBalance();

    /**
     * Copy constructor.
     * @param other the AccountBalance to copy from
     */
    AccountBalance(const AccountBalance &other);

    /**
     * Assignment operator.
     * @param other the AccountBalance to assign from
     * @return pointer to this AccountBalance
     */
    AccountBalance &operator=(const AccountBalance &other);

    /**
     * Destructor.
     */
    ~AccountBalance();

    /**
     * Sets the currency in use.
     * @param currency the new currency (Euro, US Dollar)
     */
    void setCurrency(const QString &currency);

    /**
     * Gets the currency.
     * @return the currency
     */
    QString currency() const;

    /**
     * Sets the balance.
     * @param balance
     */
    void setBalance(const QString &name);

    /**
     * Gets the balance.
     * @return the amount of money in the account
     */
    QString balance() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif // ACCOUNTBALANCE_H
