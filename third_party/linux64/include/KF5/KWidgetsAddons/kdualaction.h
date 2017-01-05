/* This file is part of the KDE libraries
 *
 * Copyright (c) 2010 Aurélien Gâteau <agateau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef KDUALACTION_H
#define KDUALACTION_H

#include <QAction>

#include <kwidgetsaddons_export.h>

class KDualActionPrivate;
class KGuiItem;

/**
 * @short An action which can alternate between two texts/icons when triggered.
 *
 * KDualAction should be used when you want to create an action which alternate
 * between two states when triggered but which should not be rendered as a
 * checkable widget because it is more appropriate to change the text and icon
 * of the action instead.
 *
 * You should use KDualAction to implement this kind of actions instead of
 * KToggleAction because KToggleAction is rendered as a checkable widget: this
 * means one of your state will have a checkbox in a menu and will be
 * represented as a sunken button in a toolbar.
 *
 * Porting from KToggleAction to KDualAction:
 *
 * 1. If you used the KToggleAction constructor which accepts the action text,
 * adjust the constructor: KDualAction constructor accepts both inactive and
 * active text.
 *
 * 2. Replace connections to the checked(bool) signal with a connection to the
 * activeChanged(bool) (or activeChangedByUser(bool))
 *
 * 3. Replace calls to setChecked()/isChecked() with setActive()/isActive()
 *
 * 4. Replace calls to setCheckedState(guiItem) with
 * setActiveGuiItem(guiItem)
 *
 * @author Aurélien Gâteau <agateau@kde.org>
 * @since 4.6
 */
class KWIDGETSADDONS_EXPORT KDualAction : public QAction
{
    Q_OBJECT
public:
    /**
     * Constructs a KDualAction with the specified parent. Texts must be set
     * with setTextForState() or setGuiItemForState().
     */
    explicit KDualAction(QObject *parent);

    /**
     * Constructs a KDualAction with the specified parent and texts.
     */
    KDualAction(const QString &inactiveText, const QString &activeText, QObject *parent);

    ~KDualAction();

    /**
     * Sets the KGuiItem for the active state
     */
    void setActiveGuiItem(const KGuiItem &);

    /**
     * Gets the KGuiItem for the active state
     */
    KGuiItem activeGuiItem() const;

    /**
     * Sets the KGuiItem for the inactive state
     */
    void setInactiveGuiItem(const KGuiItem &);

    /**
     * Gets the KGuiItem for the inactive state
     */
    KGuiItem inactiveGuiItem() const;

    /**
     * Sets the icon for the active state
     */
    void setActiveIcon(const QIcon &);

    /**
     * Gets the icon for the active state
     */
    QIcon activeIcon() const;

    /**
     * Sets the icon for the inactive state
     */
    void setInactiveIcon(const QIcon &);

    /**
     * Gets the icon for the inactive state
     */
    QIcon inactiveIcon() const;

    /**
     * Sets the text for the active state
     */
    void setActiveText(const QString &);

    /**
     * Gets the text for the active state
     */
    QString activeText() const;

    /**
     * Sets the text for the inactive state
     */
    void setInactiveText(const QString &);

    /**
     * Gets the text for the inactive state
     */
    QString inactiveText() const;

    /**
     * Sets the tooltip for the active state
     */
    void setActiveToolTip(const QString &);

    /**
     * Gets the tooltip for the active state
     */
    QString activeToolTip() const;

    /**
     * Sets the tooltip for the inactive state
     */
    void setInactiveToolTip(const QString &);

    /**
     * Gets the tooltip for the inactive state
     */
    QString inactiveToolTip() const;

    /**
     * Convenience method to set the icon for both active and inactive states.
     */
    void setIconForStates(const QIcon &icon);

    /**
     * Returns the action state.
     * The action is inactive by default.
     */
    bool isActive() const;

    /**
     * Defines whether the current action should automatically be changed when
     * the user triggers this action.
     */
    void setAutoToggle(bool);

    /**
     * Returns whether the current action will automatically be changed when
     * the user triggers this action. The default value is true.
     */
    bool autoToggle() const;

public Q_SLOTS:
    /**
     * Sets the action state.
     * activeChanged() will be emitted but not activeChangedByUser().
     */
    void setActive(bool state);

Q_SIGNALS:
    /**
     * Emitted when the state changes. This signal is emitted when the user
     * trigger the action and when setActive() is called.
     */
    void activeChanged(bool);

    /**
     * Only emitted when the state changes because the user triggered the
     * action.
     */
    void activeChangedByUser(bool);

private:
    Q_PRIVATE_SLOT(d, void slotTriggered())
    KDualActionPrivate *const d;
    friend class KDualActionPrivate;
};

#endif /* KDUALACTION_H */
