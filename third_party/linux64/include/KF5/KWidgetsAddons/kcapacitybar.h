/*
 * This file is part of the KDE project
 * Copyright (C) 2008 Rafael Fernández López <ereslibre@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KCAPACITYBAR_H
#define KCAPACITYBAR_H

#include <QWidget>

#include <kwidgetsaddons_export.h>

class QPaintEvent;

/**
  * @brief This widget shows a bar which is filled to show the level of usage of
  *        a certain device.
  *
  * This widget represents a bar which goal is to show the level of usage of a
  * device. Its look is similar to a progress bar, but different, because this
  * widget does not want to give a notion of progress.
  *
  * @since 4.2
  *
  * \image html kcapacitybar.png "KDE Capacity Bar"
  *
  * @author Rafael Fernández López <ereslibre@kde.org>
  */
class KWIDGETSADDONS_EXPORT KCapacityBar
    : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(DrawTextMode drawTextMode READ drawTextMode WRITE setDrawTextMode)
    Q_PROPERTY(bool fillFullBlocks READ fillFullBlocks WRITE setFillFullBlocks)
    Q_PROPERTY(bool continuous READ continuous WRITE setContinuous)
    Q_PROPERTY(int barHeight READ barHeight WRITE setBarHeight)
    Q_PROPERTY(Qt::Alignment horizontalTextAlignment READ horizontalTextAlignment
               WRITE setHorizontalTextAlignment)

public:
    enum DrawTextMode {
        DrawTextInline = 0,     ///< If any text set, draw it into the capacity bar
        DrawTextOutline         ///< If any text set, draw it out of the capacity bar
    };
    Q_ENUM(DrawTextMode)


    /**
     * Constructs a capacity bar with DrawTextOutline as draw text mode.
     * @param parent The parent of the widget.
     * @since 5.24
     */
    explicit KCapacityBar(QWidget *parent = Q_NULLPTR);

    /**
      * Capacity bar constructor.
      *
      * @param drawTextMode If any text set, whether to draw it into the capacity bar
      *                     or not.
      * @param parent       The parent of the widget.
      */
    explicit KCapacityBar(DrawTextMode drawTextMode, QWidget *parent = 0);
    ~KCapacityBar();

    /**
      * Capacity bar fill value.
      *
      * @param value    This parameter can take values from 0 to 100.
      *
      * @note Its value is 0 by default.
      */
    void setValue(int value);

    /**
      * @return The fill value of the capacity bar.
      */
    int value() const;

    /**
      * Sets the text for the capacity bar.
      *
      * @param text The text that the capacity bar will show.
      *
      * @note This is an empty string by default.
      */
    void setText(const QString &text);

    /**
      * @return The text that the capacity bar will show.
      */
    QString text() const;

    /**
      * When the capacity bar is non-continuous, sets whether the last block
      * shown should be drawn full or can be cut off (depending on the capacity
      * bar width, and the value set on it).
      *
      * @param fillFullBlocks If true, the last block drawn will be fully filled,
      *                       on other case, the last block drawn could be cut off.
      *
      * @note This method is only relevant if the capacity bar is in
      *       non-continuous mode.
      *
      * @note Its value is true by default.
      *
      * @see setContinuous, continuous
      */
    void setFillFullBlocks(bool fillFullBlocks);

    /**
      * @return Whether the last block shown can be cut off when necessary.
      */
    bool fillFullBlocks() const;

    /**
      * Sets whether the fill of the capacity bar should be continuous or in
      * block mode.
      *
      * @param continuous If true, the fill of the capacity bar is done in a
      *                   continuous way. In other case, the fill is done with
      *                   separated blocks.
      *
      * @note Its value is true by default.
      */
    void setContinuous(bool continuous);

    /**
      * @return Whether the fill of the capacity bar should be continuous or
      *         block-based.
      */
    bool continuous() const;

    /**
      * Sets the height (in pixels) of the bar.
      *
      * @param barHeight The preferred height (in pixels) of the capacity bar.
      *
      * @note If you set a certain text and the capacity bar is in inline mode,
      *       the height of the bar will be the maximum of the font height and
      *       this value.
      *
      * @note If you set a certain text and the capacity bar is in outline mode,
      *       the height of the whole capacity bar will be bigger than this
      *       value. Take in count the height of this widget is got from adding
      *       the bar height, the font metrics height and a small separator
      *       between the bar and the outline text.
      *
      * @note Its value is 12 pixels by default.
      */
    void setBarHeight(int barHeight);

    /**
      * @return The preferred height of the capacity bar.
      */
    int barHeight() const;

    /**
      * If the capacity bar is in outline text mode, draw the text with
      * @p textAlignment alignment.
      *
      * @param textAlignment    Sets the horizontal alignment for the text if
      *                         the capacity bar is in outline text mode.
      *
      * @note If @p textAlignemt contains vertical alignment flags, they will be
      *       ignored.
      *
      * @note If the capacity bar is in inline text mode, the text is always
      *       centered, and both vertical and horizontal flags set through this
      *       method are ignored.
      *
      * @note Its value is centered by default.
      */
    void setHorizontalTextAlignment(Qt::Alignment textAlignment);

    /**
      * @return The horizontal alignment for the text that will be drawn.
      */
    Qt::Alignment horizontalTextAlignment() const;

    /**
     * Set the way text is drawn if any is set
     *
     * @param drawTextMode If any text set, whether to draw it into the capacity bar
     *                     or not.
     */
    void setDrawTextMode(DrawTextMode mode);

    /**
     * The way text is drawn, inside the capacity bar or outside of it
     */
    DrawTextMode drawTextMode() const;

    /**
      * This method allows you to draw the widget, directly, for example on
      * item delegates. You only need the painter object and the rect where
      * this widget should be drawn.
      */
    void drawCapacityBar(QPainter *p, const QRect &rect) const;

    // Reimplemented from QWidget
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

protected:
    // Reimplemented from QWidget
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    // Reimplemented from QWidget
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    /**
      * @internal
      */
    class Private;
    Private *const d;
};

#endif
