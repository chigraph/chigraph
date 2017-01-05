/*
    This file is part of KDE.

    Copyright (c) 2009 Eckhart Wörner <ewoerner@kde.org>
    Copyright (c) 2011 Laszlo Papp <djszapi@archlinux.us>

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

#ifndef ATTICA_PLATFORMDEPENDENT_H
#define ATTICA_PLATFORMDEPENDENT_H

#include <QtCore/QList>
#include <QtCore/QtPlugin>

class QByteArray;
class QIODevice;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QString;
class QUrl;
class QStringList;

namespace Attica
{

class PlatformDependent
{
public:
    virtual ~PlatformDependent() {}
    virtual QList<QUrl> getDefaultProviderFiles() const = 0;
    virtual void addDefaultProviderFile(const QUrl &url) = 0;
    virtual void removeDefaultProviderFile(const QUrl &url) = 0;

    /**
     * Providers are enabled by default. Use this call to disable or enable them later.
     */
    virtual void enableProvider(const QUrl &baseUrl, bool enabled) const = 0;
    virtual bool isEnabled(const QUrl &baseUrl) const  = 0;

    virtual bool hasCredentials(const QUrl &baseUrl) const = 0;
    virtual bool loadCredentials(const QUrl &baseUrl, QString &user, QString &password) = 0;
    virtual bool askForCredentials(const QUrl &baseUrl, QString &user, QString &password) = 0;
    virtual bool saveCredentials(const QUrl &baseUrl, const QString &user, const QString &password) = 0;
    virtual QNetworkReply *get(const QNetworkRequest &request) = 0;
    virtual QNetworkReply *post(const QNetworkRequest &request, QIODevice *data) = 0;
    virtual QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data) = 0;
    virtual void setNam(QNetworkAccessManager *) {}
    virtual QNetworkAccessManager *nam() = 0;
};

}

Q_DECLARE_INTERFACE(Attica::PlatformDependent, "org.kde.Attica.Internals/1.2")

#endif
