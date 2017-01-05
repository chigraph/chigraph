/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997 Matthias Kalle Dalheimer <kalle@kde.org>
   Copyright (c) 2001 Waldo Bastian <bastian@kde.org>

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

#ifndef KCONFIGGROUP_H
#define KCONFIGGROUP_H

#include "kconfigbase.h"

#include <kconfigcore_export.h>

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QVariant>
#include <QtCore/QStringList>

class KConfig;
class KConfigGroupPrivate;
class KSharedConfig;

/**
 * \class KConfigGroup kconfiggroup.h <KConfigGroup>
 *
 * A class for one specific group in a KConfig object.
 *
 * If you want to access the top-level entries of a KConfig
 * object, which are not associated with any group, use an
 * empty group name.
 *
 * A KConfigGroup will be read-only if it is constructed from a
 * const config object or from another read-only group.
 */
class KCONFIGCORE_EXPORT KConfigGroup : public KConfigBase
{
public:
    /**
     * Constructs an invalid group.
     *
     * \see isValid
     */
    KConfigGroup();

    /**
     * Construct a config group corresponding to @p group in @p master.
     *
     * This allows the creation of subgroups by passing another
     * group as @p master.
     *
     * @p group is the group name encoded in UTF-8.
     */
    KConfigGroup(KConfigBase *master, const QString &group);
    /** Overload for KConfigGroup(KConfigBase*,const QString&) */
    KConfigGroup(KConfigBase *master, const char *group);

    /**
     * Construct a read-only config group.
     *
     * A read-only group will silently ignore any attempts to write to it.
     *
     * This allows the creation of subgroups by passing an existing group
     * as @p master.
     */
    KConfigGroup(const KConfigBase *master, const QString &group);
    /** Overload for KConfigGroup(const KConfigBase*,const QString&) */
    KConfigGroup(const KConfigBase *master, const char *group);

    /** Overload for KConfigGroup(const KConfigBase*,const QString&) */
    KConfigGroup(const QExplicitlySharedDataPointer<KSharedConfig> &master, const QString &group);
    /** Overload for KConfigGroup(const KConfigBase*,const QString&) */
    KConfigGroup(const QExplicitlySharedDataPointer<KSharedConfig> &master, const char *group);

    /**
     * Creates a copy of a group.
     */
    KConfigGroup(const KConfigGroup &);
    KConfigGroup &operator=(const KConfigGroup &);

    ~KConfigGroup();

    /**
     * Whether the group is valid.
     *
     * A group is invalid if it was constructed without arguments.
     *
     * You should not call any functions on an invalid group.
     *
     * @return @c true if the group is valid, @c false if it is invalid.
     */
    bool isValid() const;

    /**
     * The name of this group.
     *
     * The root group is named "<default>".
     */
    QString name() const;

    /**
     * Check whether the containing KConfig object acutally contains a
     * group with this name.
     */
    bool exists() const;

    /**
     * @reimp
     *
     * Syncs the parent config.
     */
    bool sync() Q_DECL_OVERRIDE;

    /// @reimp
    void markAsClean() Q_DECL_OVERRIDE;

    /// @reimp
    AccessMode accessMode() const Q_DECL_OVERRIDE;

    /**
     * Return the config object that this group belongs to
     */
    KConfig *config();
    /**
     * Return the config object that this group belongs to
     */
    const KConfig *config() const;

    /**
     * Changes the group of the object
     *
     * @deprecated
     * Create another KConfigGroup from the parent of this group instead.
     */
#ifndef KDE_NO_DEPRECATED
    KCONFIGCORE_DEPRECATED void changeGroup(const QString &group);
#endif
    /**
     * Overload for changeGroup(const QString&)
     *
     * @deprecated
     * Create another KConfigGroup from the parent of this group instead.
     */
#ifndef KDE_NO_DEPRECATED
    KCONFIGCORE_DEPRECATED void changeGroup(const char *group);
#endif

    /**
     * Copies the entries in this group to another configuration object
     *
     * @note @p other can be either another group or a different file.
     *
     * @param other  the configuration object to copy this group's entries to
     * @param pFlags the flags to use when writing the entries to the
     *               other configuration object
     *
     * @since 4.1
     */
    void copyTo(KConfigBase *other, WriteConfigFlags pFlags = Normal) const;

