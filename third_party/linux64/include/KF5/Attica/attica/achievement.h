/*
    This file is part of KDE.

    Copyright (c) 2011 Laszlo Papp <djszapi@archlinux.us>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ATTICA_ACHIEVEMENT_H
#define ATTICA_ACHIEVEMENT_H

#include "attica_export.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

namespace Attica
{

class ATTICA_EXPORT Achievement
{
public:
    typedef QList<Achievement> List;
    class Parser;

    enum Type {
        FlowingAchievement,
        SteppedAchievement,
        NamedstepsAchievement,
        SetAchievement
    };
    static Achievement::Type stringToAchievementType(const QString &achievementTypeString);
    static QString achievementTypeToString(const Achievement::Type type);

    enum Visibility {
        VisibleAchievement,
        DependentsAchievement,
        SecretAchievement
    };
    static Achievement::Visibility stringToAchievementVisibility(const QString &achievementVisibilityString);
    static QString achievementVisibilityToString(const Achievement::Visibility visibility);

    Achievement();
    Achievement(const Achievement &other);
    Achievement &operator=(const Achievement &other);
    ~Achievement();

    void setId(const QString &id);
    QString id() const;

    void setContentId(const QString &contentId);
    QString contentId() const;

    void setName(const QString &name);
    QString name() const;

    void setDescription(const QString &description);
    QString description() const;

    void setExplanation(const QString &explanation);
    QString explanation() const;

    void setPoints(const int points);
    int points() const;

    void setImage(const QUrl &image);
    QUrl image() const;

    void setDependencies(const QStringList &dependencies);
    void addDependency(const QString &dependency);
    void removeDependency(const QString &dependency);
    QStringList dependencies() const;

    void setVisibility(Achievement::Visibility visibility);
    Achievement::Visibility visibility() const;

    void setType(Achievement::Type type);
    Achievement::Type type() const;

    void setOptions(const QStringList &options);
    void addOption(const QString &option);
    void removeOption(const QString &option);
    QStringList options() const;

    void setSteps(const int steps);
    int steps() const;

    void setProgress(const QVariant &progress);
    QVariant progress() const;

    bool isValid() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif
