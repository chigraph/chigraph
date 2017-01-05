/*
 * This file is part of KDE.
 *
 * Copyright (c) 2001,2002,2003 Cornelius Schumacher <schumacher@kde.org>
 * Copyright (c) 2003 Waldo Bastian <bastian@kde.org>
 *
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
 */

#ifndef KCONFIGSKELETON_H
#define KCONFIGSKELETON_H

#include <kconfiggui_export.h>

#include <kcoreconfigskeleton.h>

#include <QColor>
#include <QFont>

/**
 * @short Class for handling preferences settings for an application.
 * @author Cornelius Schumacher
 *
 * This class extends KCoreConfigSkeleton by support for GUI types.
 *
 */
class KCONFIGGUI_EXPORT KConfigSkeleton : public KCoreConfigSkeleton
{
    Q_OBJECT
public:
    /**
     * Class for handling a color preferences item.
     */
    class KCONFIGGUI_EXPORT ItemColor: public KConfigSkeletonGenericItem < QColor >
    {
    public:
        /** @copydoc KConfigSkeletonGenericItem::KConfigSkeletonGenericItem */
        ItemColor(const QString &_group, const QString &_key,
                  QColor &reference,
                  const QColor &defaultValue = QColor(128, 128, 128));

        /** @copydoc KConfigSkeletonItem::readConfig(KConfig*) */
        void readConfig(KConfig *config) Q_DECL_OVERRIDE;

        /** @copydoc KConfigSkeletonItem::setProperty(const QVariant&) */
        void setProperty(const QVariant &p) Q_DECL_OVERRIDE;

        /** @copydoc KConfigSkeletonItem::isEqual(const QVariant &) */
        bool isEqual(const QVariant &p) const Q_DECL_OVERRIDE;

        /** @copydoc KConfigSkeletonItem::property() */
        QVariant property() const Q_DECL_OVERRIDE;
    };

    /**
     * Class for handling a font preferences item.
     */
    class KCONFIGGUI_EXPORT ItemFont: public KConfigSkeletonGenericItem < QFont >
    {
    public:
        /** @copydoc KConfigSkeletonGenericItem::KConfigSkeletonGenericItem */
        ItemFont(const QString &_group, const QString &_key, QFont &reference,
                 const QFont &defaultValue = QFont());

        /** @copydoc KConfigSkeletonItem::readConfig(KConfig*) */
        void readConfig(KConfig *config) Q_DECL_OVERRIDE;

        /** @copydoc KConfigSkeletonItem::setProperty(const QVariant&) */
        void setProperty(const QVariant &p) Q_DECL_OVERRIDE;

        /** @copydoc KConfigSkeletonItem::isEqual(const QVariant &) */
        bool isEqual(const QVariant &p) const Q_DECL_OVERRIDE;

        /** @copydoc KConfigSkeletonItem::property() */
        QVariant property() const Q_DECL_OVERRIDE;
    };

public:
    /**
     * Constructor.
     *
     * @param configname name of config file. If no name is given, the default
     * config file as returned by KSharedConfig::openConfig() is used.
     */
    explicit KConfigSkeleton(const QString &configname = QString(), QObject *parent = Q_NULLPTR);

    /**
     * Constructor.
     *
     * @param config configuration object to use.
     */
    explicit KConfigSkeleton(KSharedConfig::Ptr config, QObject *parent = Q_NULLPTR);

    /**
     * Register an item of type QColor.
     *
     * @param name Name used to identify this setting. Names must be unique.
     * @param reference Pointer to the variable, which is set by read()
     * calls and read by save() calls.
     * @param defaultValue Default value, which is used when the config file
     * does not yet contain the key of this item.
     * @param key Key used in config file. If key is null, name is used as key.
     * @return The created item
     */
    ItemColor *addItemColor(const QString &name, QColor &reference,
                            const QColor &defaultValue = QColor(128, 128, 128),
                            const QString &key = QString());

    /**
     * Register an item of type QFont.
     *
     * @param name Name used to identify this setting. Names must be unique.
     * @param reference Pointer to the variable, which is set by read()
     * calls and read by save() calls.
     * @param defaultValue Default value, which is used when the config file
     * does not yet contain the key of this item.
     * @param key Key used in config file. If key is null, name is used as key.
     * @return The created item
     */
    ItemFont *addItemFont(const QString &name, QFont &reference,
                          const QFont &defaultValue = QFont(),
                          const QString &key = QString());

};

#endif
