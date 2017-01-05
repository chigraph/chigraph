/**
  * This file is part of the KDE project
  * Copyright (C) 2007-2008 Rafael Fernández López <ereslibre@kde.org>
  * Copyright (C) 2008 Kevin Ottens <ervin@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#ifndef KWIDGETITEMDELEGATE_H
#define KWIDGETITEMDELEGATE_H

#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QPersistentModelIndex>
#include <QAbstractItemDelegate>

#include <kitemviews_export.h>

class QObject;
class QPainter;
class QStyleOption;
class QStyleOptionViewItem;
class QAbstractItemView;
class QItemSelection;

class KWidgetItemDelegatePrivate;
class KWidgetItemDelegatePool;

/**
 * This class allows to create item delegates embedding simple widgets to interact
 * with items. For instance you can add push buttons, line edits, etc. to your delegate
 * and use them to modify the state of your model.
 *
 * @since 4.1
 */
class KITEMVIEWS_EXPORT KWidgetItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    /**
     * Creates a new ItemDelegate to be used with a given itemview.
     *
     * @param itemView the item view the new delegate will monitor
     * @param parent the parent of this delegate
     */
    explicit KWidgetItemDelegate(QAbstractItemView *itemView, QObject *parent = 0);

    /**
     * Destroys an ItemDelegate.
     */
    virtual ~KWidgetItemDelegate();

    /**
     * Retrieves the item view this delegate is monitoring.
     *
     * @return the item view this delegate is monitoring
     */
    QAbstractItemView *itemView() const;

    /**
     * Retrieves the currently focused index. An invalid index if none is focused.
     *
     * @return the current focused index, or QPersistentModelIndex() if none is focused.
     */
    QPersistentModelIndex focusedIndex() const;

protected:
    /**
     * Creates the list of widgets needed for an item.
     *
     * @note No initialization of the widgets is supposed to happen here.
     *       The widgets will be initialized based on needs for a given item.
     *
     * @note If you want to connect some widget signals to any slot, you should
     *       do it here.
     *
     * @arg index the index to create widgets for.
     *
     * @return the list of newly created widgets which will be used to interact with an item.
     * @see updateItemWidgets()
     */
    virtual QList<QWidget *> createItemWidgets(const QModelIndex &index) const = 0;

    /**
     * Updates a list of widgets for its use inside of the delegate (painting or
     * event handling).
     *
     * @note All the positioning and sizing should be done in item coordinates.
     *
     * @warning Do not make widget connections in here, since this method will
     * be called very regularly.
     *
     * @param widgets the widgets to update
     * @param option the current set of style options for the view.
     * @param index the model index of the item currently manipulated.
     */
    virtual void updateItemWidgets(const QList<QWidget *> widgets,
                                   const QStyleOptionViewItem &option,
                                   const QPersistentModelIndex &index) const = 0;

    /**
     * Paint the widgets of the item. This method is meant to be used in the paint()
     * method of your item delegate implementation.
     *
     * @param painter the painter the widgets will be painted on.
     * @param option the current set of style options for the view.
     * @param index the model index of the item currently painted.
     *
     * @warning since 4.2 this method is not longer needed to be called. All widgets will kept
     *          updated without the need of calling paintWidgets() in your paint() event. For the
     *          widgets of a certain index to be updated your model has to emit dataChanged() on the
     *          indexes that want to be updated.
     */
#ifndef KITEMVIEWS_NO_DEPRECATED
    KITEMVIEWS_DEPRECATED void paintWidgets(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QPersistentModelIndex &index) const;
#endif

    /**
     * Sets the list of event @p types that a @p widget will block.
     *
     * Blocked events are not passed to the view. This way you can prevent an item
     * from being selected when a button is clicked for instance.
     *
     * @param widget the widget which must block events
     * @param types the list of event types the widget must block
     */
    void setBlockedEventTypes(QWidget *widget, QList<QEvent::Type> types) const;

    /**
     * Retrieves the list of blocked event types for the given widget.
     *
     * @param widget the specified widget.
     *
     * @return the list of blocked event types, can be empty if no events are blocked.
     */
    QList<QEvent::Type> blockedEventTypes(QWidget *widget) const;

private:
    //@cond PRIVATE
    friend class KWidgetItemDelegatePool;
    friend class KWidgetItemDelegateEventListener;
    KWidgetItemDelegatePrivate *const d;
    Q_PRIVATE_SLOT(d, void _k_slotRowsInserted(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d, void _k_slotRowsAboutToBeRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d, void _k_slotRowsRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d, void _k_slotDataChanged(const QModelIndex &, const QModelIndex &))
    Q_PRIVATE_SLOT(d, void _k_slotLayoutChanged())
    Q_PRIVATE_SLOT(d, void _k_slotModelReset())
    Q_PRIVATE_SLOT(d, void _k_slotSelectionChanged(const QItemSelection &, const QItemSelection &))
    //@endcond
};

#endif
