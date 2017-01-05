/*
 * This file is part of the KDE project.
 * Copyright © 2010 Michael Pyne <mpyne@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KLOCALIMAGECACHEIMPL_H
#define KLOCALIMAGECACHEIMPL_H

#include <kguiaddons_export.h>
class KLocalImageCacheImplementationPrivate;

class QImage;
class QPixmap;
class QByteArray;
class QDateTime;
class QString;

/**
 * You are not supposed to use this class directly, use KImageCache instead
 *
 * @internal
 */
class KGUIADDONS_EXPORT KLocalImageCacheImplementation
{
private:
    KLocalImageCacheImplementation(unsigned defaultCacheSize);

public:
    virtual ~KLocalImageCacheImplementation();

    QDateTime lastModifiedTime() const;

    bool pixmapCaching() const;
    void setPixmapCaching(bool enable);

    int pixmapCacheLimit() const;
    void setPixmapCacheLimit(int size);

protected:
    void updateModifiedTime();
    QByteArray serializeImage(const QImage &image) const;

    bool insertLocalPixmap(const QString &key, const QPixmap &pixmap) const;
    bool findLocalPixmap(const QString &key, QPixmap *destination) const;
    void clearLocalCache();

private:
    KLocalImageCacheImplementationPrivate *const d; ///< @internal

    template<class T> friend class KSharedPixmapCacheMixin;
};

#endif /* KLOCALIMAGECACHEIMPL_H */
