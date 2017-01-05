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

#ifndef KTOGGLEACTION_H
#define KTOGGLEACTION_H

#include <QAction>

#include <kwidgetsaddons_export.h>

class KGuiItem;

/**
 *  @short Checkbox like action.
 *
 *  This action provides two states: checked or not.
 *
 */
class KWIDGETSADDONS_EXPORT KToggleAction : public QAction
{
    Q_OBJECT

public:
    /**
     * Constructs an action with the specified parent.
     *
     * @param parent The action's parent object.
     */
    explicit KToggleAction(QObject *parent);

    /**
     * Constructs an action with text; a shortcut may be specified by
     * the ampersand character (e.g. \"&amp;Option\" creates a shortcut with key \e O )
     *
     * This is the most common KAction used when you do not have a
     * corresponding icon (note that it won't appear in the current version
     * of the "Edit ToolBar" dialog, because an action needs an icon to be
     * plugged in a toolbar...).
     *
     * @param text The text that will be displayed.
     * @param parent The action's parent object.
     */
    KToggleAction(const QString &text, QObject *parent);

    /**
     * Constructs an action with text and an icon; a shortcut may be specified by
     * the ampersand character (e.g. \"&amp;Option\" creates a shortcut with key \e O )
     *
     * This is the other common KAction used.  Use it when you
     * \e do have a corresponding icon.
     *
     * @param icon The icon to display.
     * @param text The text that will be displayed.
     * @param parent The action's parent object.
     */
    KToggleAction(const QIcon &icon, const QString &text, QObject *parent);

    /**
     * Destructor
     */
    virtual ~KToggleAction();

    /**
     * Defines the text (and icon, tooltip, whatsthis) that should be displayed
     * instead of the normal text, when the action is checked.
     * Note that this does <em>not</em> replace the check box in front of the
     * menu.  So you should not use it to replace the text "Show <foo>" with
     * "Hide <foo>", for example.
     *
     * If hasIcon(), the icon is kept for the 'checked state', unless
     * @p checkedItem defines an icon explicitly. Same thing for tooltip and whatsthis.
     */
    void setCheckedState(const KGuiItem &checkedItem);

protected Q_SLOTS:
    virtual void slotToggled(bool checked);

private:
    class Private;
    Private *const d;
};

#endif
