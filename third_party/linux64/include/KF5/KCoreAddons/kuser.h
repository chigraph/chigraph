/*
 *  KUser - represent a user/account
 *  Copyright (C) 2002-2003 Tim Jansen <tim@tjansen.de>
 *  Copyright (C) 2003 Oswald Buddenhagen <ossi@kde.org>
 *  Copyright (C) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
 *  Copyright (C) 2014 Alex Richardson <arichardson.kde@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */
#ifndef KUSER_H
#define KUSER_H

#include <kcoreaddons_export.h>

#include <QSharedDataPointer>
#include <QtCore/QVariant>

class KUserGroup;
class QString;
class QStringList;
template <class T> class QList;

#ifdef Q_OS_WIN
typedef void *K_UID;
typedef void *K_GID;
struct WindowsSIDWrapper;
#else
#include <sys/types.h>
typedef uid_t K_UID;
typedef gid_t K_GID;
struct passwd;
struct group;
#endif

// The following is to avoid compile errors using msvc, and it is done
// using a common #define to avoid helpful people accidentally cleaning this
// not quite pretty thing and breaking it for people on windows.
// See https://git.reviewboard.kde.org/r/127598/ for details
#define KCOREADDONS_UINT_MAX (std::numeric_limits<uint>::max)()

/** A platform independent user or group ID.
 *
 *
 * This struct is required since Windows does not have an integer uid_t/gid_t type
 * but instead uses an opaque binary blob (SID) which must free allocated memory.
 * On UNIX this is simply a uid_t/gid_t and all operations are inline, so there is
 * no runtime overhead over using the uid_t/gid_t directly. On Windows this is an implicitly
 * shared class that frees the underlying SID once no more references remain.
 *
 * Unlike KUser/KUserGroup this does not query additional information, it is simply
 * an abstraction over the native user/group ID type. If more information is necessary, a
 * KUser or KUserGroup instance can be constructed from this ID
 *
 * @internal
 * @author Alex Richardson <arichardson.kde@gmail.com>
 */
template<typename T>
struct KCOREADDONS_EXPORT KUserOrGroupId {
    typedef T NativeType;
protected:
    /** Creates an invalid KUserOrGroupId */
    KUserOrGroupId();
    /** Creates a KUserOrGroupId from a native user/group ID. On windows this will not take
     * ownership over the passed SID, a copy will be created instead.
     */
    explicit KUserOrGroupId(NativeType nativeId);
    /** Copy constructor. This is very fast, objects can be passed by value */
    KUserOrGroupId(const KUserOrGroupId<T> &other);
    KUserOrGroupId &operator=(const KUserOrGroupId<T> &other);
    ~KUserOrGroupId();
public:
    /** @return true if this object references a valid user/group ID.
     * @note If this returns true it doesn't necessarily mean that the referenced user/group exists,
     * it only checks whether this value could be a valid user/group ID.
     */
    bool isValid() const;
    /**
     * @return A user/group ID that can be used in operating system specific functions
     * @note On Windows the returned pointer will be freed once the last KUserOrGroupId referencing
     * this user/group ID is deleted. Make sure that the KUserOrGroupId object remains valid as
     * long as the native pointer is needed.
     */
    NativeType nativeId() const;
    /** @return A string representation of this user ID, not the name of the user
     * On UNIX this is a simple integer, e.g. "0" for root. On Windows this is a string
     * like e.g. "S-1-5-32-544" for the Administrators group
     */
    QString toString() const;
    /** @return whether this KUserOrGroupId is equal to @p other */
    bool operator==(const KUserOrGroupId &other) const;
    /** @return whether this KUserOrGroupId is not equal to @p other */
    bool operator!=(const KUserOrGroupId &other) const;
private:
#ifdef Q_OS_WIN
    QExplicitlySharedDataPointer<WindowsSIDWrapper> data;
#else
    NativeType id;
#endif
};

