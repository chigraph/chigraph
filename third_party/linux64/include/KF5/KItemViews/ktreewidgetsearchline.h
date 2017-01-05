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

#ifndef KTREEWIDGETSEARCHLINE_H
#define KTREEWIDGETSEARCHLINE_H

#include <QLineEdit>
#include <kitemviews_export.h>

class QModelIndex;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * This class makes it easy to add a search line for filtering the items in
 * listviews based on a simple text search.
 *
 * No changes to the application other than instantiating this class with
 * appropriate QTreeWidgets should be needed.
 */

class KITEMVIEWS_EXPORT KTreeWidgetSearchLine : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(Qt::CaseSensitivity caseSensitity READ caseSensitivity WRITE setCaseSensitivity)
    Q_PROPERTY(bool keepParentsVisible READ keepParentsVisible WRITE setKeepParentsVisible)
#ifndef KITEMVIEWS_NO_DEPRECATED
    Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage)
#endif

public:
    /**
     * Constructs a KTreeWidgetSearchLine with \a treeWidget being the QTreeWidget to
     * be filtered.
     *
     * If \a treeWidget is null then the widget will be disabled until listviews
     * are set with setTreeWidget(), setTreeWidgets() or added with addTreeWidget().
     */
    explicit KTreeWidgetSearchLine(QWidget *parent = 0, QTreeWidget *treeWidget = 0);

    /**
     * Constructs a KTreeWidgetSearchLine with \a treeWidgets being the list of
     * pointers to QTreeWidgets to be filtered.
     *
     * If \a treeWidgets is empty then the widget will be disabled until listviews
     * are set with setTreeWidget(), setTreeWidgets() or added with addTreeWidget().
     */
    KTreeWidgetSearchLine(QWidget *parent, const QList<QTreeWidget *> &treeWidgets);

    /**
     * Destroys the KTreeWidgetSearchLine.
     */
    virtual ~KTreeWidgetSearchLine();

    /**
     * Returns true if the search is case sensitive.  This defaults to false.
     *
     * @see setCaseSensitive()
     */
    Qt::CaseSensitivity caseSensitivity() const;

    /**
     * Returns the current list of columns that will be searched.  If the
     * returned list is empty all visible columns will be searched.
     *
     * @see setSearchColumns
     */
    QList<int> searchColumns() const;

    /**
     * If this is true (the default) then the parents of matched items will also
     * be shown.
     *
     * @see setKeepParentsVisible()
     */
    bool keepParentsVisible() const;

    /**
     * Returns the listview that is currently filtered by the search.
     * If there are multiple listviews filtered, it returns 0.
     *
     * @see setTreeWidget(), treeWidgets()
     */
    QTreeWidget *treeWidget() const;

    /**
     * Returns the list of pointers to listviews that are currently filtered by
     * the search.
     *
     * @see setTreeWidgets(), addTreeWidget(), treeWidget()
     */
    QList<QTreeWidget *> treeWidgets() const;

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

Q_SIGNALS:
    /**
     * This signal is emitted whenever an item gets hidden or unhidden due
     * to it not matching or matching the search string.
     */
    void hiddenChanged(QTreeWidgetItem *, bool);

    /**
     * This signal is emitted when user finished entering filter text or
     * when he made a pause long enough, after the QTreeWidget items got filtered
     * @param searchString is the text currently entered by the user
     * @since 5.0
     */
    void searchUpdated(const QString &searchString);

