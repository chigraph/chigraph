/*
    Copyright (C) 2001, S.R.Haque <srhaque@iee.org>.
    Copyright (C) 2002, David Faure <david@mandrakesoft.com>
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LGPL-2.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KFIND_H
#define KFIND_H

#include "ktextwidgets_export.h"

#include <QtCore/QRect>
#include <QObject>

class QDialog;

/**
 * @brief A generic implementation of the "find" function.
 *
 * @author S.R.Haque <srhaque@iee.org>, David Faure <faure@kde.org>,
 *         Arend van Beelen jr. <arend@auton.nl>
 *
 * \b Detail:
 *
 * This class includes prompt handling etc. Also provides some
 * static functions which can be used to create custom behavior
 * instead of using the class directly.
 *
 * \b Example:
 *
 * To use the class to implement a complete find feature:
 *
 * In the slot connected to the find action, after using KFindDialog:
 * \code
 *
 *  // This creates a find-next-prompt dialog if needed.
 *  m_find = new KFind(pattern, options, this);
 *
 *  // Connect highlight signal to code which handles highlighting
 *  // of found text.
 *  connect( m_find, SIGNAL( highlight( const QString &, int, int ) ),
 *          this, SLOT( slotHighlight( const QString &, int, int ) ) );
 *  // Connect findNext signal - called when pressing the button in the dialog
 *  connect( m_find, SIGNAL( findNext() ),
 *          this, SLOT( slotFindNext() ) );
 * \endcode
 *
 * If you are using a non-modal find dialog (recommended),
 * you should call right away m_find->closeFindNextDialog().
 *
 *  Then initialize the variables determining the "current position"
 *  (to the cursor, if the option FromCursor is set,
 *   to the beginning of the selection if the option SelectedText is set,
 *   and to the beginning of the document otherwise).
 *  Initialize the "end of search" variables as well (end of doc or end of selection).
 *  Swap begin and end if FindBackwards.
 *  Finally, call slotFindNext();
 *
 * \code
 *  void slotFindNext()
 *  {
 *      KFind::Result res = KFind::NoMatch;
 *      while ( res == KFind::NoMatch && <position not at end> ) {
 *          if ( m_find->needData() )
 *              m_find->setData( <current text fragment> );
 *
 *          // Let KFind inspect the text fragment, and display a dialog if a match is found
 *          res = m_find->find();
 *
 *          if ( res == KFind::NoMatch ) {
 *              <Move to the next text fragment, honoring the FindBackwards setting for the direction>
 *          }
 *      }
 *
 *      if ( res == KFind::NoMatch ) // i.e. at end
 *          <Call either  m_find->displayFinalDialog(); m_find->deleteLater(); m_find = 0L;
 *           or           if ( m_find->shouldRestart() ) { reinit (w/o FromCursor) and call slotFindNext(); }
 *                        else { m_find->closeFindNextDialog(); }>
 *  }
 * \endcode
 *
 *  Don't forget to delete m_find in the destructor of your class,
 *  unless you gave it a parent widget on construction.
 *
 *  This implementation allows to have a "Find Next" action, which resumes the
 *  search, even if the user closed the "Find Next" dialog.
 *
 *  A "Find Previous" action can simply switch temporarily the value of
 *  FindBackwards and call slotFindNext() - and reset the value afterwards.
 */
