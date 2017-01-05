/* This file is part of the KDE libraries
   Copyright (C) 2006 Hamish Rodda <rodda@kde.org>

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

#ifndef KTOOLBARSPACERACTION_H
#define KTOOLBARSPACERACTION_H

#include <QWidgetAction>

#include <kwidgetsaddons_export.h>

/**
 * An extension to KAction which allows for adding a spacer item into a
 * QToolBar / KToolBar.
 *
 * This is useful if you want to align at the right (or left if RTL) and
 * need something that pushes the element to the end of the tool bar.
 *
 * @author Hamish Rodda <rodda@kde.org>
 */
class KWIDGETSADDONS_EXPORT KToolBarSpacerAction : public QWidgetAction
{
    Q_OBJECT

public:
    /**
     * Creates a new toolbar spacer action.
     *
     * @param parent The action's parent object.
     */
    explicit KToolBarSpacerAction(QObject *parent);

    virtual ~KToolBarSpacerAction();

    /**
     * Reimplemented from @see QActionWidget.
     */
    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;
};

#endif
