/*  This file is part of the KDE libraries
    Copyright (C) 1999 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 2
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

//krazy:excludeall=dpointer

#ifndef KMESSAGEBOX_H
#define KMESSAGEBOX_H

#include <QDialogButtonBox>
#include <QMessageBox>

#include <kguiitem.h>
#include <kstandardguiitem.h>

#include <kwidgetsaddons_export.h>

class KMessageBoxDontAskAgainInterface;
class KMessageBoxNotifyInterface;
class QDialog;
class QDialogButtonBox;
class QWidget;
class QStringList;
class KConfig;

/**
 * Easy message dialog box.
 *
 * Provides convenience functions for some i18n'ed standard dialogs,
 * as well as audible notification via @ref KNotification
 *
 * The text in message boxes is wrapped automatically. The text may either
 * be plaintext or richtext. If the text is plaintext, a newline-character
 * may be used to indicate the end of a paragraph.
 *
 * \image html kmessagebox.png "KDE Message Box (using questionYesNo())"
 *
 * @author Waldo Bastian (bastian@kde.org)
 */
namespace KMessageBox
{
/**
 * Button types.
 **/
enum ButtonCode {
    Ok = 1,
    Cancel = 2,
    Yes = 3,
    No = 4,
    Continue = 5
};

enum DialogType {
    QuestionYesNo = 1,
    WarningYesNo = 2,
    WarningContinueCancel = 3,
    WarningYesNoCancel = 4,
    Information = 5,
    // Reserved for: SSLMessageBox = 6
    Sorry = 7,
    Error = 8,
    QuestionYesNoCancel = 9
};

enum Option {
    Notify = 1,        ///< Emit a KNotify event
    AllowLink = 2,     ///< The message may contain links.
    Dangerous = 4,     ///< The action to be confirmed by the dialog is a potentially destructive one. The default button will be set to Cancel or No, depending on which is available.
    PlainCaption = 8,  ///< Do not use KApplication::makeStdCaption()
    NoExec = 16,       ///< Do not call exec() in createKMessageBox()
    WindowModal = 32   ///< The window is to be modal relative to its parent. By default, it is application modal.
};

Q_DECLARE_FLAGS(Options, Option)

} // namespace

// This declaration must be defined before first Option is used in method signatures AND outside the namespace
Q_DECLARE_OPERATORS_FOR_FLAGS(KMessageBox::Options)

