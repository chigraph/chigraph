/*
    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Bernd Johannes Wuebben <wuebben@kde.org>
    Copyright (c) 1999 Preston Brown <pbrown@kde.org>
    Copyright (c) 1999 Mario Weilguni <mweilguni@kde.org>

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
#ifndef K_FONT_CHOOSER_H
#define K_FONT_CHOOSER_H

#include <kwidgetsaddons_export.h>
#include <QWidget>

class QFont;
class QStringList;

/**
 * @short A font selection widget.
 *
 * While KFontChooser as an ordinary widget can be embedded in
 * custom dialogs and therefore is very flexible, in most cases
 * it is preferable to use the convenience functions in
 * QFontDialog.
 *
 * \image html kfontchooser.png "KDE Font Chooser Widget"
 *
 * @see KFontRequester
 *
 * @author Preston Brown <pbrown@kde.org>, Bernd Wuebben <wuebben@kde.org>
 */
class KWIDGETSADDONS_EXPORT KFontChooser : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontSelected USER true)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(Qt::CheckState sizeIsRelative READ sizeIsRelative WRITE setSizeIsRelative)
    Q_PROPERTY(QString sampleText READ sampleText WRITE setSampleText)

public:
    /**
     *  @li @p FamilyList - Identifies the family (leftmost) list.
     *  @li @p StyleList -  Identifies the style (center) list.
     *  @li @p SizeList -   Identifies the size (rightmost) list.
     */
    enum FontColumn { FamilyList = 0x01, StyleList = 0x02, SizeList = 0x04};

    /**
     *  @li @p FontDiffFamily - Identifies a requested change in the font family.
     *  @li @p FontDiffStyle -  Identifies a requested change in the font style.
     *  @li @p FontDiffSize -   Identifies a requested change in the font size.
     */
    enum FontDiff { NoFontDiffFlags = 0,
                    FontDiffFamily = 1,
                    FontDiffStyle = 2,
                    FontDiffSize = 4,
                    AllFontDiffs = FontDiffFamily | FontDiffStyle | FontDiffSize
                  };
    Q_DECLARE_FLAGS(FontDiffFlags, FontDiff)

    /**
     * @li @p FixedFontsOnly only show fixed fonts, excluding proportional fonts
     * @li @p DisplayFrame show a visual frame around the chooser
     * @li @p ShowDifferences display the font differences interfaces
     */
    enum DisplayFlag { NoDisplayFlags = 0,
                       FixedFontsOnly = 1,
                       DisplayFrame = 2,
                       ShowDifferences = 4
                     };
    Q_DECLARE_FLAGS(DisplayFlags, DisplayFlag)

    /**
     * Constructs a font picker widget.
     * It normally comes up with all font families present on the system; the
     * getFont method below does allow some more fine-tuning of the selection of fonts
     * that will be displayed in the dialog.
     * <p>Consider the following code snippet;
     * \code
     *    QStringList list;
     *    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
     *    KFontChooser *chooseFont = new KFontChooser(0, NoDisplayFlags, list);
     * \endcode
     * <p>
     * The above creates a font chooser dialog with only SmoothScaleble fonts.
     *
     * @param parent The parent widget.
     * @param flags Defines how the font chooser is displayed. @see DisplayFlags
     * @param fontList A list of fonts to display, in XLFD format.  If
     *        no list is formatted, the internal KDE font list is used.
     *        If that has not been created, X is queried, and all fonts
     *        available on the system are displayed.
     * @param visibleListSize The minimum number of visible entries in the
     *        fontlists.
     * @param sizeIsRelativeState If not zero the widget will show a
     *        checkbox where the user may choose whether the font size
     *        is to be interpreted as relative size.
     *        Initial state of this checkbox will be set according to
     *        *sizeIsRelativeState, user choice may be retrieved by
     *        calling sizeIsRelative().
     */
    explicit KFontChooser(QWidget *parent = 0L,
                          const DisplayFlags &flags = DisplayFrame,
                          const QStringList &fontList = QStringList(),
                          int visibleListSize = 8,
                          Qt::CheckState *sizeIsRelativeState = 0L);

    /**
     * Destructs the font chooser.
     */
    virtual ~KFontChooser();

    /**
     * Enables or disable a font column in the chooser.
     *
     * Use this
     * function if your application does not need or supports all font
     * properties.
     *
     * @param column Specify the columns. An or'ed combination of
     *        @p FamilyList, @p StyleList and @p SizeList is possible.
     * @param state If @p false the columns are disabled.
     */
    void enableColumn(int column, bool state);

    /**
     * Sets the currently selected font in the chooser.
     *
     * @param font The font to select.
     * @param onlyFixed Readjust the font list to display only fixed
     *        width fonts if @p true, or vice-versa.
     */
    void setFont(const QFont &font, bool onlyFixed = false);

    /**
     * @return The bitmask corresponding to the attributes the user
     *         wishes to change.
     */
    FontDiffFlags fontDiffFlags() const;

    /**
     * @return The currently selected font in the chooser.
     */
    QFont font() const;

    /**
     * Sets the color to use in the preview.
     */
    void setColor(const QColor &col);

    /**
     * @return The color currently used in the preview (default: the text
     *         color of the active color group)
     */
    QColor color() const;

    /**
     * Sets the background color to use in the preview.
     */
    void setBackgroundColor(const QColor &col);

    /**
     * @return The background color currently used in the preview (default:
     *         the base color of the active colorgroup)
     */
    QColor backgroundColor() const;

    /**
     * Sets the state of the checkbox indicating whether the font size
     * is to be interpreted as relative size.
     * NOTE: If parameter sizeIsRelative was not set in the constructor
     *       of the widget this setting will be ignored.
     */
    void setSizeIsRelative(Qt::CheckState relative);

    /**
     * @return Whether the font size is to be interpreted as relative size
     *         (default: QButton:Off)
     */
    Qt::CheckState sizeIsRelative() const;

    /**
     * @return The current text in the sample text input area.
     */
    QString sampleText() const;

    /**
     * Sets the sample text.
     *
     * Normally you should not change this
     * text, but it can be better to do this if the default text is
     * too large for the edit area when using the default font of your
     * application.
     *
     * @param text The new sample text. The current will be removed.
     */
    void setSampleText(const QString &text);

    /**
     * Shows or hides the sample text box.
     *
     * @param visible Set it to true to show the box, to false to hide it.
     */
    void setSampleBoxVisible(bool visible);

    /**
     * The selection criteria for the font families shown in the dialog.
     *  @li @p FixedWidthFont when included only fixed-width fonts are returned.
     *        The fonts where the width of every character is equal.
     *  @li @p ScalableFont when included only scalable fonts are returned;
     *        certain configurations allow bitmap fonts to remain unscaled and
     *        thus these fonts have limited number of sizes.
     *  @li @p SmoothScalableFont when included only return smooth scalable fonts.
     *        this will return only non-bitmap fonts which are scalable to any size requested.
     *        Setting this option to true will mean the "scalable" flag is irrelavant.
     */
    enum FontListCriteria { FixedWidthFonts = 0x01, ScalableFonts = 0x02, SmoothScalableFonts = 0x04 };

    /**
     * Creates a list of font strings.
     *
     * @param list The list is returned here.
     * @param fontListCriteria should contain all the restrictions for font selection as OR-ed values
     *        @see KFontChooser::FontListCriteria for the individual values
     */
    static void getFontList(QStringList &list, uint fontListCriteria);

    /**
     * Reimplemented for internal reasons.
     */
    QSize sizeHint(void) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    /**
     * Emitted whenever the selected font changes.
     */
    void fontSelected(const QFont &font);

private:
    class Private;
    Private *const d;

    Q_DISABLE_COPY(KFontChooser)

    Q_PRIVATE_SLOT(d, void _k_toggled_checkbox())
    Q_PRIVATE_SLOT(d, void _k_family_chosen_slot(const QString &))
    Q_PRIVATE_SLOT(d, void _k_size_chosen_slot(const QString &))
    Q_PRIVATE_SLOT(d, void _k_style_chosen_slot(const QString &))
    Q_PRIVATE_SLOT(d, void _k_displaySample(const QFont &font))
    Q_PRIVATE_SLOT(d, void _k_size_value_slot(double))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KFontChooser::DisplayFlags)

#endif