#ifdef Q_OS_WIN
template<> KUserOrGroupId<void *>::KUserOrGroupId();
template<> KUserOrGroupId<void *>::~KUserOrGroupId();
template<> KUserOrGroupId<void *>::KUserOrGroupId(KUserOrGroupId::NativeType nativeId);
template<> KUserOrGroupId<void *>::KUserOrGroupId(const KUserOrGroupId &other);
template<> KUserOrGroupId<void *>& KUserOrGroupId<void *>::operator=(const KUserOrGroupId &other);
template<> bool KUserOrGroupId<void *>::isValid() const;
template<> KUserOrGroupId<void *>::NativeType KUserOrGroupId<void *>::nativeId() const;
template<> QString KUserOrGroupId<void *>::toString() const;
template<> bool KUserOrGroupId<void *>::operator==(const KUserOrGroupId &other) const;
template<> bool KUserOrGroupId<void *>::operator!=(const KUserOrGroupId &other) const;
#endif

/** A platform independent user ID.
 * @see KUserOrGroupId
 * @since 5.0
 * @author Alex Richardson <arichardson.kde@gmail.com>
 */
struct KCOREADDONS_EXPORT KUserId : public KUserOrGroupId<K_UID> {
    /** Creates an invalid KUserId */
    KUserId() {}
    /** Creates an KUserId from the native user ID type */
    explicit KUserId(K_UID uid) : KUserOrGroupId(uid) {}
    KUserId(const KUserId &other) : KUserOrGroupId(other) {}
    ~KUserId() {}
    /** @return a KUserId for the user with name @p name, or an invalid KUserId if no
     * user with this name was found on the system
     */
    static KUserId fromName(const QString &name);
    /** @return a KUserId for the current user. This is like ::getuid() on UNIX. */
    static KUserId currentUserId();
    /** @return a KUserId for the current effective user. This is like ::geteuid() on UNIX.
     * @note Windows does not have setuid binaries, so on Windows this will always be the same
     * as KUserId::currentUserId()
     */
    static KUserId currentEffectiveUserId();
};

/** A platform independent group ID.
 * @see KUserOrGroupId
 * @since 5.0
 * @author Alex Richardson <arichardson.kde@gmail.com>
 */
struct KCOREADDONS_EXPORT KGroupId : public KUserOrGroupId<K_GID> {
    /** Creates an invalid KGroupId */
    KGroupId() {}
    /** Creates an KGroupId from the native group ID type */
    explicit KGroupId(K_GID gid) : KUserOrGroupId(gid) {}
    KGroupId(const KGroupId &other) : KUserOrGroupId(other) {}
    ~KGroupId() {}
    /** @return A KGroupId for the user with name @p name, or an invalid KGroupId if no
     * user with this name was found on the system
     */
    static KGroupId fromName(const QString &name);
    /** @return a KGroupId for the current user. This is like ::getgid() on UNIX. */
    static KGroupId currentGroupId();
    /** @return a KGroupId for the current effective user. This is like ::getegid() on UNIX.
     * @note Windows does not have setuid binaries, so on Windows this will always be the same
     * as KGroupId::currentGroupId()
     */
    static KGroupId currentEffectiveGroupId();
};

#ifndef Q_OS_WIN
inline uint qHash(const KUserId &id, uint seed = 0)
{
    return qHash(id.nativeId(), seed);
}
inline uint qHash(const KGroupId &id, uint seed = 0)
{
    return qHash(id.nativeId(), seed);
}
#else
// can't be inline on windows, because we would need to include windows.h (which can break code)
KCOREADDONS_EXPORT uint qHash(const KUserId &id, uint seed = 0);
KCOREADDONS_EXPORT uint qHash(const KGroupId &id, uint seed = 0);
#endif

