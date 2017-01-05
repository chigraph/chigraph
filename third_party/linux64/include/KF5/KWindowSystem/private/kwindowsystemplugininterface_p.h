/*
 * Copyright 2015 Martin Gräßlin <mgraesslin@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef KWINDOWSYSTEMPLUGININTERFACE_P_H
#define KWINDOWSYSTEMPLUGININTERFACE_P_H
#include <kwindowsystem_export.h>
#include "netwm_def.h"

#include <QObject>
#include <QWidgetList> //For WId

class KWindowEffectsPrivate;
class KWindowInfoPrivate;
class KWindowSystemPrivate;

class KWINDOWSYSTEM_EXPORT KWindowSystemPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit KWindowSystemPluginInterface(QObject *parent = 0);
    virtual ~KWindowSystemPluginInterface();

    virtual KWindowEffectsPrivate *createEffects();
    virtual KWindowSystemPrivate *createWindowSystem();
    virtual KWindowInfoPrivate *createWindowInfo(WId window, NET::Properties properties, NET::Properties2 properties2);
};

Q_DECLARE_INTERFACE(KWindowSystemPluginInterface, "org.kde.kwindowsystem.KWindowSystemPluginInterface")

#endif
