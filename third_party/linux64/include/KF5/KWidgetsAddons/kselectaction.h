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
              (C) 2006 Albert Astals Cid <aacid@kde.org>
              (C) 2006 Clarence Dang <dang@kde.org>
              (C) 2006 Michel Hermier <michel.hermier@gmail.com>
              (C) 2007 Nick Shaforostoff <shafff@ukr.net>

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

#ifndef KSELECTACTION_H
#define KSELECTACTION_H

#include <QToolButton>
#include <QWidgetAction>

#include <kwidgetsaddons_export.h>

class KSelectActionPrivate;

/**
 *  @short Action for selecting one of several items
 *
 *  Action for selecting one of several items.
 *
 *  This action shows up a submenu with a list of items.
 *  One of them can be checked. If the user clicks on an item
 *  this item will automatically be checked,
 *  the formerly checked item becomes unchecked.
 *  There can be only one item checked at a time.
 */
class KWIDGETSADDONS_EXPORT KSelectAction : public QWidgetAction
{
    Q_OBJECT
    Q_PROPERTY(QAction *currentAction READ currentAction WRITE setCurrentAction)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    Q_PROPERTY(int comboWidth READ comboWidth WRITE setComboWidth)
    Q_PROPERTY(QString currentText READ currentText)
    Q_PROPERTY(ToolBarMode toolBarMode READ toolBarMode WRITE setToolBarMode)
    Q_PROPERTY(QToolButton::ToolButtonPopupMode toolButtonPopupMode READ toolButtonPopupMode WRITE setToolButtonPopupMode)
    Q_PROPERTY(int currentItem READ currentItem WRITE setCurrentItem)
    Q_PROPERTY(QStringList items READ items WRITE setItems)
    Q_DECLARE_PRIVATE(KSelectAction)

public:
    /**
     * Constructs a selection action with the specified parent.
     *
     * @param parent The action's parent object.
     */
    explicit KSelectAction(QObject *parent);

    /**
     * Constructs a selection action with text; a shortcut may be specified by
     * the ampersand character (e.g. \"&amp;Option\" creates a shortcut with key \e O )
     *
     * This is the most common KSelectAction used when you do not have a
     * corresponding icon (note that it won't appear in the current version
     * of the "Edit ToolBar" dialog, because an action needs an icon to be
     * plugged in a toolbar...).
     *
     * @param text The text that will be displayed.
     * @param parent The action's parent object.
     */
    KSelectAction(const QString &text, QObject *parent);

    /**
     * Constructs a selection action with text and an icon; a shortcut may be specified by
     * the ampersand character (e.g. \"&amp;Option\" creates a shortcut with key \e O )
     *
     * This is the other common KSelectAction used.  Use it when you
     * \e do have a corresponding icon.
     *
     * @param icon The icon to display.
     * @param text The text that will be displayed.
     * @param parent The action's parent object.
     */
    KSelectAction(const QIcon &icon, const QString &text, QObject *parent);

    /**
     * Destructor
     */
    virtual ~KSelectAction();

    enum ToolBarMode {
        /// Creates a button which pops up a menu when interacted with, as defined by toolButtonPopupMode().
        MenuMode,
        /// Creates a combo box which contains the actions.
        /// This is the default.
        ComboBoxMode
    };
    Q_ENUM(ToolBarMode)

    /**
     * Returns which type of widget (combo box or button with drop-down menu) will be inserted
     * in a toolbar.
     */
    ToolBarMode toolBarMode() const;

    /**
     * Set the type of widget to be inserted in a toolbar to \a mode.
     */
    void setToolBarMode(ToolBarMode mode);

    /**
     * Returns the style for the list of actions, when this action is plugged
     * into a KToolBar. The default value is QToolButton::InstantPopup
     *
     * \sa QToolButton::setPopupMode()
     */
    QToolButton::ToolButtonPopupMode toolButtonPopupMode() const;

    /**
     * Set how this list of actions should behave when in popup mode and plugged into a toolbar.
     */
    void setToolButtonPopupMode(QToolButton::ToolButtonPopupMode mode);

    /**
     * The action group used to create exclusivity between the actions associated with this action.
     */
    QActionGroup *selectableActionGroup() const;

    /**
     * Returns the current QAction.
     * @see setCurrentAction
     */
    QAction *currentAction() const;

    /**
     * Returns the index of the current item.
     *
     * @sa currentItem and currentAction
     */
    int currentItem() const;

    /**
     * Returns the text of the currently selected item.
     *
     * @sa currentItem and currentAction
     */
    QString currentText() const;

    /**
     * Returns the list of selectable actions
     */
    QList<QAction *> actions() const;

    /**
     * Returns the action at \a index, if one exists.
     */
    QAction *action(int index) const;

