/* This file is part of the KDE libraries

   Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kcharselect_h
#define kcharselect_h

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QWidget>
#include <kwidgetsaddons_export.h>

class QFont;
class QUrl;

/**
 * @short Character selection widget
 *
 * This widget allows the user to select a character of a
 * specified font and to browse Unicode information
 *
 * \image html kcharselect.png "Character Selection Widget"
 *
 * You can specify the font whose characters should be displayed via
 * setCurrentFont(). Using the Controls argument in the contructor
 * you can create a compact version of KCharSelect if there is not enough
 * space and if you don't need all features.
 *
 * KCharSelect displays one Unicode block at a time and provides
 * categorized access to them. Unicode character names and further details,
 * including cross references, are displayed. Additionally, there is a search
 * to find characters.
 *
 * By default, KCharSelect is restricted to Basic Multilingual Plane (BMP)
 * characters that QChar supports, i.e. characters with code points that
 * fit into a quint16 (U+0000..U+FFFF). API methods that have a QChar
 * argument can only be used for this default mode:
 *
 * To get the current selected character, use the currentChar()
 * method. You can set the character which should be displayed with
 * setCurrentChar().
 *
 * If you want the user to select and search characters from all planes,
 * i.e. characters U+0000..U+10FFFF, use setAllPlanesEnabled(true)
 * and use the @c uint based methods currentCodePoint() and
 * setCurrentCodePoint() instead.
 *
 * Since QString does not allow @c uint code points, you either must
 * use QString::fromUcs4() and QString::ToUcs4() to convert between
 * strings and code points, or manually do the surrogate pair handling
 * using QChar::requiresSurrogates() and friends.
 *
 * @author Reginald Stadlbauer <reggie@kde.org>
 * @author Daniel Laidig <d.laidig@gmx.de>
 */

class KWIDGETSADDONS_EXPORT KCharSelect : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFont currentFont READ currentFont WRITE setCurrentFont)
    Q_PROPERTY(QChar currentChar READ currentChar WRITE setCurrentChar)
    Q_PROPERTY(uint currentCodePoint READ currentCodePoint WRITE setCurrentCodePoint NOTIFY currentCodePointChanged)
    Q_PROPERTY(QList<QChar> displayedChars READ displayedChars)
    Q_PROPERTY(QVector<uint> displayedCodePoints READ displayedCodePoints)
    Q_PROPERTY(bool allPlanesEnabled READ allPlanesEnabled WRITE setAllPlanesEnabled DESIGNABLE true)

public:
    /**
     * Flags to set the shown widgets
     */
    enum Control {
        /**
         * Shows the search widgets
         */
        SearchLine = 0x01,
        /**
         * Shows the font combo box
         */
        FontCombo = 0x02,
        /**
         * Shows the font size spin box
         */
        FontSize = 0x04,
        /**
         * Shows the category/block selection combo boxes
         */
        BlockCombos = 0x08,
        /**
         * Shows the actual table
         */
        CharacterTable = 0x10,
        /**
         * Shows the detail browser
         */
        DetailBrowser = 0x20,
        /**
         * Shows the Back/Forward buttons
         */
        HistoryButtons = 0x40,
        /**
         * Shows everything
         */
        AllGuiElements      = 65535
    };
    Q_DECLARE_FLAGS(Controls,
                    Control)

    /**
     * Constructor. @p controls can be used to show a custom set of widgets.
     *
     * @param parent     the parent widget for this KCharSelect (see QWidget documentation)
     * @param controls   selects the visible controls on the KCharSelect widget
     *
     * @since 4.2
     */
    explicit KCharSelect(
        QWidget *parent,
        const Controls controls = AllGuiElements);

    /**
     * Constructor. @p controls can be used to show a custom set of widgets.
     *
     * The widget uses the following actions:
     *   - KStandardActions::find() (edit_find)
     *   - KStandardActions::back() (go_back)
     *   - KStandardActions::forward() (go_forward)
     *
     * If you provide a KActionCollection, this will be populated with the above actions,
     * which you can then manually trigger or place in menus and toolbars.
     *
     * @param parent     the parent widget for this KCharSelect (see QWidget documentation)
     * @param actionParent if this is not @c null, KCharSelect will place its actions into this
     *                     collection
     * @param controls   selects the visible controls on the KCharSelect widget
     *
     * @since 4.2
     */
    explicit KCharSelect(
        QWidget *parent,
        QObject *actionParent,
        const Controls controls = AllGuiElements);

    ~KCharSelect();

    /**
     * Reimplemented.
     */
    QSize sizeHint() const Q_DECL_OVERRIDE;

    /**
     * Sets the allowed Unicode code planes. If @p all is @c false, then
     * only characters from the Basic Multilingual Plane (BMP) can be
     * selected, otherwise characters from all planes are allowed.
     *
     * For compatibility reasons, the default is @c false.
     *
     * If you enable support for all planes, you must use the functions
     * handling @c uint code points instead of @c QChar characters.
     * @since 5.25
     */
    void setAllPlanesEnabled(bool all);

    /**
     * @returns @c true, if characters from all Unicode code planes
     * can be selected.
     * @since 5.25
     */
    bool allPlanesEnabled() const;

    /**
     * Returns the currently selected character. If characters outside the
     * Basic Multilingual Plane (BMP) can be selected, use currentCodePoint
     * instead.
     * @sa currentCodePoint
     */
    QChar currentChar() const;

    /**
     * Returns the Unicode code point of the currently selected character.
     * @warning If you enabled support for all Unicode planes, you must use
     * QChar::requiresSurrogates() to check if the code point requires
     * conversion to a UTF-16 surrogate pair before converting it to QString.
     * You cannot convert a code point to a QChar.
     * @since 5.25
     */
    uint currentCodePoint() const;

    /**
     * Returns the currently displayed font.
     */
    QFont currentFont() const;

    /**
     * Returns a list of currently displayed characters. If characters outside the
     * Basic Multilingual Plane (BMP) can be selected, use displayedCodePoints
     * instead.
     * Warning: this method can be a bit slow
     * @sa displayedCodePoints
     */
    QList<QChar> displayedChars() const;

    /**
     * Returns a list of Unicode code points of the currently displayed characters.
     * @since 5.25
     */
    QVector<uint> displayedCodePoints() const;