class KTEXTWIDGETS_EXPORT KFind :
    public QObject
{
    Q_OBJECT

public:

    /// the options
    enum Options {
        WholeWordsOnly = 1,     ///< Match whole words only.
        FromCursor = 2,         ///< Start from current cursor position.
        SelectedText = 4,       ///< Only search selected area.
        CaseSensitive = 8,      ///< Consider case when matching.
        FindBackwards = 16,     ///< Go backwards.
        RegularExpression = 32, ///< Interpret the pattern as a regular expression.
        FindIncremental = 64,   ///< Find incremental.
        // Note that KReplaceDialog uses 256 and 512
        // User extensions can use boolean options above this value.
        MinimumUserOption = 65536 ///< user options start with this bit
    };
    Q_DECLARE_FLAGS(SearchOptions, Options)

    /**
     * Only use this constructor if you don't use KFindDialog, or if
     * you use it as a modal dialog.
     */
    KFind(const QString &pattern, long options, QWidget *parent);

    /**
     * This is the recommended constructor if you also use KFindDialog (non-modal).
     * You should pass the pointer to it here, so that when a message box
     * appears it has the right parent. Don't worry about deletion, KFind
     * will notice if the find dialog is closed.
     */
    KFind(const QString &pattern, long options, QWidget *parent, QWidget *findDialog);
    virtual ~KFind();

    enum Result { NoMatch, Match };

    /**
     * @return true if the application must supply a new text fragment
     * It also means the last call returned "NoMatch". But by storing this here
     * the application doesn't have to store it in a member variable (between
     * calls to slotFindNext()).
     */
    bool needData() const;

    /**
     * Call this when needData returns true, before calling find().
     * @param data the text fragment (line)
     * @param startPos if set, the index at which the search should start.
     * This is only necessary for the very first call to setData usually,
     * for the 'find in selection' feature. A value of -1 (the default value)
     * means "process all the data", i.e. either 0 or data.length()-1 depending
     * on FindBackwards.
     */
    void setData(const QString &data, int startPos = -1);

    /**
     * Call this when needData returns true, before calling find(). The use of
     * ID's is especially useful if you're using the FindIncremental option.
     * @param id the id of the text fragment
     * @param data the text fragment (line)
     * @param startPos if set, the index at which the search should start.
     * This is only necessary for the very first call to setData usually,
     * for the 'find in selection' feature. A value of -1 (the default value)
     * means "process all the data", i.e. either 0 or data.length()-1 depending
     * on FindBackwards.
     */
    void setData(int id, const QString &data, int startPos = -1);

    /**
     * Walk the text fragment (e.g. text-processor line, kspread cell) looking for matches.
     * For each match, emits the highlight() signal and displays the find-again dialog
     * proceeding.
     */
    Result find();

    /**
     * Return the current options.
     *
     * Warning: this is usually the same value as the one passed to the constructor,
     * but options might change _during_ the replace operation:
     * e.g. the "All" button resets the PromptOnReplace flag.
     *
     * @see KFind::Options
     */
    long options() const;

    /**
     * Set new options. Usually this is used for setting or clearing the
     * FindBackwards options.
     *
     * @see KFind::Options
     */
    virtual void setOptions(long options);

    /**
     * @return the pattern we're currently looking for
     */
    QString pattern() const;

    /**
     * Change the pattern we're looking for
     */
    void setPattern(const QString &pattern);

    /**
     * Return the number of matches found (i.e. the number of times
     * the highlight signal was emitted).
     * If 0, can be used in a dialog box to tell the user "no match was found".
     * The final dialog does so already, unless you used setDisplayFinalDialog(false).
     */
    int numMatches() const;

    /**
     * Call this to reset the numMatches count
     * (and the numReplacements count for a KReplace).
     * Can be useful if reusing the same KReplace for different operations,
     * or when restarting from the beginning of the document.
     */
    virtual void resetCounts();

    /**
     * Virtual method, which allows applications to add extra checks for
     * validating a candidate match. It's only necessary to reimplement this
     * if the find dialog extension has been used to provide additional
     * criterias.
     *
     * @param text  The current text fragment
     * @param index The starting index where the candidate match was found
     * @param matchedlength The length of the candidate match
     */
    virtual bool validateMatch(const QString &text,
                               int index,
                               int matchedlength);

    /**
     * Returns true if we should restart the search from scratch.
     * Can ask the user, or return false (if we already searched the whole document).
     *
     * @param forceAsking set to true if the user modified the document during the
     * search. In that case it makes sense to restart the search again.
     *
     * @param showNumMatches set to true if the dialog should show the number of
     * matches. Set to false if the application provides a "find previous" action,
     * in which case the match count will be erroneous when hitting the end,
     * and we could even be hitting the beginning of the document (so not all
     * matches have even been seen).
     */
    virtual bool shouldRestart(bool forceAsking = false, bool showNumMatches = true) const;

    /**
     * Search the given string, and returns whether a match was found. If one is,
     * the length of the string matched is also returned.
     *
     * A performance optimised version of the function is provided for use
     * with regular expressions.
     *
     * @param text The string to search.
     * @param pattern The pattern to look for.
     * @param index The starting index into the string.
     * @param options The options to use.
     * @param matchedlength The length of the string that was matched
     * @return The index at which a match was found, or -1 if no match was found.
     */
    static int find(const QString &text, const QString &pattern, int index, long options, int *matchedlength);

    static int find(const QString &text, const QRegExp &pattern, int index, long options, int *matchedlength);

    /**
     * Displays the final dialog saying "no match was found", if that was the case.
     * Call either this or shouldRestart().
     */
    virtual void displayFinalDialog() const;

    /**
     * Return (or create) the dialog that shows the "find next?" prompt.
     * Usually you don't need to call this.
     * One case where it can be useful, is when the user selects the "Find"
     * menu item while a find operation is under way. In that case, the
     * program may want to call setActiveWindow() on that dialog.
     */
    QDialog *findNextDialog(bool create = false);

    /**
     * Close the "find next?" dialog. The application should do this when
     * the last match was hit. If the application deletes the KFind, then
     * "find previous" won't be possible anymore.
     *
     * IMPORTANT: you should also call this if you are using a non-modal
     * find dialog, to tell KFind not to pop up its own dialog.
     */
    void closeFindNextDialog();

    /**
     * @return the current matching index ( or -1 ).
     * Same as the matchingIndex parameter passed to highlight.
     * You usually don't need to use this, except maybe when updating the current data,
     * so you need to call setData( newData, index() ).
     */
    int index() const;

Q_SIGNALS:

    /**
     * Connect to this signal to implement highlighting of found text during the find
     * operation.
     *
     * If you've set data with setData(id, text), use the signal highlight(id,
     * matchingIndex, matchedLength)
     *
     * WARNING: If you're using the FindIncremental option, the text argument
     * passed by this signal is not necessarily the data last set through
     * setData(), but can also be an earlier set data block.
     *
     * @see setData()
     */
    void highlight(const QString &text, int matchingIndex, int matchedLength);

    /**
     * Connect to this signal to implement highlighting of found text during the find
     * operation.
     *
     * Use this signal if you've set your data with setData(id, text), otherwise
     * use the signal with highlight(text, matchingIndex, matchedLength).
     *
     * WARNING: If you're using the FindIncremental option, the id argument
     * passed by this signal is not necessarily the id of the data last set
     * through setData(), but can also be of an earlier set data block.
     *
     * @see setData()
     */
    void highlight(int id, int matchingIndex, int matchedLength);

    // ## TODO docu
    // findprevious will also emit findNext, after temporarily switching the value
    // of FindBackwards
    void findNext();

    /**
     * Emitted when the options have changed.
     * This can happen e.g. with "Replace All", or if our 'find next' dialog
     * gets a "find previous" one day.
     */
    void optionsChanged();

    /**
     * Emitted when the 'find next' dialog is being closed.
     * Some apps might want to remove the highlighted text when this happens.
     * Apps without support for "Find Next" can also do m_find->deleteLater()
     * to terminate the find operation.
     */
    void dialogClosed();

protected:

    QWidget *parentWidget() const;
    QWidget *dialogsParent() const;

private:
    friend class KReplace;
    friend class KReplacePrivate;

    struct Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_slotFindNext())
    Q_PRIVATE_SLOT(d, void _k_slotDialogClosed())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KFind::SearchOptions)

#endif
