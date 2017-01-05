/*
 * This file is part of the KDE Libraries
 * Copyright (C) 2000 Timo Hummel <timo.hummel@sap.com>
 *                    Tom Braun <braunt@fh-konstanz.de>
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef KCRASH_H
#define KCRASH_H

#include <kcrash_export.h>

#include <qglobal.h>

class QString;

/**
 * This namespace contains functions to handle crashes.
 * It allows you to set a crash handler function that will be called
 * when your application crashes and also provides a default crash
 * handler that implements the following functionality:
 * @li Launches the KDE crash display application (DrKonqi) to let
 * the user report the bug and/or debug it.
 * @li Calls an emergency save function that you can set with
 * setEmergencySaveFunction() to attempt to save the application's data.
 * @li Autorestarts your application.
 *
 * @note All the above features are optional and you need to enable them
 * explicitly. By default, the defaultCrashHandler() will not do anything.
 * However, if you are using KApplication, it will by default enable launching
 * DrKonqi on crashes, unless the --nocrashhandler argument was passed on
 * the command line or the environment variable KDE_DEBUG is set to any value.
 */
namespace KCrash
{

/**
 * Initialize KCrash.
 *
 * This does nothing if $KDE_DEBUG is set.
 *
 * Call this in your main() to ensure that the crash handler is always launched.
 * @since 5.15
 */
KCRASH_EXPORT void initialize();

/**
 * The default crash handler.
 * Do not call this function directly. Instead, use
 * setCrashHandler() to set it as your application's crash handler.
 * @param signal the signal number
 * @note If you implement your own crash handler, you will have to
 * call this function from your implementation if you want to use the
 * features of this namespace.
 */
KCRASH_EXPORT void defaultCrashHandler(int signal);

/**
 * Typedef for a pointer to a crash handler function.
 * The function's argument is the number of the signal.
 */
typedef void (*HandlerType)(int);

/**
 * Install a function to be called when a crash occurs.
 * A crash occurs when one of the following signals is
 * caught: SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT.
 * @param handler this can be one of:
 * @li null, in which case signal catching is disabled
 * (by setting the signal handler for the crash signals to SIG_DFL)
 * @li a user defined function in the form:
 * static (if in a class) void myCrashHandler(int);
 * @li if handler is omitted, the default crash handler is installed
 * @note If you use setDrKonqiEnabled(true), setEmergencySaveFunction(myfunc)
 * or setFlags(AutoRestart), you do not need to call this function
 * explicitly. The default crash handler is automatically installed by
 * those functions if needed. However, if you set a custom crash handler,
 * those functions will not change it.
 */
KCRASH_EXPORT void setCrashHandler(HandlerType handler = defaultCrashHandler);

/**
 * Returns the installed crash handler.
 * @return the crash handler
 */
KCRASH_EXPORT HandlerType crashHandler();

/**
 * Installs a function which should try to save the application's data.
 * @note It is the crash handler's responsibility to call this function.
 * Therefore, if no crash handler is set, the default crash handler
 * is installed to ensure the save function will be called.
 * @param saveFunction the handler to install
 */
KCRASH_EXPORT void setEmergencySaveFunction(HandlerType saveFunction = 0);

/**
 * Returns the currently set emergency save function.
 * @return the emergency save function
 */
KCRASH_EXPORT HandlerType emergencySaveFunction();

/**
 * Options to determine how the default crash handler should behave.
 */
enum CrashFlag {
    KeepFDs = 1,          ///< don't close all file descriptors immediately
    SaferDialog = 2,      ///< start DrKonqi without arbitrary disk access
    AlwaysDirectly = 4,   ///< never try to to start DrKonqi via kdeinit. Use fork() and exec() instead.
    AutoRestart = 8       ///< autorestart this application. Only sensible for KUniqueApplications. @since 4.1.
};
Q_DECLARE_FLAGS(CrashFlags, CrashFlag)

/**
 * Set options to determine how the default crash handler should behave.
 * @param flags ORed together CrashFlags
 */
KCRASH_EXPORT void setFlags(KCrash::CrashFlags flags);

/**
 * Enables or disables launching DrKonqi from the crash handler.
 * By default, launching DrKonqi is enabled when QCoreApplication is created.
 * To disable it:
 * @code
 void disableDrKonqi()
 {
     KCrash::setDrKonqiEnabled(false);
 }
 Q_CONSTRUCTOR_FUNCTION(disableDrKonqi)
 * \endcode
 * @note It is the crash handler's responsibility to launch DrKonqi.
 * Therefore, if no crash handler is set, this method also installs
 * the default crash handler to ensure that DrKonqi will be launched.
 * @since 4.5
 */
KCRASH_EXPORT void setDrKonqiEnabled(bool enabled);

/**
 * Returns true if DrKonqi is set to be launched from the crash handler or false otherwise.
 * @since 4.5
 */
KCRASH_EXPORT bool isDrKonqiEnabled();
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KCrash::CrashFlags)

#endif

