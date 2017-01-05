/*
  Copyright 2008 Aurélien Gâteau <agateau@kde.org>
  Copyright 2009 Sebastian Trueg <trueg@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef _K_PIXMAPSEQUENCE_H_
#define _K_PIXMAPSEQUENCE_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QSize>

#include <kwidgetsaddons_export.h>

class QPixmap;

/**
 * \class KPixmapSequence kpixmapsequence.h KPixmapSequence
 *
 * \brief Loads and gives access to the frames of a typical multi-row pixmap
 * as often used for spinners.
 *
 * KPixmapSequence is implicitly shared. Copying is fast.
 *
 * \author Aurélien Gâteau <agateau@kde.org>
 * \author Sebastian Trueg <trueg@kde.org>
 *
 * \since 4.4
 */
class KWIDGETSADDONS_EXPORT KPixmapSequence
{
public:
    /**
     * Create an empty sequence
     */
    KPixmapSequence();

    /**
     * Copy constructor
     */
    KPixmapSequence(const KPixmapSequence &other);

    /**
     * Create a sequence from a pixmap.
     *
     * \param pixmap Pixmap to load
     * \param frameSize The size of the frames to load. The width of the file has to be
     * a multiple of the frame width; the same is true for the height. If an invalid
     * size is specified the file is considered to be one column of square frames.
     */
    explicit KPixmapSequence(const QPixmap &pixmap, const QSize &frameSize = QSize());

    /**
     * Create a sequence from an icon name.
     *
     * \param iconName The full path of the icon
     * \param size The icon/frame size
     */
    KPixmapSequence(const QString &fullPath, int size);

    /**
     * Destructor
     */
    ~KPixmapSequence();

    /**
     * Create a copy of \p other. The data is implicitly shared.
     */
    KPixmapSequence &operator=(const KPixmapSequence &other);

    /**
     * \return \p true if a sequence was loaded successfully.
     *
     * \sa isEmpty
     */
    bool isValid() const;

    /**
     * \return \p true if no sequence was loaded successfully.
     *
     * \sa isValid
     */
    bool isEmpty() const;

    /**
     * \return The size of an individual frame in the sequence.
     */
    QSize frameSize() const;

    /**
     * The number of frames in this sequence.
     */
    int frameCount() const;

    /**
     * Retrieve the frame at \p index.
     *
     * \param index The index of the frame in question starting at 0.
     */
    QPixmap frameAt(int index) const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