/**
 * \class KUser kuser.h <KUser>
 *
 * @short Represents a user on your system
 *
 * This class represents a user on your system. You can either get
 * information about the current user, of fetch information about
 * a user on the system. Instances of this class will be explicitly shared,
 * so copying objects is very cheap and you can safely pass objects by value.
 *
 * @author Tim Jansen <tim@tjansen.de>
 */
class KCOREADDONS_EXPORT KUser
{

public:

    enum UIDMode {
        UseEffectiveUID, ///< Use the effective user id.
        UseRealUserID    ///< Use the real user id.
    };

    /**
     * Creates an object that contains information about the current user.
     * (as returned by getuid(2) or geteuid(2), taking $LOGNAME/$USER into
     * account).
     * @param mode if #UseEffectiveUID is passed the effective
     *             user is returned.
     *        If #UseRealUserID is passed the real user will be
     *        returned.
     *        The real UID will be different than the effective UID in setuid
     *        programs; in
     *        such a case use the effective UID for checking permissions, and
     *        the real UID for displaying information about the user.
     */
    explicit KUser(UIDMode mode = UseEffectiveUID);

    /**
     * Creates an object for the user with the given user id.
     * If the user does not exist isValid() will return false.
     * @param uid the user id
     */
    explicit KUser(K_UID uid);

    /**
    * Creates an object for the user with the given user id.
    * If the KUserId object is invalid this one will be, too.
    * @param uid the user id
    */
    explicit KUser(KUserId uid);

    /**
     * Creates an object that contains information about the user with the given
     * name. If the user does not exist isValid() will return false.
     *
     * @param name the name of the user
     */
    explicit KUser(const QString &name);

    /**
     * Creates an object that contains information about the user with the given
     * name. If the user does not exist isValid() will return false.
     *
     * @param name the name of the user
     */
    explicit KUser(const char *name);

#ifndef Q_OS_WIN
    /**
     * Creates an object from a passwd structure.
     * If the pointer is null isValid() will return false.
     *
     * @param p the passwd structure to create the user from
     */
    explicit KUser(const passwd *p);
#endif

    /**
     * Creates an object from another KUser object
     * @param user the user to create the new object from
     */
    KUser(const KUser &user);

    /**
     * Copies a user
     * @param user the user to copy
     * @return this object
     */
    KUser &operator =(const KUser &user);

    /**
     * Two KUser objects are equal if the userId() are identical.
     * Invalid users never compare equal.
     */
    bool operator ==(const KUser &user) const;

    /**
     * Two KUser objects are not equal if userId() are not identical.
     * Invalid users always compare unequal.
     */
    bool operator !=(const KUser &user) const;

    /**
     * Returns true if the user is valid. A KUser object can be invalid if
     * you created it with an non-existing uid or name.
     * @return true if the user is valid
     */
    bool isValid() const;

    /** @return the native user id of the user. */
    KUserId userId() const;

    /** @return the native user id of the user. */
    KGroupId groupId() const;

#ifndef KCOREADDONS_NO_DEPRECATED
    /**
     * Returns the group id of the user.
     * @return the id of the group or -1 if user is invalid
     * @deprecated since 5.0 use KUser::groupId()
     */
    KCOREADDONS_DEPRECATED K_GID gid() const
    {
        return groupId().nativeId();
    }
#endif

    /**
     * Checks whether the user is the super user (root).
     * @return true if the user is root
     */
    bool isSuperUser() const;

    /**
     * The login name of the user.
     * @return the login name of the user or QString() if user is invalid
     */
    QString loginName() const;

    /**
     * The full name of the user.
     * @return the full name of the user or QString() if user is invalid
     * @deprecated use property(KUser::FullName) instead
     */
#ifndef KCOREADDONS_NO_DEPRECATED
    KCOREADDONS_DEPRECATED QString fullName() const
    {
        return property(FullName).toString();
    }
    /**
     * Returns the user id of the user.
     * @return the id of the user or -1 (UNIX)/ null(Windows) if user is invalid
     * @deprecated since 5.0 use KUser::userId()
     */
    KCOREADDONS_DEPRECATED K_UID uid() const
    {
        return userId().nativeId();
    }
#endif

