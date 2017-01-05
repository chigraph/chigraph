/* This file is part of the KDE libraries
    Copyright (c) 1997-1999 Matthias Kalle Dalheimer <kalle@kde.org>
    Copyright (c) 1997-2000 Matthias Ettrich <ettrich@troll.no>
    Copyright (c) 1998-2005 Stephan Kulow <coolo@kde.org>
    Copyright (c) 1999-2004 Waldo Bastian <bastian@kde.org>
    Copyright (c) 2001-2005 Lubos Lunak <l.lunak@kde.org>
    Copyright (C) 2008 Aaron Seigo <aseigo@kde.org>

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

#ifndef _KTOOLINVOCATION_H
#define _KTOOLINVOCATION_H

#include <kservice_export.h>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QStringList>

class QUrl;
class KToolInvocationPrivate;

/**
 * KToolInvocation: for starting other programs
 *
 * @section desktopfiles Desktop files for startServiceBy
 *
 * The way a service gets started depends on the 'X-DBUS-StartupType'
 * entry in the desktop file of the service:
 *
 * There are three possibilities:
 * @li X-DBUS-StartupType=None (default)
 *    Always start a new service,
 *    don't wait till the service registers with D-Bus.
 * @li X-DBUS-StartupType=Multi
 *    Always start a new service,
 *    wait until the service has registered with D-Bus.
 * @li X-DBUS-StartupType=Unique
 *    Only start the service if it isn't already running,
 *    wait until the service has registered with D-Bus.
 * The .desktop file can specify the name that the application will use when registering
 * using X-DBUS-ServiceName=org.domain.mykapp. Otherwise org.kde.binaryname is assumed.
 *
 * @section thread Multi-threading
 *
 * The static members (apart from self()), have to be called from the QApplication main thread.
 * Calls to members are only allowed if there is a Q(Core)Application object created
 * If you call the members with signal/slot connections across threads, you can't use the return values
 * If a function is called from the wrong thread and it has a return value -1 is returned
 * Investigate if this is really needed or if D-Bus is threadsafe anyway
 *
 * For more details see <a
 * href="http://techbase.kde.org/Development/Architecture/KDE4/Starting_Other_Programs#KToolInvocation::startServiceByDesktopPath">techbase</a>.
 *
 */
class KSERVICE_EXPORT KToolInvocation : public QObject
{

    Q_OBJECT
private:
    KToolInvocation();
public:
    // @internal
    ~KToolInvocation();
    static KToolInvocation *self();

public Q_SLOTS:

    /**
     * Convenience method; invokes the standard email application.
     *
     * @param address The destination address
     * @param subject Subject string. Can be QString().
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     *
     * @deprecated since 5.0, use QDesktopServices::openUrl(mailtoURL),
     * using QUrl::setPath(address) and a query item of "subject" for the subject.
     */
    KSERVICE_DEPRECATED static void invokeMailer(const QString &address, const QString &subject,
                             const QByteArray &startup_id = QByteArray());

    /**
     * Invokes the standard email application.
     *
     * @param mailtoURL A mailto URL.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param allowAttachments whether attachments specified in mailtoURL should be honoured.
     *           The default is false; do not honor requests for attachments.
     * @deprecated since 5.0, use QDesktopServices::openUrl(mailtoURL)
     */
    KSERVICE_DEPRECATED static void invokeMailer(const QUrl &mailtoURL, const QByteArray &startup_id = QByteArray(),
                             bool allowAttachments = false);

    /**
     * Convenience method; invokes the standard email application.
     *
     * All parameters are optional.
     *
     * @param to          The destination address.
     * @param cc          The Cc field
     * @param bcc         The Bcc field
     * @param subject     Subject string
     * @param body        A string containing the body of the mail (exclusive with messageFile)
     * @param messageFile A file (URL) containing the body of the mail (exclusive with body) - currently unsupported
     * @param attachURLs  List of URLs to be attached to the mail.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     */
    static void invokeMailer(const QString &to, const QString &cc, const QString &bcc,
                             const QString &subject, const QString &body,
                             const QString &messageFile = QString(),
                             const QStringList &attachURLs = QStringList(),
                             const QByteArray &startup_id = QByteArray());

