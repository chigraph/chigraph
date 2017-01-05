/* This file is part of the KDE libraries
   Copyright (C) 2001 - 2004 Anders Lund <anders@alweb.dk>

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

#ifndef KMIMETYPE_CHOOSER_H
#define KMIMETYPE_CHOOSER_H

#include <kwidgetsaddons_export.h>
#include <QDialog>
#include <QWidget>

class QTreeWidgetItem;

/**
 * This widget provides a checkable list of all available mimetypes,
 * and a list of selected ones, as well as a corresponding list of file
 * extensions, an optional text and an optional edit button (not working yet).
 * Mime types is presented in a list view, with name, comment and patterns columns.
 *
 * @author Anders Lund (anders at alweb dk), jan 23, 2002
 */
class KWIDGETSADDONS_EXPORT KMimeTypeChooser : public QWidget
{
    Q_OBJECT

public:
    /**
     * Buttons and data for display.
     */
    enum Visuals {
        Comments = 1, ///< Show the Mimetypes Comment field in a column ("HTML Document").
        Patterns = 2, ///< Show the Mimetypes Patterns field in a column ("*.html;*.htm").
        EditButton = 4 ///< Show the "Edit" button, allowing to edit the selected type.
    };
    /**
     * Create a new KMimeTypeChooser.
     *
     * @param text A Text to display above the list
     * @param selectedMimeTypes A list of mimetype names, theese will be checked
     *        in the list if they exist.
     * @param visuals A OR'd Visuals enum to decide which data and buttons to display.
     * @param defaultGroup The group to open when no groups are selected (like
     *        "text"). If not provided, no group is opened. If @p groupsToShow
     *        is provided and defaultGroup is not a member of that, it is ignored.
     * @param groupsToShow a list of mimetype groups to show. If empty, all
     *        groups are shown.
     * @param parent The parent widget to use
     */
    explicit
    KMimeTypeChooser(const QString &text = QString(),
                     const QStringList &selectedMimeTypes = QStringList(),
                     const QString &defaultGroup = QString(),
                     const QStringList &groupsToShow = QStringList(),
                     int visuals = Comments | Patterns | EditButton,
                     QWidget *parent = 0);
    ~KMimeTypeChooser();

    /**
     * @return a list of all selected selected mimetypes represented by their name.
     */
    QStringList mimeTypes() const;
    /**
     * @return a list of the fileame patterns associated with all selected mimetypes.
     */
    QStringList patterns() const;

private:
    class KMimeTypeChooserPrivate *d;

    Q_PRIVATE_SLOT(d, void _k_editMimeType())
    Q_PRIVATE_SLOT(d, void _k_slotCurrentChanged(QTreeWidgetItem *))
    Q_PRIVATE_SLOT(d, void _k_slotSycocaDatabaseChanged(QStringList))
};

/**
  * @short A Dialog to choose some mimetypes.
  * Provides a checkable tree list of mimetypes, with icons and optinally
  * comments and patterns, and an (optional) button to display the KDE mimetype
  * editor.
  *
  * Here is an example, using the dialog to set the text of two lineedits:
  *
  * @code
  *    QString text = i18n("Select the MimeTypes you want for this file type.");
  *    QStringList list = QStringList::split( QRegExp("\\s*;\\s*"), leMimetypes->text() );
  *    KMimeTypeChooserDialog dlg( i18n("Select Mime Types"), text, list, "text", this );
  *    if ( dlg.exec() == KDialog::Accepted ) {
  *      leWildcards->setText( dlg.chooser()->patterns().join(";") );
  *      leMimetypes->setText( dlg.chooser()->mimeTypes().join(";") );
  *    }
  * @endcode
  *
  * \image html kmimetypechooserdialog.png "KMimeTypeChooserDialog in action"
  *
  * @author Anders Lund (anders at alweb dk) dec 19, 2001
  */
class KWIDGETSADDONS_EXPORT KMimeTypeChooserDialog : public QDialog
{
public:
    /**
     * Create a KMimeTypeChooser dialog.
     *
     * @param caption The title of the dialog
     * @param text A Text to display above the list
     * @param selectedMimeTypes A list of mimetype names, theese will be
     *        checked in the list if they exist.
     *        patterns will be added to the list view.
     * @param visuals A OR'd KMimetypeChooser::Visuals enum to decide which data
     *        and buttons to display.
     * @param defaultGroup The group to open when no groups are selected (like
     *        "text"). If not provided, no group is opened. If @p groupsToShow
     *        is provided and defaultGroup is not a member of that, it is ignored.
     * @param groupsToShow a list of mimetype groups to show. If empty, all
     *        groups are shown.
     * @param parent The parent widget to use
     */
    explicit
    KMimeTypeChooserDialog(const QString &caption = QString(),
                           const QString &text = QString(),
                           const QStringList &selectedMimeTypes = QStringList(),
                           const QString &defaultGroup = QString(),
                           const QStringList &groupsToShow = QStringList(),
                           int visuals = KMimeTypeChooser::Comments | KMimeTypeChooser::Patterns | KMimeTypeChooser::EditButton,
                           QWidget *parent = 0);

    /**
     * @overload
     */
    KMimeTypeChooserDialog(const QString &caption,
                           const QString &text,
                           const QStringList &selectedMimeTypes,
                           const QString &defaultGroup,
                           QWidget *parent = 0);

    ~KMimeTypeChooserDialog();

    /**
     * @return a pointer to the KMimeTypeChooser widget
     */
    KMimeTypeChooser *chooser();

private:
    class Private;
    Private *const d;
};
#endif // _KMIMETYPE_CHOOSER_H_
