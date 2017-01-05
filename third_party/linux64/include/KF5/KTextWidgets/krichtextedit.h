/**
 * krichtextedit.h
 *
 * Copyright 2007 Laurent Montel <montel@kde.org>
 * Copyright 2008 Thomas McGuire <thomas.mcguire@gmx.net>
 * Copyright 2008 Stephen Kelly  <steveire@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef KRICHTEXTEDIT_H
#define KRICHTEXTEDIT_H

#include <ktextedit.h>

class QKeyEvent;

class KRichTextEditPrivate;

#include <ktextwidgets_export.h>

/**
 * The KRichTextEdit class provides a widget to edit and display rich text.
 *
 * It offers several additional rich text editing functions to KTextEdit and makes
 * them easier to access including:
 *
 * @li Changing fonts, sizes.
 * @li Font formatting, such as bold, underline, italic, foreground and
 *     background color.
 * @li Paragraph alignment
 * @li Ability to edit and remove hyperlinks
 * @li Nested list handling
 * @li Simple actions to insert tables. TODO
 *
 * The KRichTextEdit can be in two modes: Rich text mode and plain text mode.
 * Calling functions which modify the format/style of the text will automatically
 * enable the rich text mode. Rich text mode is sometimes also referred to as
 * HTML mode.
 *
 * Do not call setAcceptRichText() or acceptRichText() yourself. Instead simply
 * connect to the slots which insert the rich text, use switchToPlainText() or
 * enableRichTextMode().
 *
 * \image html krichtextedit.png "KDE Rich Text Edit Widget"
 *
 * @since 4.1
 */
class KTEXTWIDGETS_EXPORT KRichTextEdit : public KTextEdit
{
    Q_OBJECT

public:

    /**
     * The mode the edit widget is in.
     */
    enum Mode { Plain,    ///< Plain text mode
                Rich      ///< Rich text mode
              };

    /**
     * Constructs a KRichTextEdit object
     *
     * @param text The initial text of the text edit, which is interpreted as
     *             HTML.
     * @param parent The parent widget
     */
    explicit KRichTextEdit(const QString &text, QWidget *parent = 0);

    /**
     * Constructs a KRichTextEdit object.
     *
     * @param parent The parent widget
     */
    explicit KRichTextEdit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~KRichTextEdit();

    /**
     * This enables rich text mode. Nothing is done except changing the internal
     * mode and allowing rich text pastes.
     */
    void enableRichTextMode();

    /**
     * @return The current text mode
     */
    Mode textMode() const;

    /**
     * @return The plain text string if in plain text mode or the HTML code
     *         if in rich text mode. The text is not word-wrapped.
     */
    QString textOrHtml() const;

    /**
     * Replaces all the content of the text edit with the given string.
     * If the string is in rich text format, the text is inserted as rich text,
     * otherwise it is inserted as plain text.
     *
     * @param text The text to insert
     */
    void setTextOrHtml(const QString &text);

    /**
     * Returns the text of the link at the current position or an empty string
     * if the cursor is not on a link.
     *
     * @sa currentLinkUrl
     * @return The link text
     */
    QString currentLinkText() const;

    /**
     * Returns the URL target (href) of the link at the current position or an
     * empty string if the cursor is not on a link.
     *
     * @sa currentLinkText
     * @return The link target URL
     */
    QString currentLinkUrl() const;

    /**
     * If the cursor is on a link, sets the @a cursor to a selection of the
     * text of the link. If the @a cursor is not on a link, selects the current word
     * or existing selection.
     *
     * @param cursor The cursor to use to select the text.
     * @sa updateLink
     */
    void selectLinkText(QTextCursor *cursor) const;

    /**
     * Convenience function to select the link text using the active cursor.
     *
     * @sa selectLinkText
     */
    void selectLinkText() const;

    /**
     * Replaces the current selection with a hyperlink with the link URL @a linkUrl
     * and the link text @a linkText.
     *
     * @sa selectLinkText
     * @sa currentLinkUrl
     * @sa currentLinkText
     * @param linkUrl The link will get this URL as href (target)
     * @param linkText The link will get this alternative text, which is the
     *                 text displayed in the text edit.
     */
    void updateLink(const QString &linkUrl, const QString &linkText);

    /**
     * Returns true if the list item at the current position can be indented.
     *
     * @sa canDedentList
     */
    bool canIndentList() const;

