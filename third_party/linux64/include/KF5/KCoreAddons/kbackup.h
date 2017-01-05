/*
   This file is part of the KDE libraries
   Copyright 1999 Waldo Bastian <bastian@kde.org>
   Copyright 2006 Jaison Lee <lee.jaison@gmail.com>
   Copyright 2011 Romain Perier <bambi@ubuntu.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KBACKUP_H
#define KBACKUP_H

#include <kcoreaddons_export.h>
#include <QtCore/QString>

namespace KBackup
{
/**
 * @brief Function to create a backup file before saving.
 *
 * If empty (the default), the backup will be in the same directory as @p filename.
 * The backup type (simple, rcs, or numbered), extension string, and maximum
 * number of backup files are read from the user's global configuration.
 * Use simpleBackupFile() or numberedBackupFile() to force one of these
 * specific backup styles.
 * You can use this method even if you don't use KSaveFile.
 * @param filename the file to backup
 * @param backupDir optional directory where to save the backup file in.
 * @return true if successful, or false if an error has occurred.
 */
KCOREADDONS_EXPORT bool backupFile(const QString &filename,
                                   const QString &backupDir = QString());

/**
* @brief Function to create a backup file for a given filename.
*
* This function creates a backup file from the given filename.
* You can use this method even if you don't use KSaveFile.
* @param filename the file to backup
* @param backupDir optional directory where to save the backup file in.
* If empty (the default), the backup will be in the same directory as @p filename.
* @param backupExtension the extension to append to @p filename, "~" by default.
* @return true if successful, or false if an error has occurred.
*/
KCOREADDONS_EXPORT bool simpleBackupFile(const QString &filename,
        const QString &backupDir = QString(),
        const QString &backupExtension = QStringLiteral("~"));

/**
 * @brief Function to create a backup file for a given filename.
 *
 * This function creates a series of numbered backup files from the
 * given filename.
 *
 * The backup file names will be of the form:
 *     \<name\>.\<number\>\<extension\>
 * for instance
 *     \verbatim chat.3.log \endverbatim
 *
 * The new backup file will be have the backup number 1.
 * Each existing backup file will have its number incremented by 1.
 * Any backup files with numbers greater than the maximum number
 * permitted (@p maxBackups) will be removed.
 * You can use this method even if you don't use KSaveFile.
 *
 * @param filename the file to backup
 * @param backupDir optional directory where to save the backup file in.
 * If empty (the default), the backup will be in the same directory as
 * @p filename.
 * @param backupExtension the extension to append to @p filename,
 * which is "~" by default.  Do not use an extension containing digits.
 * @param maxBackups the maximum number of backup files permitted.
 * For best performance a small number (10) is recommended.
 * @return true if successful, or false if an error has occurred.
 */
KCOREADDONS_EXPORT bool numberedBackupFile(const QString &filename,
        const QString &backupDir = QString(),
        const QString &backupExtension = QStringLiteral("~"),
        const uint maxBackups = 10
                                          );

/**
 * @brief Function to create an rcs backup file for a given filename.
 *
 * This function creates a rcs-formatted backup file from the
 * given filename.
 *
 * The backup file names will be of the form:
 *     \<name\>,v
 * for instance
 *     \verbatim photo.jpg,v \endverbatim
 *
 * The new backup file will be in RCS format.
 * Each existing backup file will be committed as a new revision.
 * You can use this method even if you don't use KSaveFile.
 *
 * @param filename the file to backup
 * @param backupDir optional directory where to save the backup file in.
 * If empty (the default), the backup will be in the same directory as
 * @p filename.
 * @param backupMessage is the RCS commit message for this revision.
 * @return true if successful, or false if an error has occurred.
 */
KCOREADDONS_EXPORT bool rcsBackupFile(const QString &filename,
                                      const QString &backupDir = QString(),
                                      const QString &backupMessage = QString()
                                     );
}

#endif
