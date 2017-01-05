/* This file is part of the KDE project
   Copyright (C) 2000 Waldo Bastian <bastian@kde.org>

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

#ifndef KSERVICEGROUP_H
#define KSERVICEGROUP_H

#include <kservice_export.h>
#include <kservice.h>

class KBuildServiceGroupFactory;

class KServiceGroupPrivate;

/**
 * KServiceGroup represents a group of service, for example
 * screensavers.
 * This class is typically used like this:
 *
 * \code
 * // Start from root group
 * KServiceGroup::Ptr group = KServiceGroup::root();
 * if (!group || !group->isValid()) return;
 *
 * KServiceGroup::List list = group->entries();
 *
 * // Iterate over all entries in the group
 * for( KServiceGroup::List::ConstIterator it = list.begin();
 *      it != list.end(); it++)
 * {
 *    KSycocaEntry *p = (*it);
 *    if (p->isType(KST_KService))
 *    {
 *       KService *s = static_cast<KService *>(p);
 *       printf("Name = %s\n", s->name().toLatin1());
 *    }
 *    else if (p->isType(KST_KServiceGroup))
 *    {
 *       KServiceGroup *g = static_cast<KServiceGroup *>(p);
 *       // Sub group ...
 *    }
 * }
 * \endcode
 *
 * @short Represents a group of services
 */
class KSERVICE_EXPORT KServiceGroup : public KSycocaEntry
{
    friend class KBuildServiceGroupFactory;
public:
    typedef QExplicitlySharedDataPointer<KServiceGroup> Ptr;
    typedef QExplicitlySharedDataPointer<KSycocaEntry> SPtr;
    typedef QList<SPtr> List;
public:
    /**
     * Construct a dummy servicegroup indexed with @p name.
     * @param name the name of the service group
     */
    KServiceGroup(const QString &name);

    /**
     * Construct a service and take all information from a config file
     * @param _fullpath full path to the config file
     * @param _relpath relative path to the config file
     */
    KServiceGroup(const QString &_fullpath, const QString &_relpath);

    virtual ~KServiceGroup();

    /**
     * Returns the relative path of the service group.
     * @return the service group's relative path
     */
    QString relPath() const;

    /**
     * Returns the caption of this group.
     * @return the caption of this group
     */
    QString caption() const;

    /**
     * Returns the name of the icon associated with the group.
     * @return the name of the icon associated with the group,
     *         or QString() if not set
     */
    QString icon() const;

    /**
     * Returns the comment about this service group.
     * @return the descriptive comment for the group, if there is one,
     *         or QString() if not set
     */
    QString comment() const;

    /**
     * Returns the total number of displayable services in this group and
     * any of its subgroups.
     * @return the number of child services
     */
    int childCount() const;

    /**
     * Returns true if the NoDisplay flag was set, i.e. if this
     * group should be hidden from menus, while still being in ksycoca.
     * @return true to hide this service group, false to display it
     */
    bool noDisplay() const;

    /**
     * Return true if we want to display empty menu entry
     * @return true to show this service group as menu entry is empty, false to hide it
     */
    bool showEmptyMenu() const;
    void setShowEmptyMenu(bool b);

    /**
     * @return true to show an inline header into menu
     */
    bool showInlineHeader() const;
    void setShowInlineHeader(bool _b);

    /**
     * @return true to show an inline alias item into menu
     */
    bool inlineAlias() const;
    void setInlineAlias(bool _b);
    /**
     * @return true if we allow to inline menu.
     */
    bool allowInline() const;
    void setAllowInline(bool _b);

    /**
     * @return inline limite value
     */
    int inlineValue() const;
    void setInlineValue(int _val);

    /**
     * Returns a list of untranslated generic names that should be
     * be suppressed when showing this group.
     * E.g. The group "Games/Arcade" might want to suppress the generic name
     * "Arcade Game" since it's redundant in this particular context.
     */
    QStringList suppressGenericNames() const;

    /**
     * @internal
     * Sets information related to the layout of services in this group.
     */
    void setLayoutInfo(const QStringList &layout);

    /**
     * @internal
     * Returns information related to the layout of services in this group.
     */
    QStringList layoutInfo() const;

    /**
     * List of all Services and ServiceGroups within this
     * ServiceGroup.
     * @param sorted true to sort items
     * @param excludeNoDisplay true to exclude items marked "NoDisplay"
     * @param allowSeparators true to allow separator items to be included
     * @param sortByGenericName true to sort GenericName+Name instead of Name+GenericName
     * @return the list of entries
     */
    List entries(bool sorted, bool excludeNoDisplay, bool allowSeparators, bool sortByGenericName = false);
    List entries(bool sorted, bool excludeNoDisplay);

    /**
     * List of all Services and ServiceGroups within this
     * ServiceGroup.
     * @param sorted true to sort items
     * @return the list of entried
     */
    List entries(bool sorted = false);

    /**
     * options for groupEntries and serviceEntries
     */
    enum EntriesOption {
        NoOptions = 0x0,
        SortEntries = 0x1,    /*< sort items */
        ExcludeNoDisplay = 0x2, /*< exclude items marked "NoDisplay" */
        AllowSeparators = 0x4, /*< allow separator items to be included */
        SortByGenericName = 0x8 /*< sort by GenericName+Name instead of Name+GenericName */
    };
    Q_DECLARE_FLAGS(EntriesOptions, EntriesOption)

    /**
     * subgroups for this service group
     */
    QList<Ptr> groupEntries(EntriesOptions options = ExcludeNoDisplay);

    /**
     * entries of this service group
     */
    KService::List serviceEntries(EntriesOptions options = ExcludeNoDisplay);

    /**
     * Returns a non-empty string if the group is a special base group.
     * By default, "Settings/" is the kcontrol base group ("settings")
     * and "System/Screensavers/" is the screensavers base group ("screensavers").
     * This allows moving the groups without breaking those apps.
     *
     * The base group is defined by the X-KDE-BaseGroup key
     * in the .directory file.
     * @return the base group name, or null if no base group
     */
    QString baseGroupName() const;

    /**
     * Returns a path to the .directory file describing this service group.
     * The path is either absolute or relative to the "apps" resource.
     */
    QString directoryEntryPath() const;

    /**
     * Returns the root service group.
     * @return the root service group
     */
    static Ptr root();

    /**
     * Returns the group with the given relative path.
     * @param relPath the path of the service group
     * @return the group with the given relative path name.
     */
    static Ptr group(const QString &relPath);

    /**
     * Returns the group of services that have X-KDE-ParentApp equal
     * to @p parent (siblings).
     * @param parent the name of the service's parent
     * @return the services group
     */
    static Ptr childGroup(const QString &parent);

protected:
    /**
     * @internal
     * Add a service to this group
     */
    void addEntry(const KSycocaEntry::Ptr &entry);

private:
    friend class KServiceGroupFactory;
    /**
     * @internal construct a service from a stream.
     * The stream must already be positionned at the correct offset
     */
    KServiceGroup(QDataStream &_str, int offset, bool deep);

    Q_DECLARE_PRIVATE(KServiceGroup)
};

#endif
