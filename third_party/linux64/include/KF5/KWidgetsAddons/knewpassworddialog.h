// vi: ts=8 sts=4 sw=4
/* This file is part of the KDE libraries
   Copyright (C) 1998 Pietro Iglio <iglio@fub.it>
   Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
   Copyright (C) 2004,2005 Andrew Coles <andrew_coles@yahoo.co.uk>
   Copyright (C) 2006,2007 Olivier Goffart  <ogoffart @ kde.org>

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
#ifndef KNEWPASSWORDDIALOG_H
#define KNEWPASSWORDDIALOG_H

#include <QDialog>

#include <kwidgetsaddons_export.h>

class QWidget;

/**
 * @short A password input dialog.
 *
 * This dialog asks the user to enter a new password.
 *
 * The password has to be entered twice to check if the passwords
 * match. A hint about the strength of the entered password is also
 * shown.
 *
 * \section usage Usage Example
 * \subsection asynchronous Asynchronous
 *
 * \code
 *  KNewPasswordDialog *dlg = new KNewPasswordDialog( parent );
 *  dlg->setPrompt( i18n( "Enter a password" ) );
 *  connect( dlg, SIGNAL( newPassword(QString) )  , this, SLOT( setPassword(QString) ) );
 *  connect( dlg, SIGNAL( rejected() )  , this, SLOT( slotCancel() ) );
 *  dlg->show();
 * \endcode
 *
 * \subsection synchronous Synchronous
 *
 * \code
 *  KNewPasswordDialog dlg( parent );
 *  dlg.setPrompt( i18n( "Enter a password" ) );
 *  if( dlg.exec() )
 *      setPassword( dlg.password() );
 * \endcode
 *
 * \image html knewpassworddialog.png "KDE New Password Dialog"
 *
 * @author Geert Jansen <jansen@kde.org>
 * @author Olivier Goffart <ogoffart@kde.org>
 */

class KWIDGETSADDONS_EXPORT KNewPasswordDialog
    : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructs a password dialog.
     *
     * @param parent Passed to lower level constructor.
     */
    explicit KNewPasswordDialog(QWidget *parent = 0);

    /**
     * Destructs the password dialog.
     */
    virtual ~KNewPasswordDialog();

    /**
     * Sets the password prompt.
     */
    void setPrompt(const QString &prompt);

    /**
     * Returns the password prompt.
     */
    QString prompt() const;

    /**
     * Sets the pixmap that appears next to the prompt in the dialog.  The default pixmap represent a simple key.
     *
     * the recommended size is KIconLoader::SizeHuge
     */
    void setPixmap(const QPixmap &);

    /**
     * Returns the pixmap that appears next to the prompt in the dialog
     */
    QPixmap pixmap() const;

    /**
     * Allow empty passwords? - Default: true
     *
     * same as setMinimumPasswordLength( allowed ? 0 : 1 )
     */
    void setAllowEmptyPasswords(bool allowed);

    /**
     * Allow empty passwords?
     *
     * @return true if minimumPasswordLength() == 0
     */
    bool allowEmptyPasswords() const;

    /**
     * Minimum acceptable password length.
     *
     * Default: 0
     *
     * @param minLength The new minimum password length
     */
    void setMinimumPasswordLength(int minLength);

    /**
     * Minimum acceptable password length.
     */
    int minimumPasswordLength() const;

    /**
     * Maximum acceptable password length.
     *
     * @param maxLength The new maximum password length.
     */
    void setMaximumPasswordLength(int maxLength);

    /**
     * Maximum acceptable password length.
     */
    int maximumPasswordLength() const;

    /**
     * Password length that is expected to be reasonably safe.
     *
     * Used to compute the strength level
     *
     * Default: 8 - the standard UNIX password length
     *
     * @param reasonableLength The new reasonable password length.
     */
    void setReasonablePasswordLength(int reasonableLength);

    /**
     * Password length that is expected to be reasonably safe.
     */
    int reasonablePasswordLength() const;

    /**
     * Set the password strength level below which a warning is given
     * Value is in the range 0 to 99. Empty passwords score 0;
     * non-empty passwords score up to 100, depending on their length and whether they
     * contain numbers, mixed case letters and punctuation.
     *
     * Default: 1 - warn if the password has no discernable strength whatsoever
     * @param warningLevel The level below which a warning should be given.
     */
    void setPasswordStrengthWarningLevel(int warningLevel);

    /**
     * Password strength level below which a warning is given
     */
    int passwordStrengthWarningLevel() const;

    /**
     * When the verification password does not match, the background color
     * of the verification field is set to @p color. As soon as the passwords match,
     * the original color of the verification field is restored.
     *
     * Default: the background color from the current theme.
     * @since 5.17
     */
    void setBackgroundWarningColor(const QColor &color);

    /**
     * The color used as warning for the verification password field's background.
     */
    QColor backgroundWarningColor() const;

    /**
     * Returns the password entered.
     * @note Only has meaningful data after accept has been called
     *       if you want to access the password from a subclass use
     *       checkAndGetPassword()
     */
    QString password() const;

    /**
     * @internal
     */
    void accept() Q_DECL_OVERRIDE;

protected:

    /**
     * Virtual function that can be overridden to provide password
     * checking in derived classes. It should return @p true if the
     * password is valid, @p false otherwise.
     */
    virtual bool checkPassword(const QString &);

    /**
     * Checks input password.
     * If the password is right, returns true
     * and fills pwd with the password.
     * Otherwise returns false and pwd will be null.
     * @since 4.2
     */
    bool checkAndGetPassword(QString *pwd);

Q_SIGNALS:

    /**
     * The dialog has been accepted, and the new password is @p password
     */
    void newPassword(const QString &password);

private:
    class KNewPasswordDialogPrivate;
    KNewPasswordDialogPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_passwordStatusChanged())
};

#endif // KNEWPASSWORDDIALOG_H

