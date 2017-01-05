/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
    Copyright (c) 1998, 1999 Waldo Bastian <bastian@kde.org>

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

#ifndef KAUTHORIZED_H
#define KAUTHORIZED_H

#include <kconfigcore_export.h>

class QUrl;
class QString;
class QStringList;

/**
* The functions in this namespace provide the core of the Kiosk action
* restriction system; the KIO and KXMLGui frameworks build on this.
*
* The relevant settings are read from the application's KSharedConfig
* instance, so actions can be disabled on a per-application or global
* basis (by using the kdeglobals file).
*/
namespace KAuthorized
{
/**
 * Returns whether the user is permitted to perform a certain action.
 *
 * All settings are read from the "[KDE Action Restrictions]" group.
 * For example, if kdeglobals contains
 * @verbatim
   [KDE Action Restrictions][$i]
   shell_access=false
   @endverbatim
 * then
 * @code
 * KAuthorized::authorize("shell_access");
 * @endcode
 * will return @c false.
 *
 * This method is intended for actions that do not necessarily have a
 * one-to-one correspondence with a menu or toolbar item (ie: a KAction
 * in a KXMLGui application).  "shell_access" is an example of such a
 * "generic" action.
 *
 * The convention for actions like "File->New" is to prepend the action
 * name with "action/", for example "action/file_new".  This is what
 * authorizeKAction() does.
 *
 * @param action  The name of the action.
 * @return        @c true if the action is authorized, @c false
 *                otherwise.
 *
 * @see authorizeKAction()
 */
KCONFIGCORE_EXPORT bool authorize(const QString &action);

/**
 * Returns whether the user is permitted to perform a certain action.
 *
 * This behaves like authorize(), except that "action/" is prepended to
 * @p action.  So if kdeglobals contains
 * @verbatim
   [KDE Action Restrictions][$i]
   action/file_new=false
   @endverbatim
 * then
 * @code
 * KAuthorized::authorizeAction("file_new");
 * @endcode
 * will return @c false.
 *
 * KXMLGui-based applications should not normally need to call this
 * function, as KActionCollection will do it automatically.
 *
 * @param action  The name of a QAction action.
 * @return        @c true if the QAction is authorized, @c false
 *                otherwise.
 * @since 5.24
 *
 * @see authorize()
 */
KCONFIGCORE_EXPORT bool authorizeAction(const QString &action);

/**
 * Returns whether the user is permitted to perform a certain action.
 *
 * This behaves like authorize(), except that "action/" is prepended to
 * @p action.  So if kdeglobals contains
 * @verbatim
   [KDE Action Restrictions][$i]
   action/file_new=false
   @endverbatim
 * then
 * @code
 * KAuthorized::authorizeKAction("file_new");
 * @endcode
 * will return @c false.
 *
 * KXMLGui-based applications should not normally need to call this
 * function, as KActionCollection will do it automatically.
 *
 * @param action  The name of a KAction action.
 * @return        @c true if the KAction is authorized, @c false
 *                otherwise.
 *
 * @see authorize()
 * @deprecated since 5.24, use authorizeAction() instead.
 */
#ifndef KDE_NO_DEPRECATED
KCONFIGCORE_DEPRECATED_EXPORT bool authorizeKAction(const QString &action);
#endif

/**
 * Returns whether the user is permitted to use a certain control
 * module.
 *
 * All settings are read from the "[KDE Control Module Restrictions]"
 * group.  For example, if kdeglobals contains
 * @verbatim
   [KDE Control Module Restrictions][$i]
   desktop-settings.desktop=false
   @endverbatim
 * then
 * @code
 * KAuthorized::authorizeControlModule("desktop-settings.desktop");
 * @endcode
 * will return @c false.
 *
 * @param menuId  The desktop menu ID for the control module.
 * @return        @c true if access to the module is authorized,
 *                @c false otherwise.
 *
 * @see authorizeControlModules()
 */
KCONFIGCORE_EXPORT bool authorizeControlModule(const QString &menuId);

/**
 * Determines which control modules from a list the user is permitted to
 * use.
 *
 * @param menuIds  A list of desktop menu IDs for control modules.
 * @return         The entries in @p menuIds for which
 *                 authorizeControlModule() returns @c true.
 *
 * @see authorizeControlModule()
 */
KCONFIGCORE_EXPORT QStringList authorizeControlModules(const QStringList &menuIds);

}

#endif
