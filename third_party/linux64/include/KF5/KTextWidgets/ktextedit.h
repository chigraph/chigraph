/* This file is part of the KDE libraries
    Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef KTEXTEDIT_H
#define KTEXTEDIT_H

#include "ktextwidgets_export.h"

#include <sonnet/highlighter.h>
#include <QTextEdit>
namespace Sonnet {
class SpellCheckDecorator;
}
/**
 * @short A KDE'ified QTextEdit
 *
 * This is just a little subclass of QTextEdit, implementing
 * some standard KDE features, like cursor auto-hiding, configurable
 * wheelscrolling (fast-scroll or zoom), spell checking and deleting of entire
 * words with Ctrl-Backspace or Ctrl-Delete.
 *
 * This text edit provides two ways of spell checking: background checking,
 * which will mark incorrectly spelled words red, and a spell check dialog,
 * which lets the user check and correct all incorrectly spelled words.
 *
 * Basic rule: whenever you want to use QTextEdit, use KTextEdit!
 *
 * \image html ktextedit.png "KDE Text Edit Widget"
 *
 * @see QTextEdit
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */
class KTEXTWIDGETS_EXPORT KTextEdit : public QTextEdit //krazy:exclude=qclasses
{
    Q_OBJECT
    #ifndef KTEXTWIDGETS_NO_DEPRECATED
    Q_PROPERTY(QString clickMessage READ clickMessage WRITE setClickMessage)
    #endif
    Q_PROPERTY(bool checkSpellingEnabled READ checkSpellingEnabled WRITE setCheckSpellingEnabled)
    Q_PROPERTY(QString spellCheckingLanguage READ spellCheckingLanguage WRITE setSpellCheckingLanguage)

public:
    /**
     * Constructs a KTextEdit object. See QTextEdit::QTextEdit
     * for details.
     */
    explicit KTextEdit(const QString &text, QWidget *parent = 0);

    /**
     * Constructs a KTextEdit object. See QTextEdit::QTextEdit
     * for details.
     */
    explicit KTextEdit(QWidget *parent = 0);

    /**
     * Destroys the KTextEdit object.
     */
    ~KTextEdit();

    /**
     * Reimplemented to set a proper "deactivated" background color.
     */
    virtual void setReadOnly(bool readOnly);

    /**
     * Turns background spell checking for this text edit on or off.
     * Note that spell checking is only available in read-writable KTextEdits.
     *
     * Enabling spell checking will set back the current highlighter to the one
     * returned by createHighlighter().
     *
     * @see checkSpellingEnabled()
     * @see isReadOnly()
     * @see setReadOnly()
     */
    virtual void setCheckSpellingEnabled(bool check);

    /**
     * Returns true if background spell checking is enabled for this text edit.
     * Note that it even returns true if this is a read-only KTextEdit,
     * where spell checking is actually disabled.
     * By default spell checking is disabled.
     *
     * @see setCheckSpellingEnabled()
     */
    virtual bool checkSpellingEnabled() const;

    /**
     * Returns true if the given paragraph or block should be spellcheck.
     * For example, a mail client does not want to check quoted text, and
     * would return false here (by checking whether the block starts with a
     * quote sign).
     *
     * Always returns true by default.
     *
     */
    virtual bool shouldBlockBeSpellChecked(const QString &block) const;

    /**
     * Selects the characters at the specified position. Any previous
     * selection will be lost. The cursor is moved to the first character
     * of the new selection.
     *
     * @param length The length of the selection, in number of characters
     * @param pos The position of the first character of the selection
     */
    void highlightWord(int length, int pos);

    /**
     * Allows to create a specific highlighter if reimplemented.
     *
     * By default, it creates a normal highlighter, based on the config
     * file given to setSpellCheckingConfigFileName().
     *
     * This highlighter is set each time spell checking is toggled on by
     * calling setCheckSpellingEnabled(), but can later be overridden by calling
     * setHighlighter().
     *
     * @see setHighlighter()
     * @see highlighter()
     * @see setSpellCheckingConfigFileName()
     */
    virtual void createHighlighter();

    /**
     * Returns the current highlighter, which is 0 if spell checking is disabled.
     * The default highlighter is the one created by createHighlighter(), but
     * might be overridden by setHighlighter().
     *
     * @see setHighlighter()
     * @see createHighlighter()
     */
    Sonnet::Highlighter *highlighter() const;

    /**
     * Sets a custom backgound spell highlighter for this text edit.
     * Normally, the highlighter returned by createHighlighter() will be
     * used to detect and highlight incorrectly spelled words, but this
     * function allows to set a custom highlighter.
     *
     * This has to be called after enabling spell checking with
     * setCheckSpellingEnabled(), otherwise it has no effect.
     *
     * Ownership is transferred to the KTextEdit
     *
     * @see highlighter()
     * @see createHighlighter()
     * @param highLighter the new highlighter which will be used now
     */
    void setHighlighter(Sonnet::Highlighter *_highLighter);

    /**
     * Return standard KTextEdit popupMenu
     * @since 4.1
     */
    virtual QMenu *mousePopupMenu();

    /**
     * Enable find replace action.
     * @since 4.1
     */
    void enableFindReplace(bool enabled);

    /**
     * @return the spell checking language which was set by
     *         setSpellCheckingLanguage(), the spellcheck dialog or the spellcheck
     *         config dialog, or an empty string if that has never been called.
     * @since 4.2
     */
    const QString &spellCheckingLanguage() const;

    /**
     * This makes the text edit display a grayed-out hinting text as long as
     * the user didn't enter any text. It is often used as indication about
     * the purpose of the text edit.
     * @deprecated since 5.0, use QTextEdit::setPlaceholderText instead
     */
#ifndef KTEXTWIDGETS_NO_DEPRECATED
    inline KTEXTWIDGETS_DEPRECATED void setClickMessage(const QString &msg) {setPlaceholderText(msg);}
#endif

