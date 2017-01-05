/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2003 Andras Mantia <amantia@kde.org>
              (C) 2005-2006 Hamish Rodda <rodda@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KFONTACTION_H
#define KFONTACTION_H

#include <kselectaction.h>

/**
 * An action to select a font family.
 * On a toolbar this will show a combobox with all the fonts on the system.
 */
class KWIDGETSADDONS_EXPORT KFontAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY(QString font READ font WRITE setFont)

public:
    KFontAction(uint fontListCriteria, QObject *parent);
    explicit KFontAction(QObject *parent);
    KFontAction(const QString &text, QObject *parent);
    KFontAction(const QIcon &icon, const QString &text, QObject *parent);
    virtual ~KFontAction();

    QString font() const;

    void setFont(const QString &family);

    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;

private:
    class KFontActionPrivate;
    KFontActionPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_slotFontChanged(const QFont &))
};

#endif