    /**
     * Changes the configuration object that this group belongs to
     *
     * @note @p other can be another group, the top-level KConfig object or
     * a different KConfig object entirely.
     *
     * If @p parent is already the parent of this group, this method will have
     * no effect.
     *
     * @param parent the config object to place this group under
     * @param pFlags the flags to use in determining which storage source to
     *               write the data to
     *
     * @since 4.1
     */
    void reparent(KConfigBase *parent, WriteConfigFlags pFlags = Normal);

    /**
     * Returns the group that this group belongs to
     *
     * @return the parent group, or an invalid group if this is a top-level
     *          group
     *
     * @since 4.1
     */
    KConfigGroup parent() const;

    /**
     * @reimp
     */
    QStringList groupList() const Q_DECL_OVERRIDE;

    /**
     * Returns a list of keys this group contains
     */
    QStringList keyList() const;

    /**
     * Delete all entries in the entire group
     *
     * @param pFlags flags passed to KConfig::deleteGroup
     *
     * @see deleteEntry()
     */
    void deleteGroup(WriteConfigFlags pFlags = Normal);
    using KConfigBase::deleteGroup;

    /**
     * Reads the value of an entry specified by @p pKey in the current group
     *
     * This template method makes it possible to write
     *    QString foo = readEntry("...", QString("default"));
     * and the same with all other types supported by QVariant.
     *
     * The return type of the method is simply the same as the type of the default value.
     *
     * @note readEntry("...", Qt::white) will not compile because Qt::white is an enum.
     * You must turn it into readEntry("...", QColor(Qt::white)).
     *
     * @note Only the following QVariant types are allowed : String,
     * StringList, List, Font, Point, Rect, Size, Color, Int, UInt, Bool,
     * Double, LongLong, ULongLong, DateTime and Date.
     *
     * @param key The key to search for
     * @param aDefault A default value returned if the key was not found
     * @return The value for this key, or @p aDefault.
     *
     * @see writeEntry(), deleteEntry(), hasKey()
     */
    template <typename T>
    T readEntry(const QString &key, const T &aDefault) const
    {
        return readEntry(key.toUtf8().constData(), aDefault);
    }
    /** Overload for readEntry(const QString&, const T&) const */
    template <typename T>
    T readEntry(const char *key, const T &aDefault) const;

    /**
     * Reads the value of an entry specified by @p key in the current group
     *
     * @param key the key to search for
     * @param aDefault a default value returned if the key was not found
     * @return the value for this key, or @p aDefault if the key was not found
     *
     * @see writeEntry(), deleteEntry(), hasKey()
     */
    QVariant readEntry(const QString &key, const QVariant &aDefault) const;
    /** Overload for readEntry(const QString&, const QVariant&) */
    QVariant readEntry(const char *key, const QVariant &aDefault) const;

    /**
     * Reads the string value of an entry specified by @p key in the current group
     *
     * If you want to read a path, please use readPathEntry().
     *
     * @param key the key to search for
     * @param aDefault a default value returned if the key was not found
     * @return the value for this key, or @p aDefault if the key was not found
     *
     * @see readPathEntry(), writeEntry(), deleteEntry(), hasKey()
     */
    QString readEntry(const QString &key, const QString &aDefault) const;
    /** Overload for readEntry(const QString&, const QString&) */
    QString readEntry(const char *key, const QString &aDefault) const;

    /** Overload for readEntry(const QString&, const QString&) */
    QString readEntry(const QString &key, const char *aDefault = Q_NULLPTR) const;
    /** Overload for readEntry(const QString&, const QString&) */
    QString readEntry(const char *key, const char *aDefault = Q_NULLPTR) const;

    /**
     * @copydoc readEntry(const char*, const QStringList&) const
     *
     * @warning This function doesn't convert the items returned
     *          to any type. It's actually a list of QVariant::String's. If you
     *          want the items converted to a specific type use
     *          readEntry(const char*, const QList<T>&) const
     */
    QVariantList readEntry(const QString &key, const QVariantList &aDefault) const;
    /** Overload for readEntry(const QString&, const QVariantList&) */
    QVariantList readEntry(const char *key, const QVariantList &aDefault) const;