public Q_SLOTS:
    /**
     * Highlights the character @p c. If the character is not displayed, the block is changed.
     *
     * @param c the character to highlight
     */
    void setCurrentChar(const QChar &c);

    /**
     * Highlights the character with the specified @p codePoint. If the character is
     * outside the Basic Multilingual Plane (BMP), then you must enable support
     * for all planes for this to work.
     *
     * @param codePoint the Unicode code point of the character to highlight
     *
     * @sa allPlanesEnabled
     * @since 5.25
     */
    void setCurrentCodePoint(uint codePoint);

    /**
     * Sets the font which is displayed to @p font
     *
     * @param font the display font for the widget
     */
    void setCurrentFont(const QFont &font);

Q_SIGNALS:
    /**
     * A new font is selected or the font size changed.
     *
     * @param font the new font
     */
    void currentFontChanged(const QFont &font);
    /**
     * The current character is changed.
     *
     * @param c the new character
     */
    void currentCharChanged(const QChar &c);
    /**
     * The current character is changed.
     *
     * @param codePoint the Unicode code point of the new character
     * @since 5.25
     */
    void currentCodePointChanged(uint codePoint);
    /**
     * The currently displayed characters are changed (search results or block).
     */
    void displayedCharsChanged();
    /**
     * A character is selected to be inserted somewhere.
     *
     * @param c the selected character
     */
    void charSelected(const QChar &c);
    /**
     * A character is selected to be inserted somewhere.
     *
     * @param codePoint the Unicode code point of the selected character
     * @since 5.25
     */
    void codePointSelected(uint codePoint);

private:
    Q_PRIVATE_SLOT(d, void _k_activateSearchLine())
    Q_PRIVATE_SLOT(d, void _k_back())
    Q_PRIVATE_SLOT(d, void _k_forward())
    Q_PRIVATE_SLOT(d, void _k_fontSelected())
    Q_PRIVATE_SLOT(d, void _k_charSelected(uint c))
    Q_PRIVATE_SLOT(d, void _k_updateCurrentChar(uint c))
    Q_PRIVATE_SLOT(d, void _k_slotUpdateUnicode(uint c))
    Q_PRIVATE_SLOT(d, void _k_sectionSelected(int index))
    Q_PRIVATE_SLOT(d, void _k_blockSelected(int index))
    Q_PRIVATE_SLOT(d, void _k_searchEditChanged())
    Q_PRIVATE_SLOT(d, void _k_search())
    Q_PRIVATE_SLOT(d, void _k_linkClicked(QUrl))

    class KCharSelectPrivate;
    KCharSelectPrivate *const d;

    void initWidget(const Controls, QObject *);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KCharSelect::Controls)

#endif
