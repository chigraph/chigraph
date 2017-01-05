/* This file is part of the KDE libraries
   Copyright (C) 2000 Ronny Standtke <Ronny.Standtke@gmx.de>

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

#ifndef KSQUEEZEDTEXTLABEL_H
#define KSQUEEZEDTEXTLABEL_H

#include <kwidgetsaddons_export.h>
#include <QLabel>

class KSqueezedTextLabelPrivate;

/**
 * @short A replacement for QLabel that squeezes its text
 *
 * A label class that squeezes its text into the label
 *
 * If the text is too long to fit into the label it is divided into
 * remaining left and right parts which are separated by three dots.
 *
 * Example:
 * http://www.kde.org/documentation/index.html could be squeezed to
 * http://www.kde...ion/index.html
 *
 * \image html ksqueezedtextlabel.png "KSqueezedTextLabel Widget"
 *
 * @author Ronny Standtke <Ronny.Standtke@gmx.de>
 */

/*
 * QLabel
 */
class KWIDGETSADDONS_EXPORT KSqueezedTextLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(Qt::TextElideMode textElideMode READ textElideMode WRITE setTextElideMode)

public:
    /**
     * Default constructor.
     */
    explicit KSqueezedTextLabel(QWidget *parent = 0);
    explicit KSqueezedTextLabel(const QString &text, QWidget *parent = 0);

    virtual ~KSqueezedTextLabel();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    /**
     * Overridden for internal reasons; the API remains unaffected.
     */
    virtual void setAlignment(Qt::Alignment);

    /**
     *  Returns the text elide mode.
     */
    Qt::TextElideMode textElideMode() const;

    /**
     * Sets the text elide mode.
     * @param mode The text elide mode.
     */
    void setTextElideMode(Qt::TextElideMode mode);

    /**
     * Get the full text set via setText.
     *
     * @since 4.4
     */
    QString fullText() const;

public Q_SLOTS:
    /**
     * Sets the text. Note that this is not technically a reimplementation of QLabel::setText(),
     * which is not virtual (in Qt 4.3). Therefore, you may need to cast the object to
     * KSqueezedTextLabel in some situations:
     * \Example
     * \code
     * KSqueezedTextLabel* squeezed = new KSqueezedTextLabel("text", parent);
     * QLabel* label = squeezed;
     * label->setText("new text");    // this will not work
     * squeezed->setText("new text"); // works as expected
     * static_cast<KSqueezedTextLabel*>(label)->setText("new text");  // works as expected
     * \endcode
     * @param mode The new text.
     */
    void setText(const QString &text);
    /**
     * Clears the text. Same remark as above.
     *
     */
    void clear();

protected:
    /**
     * \reimp
     */
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

    /**
     * Called when widget is resized
     */
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    /**
     * \reimp
     */
    void contextMenuEvent(QContextMenuEvent *) Q_DECL_OVERRIDE;
    /**
     * does the dirty work
     */
    void squeezeTextToLabel();

private:
    Q_PRIVATE_SLOT(d, void _k_copyFullText())
    KSqueezedTextLabelPrivate *const d;
};

#endif // KSQUEEZEDTEXTLABEL_H
