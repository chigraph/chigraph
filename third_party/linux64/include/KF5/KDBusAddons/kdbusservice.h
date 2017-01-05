/* This file is part of libkdbusaddons

   Copyright (c) 2011 David Faure <faure@kde.org>
   Copyright (c) 2011 Kevin Ottens <ervin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KDBUSSERVICE_H
#define KDBUSSERVICE_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <kdbusaddons_export.h>

class KDBusServicePrivate;

/**
 * KDBusService takes care of registering the current process with D-Bus.
 *
 * This registers the application at a predictable location on D-Bus, registers
 * the QCoreApplication (or subclass) object at /MainApplication, and
 * assists in implementing the application side of D-Bus activation from
 * the <a
 * href="http://standards.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html">Desktop
 * Entry Specification</a>.
 *
 * An application can either work in Multiple mode or Unique mode.
 *
 * In Multiple mode, the application can be launched many times.  The service
 * name in the D-Bus registration will contain the PID to distinguish the
 * various instances; for example: <tt>org.kde.konqueror-12345</tt>.
 *
 * In Unique mode, only one instance of this application can ever run.
 * The first instance of the application registers with D-Bus without the PID,
 * and any attempt to run the application again will cause the
 * activateRequested() signal to be emitted in the already-running instance; the
 * duplicate instance will then quit. The exit value can be set by the already
 * running instance with setExitValue(), the default value is @c 0.
 *
 * Unique-mode applications should usually delay parsing command-line arguments
 * until after creating a KDBusService object; that way they know they are the
 * original instance of the application.
 *
 * Applications that set the D-Bus activation entries in their desktop files
 * should use Unique mode and connect to the signals emitted by this class.
 * Note that the D-Bus interface is exported for Multiple-mode applications as
 * well, so it also makes sense for such applications to connect to the signals
 * emitted by this class.
 *
 * @note In order to avoid a race, the application should export its objects to
 * D-Bus before allowing the event loop to run (for example, by calling
 * QCoreApplication::exec()).  Otherwise, the application will appear on the bus
 * before its objects are accessible via D-Bus, which could be a problem for
 * other applications or scripts which start the application in order to talk
 * D-Bus to it immediately.
 *
 * Example usage:
 *
 * @code
     QApplication app(argc, argv);
     app.setApplicationName("kuiserver");
     app.setOrganizationDomain("kde.org");
     // Create your dbus objects here
     // ...
     KDBusService service(KDBusService::Unique);
     // If this point is reached, this is the only running instance
     // i.e. org.kde.kuiserver has been registered
     return app.exec();
 * @endcode
 *
 * @since 5.0
 */
class KDBUSADDONS_EXPORT KDBusService : public QObject
{
    Q_OBJECT

public:
    /** Options to control the behaviour of KDBusService */
    enum StartupOption {
        /** Indicates that only one instance of this application should ever
         * exist.
         *
         * Cannot be combined with @c Multiple.
         */
        Unique = 1,
        /** Indicates that multiple instances of the application may exist.
         *
         * Cannot be combined with @c Unique.  This is the default.
         */
        Multiple = 2,
        /** Indicates that the application should not exit if it failed to
         * register with D-Bus.
         *
         * If not set, KDBusService will quit the application if it failed to
         * register the service with D-Bus or a @c Unique instance can not be
         * activated. A @c Multiple instance will exit with error code @c 1.
         * The exit value of a @c Unique instance can be set from the running
         * instance with setExitValue(), the default value is @c 0.
         */
        NoExitOnFailure = 4
    };
    Q_ENUM(StartupOption)

    Q_DECLARE_FLAGS(StartupOptions, StartupOption)
    Q_FLAG(StartupOptions)

    /**
     * Tries to register the current process to D-Bus at an address based on the
     * application name and organization domain.
     *
     * The DBus service name is the reversed organization domain, followed by
     * the application name.  If @p options includes the @c Multiple flag, the
     * application PID will be appended.  For example,
     * @code
     * app.setApplicationName("kuiserver");
     * app.setOrganizationDomain("kde.org");
     * @endcode
     * will make KDBusService register as @c org.kde.kuiserver in @c Unique
     * mode, and @c org.kde.kuiserver-1234 (if the process has PID @c 1234) in
     * @c Multiple mode.
     */
    explicit KDBusService(StartupOptions options = Multiple, QObject *parent = 0);

