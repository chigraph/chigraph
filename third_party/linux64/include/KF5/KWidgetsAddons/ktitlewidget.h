/* This file is part of the KDE libraries
   Copyright (C) 2007-2009 Urs Wolfer <uwolfer @ kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KTITLEWIDGET_H
#define KTITLEWIDGET_H

#include <kwidgetsaddons_export.h>

#include <QWidget>

/**
 * @short Standard title widget.
 *
 * This class provides a widget often used for dialog titles.
 * \image html ktitlewidget.png "KTitleWidget with title and icon"
 *
 * KTitleWidget uses the general application font at 1.4 times its size to
 * style the text. This is a visual change from 4.x.
 *
 * @section Usage
 * KTitleWidget is very simple to use. You can either use its default text
 * (and pixmap) properties or display your own widgets in the title widget.
 *
 * A title text with a right-aligned pixmap:
 * @code
KTitleWidget *titleWidget = new KTitleWidget(this);
titleWidget->setText(i18n("Title"));
titleWidget->setPixmap(QIcon::fromTheme("screen").pixmap(22, 22));
 * @endcode
 *
 * Use it with an own widget:
 * @code
KTitleWidget *checkboxTitleWidget = new KTitleWidget(this);

QWidget *checkBoxTitleMainWidget = new QWidget(this);
QVBoxLayout *titleLayout = new QVBoxLayout(checkBoxTitleMainWidget);
titleLayout->setMargin(6);

QCheckBox *checkBox = new QCheckBox("Text Checkbox", checkBoxTitleMainWidget);
titleLayout->addWidget(checkBox);

checkboxTitleWidget->setWidget(checkBoxTitleMainWidget);
 * @endcode
 *
 * @see KPageView
 * @author Urs Wolfer \<uwolfer @ kde.org\>
 */

class KWIDGETSADDONS_EXPORT KTitleWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
    Q_PROPERTY(int autoHideTimeout READ autoHideTimeout WRITE setAutoHideTimeout)

public:
    /**
     * Possible title pixmap alignments.
     *
     * @li ImageLeft: Display the pixmap left
     * @li ImageRight: Display the pixmap right (default)
     */
    enum ImageAlignment {
        ImageLeft, /**< Display the pixmap on the left */
        ImageRight /**< Display the pixmap on the right */
    };
    Q_ENUM(ImageAlignment)

    /**
     * Comment message types
     */
    enum MessageType {
        PlainMessage, /**< Normal comment */
        InfoMessage, /**< Information the user should be alerted to */
        WarningMessage, /**< A warning the user should be alerted to */
        ErrorMessage /**< An error message */
    };

    /**
     * Constructs a title widget with the given @param parent.
     */
    explicit KTitleWidget(QWidget *parent = 0);

    virtual ~KTitleWidget();

    /**
     * @param widget Widget displayed on the title widget.
     */
    void setWidget(QWidget *widget);

    /**
     * @return the text displayed in the title
     * @see setText()
     */
    QString text() const;

    /**
     * @return the text displayed in the comment below the title, if any
     * @see setComment()
     */
    QString comment() const;

    /**
     * @return the pixmap displayed in the title
     * @see setPixmap()
     */
    const QPixmap *pixmap() const;

    /**
     * Sets this label's buddy to buddy.
     * When the user presses the shortcut key indicated by the label in this
     * title widget, the keyboard focus is transferred to the label's buddy
     * widget.
     * @param buddy the widget to activate when the shortcut key is activated
     */
    void setBuddy(QWidget *buddy);

    /**
     * Get the current timeout value in milliseconds
     * @return timeout value in msecs
     */
    int autoHideTimeout() const;

public Q_SLOTS:
    /**
     * @param text Text displayed on the label. It can either be plain text or rich text. If it
     * is plain text, the text is displayed as a bold title text.
     * @param alignment Alignment of the text. Default is left and vertical centered.
     * @see text()
     */
    void setText(const QString &text, Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter);
    /**
     * @param text Text displayed on the label. It can either be plain text or rich text. If it
     * is plain text, the text is displayed as a bold title text.
     * @param type The sort of message it is; will also set the icon accordingly @see MessageType
     * @see text()
     */
    void setText(const QString &text, MessageType type);

    /**
     * @param comment Text displayed beneath the main title as a comment.
     *                It can either be plain text or rich text.
     * @param type The sort of message it is. @see MessageType
     * @see comment()
     */
    void setComment(const QString &comment, MessageType type = PlainMessage);

    /**
     * @param pixmap Pixmap displayed in the header. The pixmap is by default right, but
     * @param alignment can be used to display it also left.
     * @see pixmap()
     */
    void setPixmap(const QPixmap &pixmap, ImageAlignment alignment = ImageRight);

    /**
     * @param icon name of the icon to display in the header. The pixmap is by default right, but
     * @param alignment can be used to display it also left.
     * @see pixmap()
     */
    void setPixmap(const QString &icon, ImageAlignment alignment = ImageRight);

    /**
     * @param pixmap the icon to display in the header. The pixmap is by default right, but
     * @param alignment can be used to display it also left.
     * @see pixmap()
     */
    void setPixmap(const QIcon &icon, ImageAlignment alignment = ImageRight);

    /**
     * @param pixmap the icon to display in the header. The pixmap is by default right, but
     * @param alignment can be used to display it also left.
     * @see pixmap()
     */
    void setPixmap(MessageType type, ImageAlignment alignment = ImageRight);

    /**
     * Set the autohide timeout of the label
     * Set value to 0 to disable autohide, which is the default.
     * @param msecs timeout value in milliseconds
     */
    void setAutoHideTimeout(int msecs);

protected:
    void changeEvent(QEvent *e) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_timeoutFinished())
    Q_DISABLE_COPY(KTitleWidget)
};

#endif
