/* This file is part of the KDE libraries
    Copyright (C) 2006,2007 Andreas Hartmetz (ahartmetz@gmail.com)
    Copyright (C) 2008 Urs Wolfer (uwolfer @ kde.org)

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

#ifndef KEXTENDABLEITEMDELEGATE_H
#define KEXTENDABLEITEMDELEGATE_H

#include <QStyledItemDelegate>

#include <kitemviews_export.h>

/**
  * This delegate makes it possible to display an arbitrary QWidget ("extender") that spans all columns below a line of items.
  * The extender will logically belong to a column in the row above it.
  *
  * It is your responsibility to devise a way to trigger extension and contraction of items, by calling
  * extendItem() and contractItem(). You can e.g. reimplement itemActivated() and similar functions.
  *
  * @warning extendItem() reparents the provided widget @a extender to the
  * viewport of the itemview it belongs to. The @a extender is destroyed when
  * you call contractItem() for the associated index. If you fail to do that
  * and the associated item gets deleted you're in trouble. It remains as a
  * visible artefact in your treeview. Additionally when closing your
  * application you get an assertion failure from KExtendableItemDelegate. Make
  * sure that you always call contractItem for indices before you delete them.
  *
  * @author Andreas Hartmetz <ahartmetz@gmail.com>
  *
  * @since 4.1
  */

class QAbstractItemView;

class KITEMVIEWS_EXPORT KExtendableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    enum auxDataRoles {ShowExtensionIndicatorRole = Qt::UserRole + 200};

    /**
     * Create a new KExtendableItemDelegate that belongs to @p parent. In contrast to generic
     * QAbstractItemDelegates, an instance of this class can only ever be the delegate for one
     * instance of af QAbstractItemView subclass.
     */
    KExtendableItemDelegate(QAbstractItemView *parent);
    virtual ~KExtendableItemDelegate();

    /**
     * Re-implemented for internal reasons. API not affected.
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    /**
     * Re-implemented for internal reasons. API not affected.
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    /**
     * Insert the @p extender for item at @p index into the view.
     * If you need a parent for the extender at construction time, use the itemview's viewport().
     * The delegate takes ownership of the extender; the extender will also be reparented and
     * resized to the viewport.
     */
    void extendItem(QWidget *extender, const QModelIndex &index);

    /**
     * Remove the extender of item at @p index from the view. The extender widget
     * will be deleted.
     */
    void contractItem(const QModelIndex &index);

    /**
     * Close all extenders and delete all extender widgets.
     */
    void contractAll();

    /**
     * Return whether there is an extender that belongs to @p index.
     */
    bool isExtended(const QModelIndex &index) const;

    /**
     * Reimplement this function to adjust the internal geometry of the extender.
     * The external geometry of the extender will be set by the delegate.
     */
    virtual void updateExtenderGeometry(QWidget *extender, const QStyleOptionViewItem &option, const QModelIndex &index) const;

Q_SIGNALS:
    /**
     * This signal indicates that the item at @p index was extended with @p extender.
     */
    void extenderCreated(QWidget *extender, const QModelIndex &index);

    /**
     * This signal indicates that the @p extender belonging to @p index has emitted the destroyed() signal.
     */
    void extenderDestroyed(QWidget *extender, const QModelIndex &index);

protected:
    /**
     * Reimplement this function to fine-tune the position of the extender. @p option.rect will be a rectangle
     * that is as wide as the viewport and as high as the usual item height plus the extender size hint's height.
     * Its upper left corner will be at the upper left corner of the usual item.
     * You can place the returned rectangle of this function anywhere inside that area.
     */
    QRect extenderRect(QWidget *extender, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /**
     * The pixmap that is displayed to extend an item. @p pixmap must have the same size as the pixmap in setContractPixmap.
     */
    void setExtendPixmap(const QPixmap &pixmap);

    /**
     * The pixmap that is displayed to contract an item. @p pixmap must have the same size as the pixmap in setExtendPixmap.
     */
    void setContractPixmap(const QPixmap &pixmap);

    /**
     * Return the pixmap that is displayed to extend an item.
     */
    QPixmap extendPixmap();

    /**
     * Return the pixmap that is displayed to contract an item.
     */
    QPixmap contractPixmap();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_extenderDestructionHandler(QObject *destroyed))
    Q_PRIVATE_SLOT(d, void _k_verticalScroll())
};
#endif // KEXTENDABLEITEMDELEGATE_H
