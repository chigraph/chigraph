/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
   Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997 Matthias Kalle Dalheimer <kalle@kde.org>

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

#ifndef KCONFIGBASE_H
#define KCONFIGBASE_H

#include <kconfigcore_export.h>

#include <QtCore/QtGlobal>

class QStringList;
class KConfigGroup;
class KConfigBasePrivate;

/**
 * \class KConfigBase kconfigbase.h <KConfigBase>
 * \brief Interface to interact with configuration.
 *
 * KConfigBase allows a component of an application to persists its configuration
 * without the component knowing if it is storing the configuration into a top
 * level KConfig or a KConfigGroup inside a KConfig instance.
 */
class KCONFIGCORE_EXPORT KConfigBase
{
public:
    /**
     * Flags to control write entry
     */
    enum WriteConfigFlag {
        Persistent = 0x01,
        /**<
         * Save this entry when saving the config object.
         */
        Global = 0x02,
        /**<
         * Save the entry to the global %KDE config file instead of the
         * application specific config file.
         */
        Localized = 0x04,
        /**<
         * Add the locale tag to the key when writing it.
         */
        Normal = Persistent
                 /**<
                  * Save the entry to the application specific config file without
                  * a locale tag. This is the default.
                  */

    };
    Q_DECLARE_FLAGS(WriteConfigFlags, WriteConfigFlag)

    /**
     * Destructs the KConfigBase object.
     */
    virtual ~KConfigBase();

    /**
     * Returns a list of groups that are known about.
     *
     * @return The list of groups.
     **/
    virtual QStringList groupList() const = 0;

    /**
     * Returns true if the specified group is known about.
     *
     * @param group The group to search for.
     * @return true if the group exists.
     */
    bool hasGroup(const QString &group) const;
    bool hasGroup(const char *group) const;
    bool hasGroup(const QByteArray &group) const;

    /**
     * Returns an object for the named subgroup.
     *
     * @param group the group to open. Pass a null string on to the KConfig
     *   object to obtain a handle on the root group.
     * @return The list of groups.
     **/
    KConfigGroup group(const QByteArray &group);
    KConfigGroup group(const QString &group);
    KConfigGroup group(const char *group);

    /**
     * @overload
     **/
    const KConfigGroup group(const QByteArray &group) const;
    const KConfigGroup group(const QString &group) const;
    const KConfigGroup group(const char *group) const;

    /**
     * Delete @p aGroup. This marks @p aGroup as @em deleted in the config object. This effectively
     * removes any cascaded values from config files earlier in the stack.
     */
    void deleteGroup(const QByteArray &group, WriteConfigFlags flags = Normal);
    void deleteGroup(const QString &group, WriteConfigFlags flags = Normal);
    void deleteGroup(const char *group, WriteConfigFlags flags = Normal);

    /**
     * Syncs the configuration object that this group belongs to.
     * Unrelated concurrent changes to the same file are merged and thus
     * not overwritten. Note however, that this object is @em not automatically
     * updated with those changes.
     */
    virtual bool sync() = 0;

    /**
     * Reset the dirty flags of all entries in the entry map, so the
     * values will not be written to disk on a later call to sync().
     */
    virtual void markAsClean() = 0;

    /**
     * Possible return values for accessMode().
     */
    enum AccessMode { NoAccess, ReadOnly, ReadWrite };

    /**
     * Returns the access mode of the app-config object.
     *
     * Possible return values
     * are NoAccess (the application-specific config file could not be
     * opened neither read-write nor read-only), ReadOnly (the
     * application-specific config file is opened read-only, but not
     * read-write) and ReadWrite (the application-specific config
     * file is opened read-write).
     *
     * @return the access mode of the app-config object
     */
    virtual AccessMode accessMode() const = 0;

    /**
     * Checks whether this configuration object can be modified.
     * @return whether changes may be made to this configuration object.
     */
    virtual bool isImmutable() const = 0;

    /**
     * Can changes be made to the entries in @p aGroup?
     *
     * @param aGroup The group to check for immutability.
     * @return @c false if the entries in @p aGroup can be modified.
     */
    bool isGroupImmutable(const QByteArray &aGroup) const;
    bool isGroupImmutable(const QString &aGroup) const;
    bool isGroupImmutable(const char *aGroup) const;

protected:
    KConfigBase();

    virtual bool hasGroupImpl(const QByteArray &group) const = 0;
    virtual KConfigGroup groupImpl(const QByteArray &b) = 0;
    virtual const KConfigGroup groupImpl(const QByteArray &b) const = 0;
    virtual void deleteGroupImpl(const QByteArray &group, WriteConfigFlags flags = Normal) = 0;
    virtual bool isGroupImmutableImpl(const QByteArray &aGroup) const = 0;

    /** Virtual hook, used to add new "virtual" functions while maintaining
     * binary compatibility. Unused in this class.
     */
    virtual void virtual_hook(int id, void *data);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KConfigBase::WriteConfigFlags)

#endif // KCONFIG_H