    /**
     * Destroys this object (but does not unregister the application).
     *
     * Deleting this object before unregister() is called (either manually or
     * because QCoreApplication::aboutToQuit() was emitted) could confuse
     * clients, who will see the service on the bus but will be unable to use
     * the activation methods.
     */
    ~KDBusService();

    /**
     * Returns true if the D-Bus registration succeeded.
     *
     * Note that this is only useful when specifying the option NoExitOnFailure.
     * Otherwise, the simple fact that this process is still running indicates
     * that the registration succeeded.
     */
    bool isRegistered() const;

    /**
     * Returns the error message from the D-Bus registration if it failed.
     *
     * Note that this is only useful when specifying the option NoExitOnFailure.
     * Otherwise the process has quit by the time you can get a chance to call this.
     */
    QString errorMessage() const;

    /**
     * Sets the exit value to be used for a duplicate instance.
     *
     * If this is a @c Unique application, a slot connected to
     * activateRequested() can use this to specify a non-zero exit value for the
     * duplicate instance.  This would typically be done if invalid command-line
     * arguments are passed.
     *
     * Note that this will only work if the signal-slot connection type is
     * Qt::DirectConnection.
     *
     * @param value  The exit value for the duplicate instance.
     */
    void setExitValue(int value);

Q_SIGNALS:
    /**
     * Signals that the application is to be activated.
     *
     * If this is a @c Unique application, when KDBusService is constructed in
     * subsequent instances of the application (ie: when the executable is run
     * when an instance is already running), it will cause this signal to be
     * emitted in the already-running instance (with the arguments passed to the
     * duplicate instance), and the duplicate instance will then exit.
     *
     * If this application's desktop file indicates that it supports D-Bus
     * activation (DBusActivatable=true), a command launcher may also call the Activate()
     * D-Bus method to trigger this signal. In this case, @p args will be empty.
     *
     * In single-window applications, the connected signal should typically
     * raise the window.
     *
     * @param arguments  The arguments the executable was called with.
     *                   See QCoreApplication::arguments().
     *
     * A typical implementation of the slot would be
     * @code
     *    commandLineParser->parse(arguments); // same QCommandLineParser instance as the one used in main()
     *    handleCmdLine(workingDirectory); // shared method with main(), which uses commandLineParser to handle options and positional arguments
     *    // and for GUI applications, also terminate startup notification and activate the mainwindow:
     *    KStartupInfo::setNewStartupId(mainWindow, KStartupInfo::startupId());
     *    KWindowSystem::forceActiveWindow(mainWindow->winId());
     * @endcode
     *
     * @see setExitValue()
     */
    void activateRequested(const QStringList &arguments, const QString &workingDirectory);

    /**
     * Signals that one or more files should be opened in the application.
     *
     * @param uris  The URLs of the files to open.
     */
    void openRequested(const QList<QUrl> &uris);

    /**
     * Signals that an application action should be triggered.
     *
     * See the desktop entry specification for more information.
     */
    void activateActionRequested(const QString &actionName, const QVariant &parameter);

public Q_SLOTS:
    /**
     * Unregister from D-Bus.
     *
     * This is called automatically when the application is about to quit, to
     * make sure it doesn't keep receiving calls to its D-Bus interface while it
     * is doing final cleanups.
     */
    void unregister();

private:
    // fdo.Application spec
    void Activate(const QVariantMap &platform_data);
    void Open(const QStringList &uris, const QVariantMap &platform_data);
    void ActivateAction(const QString &action_name, const QVariantList &maybeParameter, const QVariantMap &platform_data);
    friend class KDBusServiceAdaptor;
    // org.kde.KDBusService
    int CommandLine(const QStringList &arguments, const QString &workingDirectory, const QVariantMap &platform_data);
    friend class KDBusServiceExtensionsAdaptor;

private:
    KDBusServicePrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KDBusService::StartupOptions)

#endif /* KDBUSSERVICE_H */
