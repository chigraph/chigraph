/*
  This file is part of the KDE libraries
  Copyright (c) 1999 Preston Brown <pbrown@kde.org>
  Copyright (C) 1997-1999 Matthias Kalle Dalheimer (kalle@kde.org)

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

#ifndef KSHAREDCONFIG_H
#define KSHAREDCONFIG_H

#include <kconfig.h>
#include <QExplicitlySharedDataPointer>

/**
 * \class KSharedConfig ksharedconfig.h <KSharedConfig>
 *
 * KConfig variant using shared memory
 *
 * KSharedConfig provides a shared (reference counted) variant
 * of KConfig.  This allows you to use manipulate the same configuration
 * files from different places in your code without worrying about
 * accidentally overwriting changes.
 *
 * The openConfig() method is threadsafe: every thread gets a separate repository
 * of shared KConfig objects. This means, however, that you'll be responsible for
 * synchronizing the instances of KConfig for the same filename between threads,
 * using reparseConfiguration after a manual change notification, just like you have
 * to do between processes.
 */
class KCONFIGCORE_EXPORT KSharedConfig : public KConfig, public QSharedData //krazy:exclude=dpointer (only for refcounting)
{
public:
    typedef QExplicitlySharedDataPointer<KSharedConfig> Ptr;

public:
    /**
     * Creates a KSharedConfig object to manipulate a configuration file
     *
     * If an absolute path is specified for @p fileName, that file will be used
     * as the store for the configuration settings.  If a non-absolute path
     * is provided, the file will be looked for in the standard directory
     * specified by resourceType.  If no path is provided, a default
     * configuration file will be used based on the name of the main
     * application component.
     *
     * @p mode determines whether the user or global settings will be allowed
     * to influence the values returned by this object.  See KConfig::OpenFlags for
     * more details.
     *
     * @param fileName     the configuration file to open. If empty, it will be determined
     *                     automatically (from --config on the command line, otherwise
     *                     from the application name + "rc")
     * @param mode         how global settings should affect the configuration
     *                     options exposed by this KConfig object
     * @param resourceType The standard directory to look for the configuration
     *                     file in (see QStandardPaths)
     *
     * @sa KConfig
     */
    static KSharedConfig::Ptr openConfig(const QString &fileName = QString(),
                                         OpenFlags mode = FullConfig,
                                         QStandardPaths::StandardLocation type = QStandardPaths::GenericConfigLocation);

    virtual ~KSharedConfig();

private:
    Q_DISABLE_COPY(KSharedConfig)
    KConfigGroup groupImpl(const QByteArray &aGroup) Q_DECL_OVERRIDE;
    const KConfigGroup groupImpl(const QByteArray &aGroup) const Q_DECL_OVERRIDE;

    KSharedConfig(const QString &file, OpenFlags mode,
                  QStandardPaths::StandardLocation resourceType);

};

typedef KSharedConfig::Ptr KSharedConfigPtr;

#endif // multiple inclusion guard
