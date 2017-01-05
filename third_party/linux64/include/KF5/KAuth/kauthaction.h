/*
*   Copyright (C) 2009-2012 Dario Freddi <drf@kde.org>
*   Copyright (C) 2008 Nicola Gigante <nicola.gigante@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation; either version 2.1 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .
*/

#ifndef ACTION_H
#define ACTION_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QHash>
#include <QtCore/QSharedDataPointer>

#include <kauth_export.h>

namespace KAuth
{

class ExecuteJob;

class ActionData;
/**
 * @brief Class to access, authorize and execute actions.
 *
 * This is the main class of the kauth API. It provides the interface to
 * manipulate actions. Every action is identified by its name. Every instance
 * of the Action class with the same name refers to the same action.
 *
 * Once you have an action object you can tell the helper to execute it
 * (asking the user to authenticate if needed) with the execute() method.
 * The simplest thing to do is to execute a single action synchronously
 * blocking for the reply by callin exec() on the job object returned by
 * execute().
 *
 * For asynchronous calls, use KAuth::ExecuteJob::start() instead.
 * It sends the request
 * to the helper and returns immediately. Before doing so you should however
 * connect to at least the KJob::result(KJob *) signal to receive a slot call
 * once the action is done executing.
 *
 * To use the execute() method you have to set the default helper's ID using
 * the setHelperID() static method. Alternatively, you can specify the helperID using
 * the overloaded version of the methods that takes it as a parameter.
 *
 * Each action object contains a QVariantMap object that is passed directly to the
 * helper when the action is executed. You can access this map using the arguments()
 * method. You can insert into it any kind of custom data you need to pass to the helper.
 *
 * @code
 * void MyApp::runAction()
 * {
 *     action = KAuth::Action("org.kde.myapp.action");
 *     KAuth::ExecuteJob *job = action.execute();
 *     connect(job, &KAuth::ExecuteJob::result, this, &MyApp::actionResult);
 *     job->start();
 * }
 *
 * void MyApp::actionResult(KJob *kjob)
 * {
 *     auto job = qobject_cast<KAuth::ExecuteJob *>(kjob);
 *     qDebug() << job.error() << job.data();
 * }
 * @endcode
 *
 * @since 4.4
 */
class KAUTH_EXPORT Action
{
Q_GADGET
public:
    /**
     * The three values returned by authorization methods
     */
    enum AuthStatus {
        DeniedStatus, ///< The authorization has been denied by the authorization backend
        ErrorStatus, ///< An error occurred
        InvalidStatus, ///< An invalid action cannot be authorized
        AuthorizedStatus, ///< The authorization has been granted by the authorization backend
        AuthRequiredStatus, ///< The user could obtain the authorization after authentication
        UserCancelledStatus ///< The user pressed Cancel the authentication dialog. Currently used only on the mac
    };
    Q_ENUM(AuthStatus)

    enum ExecutionMode {
        ExecuteMode,
        AuthorizeOnlyMode
    };
    Q_ENUM(ExecutionMode)

    /**
     * @brief Default constructor
     *
     * This constructor sets the name to the empty string.
     * Such an action is invalid and cannot be authorized nor executed, so
     * you need to call setName() before you can use the object.
     */
    Action();

    /** Copy constructor */
    Action(const Action &action);

    /**
     * This creates a new action object with this name
     * @param name The name of the new action
     */
    Action(const QString &name);

    /**
     * This creates a new action object with this name and details
     * @param name The name of the new action
     * @param details The details of the action
     *
     * @see setDetails
     */
    Action(const QString &name, const QString &details);

    /// Virtual destructor
    ~Action();

    /// Assignment operator
    Action &operator=(const Action &action);

    /**
     * @brief Comparison operator
     *
     * This comparison operator compares the <b>names</b> of two
     * actions and returns whether they are the same. It does not
     * care about the arguments stored in the actions. However,
     * if two actions are invalid they'll match as equal, even
     * if the invalid names are different.
     *
     * @returns true if the two actions are the same or both invalid
     */
    bool operator==(const Action &action) const;

    /**
    * @brief Negated comparison operator
    *
    * Returns the negation of operator==
    *
    * @returns true if the two actions are different and not both invalid
    */
    bool operator!=(const Action &action) const;

    /**
     * @brief Gets the action's name.
     *
     * This is the unique attribute that identifies
     * an action object. Two action objects with the same
     * name always refer to the same action.
     *
     * @return The action name
     */
    QString name() const;

    /**
     * @brief Sets the action's name.
     *
     * It's not common to change the action name
     * after its creation. Usually you set the name
     * with the constructor (and you have to, because
     * there's no default constructor)
     */
    void setName(const QString &name);

    /**
     * @brief Gets the action's timeout.
     *
     * The timeout of the action in milliseconds
     * -1 means the default DBus timeout (usually 25 seconds)
     *
     * @since 5.29
     *
     * @return The action timeouts
     */
    int timeout() const;

    /**
     * @brief Sets the action's timeout.
     *
     * The timeout of the action in milliseconds
     * -1 means the default DBus timeout (usually 25 seconds)
     *
     * @since 5.29
     *
     */
    void setTimeout(int timeout);

    /**
     * @brief Sets the action's details
     *
     * You can use this function to provide the user more details
     * (if the backend supports it) on the action being authorized in
     * the authorization dialog
     */
    void setDetails(const QString &details);

    /**
     * @brief Gets the action's details
     *
     * The details that will be shown in the authorization dialog, if the
     * backend supports it.
     *
     * @return The action's details
     */
    QString details() const;