namespace KMessageBox
{
/**
 * Display a simple "question" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Question").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Option
 *
 * @return  'Yes' is returned if the Yes-button is pressed. 'No' is returned
 *          if the No-button is pressed.
 *
 * To be used for questions like "Do you have a printer?"
 *
 * The default button is "Yes". Pressing "Esc" selects "No".
 */

KWIDGETSADDONS_EXPORT ButtonCode questionYesNo(QWidget *parent,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo =  KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);
/**
 * Display a simple "question" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Question").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param buttonCancel  The text for the third button.
 *                  The default is KStandardGuiItem::cancel().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  'Yes' is returned if the Yes-button is pressed. 'No' is returned
 *          if the No-button is pressed.
 *
 * To be used for questions like "Do you want to discard the message or save it for later?",
 *
 * The default button is "Yes". Pressing "Esc" selects "Cancel".
 */

KWIDGETSADDONS_EXPORT ButtonCode questionYesNoCancel(QWidget *parent,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * Display a "question" dialog with a listbox to show information to the user
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox. If the list is
 *                empty, it doesn't show any listbox, working as questionYesNo.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Question").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  'Yes' is returned if the Yes-button is pressed. 'No' is returned
 *          if the No-button is pressed.
 *
 * To be used for questions like "Do you really want to delete these files?"
 * And show the user exactly which files are going to be deleted in case
 * he presses "Yes"
 *
 * The default button is "Yes". Pressing "Esc" selects "No".
 */

KWIDGETSADDONS_EXPORT ButtonCode questionYesNoList(QWidget *parent,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * Display a "warning" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Warning").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  @p Yes is returned if the Yes-button is pressed. @p No is returned
 *          if the No-button is pressed.
 *
 * To be used for questions "Shall I update your configuration?"
 * The text should explain the implication of both options.
 *
 * The default button is "No". Pressing "Esc" selects "No".
 */
KWIDGETSADDONS_EXPORT ButtonCode warningYesNo(QWidget *parent,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Options(Notify | Dangerous));

/**
 * Display a "warning" dialog with a listbox to show information to the user
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox. If the list is
 *                empty, it doesn't show any listbox, working as questionYesNo.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Question").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  'Yes' is returned if the Yes-button is pressed. 'No' is returned
 *          if the No-button is pressed.
 *
 * To be used for questions like "Do you really want to delete these files?"
 * And show the user exactly which files are going to be deleted in case
 * he presses "Yes"
 *
 * The default button is "No". Pressing "Esc" selects "No".
 */

KWIDGETSADDONS_EXPORT ButtonCode warningYesNoList(QWidget *parent,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Options(Notify | Dangerous));

/**
 * Display a "warning" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Warning").
 * @param buttonContinue The text for the first button.
 *                       The default is KStandardGuiItem::cont().
 * @param buttonCancel The text for the second button.
 *                     The default is KStandardGuiItem::cancel().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  @p Continue is returned if the Continue-button is pressed.
 *          @p Cancel is returned if the Cancel-button is pressed.
 *
 * To be used for questions like "You are about to Print. Are you sure?"
 * the continueButton should then be labeled "Print".
 *
 * The default button is buttonContinue. Pressing "Esc" selects "Cancel".
 */
KWIDGETSADDONS_EXPORT ButtonCode warningContinueCancel(QWidget *parent,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonContinue = KStandardGuiItem::cont(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * Display a "warning" dialog with a listbox to show information to the user.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox. If the
 *                list is empty, it doesn't show any listbox, working
 *                as warningContinueCancel.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Warning").
 * @param buttonContinue The text for the first button.
 *                       The default is KStandardGuiItem::cont().
 * @param buttonCancel The text for the second button.
 *                     The default is KStandardGuiItem::cancel().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further confirmation can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 *
 * @param options  see Options
 *
 * @return  @p Continue is returned if the Continue-button is pressed.
 *          @p Cancel is returned if the Cancel-button is pressed.
 *
 * To be used for questions like "You are about to Print. Are you sure?"
 * the continueButton should then be labeled "Print".
 *
 * The default button is buttonContinue. Pressing "Esc" selects "Cancel".
 */
KWIDGETSADDONS_EXPORT ButtonCode warningContinueCancelList(QWidget *parent,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonContinue = KStandardGuiItem::cont(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * Display a Yes/No/Cancel "warning" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Warning").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param buttonCancel  The text for the third button.
 *                  The default is KStandardGuiItem::cancel().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further questions can be turned off. If turned off
 *                all questions will be automatically answered with the
 *                last answer (either Yes or No).
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  @p Yes is returned if the Yes-button is pressed. @p No is returned
 *          if the No-button is pressed. @p Cancel is retunred if the Cancel-
 *          button is pressed.
 *
 * To be used for questions "Do you want to save your changes?"
 * The text should explain the implication of choosing 'No'.
 *
 * The default button is "Yes". Pressing "Esc" selects "Cancel"
 */

KWIDGETSADDONS_EXPORT ButtonCode warningYesNoCancel(QWidget *parent,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * Display a Yes/No/Cancel "warning" dialog with a listbox to show information
 * to the user.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox. If the
 *                list is empty, it doesn't show any listbox, working
 *                as warningYesNoCancel.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Warning").
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param buttonCancel  The text for the third button.
 *                  The default is KStandardGuiItem::cancel().
 * @param dontAskAgainName If provided, a checkbox is added with which
 *                further questions can be turned off. If turned off
 *                all questions will be automatically answered with the
 *                last answer (either Yes or No).
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 *                If @p dontAskAgainName starts with a ':' then the setting
 *                is stored in the global config file.
 * @param options  see Options
 *
 * @return  @p Yes is returned if the Yes-button is pressed. @p No is returned
 *          if the No-button is pressed. @p Cancel is retunred if the Cancel-
 *          button is pressed.
 *
 * To be used for questions "Do you want to save your changes?"
 * The text should explain the implication of choosing 'No'.
 *
 * The default button is "Yes". Pressing "Esc" selects "Cancel"
 */
KWIDGETSADDONS_EXPORT ButtonCode warningYesNoCancelList(QWidget *parent,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * Display an "Error" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Error").
 * @param options  see Options
 *
 * Your program messed up and now it's time to inform the user.
 * To be used for important things like "Sorry, I deleted your hard disk."
 *
 * If your program detects the action specified by the user is somehow
 * not allowed, this should never be reported with error(). Use sorry()
 * instead to explain to the user that this action is not allowed.
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 * NOTE: The OK button will always have the i18n'ed text '&OK'.
 */

KWIDGETSADDONS_EXPORT void error(QWidget *parent,
                                 const QString &text,
                                 const QString &caption = QString(),
                                 Options options = Notify);

/**
 * Display an "Error" dialog with a listbox.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox. If the
 *                list is empty, it doesn't show any listbox, working
 *                as error().
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Error").
 * @param options  see Options
 *
 * Your program messed up and now it's time to inform the user.
 * To be used for important things like "Sorry, I deleted your hard disk."
 *
 * If your program detects the action specified by the user is somehow
 * not allowed, this should never be reported with error(). Use sorry()
 * instead to explain to the user that this action is not allowed.
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 * NOTE: The OK button will always have the i18n'ed text '&OK'.
 */

KWIDGETSADDONS_EXPORT void errorList(QWidget *parent,
                                     const QString &text,
                                     const QStringList &strlist,
                                     const QString &caption = QString(),
                                     Options options = Notify);

/**
 * Displays an "Error" dialog with a "Details >>" button.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param details Detailed message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Error").
 * @param options  see Options
 *
 * Your program messed up and now it's time to inform the user.
 * To be used for important things like "Sorry, I deleted your hard disk."
 *
 * The @p details message can contain additional information about
 * the problem and can be shown on request to advanced/interested users.
 *
 * If your program detects the action specified by the user is somehow
 * not allowed, this should never be reported with error(). Use sorry()
 * instead to explain to the user that this action is not allowed.
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 * NOTE: The OK button will always have the i18n'ed text '&OK'.
 */
KWIDGETSADDONS_EXPORT void detailedError(QWidget *parent,
        const QString &text,
        const QString &details,
        const QString &caption = QString(),
        Options options = Notify);

/**
 * Display a "Sorry" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Sorry").
 * @param options  see OptionsType
 *
 * Either your program messed up and asks for understanding
 * or your user did something stupid.
 *
 * To be used for small problems like
 * "Sorry, I can't find the file you specified."
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 * NOTE: The ok button will always have the i18n'ed text '&OK'.
 */

KWIDGETSADDONS_EXPORT void sorry(QWidget *parent,
                                 const QString &text,
                                 const QString &caption = QString(),
                                 Options options = Notify);

/**
 * Displays a "Sorry" dialog with a "Details >>" button.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param details Detailed message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Sorry").
 * @param options  see Options
 *
 * Either your program messed up and asks for understanding
 * or your user did something stupid.
 *
 * To be used for small problems like
 * "Sorry, I can't find the file you specified."
 *
 * And then @p details can contain something like
 * "foobar.txt was not found in any of
 *  the following directories:
 *  /usr/bin,/usr/local/bin,/usr/sbin"
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 * NOTE: The ok button will always have the i18n'ed text '&OK'.
 */

KWIDGETSADDONS_EXPORT void detailedSorry(QWidget *parent,
        const QString &text,
        const QString &details,
        const QString &caption = QString(),
        Options options = Notify);

/**
 * Display an "Information" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Information").
 * @param dontShowAgainName If provided, a checkbox is added with which
 *                further notifications can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 * @param options  see Options
 *
 *
 * Your program wants to tell the user something.
 * To be used for things like:
 * "Your bookmarks have been rearranged."
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 *  NOTE: The OK button will always have the i18n'ed text '&OK'.
 */

KWIDGETSADDONS_EXPORT void information(QWidget *parent,
                                       const QString &text,
                                       const QString &caption = QString(),
                                       const QString &dontShowAgainName = QString(),
                                       Options options = Notify);

/**
 * Display an "Information" dialog with a listbox.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox. If the
 *                list is empty, it doesn't show any listbox, working
 *                as information.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("Information").
 * @param dontShowAgainName If provided, a checkbox is added with which
 *                further notifications can be turned off.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 *                The setting is stored in the "Notification Messages" group.
 * @param options  see Options
 *
 *
 * Your program wants to tell the user something.
 * To be used for things like:
 * "The following bookmarks have been rearranged:"
 *
 * The default button is "&OK". Pressing "Esc" selects the OK-button.
 *
 *  NOTE: The OK button will always have the i18n'ed text '&OK'.
 */

KWIDGETSADDONS_EXPORT void informationList(QWidget *parent,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const QString &dontShowAgainName = QString(),
        Options options = Notify);

/**
 * Enable all messages which have been turned off with the
 * @p dontShowAgainName feature.
 */
KWIDGETSADDONS_EXPORT void enableAllMessages();

/**
 * Re-enable a specific @p dontShowAgainName messages that had
 * previously been turned off.
 * @see saveDontShowAgainYesNo()
 * @see saveDontShowAgainContinue()
 */
KWIDGETSADDONS_EXPORT void enableMessage(const QString &dontShowAgainName);

/**
 * Display an "About" dialog.
 *
 * @param parent  Parent widget.
 * @param text    Message string.
 * @param caption Message box title. The application name is added to
 *                the title. The default title is i18n("About \<appname\>").
 * @param options  see Options
 *
 *
 * Your program wants to show some general information about the application
 * like the authors's names and email addresses.
 *
 * The default button is "&OK".
 *
 *  NOTE: The ok button will always have the i18n'ed text '&OK'.
 */
KWIDGETSADDONS_EXPORT void about(QWidget *parent,
                                 const QString &text,
                                 const QString &caption = QString(),
                                 Options options = Notify);

/**
 * Alternate method to show a messagebox:
 *
 * @param parent Parent widget.
 * @param type type of message box: QuestionYesNo, WarningYesNo, WarningContinueCancel...
 * @param text Message string.
 * @param caption Message box title.
 * @param buttonYes The text for the first button.
 *                  The default is KStandardGuiItem::yes().
 * @param buttonNo  The text for the second button.
 *                  The default is KStandardGuiItem::no().
 * @param buttonCancel  The text for the third button.
 *                  The default is KStandardGuiItem::cancel().
 * @param dontShowAskAgainName If provided, a checkbox is added with which
 *                further questions/information can be turned off. If turned off
 *                all questions will be automatically answered with the
 *                last answer (either Yes or No), if the message box needs an answer.
 *                The string is used to lookup and store the setting
 *                in the applications config file.
 * @param options  see Options
 * Note: for ContinueCancel, buttonYes is the continue button and buttonNo is unused.
 *       and for Information, none is used.
 * @return a button code, as defined in KMessageBox.
 */

KWIDGETSADDONS_EXPORT ButtonCode messageBox(QWidget *parent, DialogType type, const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontShowAskAgainName = QString(),
        Options options = Notify);

/**
 * @return true if the corresponding yes/no message box should be shown.
 * @param dontShowAgainName the name that identify the message box. If
 * empty, true is always returned.
 * @param result is set to the result (Yes or No) that was chosen the last
 * time the message box was shown. Only meaningful, if the message box
 * should not be shown.
 */
KWIDGETSADDONS_EXPORT bool shouldBeShownYesNo(const QString &dontShowAgainName,
        ButtonCode &result);
/**
 * @return true if the corresponding continue/cancel message box should be
 * shown.
 * @param dontShowAgainName the name that identify the message box. If
 * empty, true is always returned.
 */
KWIDGETSADDONS_EXPORT bool shouldBeShownContinue(const QString &dontShowAgainName);

/**
 * Save the fact that the yes/no message box should not be shown again.
 * @param dontShowAgainName the name that identify the message box. If
 * empty, this method does nothing.
 * @param result the value (Yes or No) that should be used as the result
 * for the message box.
 */
KWIDGETSADDONS_EXPORT void saveDontShowAgainYesNo(const QString &dontShowAgainName,
        ButtonCode result);

/**
 * Save the fact that the continue/cancel message box should not be shown
 * again.
 * @param dontShowAgainName the name that identify the message box. If
 * empty, this method does nothing.
 */
KWIDGETSADDONS_EXPORT void saveDontShowAgainContinue(const QString &dontShowAgainName);

/**
 * Use @p cfg for all settings related to the dontShowAgainName feature.
 * If @p cfg is 0 (default) KGlobal::config() will be used.
 */
KWIDGETSADDONS_EXPORT void setDontShowAgainConfig(KConfig *cfg);

#ifndef KWIDGETSADDONS_NO_DEPRECATED
/**
 * @deprecated since 5.0, use setDontShowAgainConfig()
 */
KWIDGETSADDONS_DEPRECATED_EXPORT inline void setDontShowAskAgainConfig(KConfig *cfg) { setDontShowAgainConfig(cfg); }
#endif

/**
 * Use @p dontAskAgainInterface for all settings related to the donShowAgain feature.
 * This method does not take ownership of @p dontAskAgainInterface.
 *
 * @since 5.0
 */
KWIDGETSADDONS_EXPORT void setDontShowAgainInterface(KMessageBoxDontAskAgainInterface *dontAskAgainInterface);

/**
 * Use @p notifyInterface to send notifications.
 * This method does not take ownership of @p notifyInterface.
 *
 * @since 5.0
 */
KWIDGETSADDONS_EXPORT void setNotifyInterface(KMessageBoxNotifyInterface *notifyInterface);

/**
 * Create content and layout of a standard dialog
 *
 * @param dialog  The parent dialog base
 * @param buttons a QDialogButtonBox instance. This function will take care of connecting to it.
 * @param icon    Which predefined icon the message box shall show.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox.
 *                If the list is empty, it doesn't show any listbox
 * @param ask     The text of the checkbox. If empty none will be shown.
 * @param checkboxReturn The result of the checkbox. If it's initially
 *                true then the checkbox will be checked by default.
 *                May be 0.
 * @param options  see Options
 * @param details Detailed message string.
 * @return A QDialogButtonBox::StandardButton button code, not a KMessageBox
 *         button code, based on the buttonmask given to the constructor of
 *         the @p dialog (ie. will return QDialogButtonBox::Yes instead of
 *         KMessageBox::Yes). Will return QDialogButtonBox::NoButton if the
 *         message box is queued for display instead of exec()ed immediately
 *         or if the option NoExec is set.
 * @note   Unless NoExec is used,
 *         the @p dialog that is passed in is deleted by this
 *         function. Do not delete it yourself.
 */
KWIDGETSADDONS_EXPORT QDialogButtonBox::StandardButton createKMessageBox(QDialog *dialog, QDialogButtonBox *buttons,
        QMessageBox::Icon icon, //krazy:exclude=qclasses
        const QString &text, const QStringList &strlist,
        const QString &ask, bool *checkboxReturn,
        Options options, const QString &details = QString());

/**
 * Create content and layout of a standard dialog
 *
 * @param dialog  The parent dialog base
 * @param buttons a QDialogButtonBox instance. This function will take care of connecting to it.
 * @param icon    A QPixmap containing the icon to be displayed in the
 *                dialog next to the text.
 * @param text    Message string.
 * @param strlist List of strings to be written in the listbox.
 *                If the list is empty, it doesn't show any listbox
 * @param ask     The text of the checkbox. If empty none will be shown.
 * @param checkboxReturn The result of the checkbox. If it's initially
 *                true then the checkbox will be checked by default.
 *                May be 0.
 * @param options  see Options
 * @param details Detailed message string.
 * @param notifyType The type of notification to send when this message
 *                is presentend.
 * @return A QDialogButtonBox::StandardButton button code, not a KMessageBox
 *         button code, based on the buttonmask given to the constructor of
 *         the @p dialog (ie. will return QDialogButtonBox::Yes instead of
 *         KMessageBox::Yes). Will return QDialogButtonBox::NoButton if the
 *         message box is queued for display instead of exec()ed immediately
 *         or if the option NoExec is set.
 * @note   Unless NoExec is used,
 *         the @p dialog that is passed in is deleted by this
 *         function. Do not delete it yourself.
 */
KWIDGETSADDONS_EXPORT QDialogButtonBox::StandardButton createKMessageBox(QDialog *dialog, QDialogButtonBox *buttons,
        const QIcon &icon,
        const QString &text, const QStringList &strlist,
        const QString &ask, bool *checkboxReturn,
        Options options, const QString &details = QString(),
        QMessageBox::Icon notifyType = QMessageBox::Information); //krazy:exclude=qclasses

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode questionYesNoWId(WId parent_id,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo =  KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode questionYesNoCancelWId(WId parent_id,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode questionYesNoListWId(WId parent_id,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode warningYesNoWId(WId parent_id,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Options(Notify | Dangerous));

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode warningYesNoListWId(WId parent_id,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const QString &dontAskAgainName = QString(),
        Options options = Options(Notify | Dangerous));

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode warningContinueCancelWId(WId parent_id,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonContinue = KStandardGuiItem::cont(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode warningContinueCancelListWId(WId parent_id,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonContinue = KStandardGuiItem::cont(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode warningYesNoCancelWId(WId parent_id,
        const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode warningYesNoCancelListWId(WId parent_id,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontAskAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void errorWId(WId parent_id,
                                    const QString &text,
                                    const QString &caption = QString(),
                                    Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void errorListWId(WId parent_id,
                                        const QString &text,
                                        const QStringList &strlist,
                                        const QString &caption = QString(),
                                        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void detailedErrorWId(WId parent_id,
        const QString &text,
        const QString &details,
        const QString &caption = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void sorryWId(WId parent_id,
                                    const QString &text,
                                    const QString &caption = QString(),
                                    Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void detailedSorryWId(WId parent_id,
        const QString &text,
        const QString &details,
        const QString &caption = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void informationWId(WId parent_id,
        const QString &text,
        const QString &caption = QString(),
        const QString &dontShowAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT void informationListWId(WId parent_id,
        const QString &text,
        const QStringList &strlist,
        const QString &caption = QString(),
        const QString &dontShowAgainName = QString(),
        Options options = Notify);

/**
 * This function accepts the window id of the parent window, instead
 * of QWidget*. It should be used only when necessary.
 */
KWIDGETSADDONS_EXPORT ButtonCode messageBoxWId(WId parent_id, DialogType type, const QString &text,
        const QString &caption = QString(),
        const KGuiItem &buttonYes = KStandardGuiItem::yes(),
        const KGuiItem &buttonNo = KStandardGuiItem::no(),
        const KGuiItem &buttonCancel = KStandardGuiItem::cancel(),
        const QString &dontShowAskAgainName = QString(),
        Options options = Notify);

}

#endif
