/*  This file is part of the KDE libraries
    Copyright (c) 2006 Jacob R Rideout <kde@jacobrideout.net>

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

#ifndef KAUTOSAVEFILE_H
#define KAUTOSAVEFILE_H

#include <kcoreaddons_export.h>

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QUrl>

class KAutoSaveFilePrivate;
/**
 * \class KAutoSaveFile kautosavefile.h <KAutoSaveFile>
 *
 * @brief Creates and manages a temporary "auto-save" file.
 * Autosave files are temporary files that applications use to store
 * the unsaved data in a file they have open for
 * editing. KAutoSaveFile allows you to easily create and manage such
 * files, as well as to recover the unsaved data left over by a
 * crashed or otherwise gone process.
 *
 * Each KAutoSaveFile object is associated with one specific file that
 * the application holds open. KAutoSaveFile is also a QObject, so it
 * can be reparented to the actual opened file object, so as to manage
 * the lifetime of the temporary file.
 *
 * Typical use consists of:
 * - verifying whether stale autosave files exist for the opened file
 * - deciding whether to recover the old, autosaved data
 * - if not recovering, creating a KAutoSaveFile object for the opened file
 * - during normal execution of the program, periodically save unsaved
 *   data into the KAutoSaveFile file.
 *
 * KAutoSaveFile holds a lock on the autosave file, so it's safe to
 * delete the file and recreate it later. Because of that, disposing
 * of stale autosave files should be done with releaseLock(). No lock is
 * held on the managed file.
 *
 * Examples:
 * Opening a new file:
 * @code
 *   void Document::open(const QUrl &url)
 *   {
 *       // check whether autosave files exist:
 *       QList<KAutoSaveFile *> staleFiles = KAutoSaveFile::staleFiles(url);
 *       if (!staleFiles.isEmpty()) {
 *           if (KMessageBox::questionYesNo(parent,
 *                                          "Auto-saved files exist. Do you want to recover them now?",
 *                                          "File Recovery",
 *                                          "Recover", "Don't recover") == KMessage::Yes) {
 *               recoverFiles(staleFiles);
 *               return;
 *           } else {
 *               // remove the stale files
 *               foreach (KAutoSaveFile *stale, staleFiles) {
 *                   stale->open(QIODevice::ReadWrite);
 *                   delete stale;
 *               }
 *           }
 *       }
 *
 *       // create new autosave object
 *       m_autosave = new KAutoSaveFile(url, this);
 *
 *       // continue the process of opening file 'url'
 *       ...
 *   }
 * @endcode
 *
 * The function recoverFiles could loop over the list of files and do this:
 * @code
 *   foreach (KAutoSaveFile *stale, staleFiles) {
 *       if (!stale->open(QIODevice::ReadWrite)) {
 *           // show an error message; we could not steal the lockfile
 *           // maybe another application got to the file before us?
 *           delete stale;
 *           continue;
 *       }
 *       Document *doc = new Document;
 *       doc->m_autosave = stale;
 *       stale->setParent(doc); // reparent
 *
 *       doc->setUrl(stale->managedFile());
 *       doc->setContents(stale->readAll());
 *       doc->setState(Document::Modified); // mark it as modified and unsaved
 *
 *       documentManager->addDocument(doc);
 *   }
 * @endcode
 *
 * If the file is unsaved, periodically write the contents to the save file:
 * @code
 *   if (!m_autosave->isOpen() && !m_autosave->open(QIODevice::ReadWrite)) {
 *       // show error: could not open the autosave file
 *   }
 *   m_autosave->write(contents());
 * @endcode
 *
 * When the user saves the file, the autosaved file is no longer
 * necessary and can be removed or emptied.
 * @code
 *    m_autosave->resize(0);    // leaves the file open
 * @endcode
 *
 * @code
 *    m_autosave->remove();     // closes the file
 * @endcode
 *
 * @author Jacob R Rideout <kde@jacobrideout.net>
 */