    /**
     * @brief Returns if the object represents a valid action
     *
     * Action names have to respect a simple syntax.
     * They have to be all in lowercase characters, separated
     * by dots. Dots can't appear at the beginning and at the end of
     * the name.
     *
     * In other words, the action name has to match this perl-like
     * regular expression:
     * @verbatim
     * /^[a-z]+(\.[a-z]+)*$/
     * @endverbatim
     *
     * This method returns false if the action name doesn't match the
     * valid syntax.
     *
     * If the backend supports it, this method also checks if the action is
     * valid and recognized by the backend itself.
     *
     * Invalid actions cannot be authorized nor executed.
     * The empty string is not a valid action name, so the default
     * constructor returns an invalid action.
     */
    bool isValid() const;

    /**
     * @brief Gets the default helper ID used for actions execution
     *
     * The helper ID is the string that uniquely identifies the helper in
     * the system. It is the string passed to the KAUTH_HELPER() macro
     * in the helper source. Because one could have different helpers,
     * you need to specify an helper ID for each execution, or set a default
     * ID by calling setHelperID(). This method returns the current default
     * value.
     *
     * @return The default helper ID.
     */
    QString helperId() const;

    /**
     * @brief Sets the default helper ID used for actions execution
     *
     * This method sets the helper ID which contains the body of this action.
     * If the string is non-empty, the corresponding helper will be fired and
     * the action executed inside the helper. Otherwise, the action will be just
     * authorized.
     *
     * @note To unset a previously set helper, just pass an empty string
     *
     * @param id The default helper ID.
     *
     * @see hasHelper
     * @see helperId
     */
    void setHelperId(const QString &id);

    /**
     * @brief Checks if the action has an helper
     *
     * This function can be used to check if an helper will be called upon the
     * execution of an action. Such an helper can be set through setHelperID. If
     * this function returns false, upon execution the action will be just authorized.
     *
     * @since 4.5
     *
     * @return Whether the action has an helper or not
     *
     * @see setHelperID
     */
    bool hasHelper() const;

    /**
     * @brief Sets the map object used to pass arguments to the helper.
     *
     * This method sets the variant map that the application
     * can use to pass arbitrary data to the helper when executing the action.
     *
     * @param arguments The new arguments map
     */
    void setArguments(const QVariantMap &arguments);

    /**
     * @brief Returns map object used to pass arguments to the helper.
     *
     * This method returns the variant map that the application
     * can use to pass arbitrary data to the helper when executing the action.
     *
     * @return The arguments map that will be passed to the helper.
     */
    QVariantMap arguments() const;

    /**
    * @brief Convenience method to add an argument.
    *
    * This method adds the pair @c key/value to the QVariantMap used to
    * send custom data to the helper.
    *
    * Use this method if you don't want to create a new QVariantMap only to
    * add a new entry.
    *
    * @param key The new entry's key
    * @param value The value of the new entry
    */
    void addArgument(const QString &key, const QVariant &value);

    /**
     * @brief Gets information about the authorization status of an action
     *
     * This methods query the authorization backend to know if the user can try
     * to acquire the authorization for this action. If the result is Action::AuthRequired,
     * the user can try to acquire the authorization by authenticating.
     *
     * It should not be needed to call this method directly, because the execution methods
     * already take care of all the authorization stuff.
     *
     * @return @c Action::Denied if the user doesn't have the authorization to execute the action,
     *         @c Action::Authorized if the action can be executed,
     *         @c Action::AuthRequired if the user could acquire the authorization after authentication,
     *         @c Action::UserCancelled if the user cancels the authentication dialog. Not currently supported by the Polkit backend
     */
    AuthStatus status() const;

    /**
     * @brief Synchronously executes the action
     *
     * This is the simpler of all the action execution methods. It sends an execution request to the
     * caller, and returns the reply directly to the caller. The ActionReply object will contain the
     * custom data coming from the helper.
     *
     * The method blocks the execution, and will
     * return only when the action has been completed (or failed). Take note, however, that with the D-Bus
     * helper proxy (currently the only one implemented on all the supported platforms), the request is
     * sent using the QDBus::BlockWithGui flag.
     *
     * This means the method will enter a local eventloop to wait
     * for the reply. This allows the application GUI to stay responsive, but you have to be prepared to
     * receive other events in the meantime.
     *
     * All the signals from the ActionWatcher class are emitted also with this method (although they're more
     * useful with the asynchronous calls)
     *
     * The method checks for authorization before to execute the action. If the user is not authorized, the
     * return value will be ActionReply::AuthorizationDeniedReply.
     * If the user cancels the authentication, the return value should be ActionReply::UserCancelledReply.
     * Due to policykit limitations, this currently only with the Mac OS X backend.
     *
     * If the helper is busy executing another action (or action group) the reply will be ActionReply::HelperBusyReply
     *
     * If the request cannot be sent for bus errors, the method returns ActionReply::DBusErrorReply.
     *
     * @return The reply from the helper, or an error reply if something's wrong.
     */
    ExecuteJob *execute(ExecutionMode mode = ExecuteMode);

    /**
     * @brief Sets a parent widget for the authentication dialog
     *
     * This function is used for explicitly setting a parent window for an eventual authentication dialog required when
     * authorization is triggered. Some backends, in fact, (like polkit-1) need to have a parent explicitly set for displaying
     * the dialog correctly.
     *
     * @note If you are using KAuth through one of KDE's GUI components (KPushButton, KCModule...) you do not need and should not
     *       call this function, as it is already done by the component itself.
     *
     * @since 4.6
     *
     * @param parent A QWidget which will be used as the dialog's parent
     */
    void setParentWidget(QWidget *parent);

    /**
     * @brief Returns the parent widget for the authentication dialog for this action
     *
     * @since 4.6
     *
     * @returns A QWidget which will is being used as the dialog's parent
     */
    QWidget *parentWidget() const;

private:
    QSharedDataPointer<ActionData> d;
};

} // namespace Auth

#endif
