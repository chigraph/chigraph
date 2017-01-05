/* This file is part of libkdbus

   Copyright (C) 2009 Tobias Koenig (tokoe@kde.org)
   Copyright (C) 2011 Kevin Ottens (ervin@kde.org)

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

#ifndef KDBUSINTERPROCESSLOCK_H
#define KDBUSINTERPROCESSLOCK_H

#include <QtCore/QObject>

#include <kdbusaddons_export.h>

class KDBusInterProcessLockPrivate;

/**
 * @short A class for serializing access to a resource that is shared between multiple processes.
 *
 * This class can be used to serialize access to a resource between
 * multiple processes. Instead of using lock files, which could
 * become stale easily, the registration of dummy dbus services is used
 * to allow only one process at a time to access the resource.
 *
 * Example:
 *
 * @code
 *
 * KDBusInterProcessLock *lock = new KDBusInterProcessLock("myresource");
 * connect(lock, SIGNAL(lockGranted(KDBusInterProcessLock *)),
 *               this, SLOT(doCriticalTask(KDBusInterProcessLock *)));
 * lock->lock();
 *
 * ...
 *
 * ... ::doCriticalTask(KDBusInterProcessLock *lock)
 * {
 *    // change common resource
 *
 *    lock->unlock();
 * }
 *
 * @endcode
 *
 * @author Tobias Koenig <tokoe@kde.org>
 */
class KDBUSADDONS_EXPORT KDBusInterProcessLock : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new inter process lock object.
     *
     * @param resource The identifier of the resource that shall be locked.
     *                 This identifier can be any string, however it must be unique for
     *                 the resource and every client that wants to access the resource must
     *                 know it.
     */
    KDBusInterProcessLock(const QString &resource);

    /**
     * Destroys the inter process lock object.
     */
    ~KDBusInterProcessLock();

    /**
     * Returns the identifier of the resource the lock is set on.
     */
    QString resource() const;

    /**
     * Requests the lock.
     *
     * The lock is granted as soon as the lockGranted() signal is emitted.
     */
    void lock();

    /**
     * Releases the lock.
     *
     * @note This method should be called as soon as the critical area is left
     *       in your code path and the lock is no longer needed.
     */
    void unlock();

    /**
     * Waits for the granting of a lock by starting an internal event loop.
     */
    void waitForLockGranted();

Q_SIGNALS:
    /**
     * This signal is emitted when the requested lock has been granted.
     *
     * @param lock The lock that has been granted.
     */
    void lockGranted(KDBusInterProcessLock *lock);

private:
    friend class KDBusInterProcessLockPrivate;
    KDBusInterProcessLockPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_serviceRegistered(const QString &))
};

#endif
