/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2007 John Layt <john@layt.net>
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KDATEVALIDATOR_H
#define KDATEVALIDATOR_H

#include <kguiaddons_export.h>

#include <QValidator>

/**
* Validates user-entered dates.
*/
class KGUIADDONS_EXPORT KDateValidator : public QValidator
{
public:
    explicit KDateValidator(QObject *parent = 0);
    State validate(QString &text, int &e) const Q_DECL_OVERRIDE;
    void fixup(QString &input) const Q_DECL_OVERRIDE;
    State date(const QString &text, QDate &date) const;
private:
    class KDateValidatorPrivate;
    friend class KDateValidatorPrivate;
    KDateValidatorPrivate *const d;
};

#endif // KDATEVALIDATOR_H