    /**
     * The path to the user's home directory.
     * @return the home directory of the user or QString() if the
     *         user is invalid
     */
    QString homeDir() const;

    /**
     * The path to the user's face file.
     * @return the path to the user's face file or QString() if no
     *         face has been set
     */
    QString faceIconPath() const;

    /**
     * The path to the user's login shell.
     * @return the login shell of the user or QString() if the
     *         user is invalid
     */
    QString shell() const;

    /**
     * @param maxCount the maximum number of groups to return
     * @return all groups of the user
     */
    QList<KUserGroup> groups(uint maxCount = KCOREADDONS_UINT_MAX) const;

    /**
     * @param maxCount the maximum number of groups to return
     * @return all group names of the user
     */
    QStringList groupNames(uint maxCount = KCOREADDONS_UINT_MAX) const;

    enum UserProperty { FullName, RoomNumber, WorkPhone, HomePhone };

    /**
     * Returns an extended property.
     *
     * Under Windows, @p RoomNumber, @p WorkPhone and @p HomePhone are unsupported.
     *
     * @return a QVariant with the value of the property or an invalid QVariant,
     *         if the property is not set
     */
    QVariant property(UserProperty which) const;

    /**
     * Destructor.
     */
    ~KUser();

    /**
     * @param maxCount the maximum number of users to return
     * @return all users of the system.
     */
    static QList<KUser> allUsers(uint maxCount = KCOREADDONS_UINT_MAX);

    /**
    * @param maxCount the maximum number of users to return
    * @return all user names of the system.
    */
    static QStringList allUserNames(uint maxCount = KCOREADDONS_UINT_MAX);

private:
    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

/**
 * \class KUserGroup kuser.h <KUserGroup>
 *
 * @short Represents a group on your system
 *
 * This class represents a group on your system. You can either get
 * information about the group of the current user, of fetch information about
 * a group on the system. Instances of this class will be explicitly shared,
 * so copying objects is very cheap and you can safely pass objects by value.
 *
 * @author Jan Schaefer <j_schaef@informatik.uni-kl.de>
 */
class KCOREADDONS_EXPORT KUserGroup
{

public:

    /**
     * Create an object from a group name.
     * If the group does not exist, isValid() will return false.
     * @param name the name of the group
     */
    explicit KUserGroup(const QString &name);

    /**
     * Create an object from a group name.
     * If the group does not exist, isValid() will return false.
     * @param name the name of the group
     */
    explicit KUserGroup(const char *name);

    /**
    * Creates an object for the group with the given group id.
    * If the KGroupId object is invalid this one will be, too.
    * @param gid the group id
    */
    explicit KUserGroup(KGroupId gid);

    /**
     * Create an object from the group of the current user.
     * @param mode if #KUser::UseEffectiveUID is passed the effective user
     *        will be used. If #KUser::UseRealUserID is passed the real user
     *        will be used.
     *        The real UID will be different than the effective UID in setuid
     *        programs; in  such a case use the effective UID for checking
     *        permissions, and the real UID for displaying information about
     *        the group associated with the user.
     */
    explicit KUserGroup(KUser::UIDMode mode = KUser::UseEffectiveUID);

    /**
     * Create an object from a group id.
     * If the group does not exist, isValid() will return false.
     * @param gid the group id
     */
    explicit KUserGroup(K_GID gid);

#ifndef Q_OS_WIN
    /**
     * Creates an object from a group structure.
     * If the pointer is null, isValid() will return false.
     * @param g the group structure to create the group from.
     */
    explicit KUserGroup(const group *g);
#endif

    /**
     * Creates a new KUserGroup instance from another KUserGroup object
     * @param group the KUserGroup to copy
     */
    KUserGroup(const KUserGroup &group);

