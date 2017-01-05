/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
 *  Copyright (C) 2005-2008 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
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
 **/

#ifndef KSYCOCA_H
#define KSYCOCA_H

#include <kservice_export.h>
#include <ksycocatype.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QDataStream;
class KSycocaFactory;
class KSycocaFactoryList;
class KSycocaPrivate;

/**
 * Executable name of the kbuildsycoca program
 */
#define KBUILDSYCOCA_EXENAME "kbuildsycoca5"

/**
 * @internal
 * Read-only SYstem COnfiguration CAche
 */
class KSERVICE_EXPORT KSycoca : public QObject
{
    Q_OBJECT
    //Q_CLASSINFO("D-Bus Interface", "org.kde.KSycoca")

protected:
    /**
     * @internal
     * Building database
     */
    explicit KSycoca(bool /* buildDatabase */);

public:
    /**
     * type of database
     * @see absoluteFilePath()
     */
    typedef enum { LocalDatabase, GlobalDatabase } DatabaseType;

    /**
     * Read-only database
     */
    KSycoca();

    /**
     * Get or create the only instance of KSycoca (read-only)
     */
    static KSycoca *self();

    virtual ~KSycoca();

    /**
     * @return the compiled-in version, i.e. the one used when writing a new ksycoca
     */
    static int version();

    /**
     * @return true if the ksycoca database is available
     * This is usually the case, except if KDE isn't installed yet,
     * or before kded is started.
     */
    static bool isAvailable();

    /**
     * @internal - called by factories in read-only mode
     * This is how factories get a stream to an entry
     */
    QDataStream *findEntry(int offset, KSycocaType &type); // KF6: make it private
    /**
     * @internal - called by factories in read-only mode
     * Returns stream(), but positioned for reading this factory, 0 on error.
     */
    QDataStream *findFactory(KSycocaFactoryId id); // KF6: make it private

    /**
     * @internal - returns absolute file path of the database
     *
     * For the global database type, the database is searched under
     * the 'share/ksycoca' install path.
     * Otherwise, the value from the environment variable KDESYCOCA
     * is returned if set. If not set the path is built based on
     * QStandardPaths cache save location, typically ~/.cache on Unix.
     *
     * Since 5.15, the filename includes language and a sha1 of the directories
     * in GenericDataLocation, i.e. the directories with the desktop files.
     * This allows to have one database per setup, when using different install prefixes
     * or when switching languages.
     */
    static QString absoluteFilePath(DatabaseType type = LocalDatabase);

    /**
     * @internal - returns language stored inside database
     */
    KSERVICE_DEPRECATED QString language(); // KF6: remove

    /**
     * @internal - returns timestamp of database
     *
     * The database contains all changes made _before_ this time and
     * _might_ contain changes made after that.
     */
    KSERVICE_DEPRECATED quint32 timeStamp(); // KF6: remove

    /**
     * @internal - returns update signature of database
     *
     * Signature that keeps track of changes to
     * $KDEDIR/share/services/update_ksycoca
     *
     * Touching this file causes the database to be recreated
     * from scratch.
     */
    KSERVICE_DEPRECATED quint32 updateSignature(); // KF6: remove

    /**
     * @internal - returns all directories with information
     * stored inside sycoca.
     */
    QStringList allResourceDirs(); // KF6: make it private

    /**
     * @internal - add a factory
     */
    void addFactory(KSycocaFactory *); // KF6: make it private

    /**
     * @internal
     * @return true if building (i.e. if a KBuildSycoca);
     */
    virtual bool isBuilding();

    /**
     * @internal - disables launching of kbuildsycoca
     * @deprecated since 5.15, it only outputs a warning
     */
    KSERVICE_DEPRECATED static void disableAutoRebuild(); // KF6: remove

    /**
     * When you receive a "databaseChanged" signal, you can query here if
     * a change has occurred in a specific resource type.
     * @see KStandardDirs for the various resource types.
     *
     * This method is meant to be called from the GUI thread only.
     * @deprecated use the signal databaseChanged(QStringList) instead.
     */
#ifndef KSERVICE_NO_DEPRECATED
    static KSERVICE_DEPRECATED bool isChanged(const char *type);
#endif

    /**
     * A read error occurs.
     * @internal
     */
    static void flagError();

    /**
     * Ensures the ksycoca database is up to date.
     * If the database was modified by another process, close it, so the next use reopens it.
     * If the desktop files have been modified more recently than the database, update it.
     *
     * Update the sycoca file from the files on disk (e.g. desktop files or mimeapps.list).
     * You don't normally have to call this because the next use of KSycoca
     * (e.g. via KMimeTypeTrader, KService etc.) will notice that the sycoca
     * database is out of date, by looking a directory modification times.
     * In addition, in a full KDE session, kded monitors directories to detect changes.
     *
     * This is however useful for GUIs that allow to create a new desktop file
     * and then want to ensure it is available immediately in KSycoca.
     * This is also useful after modifying a mimeapps.list file.
     *
     * KBuildSycocaProgressDialog can also be used instead of this method, in GUI apps.
     *
     * @since 5.15
     */
    void ensureCacheValid(); // Warning for kservice code: this can delete all the factories.

Q_SIGNALS:
    /**
     * Connect to this to get notified when the database changes.
     *
     * Example: after creating a .desktop file in KOpenWithDialog, it
     * must wait until kbuildsycoca5 finishes until the KService::Ptr is available.
     * Other examples: anything that displays a list of apps or plugins to the user
     * and which is always visible (otherwise querying sycoca before showing
     * could be enough).
     */
    void databaseChanged();

    /**
     * Connect to this to get notified when the database changes
     * Example: when mimetype definitions have changed, applications showing
     * files as icons refresh icons to take into account the new mimetypes.
     * Another example: after creating a .desktop file in KOpenWithDialog,
     * it must wait until kbuildsycoca5 finishes until the KService::Ptr is available.
     *
     * @param changedResources List of resources where changes were detected.
     * This can include the following resources (as defined in KStandardDirs) :
     * apps, xdgdata-apps, services, servicetypes, xdgdata-mime.
     */
    void databaseChanged(const QStringList &changedResources); // KF6: deprecate

protected:
    // @internal used by kbuildsycoca
    KSycocaFactoryList *factories();

    // @internal used by factories and kbuildsycoca
    QDataStream *&stream();
    friend class KSycocaFactory;
    friend class KSycocaDict;

    void connectNotify(const QMetaMethod &signal) Q_DECL_OVERRIDE;

private:
    /**
     * Clear all caches related to ksycoca contents.
     * @internal only used by kded and kbuildsycoca.
     */
    static void clearCaches();

    bool needsRebuild();

    friend class KBuildSycoca;
    friend class Kded;
    friend class KSycocaPrivate;
    friend class KSycocaXdgDirsTest;

    Q_DISABLE_COPY(KSycoca)
    KSycocaPrivate *const d;
};

#endif

