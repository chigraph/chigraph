/*
    This file is part of KDE.

    Copyright (c) 2009 Frederik Gladhorn <gladhorn@kde.org>
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
#ifndef ATTICA_ITEMJOB_H
#define ATTICA_ITEMJOB_H

#include "attica_export.h"
#include "getjob.h"
#include "deletejob.h"
#include "postjob.h"
#include "putjob.h"

namespace Attica
{
class Provider;

template <class T>
class ATTICA_EXPORT ItemJob : public GetJob
{
public:
    T result() const;

private:
    ItemJob(PlatformDependent *, const QNetworkRequest &request);
    void parse(const QString &xml) Q_DECL_OVERRIDE;
    T m_item;
    friend class Attica::Provider;
};

template <class T>
class ATTICA_EXPORT ItemDeleteJob : public DeleteJob
{
public:
    T result() const;

private:
    ItemDeleteJob(PlatformDependent *, const QNetworkRequest &request);
    void parse(const QString &xml) Q_DECL_OVERRIDE;
    T m_item;
    friend class Attica::Provider;
};

template <class T>
class ATTICA_EXPORT ItemPostJob : public PostJob
{
public:
    T result() const;

private:
    ItemPostJob(PlatformDependent *internals, const QNetworkRequest &request, QIODevice *data);
    ItemPostJob(PlatformDependent *internals, const QNetworkRequest &request, const StringMap &parameters = StringMap());

    void parse(const QString &xml) Q_DECL_OVERRIDE;
    T m_item;
    friend class Attica::Provider;
};

template <class T>
class ATTICA_EXPORT ItemPutJob : public PutJob
{
public:
    T result() const;

private:
    ItemPutJob(PlatformDependent *internals, const QNetworkRequest &request, QIODevice *data);
    ItemPutJob(PlatformDependent *internals, const QNetworkRequest &request, const StringMap &parameters = StringMap());

    void parse(const QString &xml) Q_DECL_OVERRIDE;
    T m_item;
    friend class Attica::Provider;
};

}

#endif
