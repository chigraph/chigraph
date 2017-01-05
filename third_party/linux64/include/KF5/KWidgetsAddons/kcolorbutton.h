/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)

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

#ifndef KCOLORBUTTON_H
#define KCOLORBUTTON_H

#include <kwidgetsaddons_export.h>

#include <QPushButton>

class KColorButtonPrivate;
/**
* @short A pushbutton to display or allow user selection of a color.
*
* This widget can be used to display or allow user selection of a color.
*
* @see QColorDialog
*
* \image html kcolorbutton.png "KDE Color Button"
*/
class KWIDGETSADDONS_EXPORT KColorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY changed USER true)
    Q_PROPERTY(QColor defaultColor READ defaultColor WRITE setDefaultColor)
    Q_PROPERTY(bool alphaChannelEnabled READ isAlphaChannelEnabled WRITE setAlphaChannelEnabled)

public:
    /**
     * Creates a color button.
     */
    explicit KColorButton(QWidget *parent = 0);

    /**
     * Creates a color button with an initial color @p c.
     */
    explicit KColorButton(const QColor &c, QWidget *parent = 0);

    /**
     * Creates a color button with an initial color @p c and default color @p defaultColor.
     */
    KColorButton(const QColor &c, const QColor &defaultColor, QWidget *parent = 0);

    virtual ~KColorButton();

    /**
     * Returns the currently chosen color.
     */
    QColor color() const;

    /**
     * Sets the current color to @p c.
     */
    void setColor(const QColor &c);

    /**
     * When set to true, allow the user to change the alpha component
     * of the color. The default value is false.
     * @since 4.5
     */
    void setAlphaChannelEnabled(bool alpha);

    /**
     * Returns true if the user is allowed to change the alpha component.
     * @since 4.5
     */
    bool isAlphaChannelEnabled() const;

    /**
     * Returns the default color or an invalid color
     * if no default color is set.
     */
    QColor defaultColor() const;

    /**
     * Sets the default color to @p c.
     */
    void setDefaultColor(const QColor &c);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    /**
     * Emitted when the color of the widget
     * is changed, either with setColor() or via user selection.
     */
    void changed(const QColor &newColor);

protected:
    void paintEvent(QPaintEvent *pe) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

private:
    class KColorButtonPrivate;
    KColorButtonPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_chooseColor())
    Q_PRIVATE_SLOT(d, void _k_colorChosen())
};

#endif

