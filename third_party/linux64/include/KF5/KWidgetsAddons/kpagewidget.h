/*
    This file is part of the KDE Libraries

    Copyright (C) 2006 Tobias Koenig (tokoe@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KPAGEWIDGET_H
#define KPAGEWIDGET_H

#include <kpagewidgetmodel.h>

#include "kpageview.h"

class KPageWidgetPrivate;
/**
 * @short Page widget with many layouts (faces).
 * @see KPageView with hierarchical page model.
 *
 * @author Tobias Koenig (tokoe@kde.org)
 */
class KWIDGETSADDONS_EXPORT KPageWidget : public KPageView
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(KPageWidget)

public:
    /**
     * Creates a new page widget.
     *
     * @param parent The parent widget.
     */
    explicit KPageWidget(QWidget *parent = 0);

    /**
     * Destroys the page widget.
     */
    ~KPageWidget();

    /**
     * Adds a new top level page to the widget.
     *
     * @param widget The widget of the page.
     * @param name The name which is displayed in the navigation view.
     *
     * @returns The associated @see KPageWidgetItem.
     */
    KPageWidgetItem *addPage(QWidget *widget, const QString &name);

    /**
     * Adds a new top level page to the widget.
     *
     * @param item The @see KPageWidgetItem which describes the page.
     */
    void addPage(KPageWidgetItem *item);

    /**
     * Inserts a new page in the widget.
     *
     * @param before The new page will be insert before this @see KPageWidgetItem
     *               on the same level in hierarchy.
     * @param widget The widget of the page.
     * @param name The name which is displayed in the navigation view.
     *
     * @returns The associated @see KPageWidgetItem.
     */
    KPageWidgetItem *insertPage(KPageWidgetItem *before, QWidget *widget, const QString &name);

    /**
     * Inserts a new page in the widget.
     *
     * @param before The new page will be insert before this @see KPageWidgetItem
     *               on the same level in hierarchy.
     *
     * @param item The @see KPageWidgetItem which describes the page.
     */
    void insertPage(KPageWidgetItem *before, KPageWidgetItem *item);

    /**
     * Inserts a new sub page in the widget.
     *
     * @param parent The new page will be insert as child of this @see KPageWidgetItem.
     * @param widget The widget of the page.
     * @param name The name which is displayed in the navigation view.
     *
     * @returns The associated @see KPageWidgetItem.
     */
    KPageWidgetItem *addSubPage(KPageWidgetItem *parent, QWidget *widget, const QString &name);

    /**
     * Inserts a new sub page in the widget.
     *
     * @param parent The new page will be insert as child of this @see KPageWidgetItem.
     *
     * @param item The @see KPageWidgetItem which describes the page.
     */
    void addSubPage(KPageWidgetItem *parent, KPageWidgetItem *item);

    /**
     * Removes the page associated with the given @see KPageWidgetItem.
     */
    void removePage(KPageWidgetItem *item);

    /**
     * Sets the page which is associated with the given @see KPageWidgetItem to
     * be the current page and emits the currentPageChanged() signal.
     */
    void setCurrentPage(KPageWidgetItem *item);

    /**
     * Returns the @see KPageWidgetItem for the current page or 0 if there is no
     * current page.
     */
    KPageWidgetItem *currentPage() const;

Q_SIGNALS:
    /**
     * This signal is emitted whenever the current page has changed.
     *
     * @param item The new current page or 0 if no current page is available.
     */
    void currentPageChanged(KPageWidgetItem *current, KPageWidgetItem *before);

    /**
     * This signal is emitted whenever a checkable page changes its state. @param checked is true
     * when the @param page is checked, or false if the @param page is unchecked.
     */
    void pageToggled(KPageWidgetItem *page, bool checked);

    /**
     * This signal is emitted when a page is removed.
     * @param page The page which is removed
     * */
    void pageRemoved(KPageWidgetItem *page);

protected:
    KPageWidget(KPageWidgetPrivate &dd, QWidget *parent);

private:
    Q_PRIVATE_SLOT(d_func(), void _k_slotCurrentPageChanged(const QModelIndex &, const QModelIndex &))
};

#endif
