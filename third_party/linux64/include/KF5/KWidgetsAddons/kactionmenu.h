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

#ifndef KACTIONMENU_H
#define KACTIONMENU_H

#include <QWidgetAction>

#include <kwidgetsaddons_export.h>

class QMenu;

/**
 * A KActionMenu is an action that has several properties specific to holding a
 * sub-menu of other actions.
 *
 * Any QAction can be used to create a submenu.
 *
 * Plugged in a popupmenu, it will create a submenu.
 * Plugged in a toolbar, it will create a button with a popup menu.
 *
 * This is the action used by the XMLGUI since it holds other actions.
 * If you want a submenu for selecting one tool among many (without icons), see KSelectAction.
 * See also setDelayed about the main action.
 */
class KWIDGETSADDONS_EXPORT KActionMenu : public QWidgetAction
{
    Q_OBJECT
    Q_PROPERTY(bool delayed READ delayed WRITE setDelayed)
    Q_PROPERTY(bool stickyMenu READ stickyMenu WRITE setStickyMenu)

public:
    explicit KActionMenu(QObject *parent);
    KActionMenu(const QString &text, QObject *parent);
    KActionMenu(const QIcon &icon, const QString &text, QObject *parent);
    virtual ~KActionMenu();

    /**
     * @deprecated
     */
#ifndef KWIDGETSADDONS_NO_DEPRECATED
    KWIDGETSADDONS_DEPRECATED void remove(QAction *);
#endif

    void addAction(QAction *action);
    QAction *addSeparator();
    void insertAction(QAction *before, QAction *action);
    QAction *insertSeparator(QAction *before);
    void removeAction(QAction *action);

    /**
     * Returns this action's menu as a KMenu, if it is one.
     * If none exists, one will be created.
     * @deprecated use menu() instead.
     */
#ifndef KWIDGETSADDONS_NO_DEPRECATED
    inline KWIDGETSADDONS_DEPRECATED QMenu *popupMenu()
    {
        return menu();
    }
#endif

    /**
     * Returns true if this action creates a delayed popup menu
     * when plugged in a KToolBar.
     */
    bool delayed() const;

    /**
     * If set to true, this action will create a delayed popup menu
     * when plugged in a KToolBar. Otherwise it creates a normal popup.
     * Default: delayed
     *
     * Remember that if the "main" action (the toolbar button itself)
     * cannot be clicked, then you should call setDelayed(false).
     *
     * In the other case, if the main action can be clicked, it can only happen
     * in a toolbar: in a menu, the parent of a submenu can't be activated.
     * To get a "normal" menu item when plugged a menu (and no submenu)
     * use KToolBarPopupAction.
     */
    void setDelayed(bool delayed);

    /**
     * Returns true if this action creates a sticky popup menu.
     * @see setStickyMenu().
     */
    bool stickyMenu() const;

    /**
     * If set to true, this action will create a sticky popup menu
     * when plugged in a KToolBar.
     * "Sticky", means it's visible until a selection is made or the mouse is
     * clicked elsewhere. This feature allows you to make a selection without
     * having to press and hold down the mouse while making a selection.
     * Default: sticky.
     */
    void setStickyMenu(bool sticky);

    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;

private:
    class KActionMenuPrivate *const d;
};

#endif