public Q_SLOTS:
    /**
     * Adds a QTreeWidget to the list of listviews filtered by this search line.
     * If \a treeWidget is null then the widget will be disabled.
     *
     * @see treeWidget(), setTreeWidgets(), removeTreeWidget()
     */
    void addTreeWidget(QTreeWidget *treeWidget);

    /**
     * Removes a QTreeWidget from the list of listviews filtered by this search
     * line. Does nothing if \a treeWidget is 0 or is not filtered by the quick search
     * line.
     *
     * @see listVew(), setTreeWidgets(), addTreeWidget()
     */
    void removeTreeWidget(QTreeWidget *treeWidget);

    /**
     * Updates search to only make visible the items that match \a pattern.  If
     * \a s is null then the line edit's text will be used.
     */
    virtual void updateSearch(const QString &pattern = QString());

    /**
     * Make the search case sensitive or case insensitive.
     *
     * @see caseSenstivity()
     */
    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);

    /**
     * When a search is active on a list that's organized into a tree view if
     * a parent or ancesestor of an item is does not match the search then it
     * will be hidden and as such so too will any children that match.
     *
     * If this is set to true (the default) then the parents of matching items
     * will be shown.
     *
     * \warning setKeepParentsVisible(true) does not have the expected effect
     * on items being added to or removed from the view while a search is active.
     * When a new search starts afterwards the behavior will be normal.
     *
     * @see keepParentsVisible
     */
    void setKeepParentsVisible(bool value);

    /**
     * Sets the list of columns to be searched.  The default is to search all,
     * visible columns which can be restored by passing \a columns as an empty
     * list.
     * If listviews to be filtered have different numbers or labels of columns
     * this method has no effect.
     *
     * @see searchColumns
     */
    void setSearchColumns(const QList<int> &columns);

    /**
     * Sets the QTreeWidget that is filtered by this search line, replacing any
     * previously filtered listviews.  If \a treeWidget is null then the widget will be
     * disabled.
     *
     * @see treeWidget(), setTreeWidgets()
     */
    void setTreeWidget(QTreeWidget *treeWidget);

    /**
     * Sets QTreeWidgets that are filtered by this search line, replacing any
     * previously filtered listviews.  If \a treeWidgets is empty then the widget will
     * be disabled.
     *
     * @see treeWidgets(), addTreeWidget(), setTreeWidget()
     */
    void setTreeWidgets(const QList<QTreeWidget *> &treeWidgets);

protected:
    /**
     * Returns true if \a item matches the search \a pattern.  This will be evaluated
     * based on the value of caseSensitive().  This can be overridden in
     * subclasses to implement more complicated matching schemes.
     */
    virtual bool itemMatches(const QTreeWidgetItem *item, const QString &pattern) const;

    /**
    * Re-implemented for internal reasons.  API not affected.
    */
    void contextMenuEvent(QContextMenuEvent *) Q_DECL_OVERRIDE;

    /**
     * Updates search to only make visible appropriate items in \a treeWidget.  If
     * \a treeWidget is null then nothing is done.
     */
    virtual void updateSearch(QTreeWidget *treeWidget);

    /**
     * Connects signals of this listview to the appropriate slots of the search
     * line.
     */
    virtual void connectTreeWidget(QTreeWidget *);

    /**
     * Disconnects signals of a listviews from the search line.
     */
    virtual void disconnectTreeWidget(QTreeWidget *);

    /**
     * Checks columns in all listviews and decides whether choosing columns to
     * filter on makes any sense.
     *
     * Returns false if either of the following is true:
     * * there are no listviews connected,
     * * the listviews have different numbers of columns,
     * * the listviews have only one column,
     * * the listviews differ in column labels.
     *
     * Otherwise it returns true.
     *
     * @see setSearchColumns()
     */
    virtual bool canChooseColumnsCheck();

    /**
     * Re-implemented for internal reasons.  API not affected.
     */
    bool event(QEvent *event) Q_DECL_OVERRIDE;
private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_rowsInserted(const QModelIndex &, int, int) const)
    Q_PRIVATE_SLOT(d, void _k_treeWidgetDeleted(QObject *))
    Q_PRIVATE_SLOT(d, void _k_slotColumnActivated(QAction *))
    Q_PRIVATE_SLOT(d, void _k_slotAllVisibleColumns())
    Q_PRIVATE_SLOT(d, void _k_queueSearch(const QString &))
    Q_PRIVATE_SLOT(d, void _k_activateSearch())
};

#endif