    /**
     * Copies a group
     * @param group the group that should be copied
     * @return this group
     */
    KUserGroup &operator =(const KUserGroup &group);

    /**
     * Two KUserGroup objects are equal if their gid()s are identical.
     * Invalid groups never compare equal.
     * @return true if the groups are identical
     */
    bool operator ==(const KUserGroup &group) const;

    /**
     * Two KUserGroup objects are not equal if their gid()s are not identical.
     * Invalid groups always compare unequal.
     * @return true if the groups are not identical
     */
    bool operator !=(const KUserGroup &group) const;

    /**
     * Returns whether the group is valid.
     * A KUserGroup object can be invalid if it is
     * created with a non-existing gid or name.
     * @return true if the group is valid
     */
    bool isValid() const;

#ifndef KCOREADDONS_NO_DEPRECATED
    /**
     * Returns the group id of the group.
     * @return the group id of the group or -1 if the group is invalid
     * @deprecated since 5.0 use KUserGroup::groupId()
     */
    KCOREADDONS_DEPRECATED K_GID gid() const
    {
        return groupId().nativeId();
    }
#endif

    /** @return the native group id of the user. */
    KGroupId groupId() const;

    /**
     * The name of the group.
     * @return the name of the group
     */
    QString name() const;

    /**
     * @param maxCount the maximum number of users to return
     * @return a list of all users of the group
     */
    QList<KUser> users(uint maxCount = KCOREADDONS_UINT_MAX) const;

    /**
     * @param maxCount the maximum number of groups to return
     * @return a list of all user login names of the group
     */
    QStringList userNames(uint maxCount = KCOREADDONS_UINT_MAX) const;

    /**
     * Destructor.
     */
    ~KUserGroup();

    /**
     * @param maxCount the maximum number of groups to return
     * @return a list of all groups on this system
     */
    static QList<KUserGroup> allGroups(uint maxCount = KCOREADDONS_UINT_MAX);

    /**
     * @param maxCount the maximum number of groups to return
     * @return a list of all group names on this system
     */
    static QStringList allGroupNames(uint maxCount = KCOREADDONS_UINT_MAX);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#if !defined(Q_OS_WIN)
// inline UNIX implementation of KUserOrGroupId
template<typename T>
inline bool KUserOrGroupId<T>::isValid() const
{
    return id != NativeType(-1);
}
template<typename T>
inline bool KUserOrGroupId<T>::operator==(const KUserOrGroupId<T> &other) const
{
    return id == other.id;
}
template<typename T>
inline bool KUserOrGroupId<T>::operator!=(const KUserOrGroupId<T> &other) const
{
    return id != other.id;
}
template<typename T>
inline typename KUserOrGroupId<T>::NativeType KUserOrGroupId<T>::nativeId() const
{
    return id;
}
template<typename T>
inline QString KUserOrGroupId<T>::toString() const
{
    return QString::number(id);
}
template<typename T>
inline KUserOrGroupId<T>::KUserOrGroupId()
    : id(-1)
{
}
template<typename T>
inline KUserOrGroupId<T>::KUserOrGroupId(KUserOrGroupId<T>::NativeType nativeId)
    : id(nativeId)
{
}
template<typename T>
inline KUserOrGroupId<T>::KUserOrGroupId(const KUserOrGroupId<T> &other)
    : id(other.id)
{
}
template<typename T>
inline KUserOrGroupId<T> &KUserOrGroupId<T>::operator=(const KUserOrGroupId<T> &other)
{
    id = other.id;
    return *this;
}
template<typename T>
inline KUserOrGroupId<T>::~KUserOrGroupId()
{
}
#endif // !defined(Q_OS_WIN)

inline bool KUser::operator!=(const KUser &other) const
{
    return !operator==(other);
}

inline bool KUserGroup::operator!=(const KUserGroup &other) const
{
    return !operator==(other);
}

#endif
