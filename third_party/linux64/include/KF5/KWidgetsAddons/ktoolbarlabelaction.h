/* This file is part of the KDE libraries
    Copyright (C) 2004 Felix Berger <felixberger@beldesign.de>

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

#ifndef KTOOLBARLABELACTION_H
#define KTOOLBARLABELACTION_H

#include <QWidgetAction>

#include <kwidgetsaddons_export.h>

/**
 * @short Class to display a label in a toolbar.
 *
 * KToolBarLabelAction is a convenience class for displaying a label in a
 * toolbar.
 *
 * It provides easy access to the label's #setBuddy(QAction*) and #buddy()
 * methods and can be used as follows:
 *
 * \code
 *
 * KHistoryComboBox *findCombo = new KHistoryComboBox(true, this);
 *
 * KWidgetAction *action = new KWidgetAction(findCombo, i18n("Find Combo"),
 *                                            Qt::Key_F6, this, SLOT( slotFocus()),
 *                                            actionCollection(), "find_combo");
 *
 * QAction *action = new KToolBarLabelAction(action, i18n("Find "), "find_label");
 * action->setShortcut(Qt::Key_F6);
 * connect(action, SIGNAL(triggered()), this, SLOT(slotFocus()));
 *
 * \endcode
 *
 * @author Felix Berger <felixberger@beldesign.de>
 */
class KWIDGETSADDONS_EXPORT KToolBarLabelAction : public QWidgetAction
{
    Q_OBJECT

public:
    /**
     * Creates a toolbar label.
     *
     * @param text The label's and the action's text.
     * @param parent This action's parent.
     */
    KToolBarLabelAction(const QString &text, QObject *parent);

    /**
     * Creates a toolbar label setting a buddy for the label.
     *
     * @param buddy The action whose widget which is focused when the label's accelerator is
     * typed.
     * @param text The label's and the action's text.
     * @param parent This action's parent.
     */
    KToolBarLabelAction(QAction *buddy, const QString &text, QObject *parent);

    /**
     * Destroys the toolbar label.
     */
    virtual ~KToolBarLabelAction();

    /**
     * Sets the label's buddy to buddy.
     *
     * See QLabel#setBuddy() for details.
     */
    void setBuddy(QAction *buddy);

    /**
     * Returns the label's buddy or 0 if no buddy is currently set.
     *
     * See QLabel#buddy() and QLabel#setBuddy() for more information.
     */
    QAction *buddy() const;

    /**
     * Reimplemented from @see QActionWidgetFactory.
     */
    QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE;

Q_SIGNALS:
    /**
     * This signal is emmitted whenever the text of this action
     * is changed.
     */
    void textChanged(const QString &newText);

protected:
    bool event(QEvent *) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;
};

#endif
