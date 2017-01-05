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

#ifndef KFINDDIALOG_H
#define KFINDDIALOG_H

#include "ktextwidgets_export.h"

#include <QDialog>

/**
 * @brief A generic "find" dialog.
 *
 * @author S.R.Haque <srhaque@iee.org>
 *
 * \b Detail:
 *
 * This widget inherits from KDialog and implements
 * the following additional functionalities:  a find string
 * object and an area for a user-defined widget to extend the dialog.
 *
 * \b Example:
 *
 * To use the basic modal find dialog, and then run the search:
 *
 * \code
 *  KFindDialog dlg(....)
 *  if ( dlg.exec() != QDialog::Accepted )
 *      return;
 *
 *  // proceed with KFind from here
 * \endcode
 *
 * To create a non-modal find dialog:
 * \code
 *   if ( m_findDia )
 *     KWindowSystem::activateWindow( m_findDia->winId() );
 *   else
 *   {
 *     m_findDia = new KFindDialog(false,...);
 *     connect( m_findDia, SIGNAL(okClicked()), this, SLOT(findTextNext()) );
 *   }
 * \endcode
 * Don't forget to delete and reset m_findDia when closed.
 * (But do NOT delete your KFind object at that point, it's needed for "Find Next".)
 *
 * To use your own extensions: see findExtension().
 *
 * \image html kfinddialog.png "KDE Find Dialog"
 */
class KTEXTWIDGETS_EXPORT KFindDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Construct a modal find dialog
     *
     * @param parent The parent object of this widget.
     * @param options A bitfield of the Options to be checked.
     * @param findStrings The find history, see findHistory()
     * @param hasSelection Whether a selection exists
     */
    explicit KFindDialog(QWidget *parent = 0, long options = 0,
                         const QStringList &findStrings = QStringList(), bool hasSelection = false, bool replaceDialog = false);

    /**
     * Destructor.
     */
    virtual ~KFindDialog();

    /**
     * Provide the list of @p strings to be displayed as the history
     * of find strings. @p strings might get truncated if it is
     * too long.
     *
     * @param history The find history.
     * @see findHistory
     */
    void setFindHistory(const QStringList &history);

    /**
     * Returns the list of history items.
     *
     * @see setFindHistory
     */
    QStringList findHistory() const;

    /**
     * Enable/disable the 'search in selection' option, depending
     * on whether there actually is a selection.
     *
     * @param hasSelection true if a selection exists
     */
    void setHasSelection(bool hasSelection);

    /**
     * Hide/show the 'from cursor' option, depending
     * on whether the application implements a cursor.
     *
     * @param hasCursor true if the application features a cursor
     * This is assumed to be the case by default.
     */
    void setHasCursor(bool hasCursor);

    /**
    * Enable/disable the 'Find backwards' option, depending
    * on whether the application supports it.
    *
    * @param supports true if the application supports backwards find
    * This is assumed to be the case by default.
    */
    void setSupportsBackwardsFind(bool supports);

    /**
    * Enable/disable the 'Case sensitive' option, depending
    * on whether the application supports it.
    *
    * @param supports true if the application supports case sensitive find
    * This is assumed to be the case by default.
    */
    void setSupportsCaseSensitiveFind(bool supports);

    /**
    * Enable/disable the 'Whole words only' option, depending
    * on whether the application supports it.
    *
    * @param supports true if the application supports whole words only find
    * This is assumed to be the case by default.
    */
    void setSupportsWholeWordsFind(bool supports);

    /**
    * Enable/disable the 'Regular expression' option, depending
    * on whether the application supports it.
    *
    * @param supports true if the application supports regular expression find
    * This is assumed to be the case by default.
    */
    void setSupportsRegularExpressionFind(bool supports);

    /**
     * Set the options which are checked.
     *
     * @param options The setting of the Options.
     *
     * @see options()
     * @see KFind::Options
     */
    void setOptions(long options);

    /**
     * Returns the state of the options. Disabled options may be returned in
     * an indeterminate state.
     *
     * @see setOptions()
     * @see KFind::Options
     */
    long options() const;

    /**
     * Returns the pattern to find.
     */
    QString pattern() const;

    /**
     * Sets the pattern to find
     */
    void setPattern(const QString &pattern);

    /**
     * Returns an empty widget which the user may fill with additional UI
     * elements as required. The widget occupies the width of the dialog,
     * and is positioned immediately below the regular expression support
     * widgets for the pattern string.
     */
    QWidget *findExtension() const;

Q_SIGNALS:
    /**
     * This signal is sent whenever one of the option checkboxes is toggled.
     * Call options() to get the new state of the checkboxes.
     */
    void optionsChanged();

    /**
     * This signal is sent when the user clicks on Ok button.
     */
    void okClicked();

    /**
     * This signal is sent when the user clicks on Cancel button.
     */
    void cancelClicked();

protected:
    void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private:
    friend class KReplaceDialog;
    friend class KReplaceDialogPrivate;
    class KFindDialogPrivate;
    KFindDialogPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_slotPlaceholdersAboutToShow())
    Q_PRIVATE_SLOT(d, void _k_slotOk())
    Q_PRIVATE_SLOT(d, void _k_slotReject())
    Q_PRIVATE_SLOT(d, void _k_slotSelectedTextToggled(bool))
    Q_PRIVATE_SLOT(d, void _k_showPatterns())
    Q_PRIVATE_SLOT(d, void _k_showPlaceholders())
    Q_PRIVATE_SLOT(d, void _k_textSearchChanged(const QString &))
};

#endif // KFINDDIALOG_H
