/*
    This file is part of KDE.

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

#ifndef ATTICA_PLATFORMDEPENDENT_V2_H
#define ATTICA_PLATFORMDEPENDENT_V2_H

#include <QtCore/QList>
#include <QtCore/QtPlugin>

#include <platformdependent.h>

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

class PlatformDependentV2: public PlatformDependent
{
public:
    virtual ~PlatformDependentV2();
    virtual QNetworkReply *deleteResource(const QNetworkRequest &request) = 0;
    virtual QNetworkReply *put(const QNetworkRequest &request, QIODevice *data) = 0;
    virtual QNetworkReply *put(const QNetworkRequest &request, const QByteArray &data) = 0;
};

}

Q_DECLARE_INTERFACE(Attica::PlatformDependentV2, "org.kde.Attica.InternalsV2/1.2")

#endif
