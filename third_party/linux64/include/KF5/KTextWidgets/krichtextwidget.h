/* This file is part of the KDE libraries

   Copyright 2008 Stephen Kelly <steveire@gmail.com>
   Copyright 2008 Thomas McGuire <thomas.mcguire@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LGPL-2.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef KRICHTEXTWIDGET_H
#define KRICHTEXTWIDGET_H

#include "krichtextedit.h"

#include "ktextwidgets_export.h"

class QAction;

/**
 * @brief A KRichTextEdit with common actions
 *
 * This class implements common actions which are often used with KRichTextEdit.
 * All you need to do is to call createActions(), and the actions will be
 * added to your KXMLGUIWindow. Remember to also add the chosen actions to
 * your application ui.rc file.
 *
 * See the KRichTextWidget::RichTextSupportValues enum for an overview of
 * supported actions.
 *
 * @author Stephen Kelly <steveire@gmail.com>
 * @author Thomas McGuire <thomas.mcguire@gmx.net>
 *
 * \image html krichtextedit.png "KDE Rich Text Widget"
 *
 * @since 4.1
 */
class KTEXTWIDGETS_EXPORT KRichTextWidget : public KRichTextEdit
{
    Q_OBJECT
    Q_FLAGS(RichTextSupport)
    Q_PROPERTY(RichTextSupport richTextSupport READ richTextSupport WRITE setRichTextSupport)
public:

    /**
     * These flags describe what actions will be created by createActions() after
     * passing a combination of these flags to setRichTextSupport().
     */
    enum RichTextSupportValues {
        /**
         * No rich text support at all, no actions will be created. Do not use
         * in combination with other flags.
         */
        DisableRichText = 0x00,

        /**
         * Action to format the selected text as bold. If no text is selected,
         * the word under the cursor is formatted bold.
         * This is a KToggleAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportBold = 0x01,

        /**
         * Action to format the selected text as italic. If no text is selected,
         * the word under the cursor is formatted italic.
         * This is a KToggleAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportItalic = 0x02,

        /**
         * Action to underline the selected text. If no text is selected,
         * the word under the cursor is underlined.
         * This is a KToggleAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportUnderline = 0x04,

        /**
         * Action to strike out the selected text. If no text is selected,
         * the word under the cursor is struck out.
         * This is a KToggleAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportStrikeOut = 0x08,

        /**
         * Action to change the font family of the currently selected text. If
         * no text is selected, the font family of the word under the cursor is
         * changed.
         * Displayed as a combobox when inserted into the toolbar.
         * This is a KFontAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportFontFamily = 0x10,

        /**
         * Action to change the font size of the currently selected text. If no
         * text is selected, the font size of the word under the cursor is changed.
         * Displayed as a combobox when inserted into the toolbar.
         * This is a KFontSizeAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportFontSize = 0x20,

        /**
         * Action to change the text color of the currently selected text. If no
         * text is selected, the text color of the word under the cursor is
         * changed.
         * Opens a QColorDialog to select the color.
         */
        SupportTextForegroundColor = 0x40,

        /**
         * Action to change the background color of the currently selected text. If no
         * text is selected, the backgound color of the word under the cursor is
         * changed.
         * Opens a QColorDialog to select the color.
         */
        SupportTextBackgroundColor = 0x80,

        /**
         * A combination of all the flags above.
         * Includes all actions that change the format of the text.
         */
        FullTextFormattingSupport = 0xff,

        /**
         * Action to make the current line a list element, change the
         * list style or remove list formatting.
         * Displayed as a combobox when inserted into a toolbar.
         * This is a KSelectAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportChangeListStyle = 0x100,

        /**
         * Action to increase the current list nesting level. This makes it
         * possible to create nested lists.
         */
        SupportIndentLists = 0x200,

        /**
         * Action to decrease the current list nesting level.
         */
        SupportDedentLists = 0x400,

        /**
         * All of the three list actions above.
         * Includes all list-related actions.
         */
        FullListSupport = 0xf00,

// Not implemented yet.
//         SupportCreateTables = 0x1000,
//         SupportChangeCellMargin = 0x2000,
//         SupportChangeCellPadding = 0x4000,
//         SupportChangeTableBorderWidth = 0x8000,
//         SupportChangeTableBorderColor = 0x10000,
//         SupportChangeTableBorderStyle = 0x20000,
//         SupportChangeCellBackground = 0x40000,
//         SupportCellFillPatterns = 0x80000,
//
//         FullTableSupport = 0xff000,

        /**
         * Actions to align the current paragraph left, righ, center or justify.
         * These actions are KToogleActions. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportAlignment = 0x100000,

        // Not yet implemented SupportImages = 0x200000,

        /**
         * Action to insert a horizontal line.
         */
        SupportRuleLine = 0x400000,

        /**
         * Action to convert the current text to a hyperlink. If no text is selected,
         * the word under the cursor is converted.
         * This action opens a dialog where the user can enter the link target.
         */
        SupportHyperlinks = 0x800000,

        /**
         * Action to make the mouse cursor a format painter. The user can select
         * text with that painter. The selected text gets the same format as the
         * text that was previously selected.
         */
        SupportFormatPainting = 0x1000000,

        /**
         * Action to change the text of the whole text edit to plain text.
         * All rich text formatting will get lost.
         */
        SupportToPlainText = 0x2000000,

        /**
         * Actions to format text as superscript or subscript. If no text is selected,
         * the word under the cursor is formatted as selected.
         * This is a KToggleAction. The status is automatically updated when
         * the text cursor is moved.
         */
        SupportSuperScriptAndSubScript = 0x4000000,

//         SupportChangeParagraphSpacing = 0x200000,

