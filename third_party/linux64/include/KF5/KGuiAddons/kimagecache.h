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

#ifndef KIMAGECACHE_H
#define KIMAGECACHE_H

// check that KGUIADDONS_LIB is defined in case the application is not using CMake
// (if KGUIADDONS_LIB is not defined, we cannot assume that KCOREADDONS_LIB not being
// defined means that we are not linked against KCoreAddons)
#if defined(KGUIADDONS_LIB) && !defined(KCOREADDONS_LIB)
#ifdef __GNUC__
#warning "KImageCache requires KF5CoreAddons (for kshareddatacache.h)"
#else
#pragma message("KImageCache requires KF5CoreAddons (for kshareddatacache.h)")
#endif
#endif

#include <klocalimagecacheimpl.h>
#include <kshareddatacache.h>
#include <QPixmap>
#include <QImage>

#define KImageCache KSharedPixmapCacheMixin<KSharedDataCache>

/**
 * @brief A simple wrapping layer over KSharedDataCache to support caching
 * images and pixmaps.
 *
 * This class can be used to share images between different processes, which
 * is useful when it is known that such images will be used across many
 * processes, or when creating the image is expensive.
 *
 * In addition, the class also supports caching QPixmaps <em>in a single
 * process</em> using the setPixmapCaching() function.
 *
 * Tips for use: If you already have QPixmaps that you intend to use, and
 * you do not need access to the actual image data, then try to store and
 * retrieve QPixmaps for use.
 *
 * On the other hand, if you will need to store and retrieve actual image
 * data (to modify the image after retrieval for instance) then you should
 * use QImage to save the conversion cost from QPixmap to QImage.
 *
 * KSharedPixmapCacheMixin is a subclass of KSharedDataCache, so all of the methods that
 * can be used with KSharedDataCache can be used with KSharedPixmapCacheMixin,
 * <em>with the exception of KSharedDataCache::insert() and
 * KSharedDataCache::find()</em>.
 *
 * @author Michael Pyne <mpyne@kde.org>
 * @since 4.5
 */
template<class T>
class KSharedPixmapCacheMixin : public T, private KLocalImageCacheImplementation
{
public:
    /**
     * Constructs an image cache, named by @p cacheName, with a default
     * size of @p defaultCacheSize.
     *
     * @param cacheName Name of the cache to use.
     * @param defaultCacheSize The default size, in bytes, of the cache.
     *  The actual on-disk size will be slightly larger. If the cache already
     *  exists, it will not be resized. If it is required to resize the
     *  cache then use the deleteCache() function to remove that cache first.
     * @param expectedItemSize The expected general size of the items to be
     *  added to the image cache, in bytes. Use 0 if you just want a default
     *  item size.
     */
    KSharedPixmapCacheMixin(const QString &cacheName,
                            unsigned defaultCacheSize,
                            unsigned expectedItemSize = 0)
        : T(cacheName, defaultCacheSize, expectedItemSize),
          KLocalImageCacheImplementation(defaultCacheSize) {}

    /**
     * Inserts the pixmap given by @p pixmap to the cache, accessible with
     * @p key. The pixmap must be converted to a QImage in order to be stored
     * into shared memory. In order to prevent unnecessary conversions from
     * taking place @p pixmap will also be cached (but not in shared
     * memory) and would be accessible using findPixmap() if pixmap caching is
     * enabled.
     *
     * @param key Name to access @p pixmap with.
     * @param pixmap The pixmap to add to the cache.
     * @return true if the pixmap was successfully cached, false otherwise.
     * @see setPixmapCaching()
     */
    bool insertPixmap(const QString &key, const QPixmap &pixmap)
    {
        insertLocalPixmap(key, pixmap);

        // One thing to think about is only inserting things to the shared cache
        // that are frequently used. But that would require tracking the use count
        // in our local cache too, which I think is probably too much work.

        return insertImage(key, pixmap.toImage());
    }

    /**
     * Inserts the @p image into the shared cache, accessible with @p key. This
     * variant is preferred over insertPixmap() if your source data is already a
     * QImage, if it is essential that the image be in shared memory (such as
     * for SVG icons which have a high render time), or if it will need to be
     * in QImage form after it is retrieved from the cache.
     *
     * @param key Name to access @p image with.
     * @param image The image to add to the shared cache.
     * @return true if the image was successfully cached, false otherwise.
     */
    bool insertImage(const QString &key, const QImage &image)
    {
        if (this->insert(key, serializeImage(image))) {
            updateModifiedTime();
            return true;
        }

        return false;
    }

    /**
     * Copies the cached pixmap identified by @p key to @p destination. If no such
     * pixmap exists @p destination is unchanged.
     *
     * @return true if the pixmap identified by @p key existed, false otherwise.
     * @see setPixmapCaching()
     */
    bool findPixmap(const QString &key, QPixmap *destination) const
    {
        if (findLocalPixmap(key, destination)) {
            return true;
        }

        QByteArray cachedData;
        if (!this->find(key, &cachedData) || cachedData.isNull()) {
            return false;
        }

        if (destination) {
            destination->loadFromData(cachedData, "PNG");

            // Manually re-insert to pixmap cache if we'll be using this one.
            insertLocalPixmap(key, *destination);
        }

        return true;
    }

    /**
     * Copies the cached image identified by @p key to @p destination. If no such
     * image exists @p destination is unchanged.
     *
     * @return true if the image identified by @p key existed, false otherwise.
     */
    bool findImage(const QString &key, QImage *destination) const
    {
        QByteArray cachedData;
        if (!this->find(key, &cachedData) || cachedData.isNull()) {
            return false;
        }

        if (destination) {
            destination->loadFromData(cachedData, "PNG");
        }

        return true;
    }

    /**
     * Removes all entries from the cache. In addition any cached pixmaps (as per
     * setPixmapCaching()) are also removed.
     */
    void clear()
    {
        clearLocalCache();
        T::clear();
    }

    /**
     * @return The time that an image or pixmap was last inserted into a cache.
     */
    using KLocalImageCacheImplementation::lastModifiedTime;

    /**
     * @return if QPixmaps added with insertPixmap() will be stored in a local
     * pixmap cache as well as the shared image cache. The default is to cache
     * pixmaps locally.
     */
    using KLocalImageCacheImplementation::pixmapCaching;

    /**
     * Enables or disables local pixmap caching. If it is anticipated that a pixmap
     * will be frequently needed then this can actually save memory overall since the
     * X server or graphics card will not have to store duplicate copies of the same
     * image.
     *
     * @param enable Enables pixmap caching if true, disables otherwise.
     */
    using KLocalImageCacheImplementation::setPixmapCaching;

    /**
     * @return The highest memory size in bytes to be used by cached pixmaps.
     * @since 4.6
     */
    using KLocalImageCacheImplementation::pixmapCacheLimit;

    /**
     * Sets the highest memory size the pixmap cache should use.
     *
     * @param size The size in bytes
     * @since 4.6
     */
    using KLocalImageCacheImplementation::setPixmapCacheLimit;
};

#endif /* KIMAGECACHE_H */
