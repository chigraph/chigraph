/*
 * This file is part of the KDE project
 * Copyright (C) 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KTOOLTIPWIDGET_H
#define KTOOLTIPWIDGET_H

#include <kwidgetsaddons_export.h>

#include <QWidget>

/**
 * @brief A tooltip that contains a QWidget.
 *
 * This widget allows to show a tooltip that contains another widget.
 * If you only need to show text inside the tooltip, just use QWidget::setToolTip();
 *
 * To show the tooltip, you need to choose a position on the screen and also pass a
 * transient parent window (recommended on X11 and required on Wayland).
 * Use showAt() if you want to show the tooltip from a specific point.
 * Use showUnder() if you want to show it under a given rectangle.
 *
 * You can use a single instance of this class to show as many tooltips as you want.
 *
 * @since 5.30
 * @author Elvis Angelaccio <elvis.angelaccio@kde.org>
 */
class KWIDGETSADDONS_EXPORT KToolTipWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int hideDelay READ hideDelay WRITE setHideDelay)

public:
    explicit KToolTipWidget(QWidget *parent = Q_NULLPTR);
    virtual ~KToolTipWidget();

    /**
     * Show a tooltip containing @p content. The pos() of the tooltip will be @p pos.
     * You can call this method multiple times over the same KToolTipWidget instance
     * (previously shown widgets will be removed from the tooltip's layout).
     *
     * @param transientParent will be set as the transient parent of the tooltip.
     * @note The transient parent is required to show the tooltip on Wayland platforms.
     */
    void showAt(const QPoint &pos, QWidget *content, QWindow *transientParent);

    /**
     * Show a tooltip containing @p content centered below @p rect. If there is not
     * enough space in the screen below @p rect, the tooltip will be shown above
     * @p rect, if possible, or at the bottom of the screen otherwise.
     * You can call this method multiple times over the same KToolTipWidget instance
     * (previously shown widgets will be removed from the tooltip's layout).
     *
     * Typically @p rect is the visualRect() of a QAbstractItemView:
     * @snippet ktooltipwidget_test.cpp show_tooltip_widget
     *
     * @param transientParent will be set as the transient parent of the tooltip.
     * @note The transient parent is required to show the tooltip on Wayland platforms.
     */
    void showBelow(const QRect &rect, QWidget *content, QWindow *transientParent);

    /**
     * @return Delay (in ms) after which hideLater() will hide the tooltip. Default is 500.
     * @see hideLater(), setHideDelay()
     */
    int hideDelay() const;

public Q_SLOTS:

    /**
     * Hide the tooltip after a delay of hideDelay() ms (to allow interaction with the tooltip's widget).
     * If hideDelay() is 0, this is equivalent to hide().
     * @see hideDelay()
     */
    void hideLater();

    /**
     * Set after how many ms hideLater() will hide the tooltip.
     * @see hideLater(), hideDelay()
     */
    void setHideDelay(int delay);

Q_SIGNALS:
    /**
     * The tooltip has been hidden and the tooltip's widget is no longer visible.
     * This signal can be used to delete the tooltip's widget.
     */
    void hidden();

protected:
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    class KToolTipWidgetPrivate;
    QScopedPointer<KToolTipWidgetPrivate> d;

    Q_DISABLE_COPY(KToolTipWidget)
};

#endif
