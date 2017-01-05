/* This file is part of the KDE libraries
   Copyright (C) 1998 Kurt Granroth <granroth@kde.org>
   Copyright (C) 2000 Peter Putzer <putzer@kde.org>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KURLLABEL_H
#define KURLLABEL_H

#include <kwidgetsaddons_export.h>

#include <QColor>
#include <QLabel>
#include <QPixmap>

class QCursor;

/**
 * @short A drop-in replacement for QLabel that displays hyperlinks.
 *
 * KUrlLabel is a drop-in replacement for QLabel that handles text
 * in a fashion similar to how an HTML widget handles hyperlinks.  The
 * text can be underlined (or not) and set to different colors.  It
 * can also "glow" (cycle colors) when the mouse passes over it.
 *
 * KUrlLabel also provides signals for several events, including
 * the mouse leaving and entering the text area and all forms of
 * mouse clicking.
 *
 * By default KUrlLabel accepts focus. When focused, standard
 * focus rectangle is displayed as in HTML widget.
 * Pressing Enter key accepts the focused label.
 *
 * A typical usage would be something like so:
 *
 * \code
 * KUrlLabel *address = new KUrlLabel(this);
 * address->setText("My homepage");
 * address->setUrl("http://www.home.com/~me");
 * connect(address, SIGNAL(leftClickedUrl(const QString&)),
 *                  SLOT(processMyUrl(const QString&)));
 * \endcode
 *
 * In this example, the text "My homepage" would be displayed
 * as blue, underlined text.  When the mouse passed over it,
 * it would "glow" red.  When the user clicks on the text, the
 * signal leftClickedUrl() would be emitted with "http://www.home.com/~me"
 * as its argument.
 *
 * \image html kurllabel.png "KDE URL Label"
 *
 * @author Kurt Granroth <granroth@kde.org> (Interface)
 * @author Peter Putzer <putzer@kde.org> (Rewrite)
 */
class KWIDGETSADDONS_EXPORT KUrlLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString tipText READ tipText WRITE setTipText)
    Q_PROPERTY(QPixmap alternatePixmap READ alternatePixmap WRITE setAlternatePixmap)
    Q_PROPERTY(bool glowEnabled READ isGlowEnabled WRITE setGlowEnabled)
    Q_PROPERTY(bool floatEnabled READ isFloatEnabled WRITE setFloatEnabled)
    Q_PROPERTY(bool useTips READ useTips WRITE setUseTips)
    Q_PROPERTY(bool useCursor READ useCursor WRITE setUseCursor)

public:
    /**
     * Default constructor.
     *
     * Use setUrl() and setText() or QListView::setPixmap()
     * to set the resp. properties.
     */
    explicit KUrlLabel(QWidget *parent = 0L);

    /**
     * Convenience constructor.
     *
     * @param url is the URL emitted when the label is clicked.
     * @param text is the displayed string. If it's equal to QString()
     * the @p url will be used instead.
     * @param parent Passed to lower level constructor
     *
     * @p parent and @p name are passed to QLabel, which in turn passes
     * them further down
     */
    explicit KUrlLabel(const QString &url, const QString &text = QString(),
                       QWidget *parent = 0L);

    /**
     * Destructs the label.
     */
    virtual ~KUrlLabel();

    /**
     * Returns the URL.
     */
    QString url() const;

    /**
     * Returns the current tooltip text.
     */
    QString tipText() const;

    /**
     * @return true if a tooltip will be displayed.
     *
     * @see setTipText()
     */
    bool useTips() const;

    /**
     * @return true if the cursor will change while over the URL.
     *
     * @see setUseCursor ()
     */
    bool useCursor() const;

    /**
     * When this is on, the text will switch to the selected
     * color whenever the mouse passes over it.
     */
    bool isGlowEnabled() const;

    /**
     * This feature is very similar to the "glow" feature in that the color of the
     * label switches to the selected color when the cursor passes
     * over it. In addition, underlining is turned on for as
     * long as the mouse is overhead. Note that if "glow" and
     * underlining are both already turned on, this feature
     * will have no visible effect.
     */
    bool isFloatEnabled() const;

    /**
     * @return the alternate pixmap (may be 0L if none was set).
     */
    const QPixmap *alternatePixmap() const;