    /**
     * Invokes the user's preferred browser.
     * Note that you should only do this when you know for sure that the browser can
     * handle the URL (i.e. its mimetype). In doubt, if the URL can point to an image
     * or anything else than HTML, prefer to use new KRun( url ).
     *
     * See also <a
     * href="http://techbase.kde.org/Development/Architecture/KDE4/Starting_Other_Programs#KToolInvocation::invokeBrowser>techbase</a>
     * for a discussion of invokeBrowser vs KRun.
     *
     * @param url The destination address
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @deprecated since 5.0, use QDesktopServices::openUrl(url)
     */
    KSERVICE_DEPRECATED static void invokeBrowser(const QString &url,
                                                  const QByteArray &startup_id = QByteArray());

    /**
     * Invokes the standard terminal application.
     *
     * @param command the command to execute, can be empty.
     * @param workdir the initial working directory, can be empty.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     *
     * @since 4.1
     */
    static void invokeTerminal(const QString &command,
                               const QString &workdir = QString(),
                               const QByteArray &startup_id = "");

public:

    /**
     * Starts a service based on the (translated) name of the service.
     * E.g. "Web Browser"
     *
     * @param _name the name of the service
     * @param URL if not empty this URL is passed to the service
     * @param error On failure, error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param serviceName On success, serviceName contains the DCOP name
     *         under which this service is available. If empty, the service does
     *         not provide DCOP services. If the pointer is 0 the argument
     *         will be ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param noWait if set, the function does not wait till the service is running.
     * @return an error code indicating success (== 0) or failure (> 0).
     * @deprecated Use startServiceByDesktopName or startServiceByDesktopPath
     */
#ifndef KSERVICE_NO_DEPRECATED
    KSERVICE_DEPRECATED static int startServiceByName(const QString &_name, const QString &URL,
            QString *error = 0, QString *serviceName = 0, int *pid = 0,
            const QByteArray &startup_id = QByteArray(), bool noWait = false);
#endif

    /**
     * Starts a service based on the (translated) name of the service.
     * E.g. "Web Browser"
     *
     * @param _name the name of the service
     * @param URLs if not empty these URLs will be passed to the service
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param serviceName On success, @p serviceName contains the DCOP name
     *         under which this service is available. If empty, the service does
     *         not provide DCOP services. If the pointer is 0 the argument
     *         will be ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param noWait if set, the function does not wait till the service is running.
     * @return an error code indicating success (== 0) or failure (> 0).
     * @deprecated Use startServiceByDesktopName or startServiceByDesktopPath
     */
#ifndef KSERVICE_NO_DEPRECATED
    KSERVICE_DEPRECATED static int startServiceByName(const QString &_name, const QStringList &URLs = QStringList(),
            QString *error = 0, QString *serviceName = 0, int *pid = 0,
            const QByteArray &startup_id = QByteArray(), bool noWait = false);
#endif

    /**
     * Starts a service based on the desktop path of the service.
     * E.g. "Applications/konqueror.desktop" or "/home/user/bla/myfile.desktop"
     *
     * @param _name the path of the desktop file
     * @param URL if not empty this URL is passed to the service
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param serviceName On success, @p serviceName contains the DCOP name
     *         under which this service is available. If empty, the service does
     *         not provide DCOP services. If the pointer is 0 the argument
     *         will be ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param noWait if set, the function does not wait till the service is running.
     * @return an error code indicating success (== 0) or failure (> 0).
     *
     * @deprecated since 5.0 use QDBusConnectionInterface::startService("org.kde.serviceName"),
     *   to start a unique application in order to make dbus calls to it (after ensuring that
     *   it installs a dbus org.kde.serviceName.service file). Otherwise just use QProcess or KRun.
     */
    static int startServiceByDesktopPath(const QString &_name, const QString &URL,
                                         QString *error = 0, QString *serviceName = 0, int *pid = 0,
                                         const QByteArray &startup_id = QByteArray(), bool noWait = false);

    /**
     * Starts a service based on the desktop path of the service.
     * E.g. "Applications/konqueror.desktop" or "/home/user/bla/myfile.desktop"
     *
     * @param _name the path of the desktop file
     * @param URLs if not empty these URLs will be passed to the service
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored   * @param serviceName On success, @p serviceName contains the DCOP name
     *         under which this service is available. If empty, the service does
     *         not provide DCOP services. If the pointer is 0 the argument
     *         will be ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param noWait if set, the function does not wait till the service is running.
     * @return an error code indicating success (== 0) or failure (> 0).
     * @deprecated since 5.0 use QDBusConnectionInterface::startService("org.kde.serviceName"),
     *   to start a unique application in order to make dbus calls to it (after ensuring that
     *   it installs a dbus org.kde.serviceName.service file). Otherwise just use QProcess or KRun.
     */
    static int startServiceByDesktopPath(const QString &_name, const QStringList &URLs = QStringList(),
                                         QString *error = 0, QString *serviceName = 0, int *pid = 0,
                                         const QByteArray &startup_id = QByteArray(), bool noWait = false);

