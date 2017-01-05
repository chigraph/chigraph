/*
 *   Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef KWINDOWINFO_P_H
#define KWINDOWINFO_P_H
#include <kwindowsystem_export.h>
#include "netwm_def.h"

#include <QByteArray>
#include <QRect>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QSharedData>
#include <QWidgetList> //For WId

class KWindowInfoPrivateDesktopFileNameExtension;
class KWindowInfoPrivatePidExtension;

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivate  : public QSharedData
{
public:
    virtual ~KWindowInfoPrivate();

    WId win() const;

    virtual bool valid(bool withdrawn_is_valid) const = 0;
    virtual NET::States state() const = 0;
    virtual bool isMinimized() const = 0;
    virtual NET::MappingState mappingState() const = 0;
    virtual NETExtendedStrut extendedStrut() const = 0;
    virtual NET::WindowType windowType(NET::WindowTypes supported_types) const = 0;
    virtual QString visibleName() const = 0;
    virtual QString visibleNameWithState() const = 0;
    virtual QString name() const = 0;
    virtual QString visibleIconName() const = 0;
    virtual QString visibleIconNameWithState() const = 0;
    virtual QString iconName() const = 0;
    virtual bool onAllDesktops() const = 0;
    virtual bool isOnDesktop(int desktop) const = 0;
    virtual int desktop() const = 0;
    virtual QStringList activities() const = 0;
    virtual QRect geometry() const = 0;
    virtual QRect frameGeometry() const = 0;
    virtual WId transientFor() const = 0;
    virtual WId groupLeader() const = 0;
    virtual QByteArray windowClassClass() const = 0;
    virtual QByteArray windowClassName() const = 0;
    virtual QByteArray windowRole() const = 0;
    virtual QByteArray clientMachine() const = 0;
    virtual bool actionSupported(NET::Action action) const = 0;

    KWindowInfoPrivateDesktopFileNameExtension *desktopFileNameExtension() const;
    KWindowInfoPrivatePidExtension *pidExtension() const;

    static KWindowInfoPrivate *create(WId window, NET::Properties properties, NET::Properties2 properties2);

protected:
    KWindowInfoPrivate(WId window, NET::Properties properties, NET::Properties2 properties2);

    void installDesktopFileNameExtension(KWindowInfoPrivateDesktopFileNameExtension *extension);
    void installPidExtension(KWindowInfoPrivatePidExtension *extension);

private:
    class Private;
    const QScopedPointer<Private> d;
};

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivateDesktopFileNameExtension
{
public:
    virtual ~KWindowInfoPrivateDesktopFileNameExtension();

    virtual QByteArray desktopFileName() const = 0;

protected:
    explicit KWindowInfoPrivateDesktopFileNameExtension();
};

class KWINDOWSYSTEM_EXPORT KWindowInfoPrivatePidExtension
{
public:
    virtual ~KWindowInfoPrivatePidExtension();

    virtual int pid() const = 0;

protected:
    explicit KWindowInfoPrivatePidExtension();
};

#endif
