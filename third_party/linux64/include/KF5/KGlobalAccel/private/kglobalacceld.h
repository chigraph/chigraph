/*
    This file is part of the KDE libraries

    Copyright (c) 2007 Andreas Hartmetz <ahartmetz@gmail.com>
    Copyright (c) 2008 Michael Jansen <kde@michael-jansen.biz>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef KGLOBALACCELD_H
#define KGLOBALACCELD_H

#include "kf5globalaccelprivate_export.h"

#include <kglobalshortcutinfo.h>

#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtDBus/QtDBus>

struct KGlobalAccelDPrivate;

/**
 * @todo get rid of all of those QStringList parameters.
 */
class KF5GLOBALACCELPRIVATE_EXPORT KGlobalAccelD : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KGlobalAccel")
    Q_ENUMS(SetShortcutFlag)

public:

    enum SetShortcutFlag
    {
        SetPresent = 2,
        NoAutoloading = 4,
        IsDefault = 8
    };
    Q_FLAGS(SetShortcutFlags)

    KGlobalAccelD(QObject* parent = NULL);
    ~KGlobalAccelD();

    bool init();

public Q_SLOTS:

    /**
     * Get the dbus path for all known components.
     *
     * The returned path is absolute. No need to prepend anything.
     */
    Q_SCRIPTABLE QList<QDBusObjectPath> allComponents() const;

    Q_SCRIPTABLE QList<QStringList> allMainComponents() const;

    Q_SCRIPTABLE QList<QStringList> allActionsForComponent(const QStringList &actionId) const;

    Q_SCRIPTABLE QStringList action(int key) const;

    //to be called by main components not owning the action
    Q_SCRIPTABLE QList<int> shortcut(const QStringList &actionId) const;

    //to be called by main components not owning the action
    Q_SCRIPTABLE QList<int> defaultShortcut(const QStringList &actionId) const;

    /**
     * Get the dbus path for @ componentUnique
     *
     * @param componentUnique the components unique identifier
     *
     * @return the absolute dbus path
     */
    Q_SCRIPTABLE QDBusObjectPath getComponent(const QString &componentUnique) const;

    //to be called by main components owning the action
    Q_SCRIPTABLE QList<int> setShortcut(const QStringList &actionId,
                           const QList<int> &keys, uint flags);

    //this is used if application A wants to change shortcuts of application B
    Q_SCRIPTABLE void setForeignShortcut(const QStringList &actionId, const QList<int> &keys);

    //to be called when a KAction is destroyed. The shortcut stays in the data structures for
    //conflict resolution but won't trigger.
    Q_SCRIPTABLE void setInactive(const QStringList &actionId);

    Q_SCRIPTABLE void doRegister(const QStringList &actionId);

    //! @see unregister
    Q_SCRIPTABLE QT_DEPRECATED void unRegister(const QStringList &actionId);

    Q_SCRIPTABLE void activateGlobalShortcutContext(
            const QString &component,
            const QString &context);


    /**
     * Returns the shortcuts registered for @p key.
     *
     * If there is more than one shortcut they are guaranteed to be from the
     * same component but different contexts. All shortcuts are searched.
     */
    Q_SCRIPTABLE QList<KGlobalShortcutInfo> getGlobalShortcutsByKey(int key) const;

    /**
     * Return true if the @p shortcut is available for @p component.
     */
    Q_SCRIPTABLE bool isGlobalShortcutAvailable(
            int key,
            const QString &component) const;

    /**
     * Delete the shortcut with @a component and @name.
     *
     * The shortcut is removed from the registry even if it is currently
     * present. It is removed from all contexts.
     *
     * @param componentUnique the components unique identifier
     * @param shortcutUnique the shortcut id
     *
     * @return @c true if the shortcuts was deleted, @c false if it didn't * exist.
     */
    Q_SCRIPTABLE bool unregister(
            const QString &componentUnique,
            const QString &shortcutUnique);

    Q_SCRIPTABLE void blockGlobalShortcuts(bool);

Q_SIGNALS:

    Q_SCRIPTABLE void yourShortcutGotChanged(const QStringList &actionId, const QList<int> &newKeys);


private:

    void scheduleWriteSettings() const;

    KGlobalAccelDPrivate *const d;
};

#endif //KGLOBALACCELD_H
