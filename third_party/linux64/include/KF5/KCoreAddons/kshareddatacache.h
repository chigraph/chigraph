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

#ifndef KSHAREDDATACACHE_H
#define KSHAREDDATACACHE_H

#include <kcoreaddons_export.h>

class QString;
class QByteArray;

/**
 * @brief A simple data cache which uses shared memory to quickly access data
 * stored on disk.
 *
 * This class is meant to be used with KImageCache and similar classes but can
 * be used directly if used with care.
 *
 * Example usage:
 *
 * @code
 * QString loadTranslatedDocument(KSharedDataCache *cache) {
 *
 *   // Find the data
 *   QByteArray document;
 *
 *   if (!cache->find("translated-doc-template", &document)) {
 *     // Entry is not cached, manually generate and then add to cache.
 *     document = translateDocument(globalTemplate());
 *     cache->insert(document);
 *   }
 *
 *   // Don't forget to encode/decode properly
 *   return QString::fromUtf8(document);
 * }
 * @endcode
 *
 * @author Michael Pyne <mpyne@kde.org>
 * @see KImageCache
 * @since 4.5
 */
class KCOREADDONS_EXPORT KSharedDataCache
{
public:
    /**
     * Attaches to a shared cache, creating it if necessary. If supported, this
     * data cache will be shared across all processes using this cache (with
     * subsequent memory savings).  If shared memory is unsupported or a
     * failure occurs, caching will still be supported, but only in the same
     * process, and only using the same KSharedDataCache object.
     *
     * @param cacheName Name of the cache to use/share.
     * @param defaultCacheSize Amount of data to be able to store, in bytes. The
     *   actual size will be slightly larger on disk due to accounting
     *   overhead.  If the cache already existed then it <em>will not</em> be
     *   resized. For this reason you should specify some reasonable size.
     * @param expectedItemSize The average size of an item that would be stored
     *   in the cache, in bytes. Choosing an average size of zero bytes causes
     *   KSharedDataCache to use whatever it feels is the best default for the
     *   system.
     */
    KSharedDataCache(const QString &cacheName,
                     unsigned defaultCacheSize,
                     unsigned expectedItemSize = 0);
    ~KSharedDataCache();

    enum EvictionPolicy {
        // The default value for data in our shared memory will be 0, so it is
        // important that whatever we want for the default value is also 0.
        NoEvictionPreference = 0,
        EvictLeastRecentlyUsed,
        EvictLeastOftenUsed,
        EvictOldest
    };

    /**
     * @return The removal policy in use by the shared cache.
     * @see EvictionPolicy
     */
    EvictionPolicy evictionPolicy() const;

    /**
     * Sets the entry removal policy for the shared cache to
     * @p newPolicy. The default is EvictionPolicy::NoEvictionPreference.
     *
     * @see EvictionPolicy
     */
    void setEvictionPolicy(EvictionPolicy newPolicy);

    /**
     * Attempts to insert the entry @p data into the shared cache, named by
     * @p key, and returns true only if successful.
     *
     * Note that even if the insert was successful, that the newly added entry
     * may be evicted by other processes contending for the cache.
     */
    bool insert(const QString &key, const QByteArray &data);

    /**
     * Returns the data in the cache named by @p key (even if it's some other
     * process's data named with the same key!), stored in @p destination. If there is
     * no entry named by @p key then @p destination is left unchanged. The return value
     * is used to tell what happened.
     *
     * If you simply want to verify whether an entry is present in the cache then
     * see contains().
     *
     * @param key The key to find in the cache.
     * @param destination Is set to the value of @p key in the cache if @p key is
     *                    present, left unchanged otherwise.
     * @return true if @p key was present in the cache (@p destination will also be
     *         updated), false if @p key was not present (@p destination will be
     *         unchanged).
     */
    bool find(const QString &key, QByteArray *destination) const;

    /**
     * Removes all entries from the cache.
     */
    void clear();

    /**
     * Removes the underlying file from the cache. Note that this is *all* that this
     * function does. The shared memory segment is still attached and will still contain
     * all the data until all processes currently attached remove the mapping.
     *
     * In order to remove the data see clear().
     */
    static void deleteCache(const QString &cacheName);

    /**
     * Returns true if the cache currently contains the image for the given
     * filename.
     *
     * NOTE: Calling this function is threadsafe, but it is in general not
     * possible to guarantee the image stays cached immediately afterwards,
     * so if you need the result use find().
     */
    bool contains(const QString &key) const;

    /**
     * Returns the usable cache size in bytes. The actual amount of memory
     * used will be slightly larger than this to account for required
     * accounting overhead.
     */
    unsigned totalSize() const;

    /**
     * Returns the amount of free space in the cache, in bytes. Due to
     * implementation details it is possible to still not be able to fit an
     * entry in the cache at any given time even if it is smaller than the
     * amount of space remaining.
     */
    unsigned freeSize() const;

    /**
     * @return The shared timestamp of the cache. The interpretation of the
     *         timestamp returned is up to the application. KSharedDataCache
     *         will initialize the timestamp to the time returned by @c time(2)
     *         on cache creation, but KSharedDataCache will not touch the
     *         timestamp again.
     * @see setTimestamp()
     * @since 4.6
     */
    unsigned timestamp() const;

    /**
     * Sets the shared timestamp of the cache. Timestamping is supported to
     * allow applications to more effectively "version" the data stored in the
     * cache. However, the timestamp is shared with <em>all</em> applications
     * using the cache so you should always be prepared for invalid
     * timestamps.
     *
     * When the cache is first created (note that this is different from
     * attaching to an existing shared cache on disk), the cache timestamp is
     * initialized to the time returned by @c time(2). KSharedDataCache will
     * not update the timestamp again, it is only updated through this method.
     *
     * Example:
     * @code
     * QImage loadCachedImage(const QString &key)
     * {
     *     // Check timestamp
     *     if (m_sharedCache->timestamp() < m_currentThemeTimestamp) {
     *         // Cache is stale, clean it out.
     *         m_sharedCache->clear();
     *         m_sharedCache->setTimestamp(m_currentThemeTimestamp);
     *     }
     *
     *     // Check cache and load image as usual...
     * }
     * @endcode
     *
     * @param newTimestamp The new timestamp to mark the shared cache with.
     * @see timestamp()
     * @since 4.6
     */
    void setTimestamp(unsigned newTimestamp);

private:
    class Private;
    Private *d;
};

#endif
