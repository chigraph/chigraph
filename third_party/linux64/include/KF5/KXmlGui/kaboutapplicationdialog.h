/* This file is part of the KDE libraries
   Copyright (C) 2007 Urs Wolfer <uwolfer at kde.org>

   Parts of this class have been take from the KAboutApplication class, which was
   Copyright (C) 2000 Waldo Bastian (bastian@kde.org) and Espen Sand (espen@kde.org)

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

#ifndef KABOUT_APPLICATION_DIALOG_H
#define KABOUT_APPLICATION_DIALOG_H

#include <QDialog>

#include <kxmlgui_export.h>

#include <QtCore/QFlags>

class KAboutData;

/**
 * @short Standard "About Application" dialog box.
 *
 * This class provides the standard "About Application" dialog box
 * that is used by KHelpMenu. It uses the information of the global
 * KAboutData that is specified at the start of your program in
 * main(). Normally you should not use this class directly but rather
 * the KHelpMenu class or even better just subclass your toplevel
 * window from KMainWindow. If you do the latter, the help menu and
 * thereby this dialog box is available through the
 * KMainWindow::helpMenu() function.
 *
 * \image html kaboutapplicationdialog.png "KDE About Application Dialog"
 *
 * @author Urs Wolfer uwolfer @ kde.org
 */

class KXMLGUI_EXPORT KAboutApplicationDialog : public QDialog
{
    Q_OBJECT
    Q_FLAGS(Options)
public:

    /**
     * Defines some options which can be applied to the about dialog
     *
     * @since 4.4
     */
    enum Option {
        NoOptions       = 0x0, ///< No options, show the standard about dialog
        HideTranslators = 0x1, ///< Don't show the translators tab
        HideKdeVersion  = 0x2  ///< Don't show the KDE version next to the application name and version
    };
    Q_DECLARE_FLAGS(Options, Option)

    /**
     * Constructor. Creates a fully featured "About Application" dialog box.
     *
     * @param aboutData A KAboutData object which data
     *        will be used for filling the dialog.
     * @param opts Additional options that can be applied, such as hiding the KDE version
     *             or the translators tab.
     * @param parent The parent of the dialog box. You should use the
     *        toplevel window so that the dialog becomes centered.
     *
     * @since 4.4
     */
    explicit KAboutApplicationDialog(const KAboutData &aboutData, Options opts, QWidget *parent = 0);

    /**
     * Constructor. Creates a fully featured "About Application" dialog box.
     *
     * @param aboutData A pointer to a KAboutData object which data
     *        will be used for filling the dialog.
     * @param parent The parent of the dialog box. You should use the
     *        toplevel window so that the dialog becomes centered.
     */
    explicit KAboutApplicationDialog(const KAboutData &aboutData, QWidget *parent = 0);

    virtual ~KAboutApplicationDialog();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void _k_showLicense(const QString &))

    Q_DISABLE_COPY(KAboutApplicationDialog)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KAboutApplicationDialog::Options)

#endif
