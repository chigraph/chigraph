/* This file is part of the KDE libraries
   Copyright (c) 2003 Scott Wheeler <wheeler@kde.org>
   Copyright (c) 2005 Rafal Rzepecki <divide@users.sourceforge.net>
   Copyright (c) 2006 Hamish Rodda <rodda@kde.org>

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

#ifndef KTREEWIDGETSEARCHLINEWIDGET_H
#define KTREEWIDGETSEARCHLINEWIDGET_H

#include <QWidget>
#include <kitemviews_export.h>

class QModelIndex;
class QTreeWidget;
class KTreeWidgetSearchLine;

/**
 * Creates a widget featuring a KTreeWidgetSearchLine, a label with the text
 * "Search" and a button to clear the search.
 */
class KITEMVIEWS_EXPORT KTreeWidgetSearchLineWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates a KTreeWidgetSearchLineWidget for \a treeWidget with \a parent as the
     * parent.
     */
    explicit KTreeWidgetSearchLineWidget(QWidget *parent = 0, QTreeWidget *treeWidget = 0);

    /**
     * Destroys the KTreeWidgetSearchLineWidget
     */
    ~KTreeWidgetSearchLineWidget();

    /**
     * Returns a pointer to the search line.
     */
    KTreeWidgetSearchLine *searchLine() const;

protected Q_SLOTS:
    /**
     * Creates the widgets inside of the widget.  This is called from the
     * constructor via a single shot timer so that it it guaranteed to run
     * after construction is complete.  This makes it suitable for overriding in
     * subclasses.
     */
    virtual void createWidgets();

protected:
    /**
     * Creates the search line.  This can be useful to reimplement in cases where
     * a KTreeWidgetSearchLine subclass is used.
     *
     * It is const because it is be called from searchLine(), which to the user
     * doesn't conceptually alter the widget.
     */
    virtual KTreeWidgetSearchLine *createSearchLine(QTreeWidget *treeWidget) const;

private:
    class Private;
    Private *const d;
};

#endif
