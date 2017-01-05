/*
  This file is part of the KDE libraries
  Copyright (c) 2003 Scott Wheeler <wheeler@kde.org>
  Copyright (c) 2004 Gustavo Sverzut Barbieri <gsbarbieri@users.sourceforge.net>

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

#ifndef KLISTWIDGETSEARCHLINE_H
#define KLISTWIDGETSEARCHLINE_H

#include <QLineEdit>

#include <kitemviews_export.h>

class QListWidget;
class QListWidgetItem;
class QModelIndex;

/**
 * This class makes it easy to add a search line for filtering the items in a
 * listwidget based on a simple text search.
 *
 * No changes to the application other than instantiating this class with an
 * appropriate QListWidget should be needed.
 */
class KITEMVIEWS_EXPORT KListWidgetSearchLine : public QLineEdit
{
    Q_OBJECT
#ifndef KITEMVIEWS_NO_DEPRECATED
    Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage)
#endif

public:

    /**
     * Constructs a KListWidgetSearchLine with \a listWidget being the QListWidget to
     * be filtered.
     *
     * If \a listWidget is null then the widget will be disabled until a listWidget
     * is set with setListWidget().
     */
    explicit KListWidgetSearchLine(QWidget *parent = 0,
                                   QListWidget *listWidget = 0);

    /**
     * Destroys the KListWidgetSearchLine.
     */
    virtual ~KListWidgetSearchLine();

    /**
     * Returns if the search is case sensitive.  This defaults to Qt::CaseInsensitive.
     *
     * @see setCaseSensitive()
     */
    Qt::CaseSensitivity caseSensitive() const;

    /**
     * Returns the listWidget that is currently filtered by the search.
     *
     * @see setListWidget()
     */
    QListWidget *listWidget() const;

    /**
     * @return the message set with setClickMessage
     * @deprecated since 5.0, use QLineEdit::placeholderText() instead.
     **/
#ifndef KITEMVIEWS_NO_DEPRECATED
    KITEMVIEWS_DEPRECATED QString clickMessage() const { return placeholderText(); }
#endif

    /**
     * This makes the line edit display a grayed-out hinting text as long as
     * the user didn't enter any text. It is often used as indication about
     * the purpose of the line edit.
     * @deprecated since 5.0, use QLineEdit::setPlaceholderText() instead.
     */
#ifndef KITEMVIEWS_NO_DEPRECATED
    KITEMVIEWS_DEPRECATED void setClickMessage(const QString &msg) { setPlaceholderText(msg); }
#endif

public Q_SLOTS:
    /**
     * Updates search to only make visible the items that match \a s.  If
     * \a s is null then the line edit's text will be used.
     */
    virtual void updateSearch(const QString &s = QString());

    /**
     * Make the search case sensitive or case insensitive.
     *
     * @see caseSenstive()
     */
    void setCaseSensitivity(Qt::CaseSensitivity cs);

    /**
     * Sets the QListWidget that is filtered by this search line.  If \a lv is null
     * then the widget will be disabled.
     *
     * @see listWidget()
     */
    void setListWidget(QListWidget *lv);

    /**
     * Clear line edit and empty hiddenItems, returning elements to listWidget.
     */
    void clear();

protected:
    /**
     * Returns true if \a item matches the search \a s.  This will be evaluated
     * based on the value of caseSensitive().  This can be overridden in
     * subclasses to implement more complicated matching schemes.
     */
    virtual bool itemMatches(const QListWidgetItem *item,
                             const QString &s) const;
    /**
     * Re-implemented for internal reasons.  API not affected.
     */
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    class KListWidgetSearchLinePrivate;
    KListWidgetSearchLinePrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_listWidgetDeleted())
    Q_PRIVATE_SLOT(d, void _k_queueSearch(const QString &))
    Q_PRIVATE_SLOT(d, void _k_activateSearch())
    Q_PRIVATE_SLOT(d, void _k_rowsInserted(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d, void _k_dataChanged(const QModelIndex &, const QModelIndex &))
};

#endif /* KLISTWIDGETSEARCHLINE_H */
