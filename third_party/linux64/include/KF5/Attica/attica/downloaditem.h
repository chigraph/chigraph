/*
    This file is part of KDE.

    Copyright (c) 2009 Frederik Gladhorn <gladhorn@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef ATTICA_DOWNLOADITEM_H
#define ATTICA_DOWNLOADITEM_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QUrl>

#include "attica_export.h"
#include "downloaddescription.h"

namespace Attica
{

class ATTICA_EXPORT DownloadItem
{
public:
    typedef QList<DownloadItem> List;
    class Parser;

    /**
    * Creates an empty DownloadItem
    */
    DownloadItem();

    /**
    * Copy constructor.
    * @param other the DownloadItem to copy from
    */
    DownloadItem(const DownloadItem &other);

    /**
    * Assignment operator.
    * @param other the DownloadItem to assign from
    * @return pointer to this DownloadItem
    */
    DownloadItem &operator=(const DownloadItem &other);

    /**
    * Destructor.
    */
    ~DownloadItem();

    void setUrl(const QUrl &url);
    QUrl url() const;
    void setMimeType(const QString &mimeType);
    QString mimeType() const;
    void setPackageName(const QString &packageName);
    QString packageName() const;
    void setPackageRepository(const QString &packageRepository);
    QString packageRepository() const;
    void setGpgFingerprint(const QString &gpgFingerprint);
    QString gpgFingerprint() const;
    void setGpgSignature(const QString &gpgSignature);
    QString gpgSignature() const;
    void setType(Attica::DownloadDescription::Type type);
    Attica::DownloadDescription::Type type();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif // DOWNLOADITEM_H
