/* This file is part of the KDE libraries
    Copyright (C) 2006 Hamish Rodda <rodda@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KICONENGINE_H
#define KICONENGINE_H

#include "kiconthemes_export.h"
#include <QIconEngine>
#include <QPointer>

class KIconLoader;

/**
 * \short A class to provide rendering of KDE icons.
 *
 * Currently, this class is not much more than a wrapper around QIconEngine.
 * However, it should not be difficult to extend with features such as SVG
 * rendered icons.
 *
 * Icon themes specifying a KDE-Extensions string list setting, will limit
 * themselves to checking these extensions exclusively, in the order specified
 * in the setting.
 *
 * @author Hamish Rodda <rodda@kde.org>
 */
class KICONTHEMES_EXPORT KIconEngine : public QIconEngine // only exported for kdelibs4support's KIcon
{
public:
    /**
     * Constructs an icon engine for a KDE named icon.
     *
     * @param iconName the name of the icon to load
     * @param iconLoader The KDE icon loader that this engine is to use.
     * @param overlays Add one or more overlays to the icon. See KIconLoader::Overlays.
     *
     * @sa KIconLoader
     */
    KIconEngine(const QString &iconName, KIconLoader *iconLoader, const QStringList &overlays);

    /**
     * \overload
     */
    KIconEngine(const QString &iconName, KIconLoader *iconLoader);

    /**
     * Destructor.
     */
    virtual ~KIconEngine();

    /// Reimplementation
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    /// Reimplementation
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    /// Reimplementation
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    /// Reimplementation
    QString iconName() const Q_DECL_OVERRIDE;
    /// Reimplementation
    QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) const Q_DECL_OVERRIDE;

    QString key() const Q_DECL_OVERRIDE;
    QIconEngine *clone() const Q_DECL_OVERRIDE;
    bool read(QDataStream &in) Q_DECL_OVERRIDE;
    bool write(QDataStream &out) const Q_DECL_OVERRIDE;

    void virtual_hook(int id, void *data) Q_DECL_OVERRIDE;

private:
    //TODO KF6: move those into the d-pointer
    QString mIconName;
    QStringList mOverlays;
    QPointer<KIconLoader> mIconLoader;
};

inline KIconEngine::~KIconEngine()
{
}

#endif
