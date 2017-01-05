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
#ifndef ATTICA_ATTICABASEJOB_H
#define ATTICA_ATTICABASEJOB_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

#include "attica_export.h"
#include "metadata.h"

class QNetworkReply;

namespace Attica
{
class PlatformDependent;

class ATTICA_EXPORT BaseJob : public QObject
{
    Q_OBJECT

public:
    virtual ~BaseJob();

    Metadata metadata() const;

    enum NetworkRequestCustomAttributes {
        UserAttribute = QNetworkRequest::User + 1,
        PasswordAttribute
    };

public Q_SLOTS:
    void start();
    void abort();

Q_SIGNALS:
    void finished(Attica::BaseJob *job);

protected Q_SLOTS:
    void dataFinished();

protected:
    BaseJob(PlatformDependent *internals);

    void setMetadata(const Metadata &data) const;

    virtual QNetworkReply *executeRequest() = 0;
    virtual void parse(const QString &xml) = 0;
    PlatformDependent *internals();
    void setError(int errorCode);
    void setErrorString(const QString &errorString);

private Q_SLOTS:
    void doWork();
    void authenticationRequired(QNetworkReply *, QAuthenticator *);

private:
    BaseJob(const BaseJob &other);
    BaseJob &operator=(const BaseJob &other);

    class Private;
    Private *d;
};

}

#endif
