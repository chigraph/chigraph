/*
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

#ifndef _K_PIXMAPSEQUENCE_OVERLAY_PAINTER_H_
#define _K_PIXMAPSEQUENCE_OVERLAY_PAINTER_H_

#include <QtCore/QObject>
#include <QtCore/QPoint>

#include <kwidgetsaddons_export.h>

class KPixmapSequence;
class QWidget;
class QEvent;
class QRect;

/**
 * \class KPixmapSequenceOverlayPainter kpixmapsequenceoverlaypainter.h KPixmapSequenceOverlayPainter
 *
 * \brief Paints a KPixmapSequence on top of any widget at any position.
 *
 * The KPixmapSequenceOverlayPainter paints an overlay on top of an arbitrary QWidget
 * using a KPixmapSequence. This is typically used for spinners indicating that a process
 * is not finished yet.
 *
 * \author Sebastian Trueg <trueg@kde.org>
 *
 * \since 4.4
 */
class KWIDGETSADDONS_EXPORT KPixmapSequenceOverlayPainter : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    KPixmapSequenceOverlayPainter(QObject *parent = 0);
    KPixmapSequenceOverlayPainter(const KPixmapSequence &seq, QObject *parent = 0);

    /**
     * Destructor
     */
    ~KPixmapSequenceOverlayPainter();

    /**
     * The sequence used to draw the overlay.
     *
     * \sa setSequence
     */
    KPixmapSequence sequence() const;

    /**
     * The interval between frames.
     *
     * \sa setInterval
     */
    int interval() const;

    /**
     * The optional rect to draw the pixmaps in.
     * \sa setRect
     */
    QRect rect() const;

    /**
     * The alignment of the pixmaps in the rect.
     * \sa setAlignment
     */
    Qt::Alignment alignment() const;

    /**
     * The optional offset within the rect.
     * \sa setOffset
     */
    QPoint offset() const;

public Q_SLOTS:
    /**
     * Set the sequence to be used. By default the KDE busy sequence is used.
     */
    void setSequence(const KPixmapSequence &seq);

    /**
     * Set the interval between frames. The default is 200.
     */
    void setInterval(int msecs);

    /**
     * Set the widget to draw the overlay on.
     */
    void setWidget(QWidget *w);

    /**
     * Set the rect in which to place the sequence. Be aware that
     * this optional property does not scale the pixmaps (except if
     * it is smaller) but allows to change the placement.
     *
     * \param rect The rect in which to draw the pixmap using alignment
     * and offset. Be aware that setting a rect bigger than the widget
     * can lead to weird painting errors.
     *
     * Defaults to the widget's rect.
     */
    void setRect(const QRect &rect);

    /**
     * Set the alignment of the sequence in rect.
     *
     * \param align alignment of the overlay. Qt::AlignJustify does not make sense here.
     * Defaults to Qt::Center.
     */
    void setAlignment(Qt::Alignment align);

    /**
     * Set the offset relative to the placement determined by alignment
     * and rect.
     *
     * \param offset An optional offset which allows an absolute placement.
     *
     * Defaults to an empty point.
     */
    void setOffset(const QPoint &offset);

    /**
     * Start drawing the sequence.
     *
     * The overlay will be drawn until a call to stop()
     */
    void start();

    /**
     * Stop drawing the overlay.
     */
    void stop();
    
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_timeout())
};

#endif
