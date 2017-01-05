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

#ifndef KREPLACEDIALOG_H
#define KREPLACEDIALOG_H

#include "ktextwidgets_export.h"

#include "kfinddialog.h"

class KReplaceDialogPrivate;

/**
 * @short A generic "replace" dialog.
 *
 * @author S.R.Haque <srhaque@iee.org>
 *
 * \b Detail:
 *
 * This widget inherits from KFindDialog and implements
 * the following additional functionalities:  a replacement string
 * object and an area for a user-defined widget to extend the dialog.
 *
 * \b Example:
 *
 * To use the basic replace dialog:
 *
 * \code
 * \endcode
 *
 * To use your own extensions:
 *
 * \code
 * \endcode
 *
 * \image html kreplacedialog.png "KDE Replace Dialog"
 */
class KTEXTWIDGETS_EXPORT KReplaceDialog:
    public KFindDialog
{
    Q_OBJECT

public:

    // Options.

    enum Options {
        // Should the user be prompted before the replace operation?
        PromptOnReplace = 256,
        BackReference = 512
    };

    /**
     * Construct a replace dialog.read-only or rather select-only combo box with a
     * parent object and a name.
     *
     * @param parent The parent object of this widget
     * @param options A bitfield of the Options to be enabled.
     * @param findStrings A QStringList to insert in the combo box of text to find
     * @param replaceStrings A QStringList to insert in the combo box of text to
     *        replace with
     * @param hasSelection Whether a selection exists
     */
    explicit KReplaceDialog(QWidget *parent = 0, long options = 0,
                            const QStringList &findStrings = QStringList(),
                            const QStringList &replaceStrings = QStringList(),
                            bool hasSelection = true);

    /**
     * Destructor.
     */
    virtual ~KReplaceDialog();

    /**
     * Provide the list of @p strings to be displayed as the history
     * of replacement strings. @p strings might get truncated if it is
     * too long.
     *
     * @param history The replacement history.
     * @see replacementHistory
     */
    void setReplacementHistory(const QStringList &history);

    /**
     * Returns the list of history items.
     *
     * @see setReplacementHistory
     */
    QStringList replacementHistory() const;

    /**
     * Set the options which are enabled.
     *
     * @param options The setting of the Options.
     */
    void setOptions(long options);

    /**
     * Returns the state of the options. Disabled options may be returned in
     * an indeterminate state.
     *
     * @see setOptions
     */
    long options() const;

    /**
     * Returns the replacement string.
     */
    QString replacement() const;

    /**
     * Returns an empty widget which the user may fill with additional UI
     * elements as required. The widget occupies the width of the dialog,
     * and is positioned immediately the regular expression support widgets
     * for the replacement string.
     */
    QWidget *replaceExtension() const;

protected:
    void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private:

    KReplaceDialogPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_slotOk())
};

#endif // KREPLACEDIALOG_H
