/* This file is part of the KDE project
   Copyright 2007 David Faure <faure@kde.org>

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

#ifndef KSERVICEACTION_H
#define KSERVICEACTION_H

#include <kservice_export.h>
#include <QtCore/QSharedDataPointer>
class QVariant;
class KServiceActionPrivate;

/**
 * Represents an action in a .desktop file
 * Actions are defined with the config key Actions in the [Desktop Entry]
 * group, followed by one group per action, as per the desktop entry standard.
 * @see KService::actions
 */
class KSERVICE_EXPORT KServiceAction
{
public:
    /**
     * Creates a KServiceAction.
     * Normally you don't have to do this, KService creates the actions
     * when parsing the .desktop file.
     */
    KServiceAction(const QString &name, const QString &text,
                   const QString &icon, const QString &exec,
                   bool noDisplay = false);
    /**
     * @internal
     * Needed for operator>>
     */
    KServiceAction();
    /**
     * Destroys a KServiceAction.
     */
    ~KServiceAction();

    /**
     * Copy constructor
     */
    KServiceAction(const KServiceAction &other);
    /**
     * Assignment operator
     */
    KServiceAction &operator=(const KServiceAction &other);

    /**
     * Sets the action's internal data to the given @p userData.
     */
    void setData(const QVariant &userData);
    /**
     * @return the action's internal data.
     */
    QVariant data() const;

    /**
     * @return the action's internal name
     * For instance Actions=Setup;... and the group [Desktop Action Setup]
     * define an action with the name "Setup".
     */
    QString name() const;

    /**
     * @return the action's text, as defined by the Name key in the desktop action group
     */
    QString text() const;

    /**
     * @return the action's icon, as defined by the Icon key in the desktop action group
     */
    QString icon() const;

    /**
     * @return the action's exec command, as defined by the Exec key in the desktop action group
     */
    QString exec() const;

    /**
     * Returns whether the action should be suppressed in menus.
     * This is useful for having actions with a known name that the code
     * looks for explicitly, like Setup and Root for kscreensaver actions,
     * and which should not appear in popup menus.
     * @return true to suppress this service
     */
    bool noDisplay() const;

    /**
     * Returns whether the action is a separator.
     * This is true when the Actions key contains "_SEPARATOR_".
     */
    bool isSeparator() const;

private:
    QSharedDataPointer<KServiceActionPrivate> d;
    friend KSERVICE_EXPORT QDataStream &operator>>(QDataStream &str, KServiceAction &act);
    friend KSERVICE_EXPORT QDataStream &operator<<(QDataStream &str, const KServiceAction &act);
};

KSERVICE_EXPORT QDataStream &operator>>(QDataStream &str, KServiceAction &act);
KSERVICE_EXPORT QDataStream &operator<<(QDataStream &str, const KServiceAction &act);

#endif /* KSERVICEACTION_H */

