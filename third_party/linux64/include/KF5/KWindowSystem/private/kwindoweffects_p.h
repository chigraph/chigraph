/*
 * Copyright 2014 Martin Gräßlin <mgraesslin@kde.org>
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
#ifndef KWINDOWEFFECTS_P_H
#define KWINDOWEFFECTS_P_H
#include "kwindoweffects.h"

class KWINDOWSYSTEM_EXPORT KWindowEffectsPrivate
{
public:
    virtual ~KWindowEffectsPrivate();
    virtual bool isEffectAvailable(KWindowEffects::Effect effect) = 0;
    virtual void slideWindow(WId id, KWindowEffects::SlideFromLocation location, int offset) = 0;
    virtual void slideWindow(QWidget *widget, KWindowEffects::SlideFromLocation location) = 0;
    virtual QList<QSize> windowSizes(const QList<WId> &ids) = 0;
    virtual void presentWindows(WId controller, const QList<WId> &ids) = 0;
    virtual void presentWindows(WId controller, int desktop = NET::OnAllDesktops) = 0;
    virtual void highlightWindows(WId controller, const QList<WId> &ids) = 0;
    virtual void enableBlurBehind(WId window, bool enable = true, const QRegion &region = QRegion()) = 0;
    virtual void enableBackgroundContrast(WId window, bool enable = true, qreal contrast = 1, qreal intensity = 1, qreal saturation = 1, const QRegion &region = QRegion()) = 0;
    virtual void markAsDashboard(WId window) = 0;
protected:
    KWindowEffectsPrivate();
};

#endif
