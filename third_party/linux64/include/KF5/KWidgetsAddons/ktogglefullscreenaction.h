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

#ifndef KTOGGLEFULLSCREENACTION_H
#define KTOGGLEFULLSCREENACTION_H

#include <ktoggleaction.h>

/**
 * An action for switching between to/from full screen mode. Note that
 * QWidget::isFullScreen() may reflect the new or the old state
 * depending on how the action was triggered (by the application or
 * from the window manager). Also don't try to track the window state
 * yourself. Rely on this action's state (isChecked()) instead.
 *
 * Important: If you need to set/change the fullscreen state manually,
 * use KToggleFullScreenAction::setFullScreen() or a similar function,
 * do not call directly the slot connected to the toggled() signal. The slot
 * still needs to explicitly set the window state though.
 *
 * Note: Do NOT use QWidget::showFullScreen() or QWidget::showNormal().
 * They have several side-effects besides just switching the fullscreen
 * state (for example, showNormal() resets all window states, not just
 * fullscreen). Use the KToggleFullScreenAction::setFullScreen() helper function.
 */
class KWIDGETSADDONS_EXPORT KToggleFullScreenAction : public KToggleAction
{
    Q_OBJECT

public:
    /**
     * Create a KToggleFullScreenAction. Call setWindow() to associate this
     * action with a window.
     *
     *  @param parent This action's parent object.
     */
    explicit KToggleFullScreenAction(QObject *parent);

    /**
     * Create a KToggleFullScreenAction
     *  @param window the window that will switch to/from full screen mode
     *  @param parent This action's parent object.
     */
    KToggleFullScreenAction(QWidget *window, QObject *parent);

    /**
     * Destroys the toggle fullscreen action.
     */
    virtual ~KToggleFullScreenAction();

    /**
     * Sets the window that will be related to this action.
     */
    void setWindow(QWidget *window);

    /**
     * Helper function to set or reset the fullscreen state of a window.
     * Use this function rather than showFullScreen()/showNormal() QWidget functions.
     * @since 4.0.3
     */
    static void setFullScreen(QWidget *window, bool set);

protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

protected Q_SLOTS:
    void slotToggled(bool checked) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;
};

#endif
