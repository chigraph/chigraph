/*
   This file is part of the KDE libraries
   Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>

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

#ifndef KRATINGPAINTER_H
#define KRATINGPAINTER_H

#include <kwidgetsaddons_export.h>

#include <QtCore/Qt>

class QIcon;
class QPixmap;
class QPainter;
class QPoint;
class QRect;

/**
 * \brief Utility class that draws a row of stars for a rating value.
 *
 * The KRatingPainter also allows to determine a rating value from
 * a position in the draw area. it supports all different alignments
 * and custom icons.
 *
 * For showing a rating in a widget see KRatingWidget.
 *
 * \author Sebastian Trueg <trueg@kde.org>
 *
 * \since 4.1
 */
class KWIDGETSADDONS_EXPORT KRatingPainter
{
public:
    /**
     * Create a new KRatingPainter.
     * For most cases the static methods paintRating and getRatingFromPosition
     * should be sufficient.
     */
    KRatingPainter();

    /**
     * Destructor
     */
    ~KRatingPainter();

    /**
     * The maximum rating, i.e. how many stars are drawn
     * in total.
     *
     * \sa setMaxRating
     */
    int maxRating() const;

    /**
     * If half steps are enabled one star equals to 2 rating
     * points and uneven rating values result in half-stars being
     * drawn.
     *
     * \sa setHalfStepsEnabled
     */
    bool halfStepsEnabled() const;

    /**
     * The alignment of the stars.
     *
     * \sa setAlignment
     */
    Qt::Alignment alignment() const;

    /**
     * The layout direction. If RTL the stars
     * representing the rating value will be drawn from the
     * right.
     *
     * \sa setLayoutDirection
     */
    Qt::LayoutDirection layoutDirection() const;

    /**
     * The icon used to draw a star. In case a custom pixmap has been set
     * this value is ignored.
     *
     * \sa setIcon, setCustomPixmap
     */
    QIcon icon() const;

    /**
     * The rating can be painted in a disabled state where no color
     * is used and hover ratings are ignored.
     *
     * \sa setEnabled
     */
    bool isEnabled() const;

    /**
     * The custom pixmap set to draw a star. If no custom
     * pixmap has been set, an invalid pixmap is returned.
     *
     * \sa setCustomPixmap
     */
    QPixmap customPixmap() const;

    /**
     * The spacing between rating pixmaps.
     *
     * \sa setSpacing
     */
    int spacing() const;

    /**
     * The maximum rating. Defaults to 10.
     */
    void setMaxRating(int max);

    /**
     * If half steps are enabled (the default) then
     * one rating step corresponds to half a star.
     */
    void setHalfStepsEnabled(bool enabled);

    /**
     * The alignment of the stars in the drawing rect.
     * All alignment flags are supported.
     */
    void setAlignment(Qt::Alignment align);

    /**
     * LTR or RTL
     */
    void setLayoutDirection(Qt::LayoutDirection direction);

    /**
     * Set a custom icon. Defaults to "rating".
     */
    void setIcon(const QIcon &icon);

    /**
     * Enable or disable the rating. Default is enabled.
     */
    void setEnabled(bool enabled);

    /**
     * Set a custom pixmap.
     */
    void setCustomPixmap(const QPixmap &pixmap);

    /**
     * Set the spacing between rating pixmaps. Be aware that
     * for justified horizontal alignment this values may be
     * ignored.
     */
    void setSpacing(int spacing);

    /**
     * Draw the rating.
     *
     * \param painter The painter to draw the rating to.
     * \param rect The geometry of the rating. The alignment of the rating is used relative
     *             to this value.
     * \param rating The actual rating value to draw.
     * \param hoverRating The hover rating indicates the position the user hovers the mouse
     *                    pointer at. This will provide visual feedback about the new rating
     *                    if the user would actually click as well as the difference to the
     *                    current rating.
     */
    void paint(QPainter *painter, const QRect &rect, int rating, int hoverRating = -1) const;

    /**
     * Calculate the rating value from mouse position pos.
     *
     * \return The rating corresponding to pos or -1 if pos is
     * outside of the configured rect.
     */
    int ratingFromPosition(const QRect &rect, const QPoint &pos) const;

    /**
     * Convenience method that paints a rating into the given rect.
     *
     * LayoutDirection is read from QPainter.
     *
     * \param align can be aligned vertically and horizontally. Using Qt::AlignJustify will insert spacing
     * between the stars.
     */
    static void paintRating(QPainter *p, const QRect &rect, Qt::Alignment align, int rating, int hoverRating = -1);

    /**
     * Get the rating that would be selected if the user clicked position pos
     * within rect if the rating has been drawn with paintRating() using the same
     * rect and align values.
     *
     * \return The new rating or -1 if pos is outside of the rating area.
     */
    static int getRatingFromPosition(const QRect &rect, Qt::Alignment align, Qt::LayoutDirection direction, const QPoint &pos);

private:
    class Private;
    Private *const d;
};

#endif
