/*********************************************************************************
 *                                                                               *
 *   Copyright (C) 2005, 2009 by Albert Astals Cid <aacid@kde.org>               *
 *                                                                               *
 * This library is free software; you can redistribute it and/or                 *
 * modify it under the terms of the GNU Lesser General Public                    *
 * License as published by the Free Software Foundation; either                  *
 * version 2.1 of the License, or (at your option) version 3, or any             *
 * later version accepted by the membership of KDE e.V. (or its                  *
 * successor approved by the membership of KDE e.V.), which shall                *
 * act as a proxy defined in Section 6 of version 3 of the license.              *
 *                                                                               *
 * This library is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
 * Lesser General Public License for more details.                               *
 *                                                                               *
 * You should have received a copy of the GNU Lesser General Public              *
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                               *
 *********************************************************************************/

#ifndef KFONTMETRICS_H
#define KFONTMETRICS_H

#include <kguiaddons_export.h>

#include <qglobal.h>

class QPainter;
class QSizeF;
class QString;

namespace KFontUtils
{
/** Modifiers for the adaptFontSize function */
enum AdaptFontSizeOption {
    NoFlags = 0x01 /** No modifier */,
    DoNotAllowWordWrap = 0x02  /** Do not use word wrapping */
};
Q_DECLARE_FLAGS(AdaptFontSizeOptions, AdaptFontSizeOption)

/** Helper function that calculates the biggest font size (in points) used
    drawing a centered text using word wrapping.
    @param painter The painter where the text will be painted. The font set
                   in the painter is used for the calculation. Note the
                   painter font size is modified by this call
    @param text The text you want to draw
    @param width The available width for drawing
    @param height The available height for drawing
    @param maxFontSize The maximum font size (in points) to consider
    @param minFontSize The minimum font size (in points) to consider
    @param flags The modifiers for how the text is painted
    @return The calculated biggest font size (in points) that draws the text
            in the given dimensions. Can return smaller than minFontSize,
            that means the text doesn't fit in the given rectangle. Can
            return -1 on error
    @since 4.7
*/
qreal KGUIADDONS_EXPORT adaptFontSize(QPainter &painter,
                                      const QString &text,
                                      qreal width,
                                      qreal height,
                                      qreal maxFontSize = 28.0,
                                      qreal minFontSize = 1.0,
                                      AdaptFontSizeOptions flags = NoFlags);

/** Convenience function for adaptFontSize that accepts a QSizeF instead two qreals
    @since 4.7
*/
qreal KGUIADDONS_EXPORT adaptFontSize(QPainter &painter,
                                      const QString &text,
                                      const QSizeF &availableSize,
                                      qreal maxFontSize = 28.0,
                                      qreal minFontSize = 1.0,
                                      AdaptFontSizeOptions flags = NoFlags);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KFontUtils::AdaptFontSizeOptions)

#endif