        /**
         * Action to change direction of text to Right-To-Left or Left-To-Right.
         */
        SupportDirection = 0x8000000,

        /**
         * Includes all above actions for full rich text support
         */
        FullSupport = 0xffffffff
    };
    Q_DECLARE_FLAGS(RichTextSupport, RichTextSupportValues)

    /**
     * @brief Constructor
     * @param parent the parent widget
     */
    explicit KRichTextWidget(QWidget *parent);

    /**
     * Constructs a KRichTextWidget object
     *
     * @param text The initial text of the text edit, which is interpreted as
     *             HTML.
     * @param parent The parent widget
     */
    explicit KRichTextWidget(const QString &text, QWidget *parent = 0);

    /**
     * @brief Destructor
     */
    ~KRichTextWidget();

    /**
     * @brief Creates the actions and adds them to the given action collection.
     *
     * Call this before calling setupGUI() in your application, but after
     * calling setRichTextSupport().
     *
     * The XML file of your KXmlGuiWindow needs to have the action names in
     * them, so that the actions actually appear in the menu and in the toolbars.
     *
     * Below is a list of actions that are created,depending on the supported rich text
     * subset set by setRichTextSupport(). The list contains action names.
     * Those names need to be the same in your XML file.
     *
     * See the KRichTextWidget::RichTextSupportValues enum documentation for a
     * detailed explaination of each action.
     *
     * <table>
     * <tr><td><b>XML Name</b></td><td><b>RichTextSupportValues flag</b></td></tr>
     * <tr><td>format_text_foreground_color</td><td>SupportTextForegroundColor</td></tr>
     * <tr><td>format_text_background_color</td><td>SupportTextBackgroundColor</td></tr>
     * <tr><td>format_font_family</td><td>SupportFontFamily</td></tr>
     * <tr><td>format_font_size</td><td>SupportFontSize</td></tr>
     * <tr><td>format_text_bold</td><td>SupportBold</td></tr>
     * <tr><td>format_text_italic</td><td>SupportItalic</td></tr>
     * <tr><td>format_text_underline</td><td>SupportUnderline</td></tr>
     * <tr><td>format_text_strikeout</td><td>SupportStrikeOut</td></tr>
     * <tr><td>format_align_left</td><td>SupportAlignment</td></tr>
     * <tr><td>format_align_center</td><td>SupportAlignment</td></tr>
     * <tr><td>format_align_right</td><td>SupportAlignment</td></tr>
     * <tr><td>format_align_justify</td><td>SupportAlignment</td></tr>
     * <tr><td>direction_ltr</td><td>SupportDirection</td></tr>
     * <tr><td>direction_rtl</td><td>SupportDirection</td></tr>
     * <tr><td>format_list_style</td><td>SupportChangeListStyle</td></tr>
     * <tr><td>format_list_indent_more</td><td>SupportIndentLists</td></tr>
     * <tr><td>format_list_indent_less</td><td>SupportDedentLists</td></tr>
     * <tr><td>insert_horizontal_rule</td><td>SupportRuleLine</td></tr>
     * <tr><td>manage_link</td><td>SupportHyperlinks</td></tr>
     * <tr><td>format_painter</td><td>SupportFormatPainting</td></tr>
     * <tr><td>action_to_plain_text</td><td>SupportToPlainText</td></tr>
     * <tr><td>format_text_subscript & format_text_superscript</td><td>SupportSuperScriptAndSubScript</td></tr>
     * </table>
     *
     * @since 5.0
     */
    virtual QList<QAction *> createActions();

    /**
     * @brief Sets the supported rich text subset available.
     *
     * The default is KRichTextWidget::FullSupport and will be set in the
     * constructor.
     *
     * You need to call createActions() afterwards.
     *
     * @param support The supported subset.
     */
    void setRichTextSupport(const KRichTextWidget::RichTextSupport &support);

    /**
     * @brief Returns the supported rich text subset available.
     * @return The supported subset.
     */
    RichTextSupport richTextSupport() const;

    /**
     * Tells KRichTextWidget to update the state of the actions created by
     * createActions().
     * This is normally automatically done, but there might be a few cases where
     * you'll need to manually call this function.
     *
     * Call this function only after calling createActions().
     */
    void updateActionStates();

public Q_SLOTS:

    /**
     * Disables or enables all of the actions created by
     * createActions().
     * This may be useful in cases where rich text mode may be set on or off.
     *
     * @param enabled Whether to enable or disable the actions.
     */
    void setActionsEnabled(bool enabled);

protected:
    /**
     * Reimplemented.
     * Catches mouse release events. Used to know when a selection has been completed.
     */
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    //@cond PRIVATE
    class Private;
    friend class Private;
    Private *const d;
    Q_PRIVATE_SLOT(d, void _k_setTextForegroundColor())
    Q_PRIVATE_SLOT(d, void _k_setTextBackgroundColor())
    Q_PRIVATE_SLOT(d, void _k_manageLink())
    Q_PRIVATE_SLOT(d, void _k_formatPainter(bool))
    Q_PRIVATE_SLOT(d, void _k_updateCharFormatActions(const QTextCharFormat &))
    Q_PRIVATE_SLOT(d, void _k_updateMiscActions())
    Q_PRIVATE_SLOT(d, void _k_setListStyle(int))
    //@endcond
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KRichTextWidget::RichTextSupport)

#endif

