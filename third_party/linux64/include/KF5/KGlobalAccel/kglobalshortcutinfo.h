#ifndef KGLOBALSHORTCUTINFO_H
#define KGLOBALSHORTCUTINFO_H
/* Copyright (C) 2008 Michael Jansen <kde@michael-jansen.biz>

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

#include <kglobalaccel_export.h>

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QKeySequence>
#include <QtDBus/QDBusArgument>

class KGlobalShortcutInfoPrivate;

/**
 * @author Michael Jansen <kde@michael-jansen.biz>
 */
class KGLOBALACCEL_EXPORT KGlobalShortcutInfo : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "org.kde.kglobalaccel.KShortcutInfo")

    Q_SCRIPTABLE Q_PROPERTY(QString uniqueName READ uniqueName)
    Q_SCRIPTABLE Q_PROPERTY(QString friendlyName READ friendlyName)

    Q_SCRIPTABLE Q_PROPERTY(QString componentUniqueName READ componentUniqueName)
    Q_SCRIPTABLE Q_PROPERTY(QString componentFriendlyName READ componentFriendlyName)

    Q_SCRIPTABLE Q_PROPERTY(QString contextUniqueName READ contextUniqueName)
    Q_SCRIPTABLE Q_PROPERTY(QString contextFriendlyName READ contextFriendlyName)

    Q_SCRIPTABLE Q_PROPERTY(QList<QKeySequence> keys READ keys)
    Q_SCRIPTABLE Q_PROPERTY(QList<QKeySequence> defaultKeys READ keys)

public:

    KGlobalShortcutInfo();

    KGlobalShortcutInfo(const KGlobalShortcutInfo &rhs);

    ~KGlobalShortcutInfo();

    KGlobalShortcutInfo &operator= (const KGlobalShortcutInfo &rhs);

    QString contextFriendlyName() const;

    QString contextUniqueName() const;

    QString componentFriendlyName() const;

    QString componentUniqueName() const;

    QList<QKeySequence> defaultKeys() const;

    QString friendlyName() const;

    QList<QKeySequence> keys() const;

    QString uniqueName() const;

private:

    friend class GlobalShortcut;

    friend KGLOBALACCEL_EXPORT const QDBusArgument &operator>> (
        const QDBusArgument &argument,
        KGlobalShortcutInfo &shortcut);

    //! Implementation details
    KGlobalShortcutInfoPrivate *d;
};

KGLOBALACCEL_EXPORT QDBusArgument &operator<< (
    QDBusArgument &argument,
    const KGlobalShortcutInfo &shortcut);

KGLOBALACCEL_EXPORT const QDBusArgument &operator>> (
    const QDBusArgument &argument,
    KGlobalShortcutInfo &shortcut);

Q_DECLARE_METATYPE(KGlobalShortcutInfo)

#endif /* #ifndef KGLOBALSHORTCUTINFO_H */
