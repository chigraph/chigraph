/*
    Copyright (C) 2003 Nadeem Hasan <nhasan@kde.org>

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

#ifndef KFONTREQUESTER_H
#define KFONTREQUESTER_H

#include <QWidget>
#include <QFont>
#include <QtCore/QString>

#include <kwidgetsaddons_export.h>

class QLabel;
class QPushButton;

/**
 * This class provides a widget with a lineedit and a button, which invokes
 * a font dialog (QFontDialog).
 *
 * The lineedit provides a preview of the selected font. The preview text can
 * be customized. You can also have the font dialog show only the fixed fonts.
 *
 * \image html kfontrequester.png "KDE Font Requester"
 *
 * @author Nadeem Hasan <nhasan@kde.org>
 *
 */
class KWIDGETSADDONS_EXPORT KFontRequester : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString sampleText READ sampleText WRITE setSampleText)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontSelected USER true)

public:

    /**
     * Constructs a font requester widget.
     *
     * @param parent The parent widget.
     * @param onlyFixed Only display fonts which have fixed-width character
     *        sizes.
     */
    explicit KFontRequester(QWidget *parent = 0L, bool onlyFixed = false);

    ~KFontRequester();

    /**
     * @return The currently selected font in the requester.
     */
    QFont font() const;

    /**
     * @return Returns true if only fixed fonts are displayed.
     */
    bool isFixedOnly() const;

    /**
     * @return The current text in the sample text input area.
     */
    QString sampleText() const;

    /**
     * @return The current title of the widget.
     */
    QString title() const;

    /**
     * @return Pointer to the label used for preview.
     */
    QLabel *label() const;

    /**
     * @return Pointer to the pushbutton in the widget.
     */
    QPushButton *button() const;

    /**
     * Sets the currently selected font in the requester.
     *
     * @param font The font to select.
     * @param onlyFixed Display only fixed-width fonts in the font dialog
     * if @p true, or vice-versa.
     */
    virtual void setFont(const QFont &font, bool onlyFixed = false);

    /**
     * Sets the sample text.
     *
     * Normally you should not change this
     * text, but it can be better to do this if the default text is
     * too large for the edit area when using the default font of your
     * application. Default text is current font name and size. Setting
     * the text to QString() will restore the default.
     *
     * @param text The new sample text. The current will be removed.
     */
    virtual void setSampleText(const QString &text);

    /**
     * Set the title for the widget that will be used in the tooltip and
     * what's this text.
     *
     * @param title The title to be set.
     */
    virtual void setTitle(const QString &title);

Q_SIGNALS:
    /**
     * Emitted when a new @p font has been selected in the underlying dialog
     */
    void fontSelected(const QFont &font);

private:
    class KFontRequesterPrivate;
    friend class KFontRequesterPrivate;
    KFontRequesterPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_buttonClicked())

    Q_DISABLE_COPY(KFontRequester)
};

#endif // KFONTREQUESTER_H

