/*
    This file is part of KDE.

    Copyright 2010 Dan Leinir Turthra Jensen <admin@leinir.dk>

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
#ifndef ATTICA_BUILDSERVICEJOBOUTPUT_H
#define ATTICA_BUILDSERVICEJOBOUTPUT_H

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>

#include "attica_export.h"

namespace Attica
{

class ATTICA_EXPORT BuildServiceJobOutput
{
public:
    typedef QList<BuildServiceJobOutput> List;
    class Parser;

    BuildServiceJobOutput();
    BuildServiceJobOutput(const BuildServiceJobOutput &other);
    BuildServiceJobOutput &operator=(const BuildServiceJobOutput &other);
    ~BuildServiceJobOutput();

    void setOutput(const QString &output);
    QString output() const;

    bool isRunning() const;
    bool isCompleted() const;
    bool isFailed() const;

    bool isValid() const;
private:
    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Attica

#endif // ATTICA_BUILDSERVICEJOBOUTPUT_H
