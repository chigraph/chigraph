/*
    This file is part of KDE.

    Copyright (c) 2008 Cornelius Schumacher <schumacher@kde.org>

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

#ifndef ATTICA_POSTJOB_H
#define ATTICA_POSTJOB_H

#include <QtNetwork/QNetworkRequest>

#include "attica_export.h"
#include "atticabasejob.h"

// workaround to get initialization working with gcc < 4.4
typedef QMap<QString, QString> StringMap;

namespace Attica
{
class Provider;

class ATTICA_EXPORT PostJob : public BaseJob
{
    Q_OBJECT

protected:
    PostJob(PlatformDependent *internals, const QNetworkRequest &request, QIODevice *data);
    PostJob(PlatformDependent *internals, const QNetworkRequest &request, const StringMap &parameters = StringMap());
    PostJob(PlatformDependent *internals, const QNetworkRequest &request, const QByteArray &byteArray);

private:
    QNetworkReply *executeRequest() Q_DECL_OVERRIDE;
    void parse(const QString &) Q_DECL_OVERRIDE;

    QIODevice *m_ioDevice;
    QByteArray m_byteArray;

    QString m_responseData;
    const QNetworkRequest m_request;

    QString m_status;
    QString m_statusMessage;

    friend class Attica::Provider;
};

}

#endif