class KCOREADDONS_EXPORT KAutoSaveFile : public QFile
{
    Q_OBJECT
public:
    /**
     * Constructs a KAutoSaveFile for file @p filename. The temporary
     * file is not opened or created until actually needed. The file
     * @p filename does not have to exist for KAutoSaveFile to be
     * constructed (if it exists, it will not be touched).
     *
     * @param filename the filename that this KAutoSaveFile refers to
     * @param parent the parent object
     */
    explicit KAutoSaveFile(const QUrl &filename, QObject *parent = 0);

    /**
     * @overload
     * Constructs a KAutoSaveFile object. Note that you need to call
     * setManagedFile() before calling open().
     *
     * @param parent the parent object
     */
    explicit KAutoSaveFile(QObject *parent = 0);

    /**
     * Destroys the KAutoSaveFile object, removes the autosave
     * file and drops the lock being held (if any).
     */
    ~KAutoSaveFile();

    /**
     * Retrieves the URL of the file managed by KAutoSaveFile. This
     * is the same URL that was given to setManagedFile() or the
     * KAutoSaveFile constructor.
     *
     * This is the name of the real file being edited by the
     * application. To get the name of the temporary file where data
     * can be saved, use fileName() (after you have called open()).
     */
    QUrl managedFile() const;

    /**
     * Sets the URL of the file managed by KAutoSaveFile. This should
     * be the name of the real file being edited by the application.
     * If the file was previously set, this function calls releaseLock().
     *
     * @param filename the filename that this KAutoSaveFile refers to
     */
    void setManagedFile(const QUrl &filename);

    /**
     * Closes the autosave file resource and removes the lock
     * file. The file name returned by fileName() will no longer be
     * protected and can be overwritten by another application at any
     * time. To obtain a new lock, call open() again.
     *
     * This function calls remove(), so the autosave temporary file
     * will be removed too.
     */
    virtual void releaseLock();

    /**
     * Opens the autosave file and locks it if it wasn't already
     * locked. The name of the temporary file where data can be saved
     * to will be set by this function and can be retrieved with
     * fileName(). It will not change unless releaseLock() is called. No
     * other application will attempt to edit such a file either while
     * the lock is held.
     *
     * @param openmode the mode that should be used to open the file,
     *        probably QIODevice::ReadWrite
     * @returns true if the file could be opened (= locked and
     * created), false if the operation failed
     */
    bool open(OpenMode openmode) Q_DECL_OVERRIDE;

    /**
     * Checks for stale autosave files for the file @p url. Returns a list
     * of autosave files that contain autosaved data left behind by
     * other instances of the application, due to crashing or
     * otherwise uncleanly exiting.
     *
     * It is the application's job to determine what to do with such
     * unsaved data. Generally, this is done by asking the user if he
     * wants to see the recovered data, and then allowing the user to
     * save if he wants to.
     *
     * If not given, the application name is obtained from
     * QCoreApplication, so be sure to have set it correctly before
     * calling this function.
     *
     * This function returns a list of unopened KAutoSaveFile
     * objects. By calling open() on them, the application will steal
     * the lock. Subsequent releaseLock() or deleting of the object will
     * then erase the stale autosave file.
     */
    static QList<KAutoSaveFile *> staleFiles(const QUrl &url,
            const QString &applicationName =
                QString());

    /**
     * Returns all stale autosave files left behind by crashed or
     * otherwise gone instances of this application.
     *
     * If not given, the application name is obtained from
     * QCoreApplication, so be sure to have set it correctly before
     * calling this function.
     *
     * See staleFiles() for information on the returned objects.
     */
    static QList<KAutoSaveFile *> allStaleFiles(const QString &applicationName =
                QString());

private:
    Q_DISABLE_COPY(KAutoSaveFile)
    friend class KAutoSaveFilePrivate;
    KAutoSaveFilePrivate *const d;
};

#endif // KAUTOSAVEFILE_H