public Q_SLOTS:
    /**
     * Turns on or off the underlining.
     *
     *  When this is on, the
     * text will be underlined.  By default, it is @p true.
     */
    void setUnderline(bool on = true);

    /**
     * Sets the URL for this label to @p url.
     *
     * @see url
     */
    void setUrl(const QString &url);

    /**
     * Overridden for internal reasons; the API remains unaffected.
     */
    virtual void setFont(const QFont &font);

    /**
     * Turns on or off the tool tip feature.
     *
     * When this is on, the URL will be displayed as a
     * tooltip whenever the mouse passes passes over it.
     * By default, it is @p false.
     */
    void setUseTips(bool on = true);

    /**
     * Specifies what text to display when tooltips are turned on.
     *
     * If this is not used, the tip will default to the URL.
     *
     * @see setUseTips()
     */
    void setTipText(const QString &tip);

    /**
     * Sets the highlight color.
     *
     * This is the default foreground
     * color (non-selected).  By default, it is @p blue.
     */
    void setHighlightedColor(const QColor &highcolor);

    /**
     * This is an overloaded version for convenience.
     *
     * @see setHighlightedColor()
     */
    void setHighlightedColor(const QString &highcolor);

    /**
     * Sets the selected color.
     *
     * This is the color the text will change
     * to when either a mouse passes over it and "glow" mode is on or
     * when it is selected (clicked).  By default, it is @p red.
     */
    void setSelectedColor(const QColor &color);

    /**
     * This is an overloaded version for convenience.
     *
     * @see setSelectedColor()
     */
    void setSelectedColor(const QString &color);

    /**
     * Turns the custom cursor feature on or off.
     *
     * When this is on, the cursor will change to a custom cursor
     * (default is a "pointing hand") whenever the cursor passes
     * over the label. By default, it is on.
     *
     * @param on whether a custom cursor should be displayed.
     * @param cursor is the custom cursor. @p 0L indicates the default "hand cursor".
     */
    void setUseCursor(bool on, QCursor *cursor = 0L);

    /**
     * Turns on or off the "glow" feature.
     *
     * When this is on, the text will switch to the
     * selected color whenever the mouse
     * passes over it. By default, it is @p true.
     */
    void setGlowEnabled(bool glow = true);

    /**
     * Turns on or off the "float" feature.
     *
     * This feature is very similar to the "glow" feature in
     * that the color of the label switches to the selected
     * color when the cursor passes over it. In addition,
     * underlining is turned on for as long as the mouse is overhead.
     * Note that if "glow" and underlining are both already turned
     * on, this feature will have no visible effect.
     * By default, it is @p false.
     */
    void setFloatEnabled(bool do_float = true);

    /**
     * Sets the "alt" pixmap.
     *
     * This pixmap will be displayed when the
     * cursor passes over the label.  The effect is similar to the
     * trick done with 'onMouseOver' in javascript.
     *
     * @see alternatePixmap()
     */
    void setAlternatePixmap(const QPixmap &pixmap);

Q_SIGNALS:

    /**
     * Emitted when the mouse has passed over the label.
     *
     * @param url The URL for this label.
     */
    void enteredUrl(const QString &url);

    /**
     * Emitted when the mouse has passed over the label.
     */
    void enteredUrl();

    /**
     * Emitted when the mouse is no longer over the label.
     *
     * @param url The URL for this label.
     */
    void leftUrl(const QString &url);

    /**
     * Emitted when the mouse is no longer over the label.
     */
    void leftUrl();

    /**
     * Emitted when the user clicked the left mouse button on this label.
     *
     * @param url The URL for this label.
     */
    void leftClickedUrl(const QString &url);

    /**
     * Emitted when the user clicked the left mouse button on this label.
     */
    void leftClickedUrl();

    /**
     * Emitted when the user clicked the right mouse button on this label.
     *
     * @param url The URL for this label.
     */
    void rightClickedUrl(const QString &url);

    /**
     * Emitted when the user clicked the right mouse button on this label.
     */
    void rightClickedUrl();

    /**
     * Emitted when the user clicked the middle mouse button on this label.
     *
     * @param url The URL for this label.
     */
    void middleClickedUrl(const QString &url);

    /**
     * Emitted when the user clicked the left mouse button on this label.
     */
    void middleClickedUrl();

protected:
    /**
     * Overridden for internal reasons; the API remains unaffected.
     */
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

    /**
     * Overridden for internal reasons; the API remains unaffected.
     */
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;

    /**
     * Overridden for internal reasons; the API remains unaffected.
     */
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;

    /**
     * Catch parent palette changes
     */
    bool event(QEvent *) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void updateColor())
};

#endif