    /**
     * Searches for an action with the specified \a text, using a search whose
     * case sensitivity is defined by \a cs.
     */
    QAction *action(const QString &text, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    /**
     * Sets the currently checked item.
     *
     * @param action the QAction to become the currently checked item.
     *
     * \return \e true if a corresponding action was found and successfully checked.
     */
    bool setCurrentAction(QAction *action);

    /**
     * \overload setCurrentAction(QAction*)
     *
     * Convenience function to set the currently checked action to be the action
     * at index \p index.
     *
     * If there is no action at that index, the currently checked action (if any) will
     * be deselected.
     *
     * \return \e true if a corresponding action was found and thus set to the current action, otherwise \e false
     */
    bool setCurrentItem(int index);

    /**
     * \overload setCurrentAction(QAction*)
     *
     * Convenience function to set the currently checked action to be the action
     * which has \p text as its text().
     *
     * If there is no action at that index, the currently checked action (if any) will
     * be deselected.
     *
     * \return \e true if a corresponding action was found, otherwise \e false
     */
    bool setCurrentAction(const QString &text, Qt::CaseSensitivity cs = Qt::CaseSensitive);

    /**
     * Add \a action to the list of selectable actions.
     */
    void addAction(QAction *action);

    /**
     * \overload addAction(QAction* action)
     *
     * Convenience function which creates an action from \a text and inserts it into
     * the list of selectable actions.
     *
     * The newly created action is checkable and not user configurable.
     */
    QAction *addAction(const QString &text);

    /**
     * \overload addAction(QAction* action)
     *
     * Convenience function which creates an action from \a text and \a icon and inserts it into
     * the list of selectable actions.
     *
     * The newly created action is checkable and not user configurable.
     */
    QAction *addAction(const QIcon &icon, const QString &text);

    /**
     * Remove the specified \a action from this action selector.
     *
     * You take ownership here, so save or delete it in order to not leak the action.
     */
    virtual QAction *removeAction(QAction *action);

    /**
     * Inserts the action action to this widget's list of actions, before the action before.
     * It appends the action if before is 0 or before is not a valid action for this widget.
     *
     * @since 5.0
     */
    virtual void insertAction(QAction *before, QAction *action);

    /**
     * Convenience function to create the list of selectable items.
     * Any previously existing items will be cleared.
     */
    void setItems(const QStringList &lst);

    /**
     * Convenience function which returns the items that can be selected with this action.
     * It is the same as iterating selectableActionGroup()->actions() and looking at each
     * action's text().
     */
    QStringList items() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * @return true if the combo editable.
     */
    bool isEditable() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This makes the combo editable or read-only.
     */
    void setEditable(bool);

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This returns the maximum width set by setComboWidth
     */
    int comboWidth() const;

    /**
     * When this action is plugged into a toolbar, it creates a combobox.
     * This gives a _maximum_ size to the combobox.
     * The minimum size is automatically given by the contents (the items).
     */
    void setComboWidth(int width);

    /**
     * Sets the maximum items that are visible at once if the action
     * is a combobox, that is the number of items in the combobox's viewport
     */
    void setMaxComboViewCount(int n);

    /**
     * Remove and delete all the items in this action.
     *
     * @see removeAllActions()
     */
    void clear();

    /**
     * Remove all the items in this action.
     *
     * Unlike clear(), this will not delete the actions.
     *
     * @see clear()
     */
    void removeAllActions();

    /**
     * Sets whether any occurrence of the ampersand character ( &amp; ) in items
     * should be interpreted as keyboard accelerator for items displayed in a
     * menu or not.  Only applies to (overloaded) methods dealing with QStrings,
     * not those dealing with QActions.
     *
     * Defaults to true.
     *
     * \param b true if ampersands indicate a keyboard accelerator, otherwise false.
     */
    void setMenuAccelsEnabled(bool b);

    /**
     * Returns whether ampersands passed to methods using QStrings are interpreted
     * as keyboard accelerator indicators or as literal ampersands.
     */
    bool menuAccelsEnabled() const;

    /**
     * Changes the text of item @param index to @param text .
     */
    void changeItem(int index, const QString &text);

Q_SIGNALS:
    /**
     * This signal is emitted when an item is selected; @param action
     * indicates the item selected.
     */
    void triggered(QAction *action);

    /**
     * This signal is emitted when an item is selected; @param index indicates
     * the item selected.
     */
    void triggered(int index);

    /**
     * This signal is emitted when an item is selected; @param text indicates
     * the item selected.
     */
    void triggered(const QString &text);

protected Q_SLOTS:
    /**
     * This function is called whenever an action from the selections is triggered.
     */
    virtual void actionTriggered(QAction *action);

    /**
     * For structured menu building. Deselects all items if the action was unchecked by the top menu
     */
    void slotToggled(bool);

protected:
    /**
     * Reimplemented from @see QWidgetAction.
     */
    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;

    /**
     * Reimplemented from @see QWidgetAction.
     */
    void deleteWidget(QWidget *widget) Q_DECL_OVERRIDE;

    bool event(QEvent *event) Q_DECL_OVERRIDE;

    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

    /**
     * @internal
     * Creates a new KSelectAction object.
     *
     * @param dd the private d member
     * @param parent The action's parent object.
     */
    KSelectAction(KSelectActionPrivate &dd, QObject *parent);

    KSelectActionPrivate *d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void _k_comboBoxDeleted(QObject *))
    Q_PRIVATE_SLOT(d_func(), void _k_comboBoxCurrentIndexChanged(int))
};

#endif