    /**
     * Reads a list of strings from the config object
     *
     * @param key The key to search for
     * @param aDefault The default value to use if the key does not exist
     * @return The list, or @p aDefault if @p key does not exist
     *
     * @see readXdgListEntry(), writeEntry(), deleteEntry(), hasKey()
     */
    QStringList readEntry(const QString &key, const QStringList &aDefault) const;
    /** Overload for readEntry(const QString&, const QStringList&) */
    QStringList readEntry(const char *key, const QStringList &aDefault) const;

    /**
     * Reads a list of values from the config object
     *
     * @param key the key to search for
     * @param aDefault the default value to use if the key does not exist
     * @return the list, or @p aDefault if @p key does not exist
     *
     * @see readXdgListEntry(), writeEntry(), deleteEntry(), hasKey()
     */
    template<typename T>
    QList<T> readEntry(const QString &key, const QList<T> &aDefault) const
    {
        return readEntry(key.toUtf8().constData(), aDefault);
    }
    /** Overload for readEntry(const QString&, const QList<T>&) */
    template<typename T>
    QList<T> readEntry(const char *key, const QList<T> &aDefault) const;

    /**
     * Reads a list of strings from the config object, following XDG
     * desktop entry spec separator semantics
     *
     * @param pKey the key to search for
     * @param aDefault the default value to use if the key does not exist
     * @return the list, or @p aDefault if @p pKey does not exist
     *
     * @see readEntry(const QString&, const QStringList&) const
     */
    QStringList readXdgListEntry(const QString &pKey, const QStringList &aDefault = QStringList()) const;
    /** Overload for readXdgListEntry(const QString&, const QStringList&) */
    QStringList readXdgListEntry(const char *pKey, const QStringList &aDefault = QStringList()) const;

    /**
     * Reads a path
     *
     * Read the value of an entry specified by @p pKey in the current group
     * and interpret it as a path. This means, dollar expansion is activated
     * for this value, so that e.g. $HOME gets expanded.
     *
     * @param pKey The key to search for.
     * @param aDefault A default value returned if the key was not found.
     * @return The value for this key. Can be QString() if @p aDefault is null.
     */
    QString readPathEntry(const QString &pKey, const QString &aDefault) const;
    /** Overload for readPathEntry(const QString&, const QString&) */
    QString readPathEntry(const char *key, const QString &aDefault) const;

    /**
     * Reads a list of paths
     *
     * Read the value of an entry specified by @p pKey in the current group
     * and interpret it as a list of paths. This means, dollar expansion is activated
     * for this value, so that e.g. $HOME gets expanded.
     *
     * @param pKey the key to search for
     * @param aDefault a default value returned if the key was not found
     * @return the list, or @p aDefault if the key does not exist
     */
    QStringList readPathEntry(const QString &pKey, const QStringList &aDefault) const;
    /** Overload for readPathEntry(const QString&, const QStringList&) */
    QStringList readPathEntry(const char *key, const QStringList &aDefault) const;

    /**
     * Reads an untranslated string entry
     *
     * You should not normally need to use this.
     *
     * @param pKey the key to search for
     * @param aDefault a default value returned if the key was not found
     * @return the value for this key, or @p aDefault if the key does not exist
     */
    QString readEntryUntranslated(const QString &pKey,
                                  const QString &aDefault = QString()) const;
    /** Overload for readEntryUntranslated(const QString&, const QString&) */
    QString readEntryUntranslated(const char *key,
                                  const QString &aDefault = QString()) const;

