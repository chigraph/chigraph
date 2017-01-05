/* This file is part of the KDE libraries
 *  Copyright (C) 2005-2012 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KURLMIMEDATA_H
#define KURLMIMEDATA_H

#include <QMap>
#include <QUrl>
#include "kcoreaddons_export.h"
QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

/**
 * Utility functions for using URLs in QMimeData.
 * In addition to QMimeData::setUrls() and QMimeData::urls(), these functions allow to:
 *
 * - Store two sets of URLs, the KDE-specific URLs and the equivalent local-file URLs
 *     for compatibility with non-KDE applications
 * - Store KIO metadata, such as the HTTP referrer for a given URL (some websites
 *     require it for downloading e.g. an image)
 *
 * @since 5.0
 */
namespace KUrlMimeData
{
typedef QMap<QString, QString> MetaDataMap;

/**
 * Adds URLs and KIO metadata into the given QMimeData.
 *
 * WARNING: do not call this method multiple times on the same mimedata object,
 * you can add urls only once. But you can add other things, e.g. images, XML...
 *
 * @param mimeData the QMimeData instance used to drag or copy this URL
 */
KCOREADDONS_EXPORT void setUrls(const QList<QUrl> &urls, const QList<QUrl> &mostLocalUrls,
                                QMimeData *mimeData);
/**
 * @param metaData KIO metadata shipped in the mime data, which is used for instance to
 * set a correct HTTP referrer (some websites require it for downloading e.g. an image)
 */
KCOREADDONS_EXPORT void setMetaData(const MetaDataMap &metaData, QMimeData *mimeData);

/**
 * Return the list of mimeTypes that can be decoded by urlsFromMimeData
 */
KCOREADDONS_EXPORT QStringList mimeDataTypes();

/**
 * Flags to be used in urlsFromMimeData.
 */
enum DecodeOptions {
    /**
     * When the mimedata contains both KDE-style URLs (eg: desktop:/foo) and
     * the "most local" version of the URLs (eg: file:///home/dfaure/Desktop/foo),
     * decode it as local urls. Useful in paste/drop operations that end up calling KIO,
     * so that urls from other users work as well.
     */
    PreferLocalUrls,
    /**
     * When the mimedata contains both KDE-style URLs (eg: desktop:/foo) and
     * the "most local" version of the URLs (eg: file:///home/dfaure/Desktop/foo),
     * decode it as the KDE-style URL. Useful in DnD code e.g. when moving icons,
     * and the kde-style url is used as identifier for the icons.
     */
    PreferKdeUrls
};

/**
 * Extract a list of urls from the contents of @p mimeData.
 *
 * Compared to QMimeData::urls(), this method has support for retrieving KDE-specific URLs
 * when urls() would retrieve "most local URLs" instead.
 *
 * Decoding will fail if @p mimeData does not contain any URLs, or if at
 * least one extracted URL is not valid.
 *
 * @param mimeData the mime data to extract from; cannot be 0
 * @param decodeOptions options for decoding
 * @param metaData optional pointer to a map which will hold the metadata after this call
 * @return the list of urls
 */
KCOREADDONS_EXPORT QList<QUrl> urlsFromMimeData(const QMimeData *mimeData,
        DecodeOptions decodeOptions = PreferKdeUrls,
        MetaDataMap *metaData = 0);

}

#endif /* KURLMIMEDATA_H */