    /**
     * Starts a service based on the desktop name of the service.
     * E.g. "konqueror"
     *
     * @param _name the desktop name of the service
     * @param URL if not empty this URL is passed to the service
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param serviceName On success, @p serviceName contains the D-Bus service name
     *         under which this service is available. If empty, the service does
     *         not provide D-Bus services. If the pointer is 0 the argument
     *         will be ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param noWait if set, the function does not wait till the service is running.
     * @return an error code indicating success (== 0) or failure (> 0).
     * @deprecated since 5.0 use QDBusConnectionInterface::startService("org.kde.serviceName"),
     *   to start a unique application in order to make dbus calls to it (after ensuring that
     *   it installs a dbus org.kde.serviceName.service file). Otherwise just use QProcess or KRun.
     */
    static int startServiceByDesktopName(const QString &_name, const QString &URL,
                                         QString *error = 0, QString *serviceName = 0, int *pid = 0,
                                         const QByteArray &startup_id = QByteArray(), bool noWait = false);

    /**
     * Starts a service based on the desktop name of the service.
     * E.g. "konqueror"
     *
     * @param _name the desktop name of the service
     * @param URLs if not empty these URLs will be passed to the service
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param serviceName On success, @p serviceName contains the D-Bus service name
     *         under which this service is available. If empty, the service does
     *         not provide D-Bus services. If the pointer is 0 the argument
     *         will be ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @param noWait if set, the function does not wait till the service is running.
     * @return an error code indicating success (== 0) or failure (> 0).
     * @deprecated since 5.0 use QDBusConnectionInterface::startService("org.kde.serviceName"),
     *   to start a unique application in order to make dbus calls to it (after ensuring that
     *   it installs a dbus org.kde.serviceName.service file). Otherwise just use QProcess or KRun.
     */
    static int startServiceByDesktopName(const QString &_name, const QStringList &URLs = QStringList(),
                                         QString *error = 0, QString *serviceName = 0, int *pid = 0,
                                         const QByteArray &startup_id = QByteArray(), bool noWait = false);

    /**
     * Starts a program via kdeinit.
     *
     * program name and arguments are converted to according to the
     * local encoding and passed as is to kdeinit.
     *
     * @param name Name of the program to start
     * @param args Arguments to pass to the program
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @return an error code indicating success (== 0) or failure (> 0).
     */
    static int kdeinitExec(const QString &name, const QStringList &args = QStringList(),
                           QString *error = 0, int *pid = 0, const QByteArray &startup_id = QByteArray());

    /**
     * Starts a program via kdeinit and wait for it to finish.
     *
     * Like kdeinitExec(), but it waits till the program is finished.
     * As such it behaves similar to the system(...) function.
     *
     * @param name Name of the program to start
     * @param args Arguments to pass to the program
     * @param error On failure, @p error contains a description of the error
     *         that occurred. If the pointer is 0, the argument will be
     *         ignored
     * @param pid On success, the process id of the new service will be written
     *        here. If the pointer is 0, the argument will be ignored.
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @return an error code indicating success (== 0) or failure (> 0).
     */
    static int kdeinitExecWait(const QString &name, const QStringList &args = QStringList(),
                               QString *error = 0, int *pid = 0, const QByteArray &startup_id = QByteArray());

    /**
     * Ensures that kdeinit5 and klauncher are running.
     */
    static void ensureKdeinitRunning();

Q_SIGNALS:
    /**
     * Hook for KApplication in kdeui
     * @internal
     */
    void kapplication_hook(QStringList &env, QByteArray &startup_id);

private:
    int startServiceInternal(const char *_function,
                             const QString &_name, const QStringList &URLs,
                             QString *error, QString *serviceName, int *pid,
                             const QByteArray &startup_id, bool noWait,
                             const QString &workdir = QString());
    static bool isMainThreadActive(QString *error = 0);

    KToolInvocationPrivate *const d;
    friend class KToolInvocationSingleton;
};

#endif