    /**
     * Writes a value to the configuration object.
     *
     * @param key the key to write to
     * @param value the value to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see readEntry(), writeXdgListEntry(), deleteEntry()
     */
    void writeEntry(const QString &key, const QVariant &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QVariant &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QString &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QString &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QByteArray &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QByteArray &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const char *value, WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const char *value, WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
    void writeEntry(const char *key, const T &value, WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
    void writeEntry(const QString &key, const T &value, WriteConfigFlags pFlags = Normal)
    {
        writeEntry(key.toUtf8().constData(), value, pFlags);
    }

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QStringList &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QStringList &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const QString &key, const QVariantList &value,
                    WriteConfigFlags pFlags = Normal);
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    void writeEntry(const char *key, const QVariantList &value,
                    WriteConfigFlags pFlags = Normal);

    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
    void writeEntry(const QString &key, const QList<T> &value, WriteConfigFlags pFlags = Normal)
    {
        writeEntry(key.toUtf8().constData(), value, pFlags);
    }
    /** Overload for writeEntry(const QString&, const QVariant&, WriteConfigFlags) */
    template <typename T>
    void writeEntry(const char *key, const QList<T> &value, WriteConfigFlags pFlags = Normal);

    /**
     * Writes a list of strings to the config object, following XDG
     * desktop entry spec separator semantics
     *
     * @param pKey the key to write to
     * @param value the list to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see writeEntry(), readXdgListEntry()
     */
    void writeXdgListEntry(const QString &pKey, const QStringList &value,
                           WriteConfigFlags pFlags = Normal);
    /** Overload for writeXdgListEntry(const QString&, const QStringList&, WriteConfigFlags) */
    void writeXdgListEntry(const char *pKey, const QStringList &value,
                           WriteConfigFlags pFlags = Normal);

    /**
     * Writes a file path to the configuration
     *
     * If the path is located under $HOME, the user's home directory
     * is replaced with $HOME in the persistent storage.
     * The path should therefore be read back with readPathEntry()
     *
     * @param pKey the key to write to
     * @param path the path to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see writeEntry(), readPathEntry()
     */
    void writePathEntry(const QString &pKey, const QString &path,
                        WriteConfigFlags pFlags = Normal);
    /** Overload for writePathEntry(const QString&, const QString&, WriteConfigFlags) */
    void writePathEntry(const char *pKey, const QString &path,
                        WriteConfigFlags pFlags = Normal);

    /**
     * Writes a list of paths to the configuration
     *
     * If any of the paths are located under $HOME, the user's home directory
     * is replaced with $HOME in the persistent storage.
     * The paths should therefore be read back with readPathEntry()
     *
     * @param pKey the key to write to
     * @param value the list to write
     * @param pFlags the flags to use when writing this entry
     *
     * @see writeEntry(), readPathEntry()
     */
    void writePathEntry(const QString &pKey, const QStringList &value,
                        WriteConfigFlags pFlags = Normal);
    /** Overload for writePathEntry(const QString&, const QStringList&, WriteConfigFlags) */
    void writePathEntry(const char *pKey, const QStringList &value,
                        WriteConfigFlags pFlags = Normal);

    /**
     * Deletes the entry specified by @p pKey in the current group
     *
     * This also hides system wide defaults.
     *
     * @param pKey the key to delete
     * @param pFlags the flags to use when deleting this entry
     *
     * @see deleteGroup(), readEntry(), writeEntry()
     */
    void deleteEntry(const QString &pKey, WriteConfigFlags pFlags = Normal);
    /** Overload for deleteEntry(const QString&, WriteConfigFlags) */
    void deleteEntry(const char *pKey, WriteConfigFlags pFlags = Normal);

    /**
     * Checks whether the key has an entry in this group
     *
     * Use this to determine if a key is not specified for the current
     * group (hasKey() returns false).
     *
     * If this returns @c false for a key, readEntry() (and its variants)
     * will return the default value passed to them.
     *
     * @param key the key to search for
     * @return @c true if the key is defined in this group by any of the
     *         configuration sources, @c false otherwise
     *
     * @see readEntry()
     */
    bool hasKey(const QString &key) const;
    /** Overload for hasKey(const QString&) const */
    bool hasKey(const char *key) const;

    /**
     * Whether this group may be changed
     *
     * @return @c false if the group may be changed, @c true otherwise
     */
    bool isImmutable() const Q_DECL_OVERRIDE;

    /**
     * Checks if it is possible to change the given entry
     *
     * If isImmutable() returns @c true, then this method will return
     * @c true for all inputs.
     *
     * @param key the key to check
     * @return @c false if the key may be changed using this configuration
     *         group object, @c true otherwise
     */
    bool isEntryImmutable(const QString &key) const;
    /** Overload for isEntryImmutable(const QString&) const */
    bool isEntryImmutable(const char *key) const;

    /**
     * Reverts an entry to the default settings.
     *
     * Reverts the entry with key @p key in the current group in the
     * application specific config file to either the system wide (default)
     * value or the value specified in the global KDE config file.
     *
     * To revert entries in the global KDE config file, the global KDE config
     * file should be opened explicitly in a separate config object.
     *
     * @note This is @em not the same as deleting the key, as instead the
     * global setting will be copied to the configuration file that this
     * object manipulates.
     *
     * @param key The key of the entry to revert.
     */
    void revertToDefault(const QString &key);
    /** Overload for revertToDefault(const QString&) */
    void revertToDefault(const char *key);

    /**
     * Whether a default is specified for an entry in either the
     * system wide configuration file or the global KDE config file
     *
     * If an application computes a default value at runtime for
     * a certain entry, e.g. like:
     * \code
     * QColor computedDefault = qApp->palette().color(QPalette::Active, QPalette::Text);
     * QColor color = group.readEntry(key, computedDefault);
     * \endcode
     * then it may wish to make the following check before
     * writing back changes:
     * \code
     * if ( (value == computedDefault) && !group.hasDefault(key) )
     *    group.revertToDefault(key);
     * else
     *    group.writeEntry(key, value);
     * \endcode
     *
     * This ensures that as long as the entry is not modified to differ from
     * the computed default, the application will keep using the computed default
     * and will follow changes the computed default makes over time.
     *
     * @param key the key of the entry to check
     * @return @c true if the global or system settings files specify a default
     *          for @p key in this group, @c false otherwise
     */
    bool hasDefault(const QString &key) const;
    /** Overload for hasDefault(const QString&) const */
    bool hasDefault(const char *key) const;

    /**
     * Returns a map (tree) of entries for all entries in this group
     *
     * Only the actual entry string is returned, none of the
     * other internal data should be included.
     *
     * @return a map of entries in this group, indexed by key
     */
    QMap<QString, QString> entryMap() const;

protected:
    bool hasGroupImpl(const QByteArray &group) const Q_DECL_OVERRIDE;
    KConfigGroup groupImpl(const QByteArray &b) Q_DECL_OVERRIDE;
    const KConfigGroup groupImpl(const QByteArray &b) const Q_DECL_OVERRIDE;
    void deleteGroupImpl(const QByteArray &group, WriteConfigFlags flags) Q_DECL_OVERRIDE;
    bool isGroupImmutableImpl(const QByteArray &aGroup) const Q_DECL_OVERRIDE;

private:
    QExplicitlySharedDataPointer<KConfigGroupPrivate> d;

    friend class KConfigGroupPrivate;

    /**
     * Return the data in @p value converted to a QVariant
     *
     * @param pKey the name of the entry being converted, this is only used for error
     * reporting
     * @param value the UTF-8 data to be converted
     * @param aDefault the default value if @p pKey is not found
     * @return @p value converted to QVariant, or @p aDefault if @p value is invalid or cannot be converted.
     */
    static QVariant convertToQVariant(const char *pKey, const QByteArray &value, const QVariant &aDefault);
    friend class KServicePrivate; // XXX yeah, ugly^5
};

#define KCONFIGGROUP_ENUMERATOR_ERROR(ENUM) \
    "The Qt MetaObject system does not seem to know about \"" ENUM \
    "\" please use Q_ENUM or Q_FLAG to register it."

/**
 * To add support for your own enums in KConfig, you can declare them with Q_ENUM()
 * in a QObject subclass (which will make moc generate the code to turn the
 * enum into a string and vice-versa), and then (in the cpp code)
 * use the macro
 * <code>KCONFIGGROUP_DECLARE_ENUM_QOBJECT(MyClass, MyEnum)</code>
 *
 */
#define KCONFIGGROUP_DECLARE_ENUM_QOBJECT(Class, Enum)                             \
    template<>                                                                     \
    Class::Enum KConfigGroup::readEntry(const char* key, const Class::Enum& def) const\
    {                                                                              \
        const QMetaObject* M_obj = &Class::staticMetaObject;                       \
        const int M_index = M_obj->indexOfEnumerator(#Enum);                       \
        if(M_index == -1) qFatal(KCONFIGGROUP_ENUMERATOR_ERROR(#Enum));            \
        const QMetaEnum M_enum = M_obj->enumerator(M_index);                       \
        const QByteArray M_data = readEntry(key, QByteArray(M_enum.valueToKey(def)));\
        return static_cast<Class::Enum>(M_enum.keyToValue(M_data.constData()));    \
    }                                                                              \
    inline Class::Enum Q_DECL_DEPRECATED readEntry(const KConfigGroup& group, const char* key, const Class::Enum& def)\
    { return group.readEntry(key, def); }                                          \
    template<>                                                                     \
    void KConfigGroup::writeEntry(const char* key, const Class::Enum& value, KConfigBase::WriteConfigFlags flags)\
    {                                                                              \
        const QMetaObject* M_obj = &Class::staticMetaObject;                       \
        const int M_index = M_obj->indexOfEnumerator(#Enum);                       \
        if(M_index == -1) qFatal(KCONFIGGROUP_ENUMERATOR_ERROR(#Enum));            \
        const QMetaEnum M_enum = M_obj->enumerator(M_index);                       \
        writeEntry(key, QByteArray(M_enum.valueToKey(value)), flags);              \
    }                                                                              \
    inline void Q_DECL_DEPRECATED writeEntry(KConfigGroup& group, const char* key, const Class::Enum& value, KConfigBase::WriteConfigFlags flags = KConfigBase::Normal)\
    { group.writeEntry(key, value, flags); }

/**
 * Similar to KCONFIGGROUP_DECLARE_ENUM_QOBJECT but for flags declared with Q_FLAG()
 * (where multiple values can be set at the same time)
 */
#define KCONFIGGROUP_DECLARE_FLAGS_QOBJECT(Class, Flags)                            \
    template<>                                                                      \
    Class::Flags KConfigGroup::readEntry(const char* key, const Class::Flags& def) const\
    {                                                                               \
        const QMetaObject* M_obj = &Class::staticMetaObject;                        \
        const int M_index = M_obj->indexOfEnumerator(#Flags);                       \
        if(M_index == -1) qFatal(KCONFIGGROUP_ENUMERATOR_ERROR(#Flags));            \
        const QMetaEnum M_enum = M_obj->enumerator(M_index);                        \
        const QByteArray M_data = readEntry(key, QByteArray(M_enum.valueToKeys(def)));\
        return static_cast<Class::Flags>(M_enum.keysToValue(M_data.constData()));   \
    }                                                                               \
    inline Class::Flags Q_DECL_DEPRECATED readEntry(const KConfigGroup& group, const char* key, const Class::Flags& def)\
    { return group.readEntry(key, def);}                                            \
    template<>                                                                      \
    void KConfigGroup::writeEntry(const char* key, const Class::Flags& value, KConfigBase::WriteConfigFlags flags)\
    {                                                                               \
        const QMetaObject* M_obj = &Class::staticMetaObject;                        \
        const int M_index = M_obj->indexOfEnumerator(#Flags);                       \
        if(M_index == -1) qFatal(KCONFIGGROUP_ENUMERATOR_ERROR(#Flags));            \
        const QMetaEnum M_enum = M_obj->enumerator(M_index);                        \
        writeEntry(key, QByteArray(M_enum.valueToKeys(value)), flags);              \
    }                                                                               \
    inline void Q_DECL_DEPRECATED writeEntry(KConfigGroup& group, const char* key, const Class::Flags& value, KConfigBase::WriteConfigFlags flags = KConfigBase::Normal)\
    { group.writeEntry(key, value, flags); }

#include "conversioncheck.h"

template <typename T>
T KConfigGroup::readEntry(const char *key, const T &defaultValue) const
{
    ConversionCheck::to_QVariant<T>();
    return qvariant_cast<T>(readEntry(key, qVariantFromValue(defaultValue)));
}

template <typename T>
QList<T> KConfigGroup::readEntry(const char *key, const QList<T> &defaultValue) const
{
    ConversionCheck::to_QVariant<T>();
    ConversionCheck::to_QString<T>();

    QVariantList data;

    Q_FOREACH (const T &value, defaultValue) {
        data.append(qVariantFromValue(value));
    }

    QList<T> list;
    Q_FOREACH (const QVariant &value, readEntry<QVariantList>(key, data)) {
        Q_ASSERT(value.canConvert<T>());
        list.append(qvariant_cast<T>(value));
    }

    return list;
}

template <typename T>
void KConfigGroup::writeEntry(const char *key, const T &value, WriteConfigFlags pFlags)
{
    ConversionCheck::to_QVariant<T>();
    writeEntry(key, qVariantFromValue(value), pFlags);
}

template <typename T>
void KConfigGroup::writeEntry(const char *key, const QList<T> &list, WriteConfigFlags pFlags)
{
    ConversionCheck::to_QVariant<T>();
    ConversionCheck::to_QString<T>();
    QVariantList data;
    Q_FOREACH (const T &value, list) {
        data.append(qVariantFromValue(value));
    }

    writeEntry(key, data, pFlags);
}

#endif // KCONFIGGROUP_H