    /**
     * @return the message set with setClickMessage
     * @deprecated since 5.0, use QTextEdit::placeholderText instead
     */
#ifndef KTEXTWIDGETS_NO_DEPRECATED
    inline KTEXTWIDGETS_DEPRECATED QString clickMessage() const {return placeholderText();}
#endif

    /**
     * @since 4.10
     */
    void showTabAction(bool show);

    /**
     * @since 4.10
     */
    void showAutoCorrectButton(bool show);

    /**
     * @since 4.10
     * create a modal spellcheck dialogbox and spellCheckingFinished signal we sent when
     * we finish spell checking or spellCheckingCanceled signal when we cancel spell checking
     */
    void forceSpellChecking();

Q_SIGNALS:
    /**
     * emit signal when we activate or not autospellchecking
     *
     * @since 4.1
     */
    void checkSpellingChanged(bool);

    /**
     * Signal sends when spell checking is finished/stopped/completed
     * @since 4.1
     */
    void spellCheckStatus(const QString &);

    /**
     * Emitted when the user changes the language in the spellcheck dialog
     * shown by checkSpelling() or when calling setSpellCheckingLanguage().
     *
     * @param language the new language the user selected
     * @since 4.1
     */
    void languageChanged(const QString &language);

    /**
     * Emitted before the context menu is displayed.
     *
     * The signal allows you to add your own entries into the
     * the context menu that is created on demand.
     *
     * NOTE: Do not store the pointer to the QMenu
     * provided through since it is created and deleted
     * on demand.
     *
     * @param p the context menu about to be displayed
     * @since 4.5
     */
    void aboutToShowContextMenu(QMenu *menu);

    /**
     * @since 4.10
     */
    void spellCheckerAutoCorrect(const QString &currentWord, const QString &autoCorrectWord);

    /**
     * signal spellCheckingFinished is sent when we finish spell check or we click on "Terminate" button in sonnet dialogbox
     * @since 4.10
     */
    void spellCheckingFinished();

    /**
     * signal spellCheckingCanceled is sent when we cancel spell checking.
     * @since 4.10
     */
    void spellCheckingCanceled();

public Q_SLOTS:

    /**
     * Set the spell check language which will be used for highlighting spelling
     * mistakes and for the spellcheck dialog.
     * The languageChanged() signal will be emitted when the new language is
     * different from the old one.
     *
     * @since 4.1
     */
    void setSpellCheckingLanguage(const QString &language);

    /**
     * Show a dialog to check the spelling. The spellCheckStatus() signal
     * will be emitted when the spell checking dialog is closed.
     */
    void checkSpelling();

    /**
     * Opens a Sonnet::ConfigDialog for this text edit.
     * The spellcheck language of the config dialog is set to the current spellcheck
     * language of the textedit. If the user changes the language in that dialog,
     * the languageChanged() signal is emitted.
     *
     * @param configFileName The file which is used to store and load the config
     *                       settings
     * @param windowIcon the icon which is used for the titlebar of the spell dialog
     *                   window. Can be empty, then no icon is set.
     *
     * @since 4.2
     */
    void showSpellConfigDialog(const QString &windowIcon = QString());

    /**
     * Create replace dialogbox
     * @since 4.1
     */
    void replace();

    /**
     * Add custom spell checker decorator
     * @since 5.11
     */
    void addTextDecorator(Sonnet::SpellCheckDecorator *decorator);

    /**
     * @brief clearDecorator clear the spellcheckerdecorator
     * @since 5.11
     */
    void clearDecorator();

protected Q_SLOTS:
    /**
     * @since 4.1
     */
    void slotDoReplace();
    void slotReplaceNext();
    void slotDoFind();
    void slotFind();
    void slotFindNext();
    /**
     * @since 5.11
     */
    void slotFindPrevious();
    void slotReplace();
    /**
     * @since 4.3
     */
    void slotSpeakText();

protected:
    /**
     * Reimplemented to catch "delete word" shortcut events.
     */
    bool event(QEvent *) Q_DECL_OVERRIDE;

    /**
     * Reimplemented for internal reasons
     */
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

    /**
     * Reimplemented to instantiate a KDictSpellingHighlighter, if
     * spellchecking is enabled.
     */
    void focusInEvent(QFocusEvent *) Q_DECL_OVERRIDE;

    /**
     * Deletes a word backwards from the current cursor position,
     * if available.
     */
    virtual void deleteWordBack();

    /**
     * Deletes a word forwards from the current cursor position,
     * if available.
     */
    virtual void deleteWordForward();

    /**
     * Reimplemented from QTextEdit to add spelling related items
     * when appropriate.
     */
    void contextMenuEvent(QContextMenuEvent *) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void spellCheckerMisspelling(const QString &, int))
    Q_PRIVATE_SLOT(d, void spellCheckerCorrected(const QString &, int, const QString &))
    Q_PRIVATE_SLOT(d, void spellCheckerCanceled())
    Q_PRIVATE_SLOT(d, void spellCheckerAutoCorrect(const QString &, const QString &))
    Q_PRIVATE_SLOT(d, void spellCheckerFinished())
    Q_PRIVATE_SLOT(d, void undoableClear())
    Q_PRIVATE_SLOT(d, void toggleAutoSpellCheck())
    Q_PRIVATE_SLOT(d, void slotAllowTab())
    Q_PRIVATE_SLOT(d, void menuActivated(QAction *))
    Q_PRIVATE_SLOT(d, void slotFindHighlight(const QString &, int, int))
    Q_PRIVATE_SLOT(d, void slotReplaceText(const QString &, int, int, int))
};

#endif // KTEXTEDIT_H
