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
//-----------------------------------------------------------------------------
// Selector widgets for KDE Color Selector, but probably useful for other
// stuff also.

#ifndef KSELECTOR_H
#define KSELECTOR_H

#include <kwidgetsaddons_export.h>

#include <QAbstractSlider>
#include <QGradient>
#include <QWidget>

/**
 * KSelector is the base class for other widgets which
 * provides the ability to choose from a one-dimensional
 * range of values. An example is the KGradientSelector
 * which allows to choose from a range of colors.
 *
 * A custom drawing routine for the widget surface has
 * to be provided by the subclass.
 */
class KWIDGETSADDONS_EXPORT KSelector : public QAbstractSlider
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(int minValue READ minimum WRITE setMinimum)
    Q_PROPERTY(int maxValue READ maximum WRITE setMaximum)
    Q_PROPERTY(bool indent READ indent WRITE setIndent)
    Q_PROPERTY(Qt::ArrowType arrowDirection READ arrowDirection WRITE setArrowDirection)
public:

    /**
     * Constructs a horizontal one-dimensional selection widget.
     */
    explicit KSelector(QWidget *parent = 0);
    /**
     * Constructs a one-dimensional selection widget with
     * a given orientation.
     */
    explicit KSelector(Qt::Orientation o, QWidget *parent = 0);
    /*
     * Destructs the widget.
     */
    ~KSelector();

    /**
     * @return the rectangle on which subclasses should draw.
     */
    QRect contentsRect() const;

    /**
     * Sets the indent option of the widget to i.
     * This determines whether a shaded frame is drawn.
     */
    void setIndent(bool i);

    /**
     * @return whether the indent option is set.
     */
    bool indent() const;

    /**
     * Sets the arrow direction.
     */
    void setArrowDirection(Qt::ArrowType direction);

    /**
     * @return the current arrow direction
     */
    Qt::ArrowType arrowDirection() const;

protected:
    /**
     * Override this function to draw the contents of the control.
     * The default implementation does nothing.
     *
     * Draw only within contentsRect().
     */
    virtual void drawContents(QPainter *);
    /**
     * Override this function to draw the cursor which
     * indicates the current value.
     */
    virtual void drawArrow(QPainter *painter, const QPoint &pos);

    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;

private:
    QPoint calcArrowPos(int val);
    void moveArrow(const QPoint &pos);

private:
    class Private;
    friend class Private;
    Private *const d;

    Q_DISABLE_COPY(KSelector)
};

/**
 * The KGradientSelector widget allows the user to choose
 * from a one-dimensional range of colors which is given as a
 * gradient between two colors provided by the programmer.
 *
 * \image html kgradientselector.png "KDE Gradient Selector Widget"
 *
 **/
class KWIDGETSADDONS_EXPORT KGradientSelector : public KSelector
{
    Q_OBJECT

    Q_PROPERTY(QColor firstColor READ firstColor WRITE setFirstColor)
    Q_PROPERTY(QColor secondColor READ secondColor WRITE setSecondColor)
    Q_PROPERTY(QString firstText READ firstText WRITE setFirstText)
    Q_PROPERTY(QString secondText READ secondText WRITE setSecondText)

public:
    /**
     * Constructs a horizontal color selector which
     * contains a gradient between white and black.
     */
    explicit KGradientSelector(QWidget *parent = 0);
    /**
     * Constructs a colors selector with orientation o which
     * contains a gradient between white and black.
     */
    explicit KGradientSelector(Qt::Orientation o, QWidget *parent = 0);
    /**
     * Destructs the widget.
     */
    ~KGradientSelector();

    /**
     * Sets the colors that make up the gradient. Any previously set colors
     * are removed.
     * @since 4.5
     */
    void setStops(const QGradientStops &stops);

    /**
     * Get the colors that make up the gradient.
     * @since 4.5
     */
    QGradientStops stops() const;

    /**
     * Sets the two colors which span the gradient.
     */
    void setColors(const QColor &col1, const QColor &col2);
    void setText(const QString &t1, const QString &t2);

    /**
     * Set each color on its own.
     */
    void setFirstColor(const QColor &col);
    void setSecondColor(const QColor &col);

    /**
     * Set each description on its own
     */
    void setFirstText(const QString &t);
    void setSecondText(const QString &t);

    QColor firstColor() const;
    QColor secondColor() const;

    QString firstText() const;
    QString secondText() const;

protected:

    void drawContents(QPainter *) Q_DECL_OVERRIDE;
    virtual QSize minimumSize() const;

private:
    class KGradientSelectorPrivate;
    friend class KGradientSelectorPrivate;
    KGradientSelectorPrivate *const d;

    Q_DISABLE_COPY(KGradientSelector)
};

#endif      // KSELECTOR_H