    /**
     * Returns true if the list item at the current position can be dedented.
     *
     * @sa canIndentList
     */
    bool canDedentList() const;

public Q_SLOTS:

    /**
     * Sets the alignment of the current block to Left Aligned
     */
    void alignLeft();

    /**
     * Sets the alignment of the current block to Centered
     */
    void alignCenter();

    /**
     * Sets the alignment of the current block to Right Aligned
     */
    void alignRight();

    /**
     * Sets the alignment of the current block to Justified
     */
    void alignJustify();

    /**
     * Sets the direction of the current block to Right-To-Left
     *
     * @since 4.6
     */
    void makeRightToLeft();

    /**
     * Sets the direction of the current block to Left-To-Right
     *
     * @since 4.6
     */
    void makeLeftToRight();

    /**
     * Sets the list style of the current list, or creates a new list using the
     * current block. The @a _styleindex corresponds to the QTextListFormat::Style
     *
     * @param _styleIndex The list will get this style
     */
    void setListStyle(int _styleIndex);

    /**
     * Increases the nesting level of the current block or selected blocks.
     *
     * @sa canIndentList
     */
    void indentListMore();

    /**
     * Decreases the nesting level of the current block or selected blocks.
     *
     * @sa canDedentList
     */
    void indentListLess();

    /**
     * Sets the current word or selection to the font family @a fontFamily
     *
     * @param fontFamily The text's font family will be changed to this one
     */
    void setFontFamily(const QString &fontFamily);

    /**
     * Sets the current word or selection to the font size @a size
     *
     * @param size The text's font will get this size
     */
    void setFontSize(int size);

    /**
     * Sets the current word or selection to the font @a font
     *
     * @param font the font of the text will be set to this font
     */
    void setFont(const QFont &font);

    /**
     * Toggles the bold formatting of the current word or selection at the current
     * cursor position.
     *
     * @param bold If true, the text will be set to bold
     */
    void setTextBold(bool bold);

    /**
     * Toggles the italic formatting of the current word or selection at the current
     * cursor position.
     *
     * @param italic If true, the text will be set to italic
     */
    void setTextItalic(bool italic);

    /**
     * Toggles the underline formatting of the current word or selection at the current
     * cursor position.
     *
     * @param underline If true, the text will be underlined
     */
    void setTextUnderline(bool underline);

    /**
     * Toggles the strikeout formatting of the current word or selection at the current
     * cursor position.
     *
     * @param strikeOut If true, the text will be struck out
     */
    void setTextStrikeOut(bool strikeOut);

    /**
     * Sets the foreground color of the current word or selection to @a color.
     *
     * @param color The text will get this background color
     */
    void setTextForegroundColor(const QColor &color);

    /**
     * Sets the background color of the current word or selection to @a color.
     *
     * @param color The text will get this foreground color
     */
    void setTextBackgroundColor(const QColor &color);

    /**
     * Inserts a horizontal rule below the current block.
     */
    void insertHorizontalRule();

    /**
     * This will switch the editor to plain text mode.
     * All rich text formatting will be destroyed.
     */
    void switchToPlainText();

    /**
     * This will clean some of the bad html produced by the underlying QTextEdit
     * It walks over all lines and cleans up a bit. Should be improved to produce
     * our own Html.
     */
    QString toCleanHtml() const;

    /**
     * Toggles the superscript formatting of the current word or selection at the current
     * cursor position.
     *
     * @param superscript If true, the text will be set to superscript
     */
    void setTextSuperScript(bool superscript);

    /**
     * Toggles the subscript formatting of the current word or selection at the current
     * cursor position.
     *
     * @param subscript If true, the text will be set to subscript
     */
    void setTextSubScript(bool subscript);

    /**
     * @since 4.10
     * Because of binary compatibility constraints, insertPlainText
     * is not virtual. Therefore it must dynamically detect and call this slot.
     */
    void insertPlainTextImplementation();

Q_SIGNALS:

    /**
     * Emitted whenever the text mode is changed.
     *
     * @param mode The new text mode
     */
    void textModeChanged(KRichTextEdit::Mode mode);

    /**
     * Emitted whenever the user has finished making a selection. (on mouse up)
     */
    void selectionFinished();

protected:

    /**
     * Reimplemented.
     * Catches key press events. Used to handle some key presses on lists.
     */
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    //@cond PRIVATE
    KRichTextEditPrivate *const d;
    friend class KRichTextEditPrivate;
    //@endcond
};

#endif
