/*  This file is part of the KDE project
    Copyright (C) 2003,2007 Matthias Kretz <kretz@kde.org>

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

#ifndef KPLUGININFO_H
#define KPLUGININFO_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QStringList>

#include <kconfiggroup.h>
#include <kservice.h>
#include <QtCore/QList>

class KPluginMetaData;
class KPluginInfoPrivate;

/**
 * Information about a plugin.
 *
 * This holds all the information about a plugin there is. It's used for the
 * user to decide whether he wants to use this plugin or not.
 *
 * @author Matthias Kretz <kretz@kde.org>
 */
class KSERVICE_EXPORT KPluginInfo
{
public:
    typedef QList<KPluginInfo> List;

    /**
     * Read plugin info from @p filename.
     *
     * The file should be of the following form:
     * \verbatim
       [Desktop Entry]
       Encoding=UTF-8
       Icon=mypluginicon
       Type=Service
       ServiceTypes=KPluginInfo

       Name=User Visible Name
       Comment=Description of what the plugin does

       X-KDE-PluginInfo-Author=Author's Name
       X-KDE-PluginInfo-Email=author@foo.bar
       X-KDE-PluginInfo-Name=internalname
       X-KDE-PluginInfo-Version=1.1
       X-KDE-PluginInfo-Website=http://www.plugin.org/
       X-KDE-PluginInfo-Category=playlist
       X-KDE-PluginInfo-Depends=plugin1,plugin3
       X-KDE-PluginInfo-License=GPL
       X-KDE-PluginInfo-EnabledByDefault=true
       \endverbatim
     * The Name and Comment fields must always be present.
     *
     * The "X-KDE-PluginInfo" keys you may add further entries which
     * will be available using property(). The Website,Category,Require
     * keys are optional.
     * For EnabledByDefault look at isPluginEnabledByDefault.
     *
     * @param filename  The filename of the .desktop file.
     * @param resource  If filename is relative, you need to specify a resource type
     * (e.g. "service", "apps"... KStandardDirs). Otherwise,
     * resource isn't used.
     */
    explicit KPluginInfo(const QString &filename /*, QStandardPaths::StandardLocation resource = ...? GenericDataLocation + services ? Is this used? */);

    /**
     * Read plugin info from a KService object.
     *
     * The .desktop file should look like this:
     * \verbatim
       [Desktop Entry]
       Encoding=UTF-8
       Icon=mypluginicon
       Type=Service
       ServiceTypes=KPluginInfo

       X-KDE-PluginInfo-Author=Author's Name
       X-KDE-PluginInfo-Email=author@foo.bar
       X-KDE-PluginInfo-Name=internalname
       X-KDE-PluginInfo-Version=1.1
       X-KDE-PluginInfo-Website=http://www.plugin.org/
       X-KDE-PluginInfo-Category=playlist
       X-KDE-PluginInfo-Depends=plugin1,plugin3
       X-KDE-PluginInfo-License=GPL
       X-KDE-PluginInfo-EnabledByDefault=true

       Name=User Visible Name
       Comment=Description of what the plugin does
       \endverbatim
     * In the first three entries the Icon entry is optional.
     * @deprecated since 5.0, use (Q|K)PluginLoader instead and build the
     * metadata into the plugin using K_PLUGIN_FACTORY_WITH_JSON( ..., "mypluginmetadata.json", ...)
     */
#ifndef KSERVICE_NO_DEPRECATED
    explicit KSERVICE_DEPRECATED KPluginInfo(const KService::Ptr service);
#endif

    /**
     * Read plugin info from arguments passed to the plugin. These arguments should contain
     * the plugin's metadata (as read from QPluginLoader::metaData(). This constructor uses
     * the metadata read from the QVariantList. It reads the first QVariantMap it finds in a
     * field called "MetaData".
     *
     * Use (Q|K)PluginLoader and build the metadata into the plugin using
     * K_PLUGIN_FACTORY_WITH_JSON( ..., "mypluginmetadata.json", ...)
     *
     * You can use the "desktoptojson tool to generate a .json file from your .desktop file.
     * The .json file should look like this:
     *
     * \verbatim
        {
            "Comment": "Date and time by timezone",
            "Icon": "preferences-system-time",
            "Name": "Date and Time",
            "Type": "Service",
            "X-KDE-Library": "plasma_engine_time",
            "X-KDE-PluginInfo-Author": "Aaron Seigo",
            "X-KDE-PluginInfo-Category": "Date and Time",
            "X-KDE-PluginInfo-Depends": [
            ],
            "X-KDE-PluginInfo-Email": "aseigo@kde.org",
            "X-KDE-PluginInfo-EnabledByDefault": true,
            "X-KDE-PluginInfo-License": "LGPL",
            "X-KDE-PluginInfo-Name": "time",
            "X-KDE-PluginInfo-Version": "1.0",
            "X-KDE-PluginInfo-Website": "http://plasma.kde.org/",
            "X-KDE-ServiceTypes": [
                "Plasma/DataEngine"
            ],
            "X-KDE-FormFactors": [
                "tablet",
                "handset"
            ]
        }
       \endverbatim
     * @param args QVariantList with arguments, should contain a QVariantMap, keyed "MetaData"
     * as provided by QPluginLoader::metaData()
     * @param libraryPath The path to the plugin file on disk
     *
     * \see K_PLUGIN_FACTORY_WITH_JSON()
     * \see KPluginFactory::factory()
     * @since 5.0
     */
    explicit KPluginInfo(const QVariantList &args, const QString &libraryPath = QString());


    /**
     * Read plugin info from a KPluginMetaData object.
     *
     * @param md The KPluginMetaData to read the information from
     * @see K_PLUGIN_FACTORY_WITH_JSON()
     * @see KPluginLoader
     * @since 5.5
     */
    explicit KPluginInfo(const KPluginMetaData &md);

    /**
     * Creates an invalid plugin.
     *
     * \see isValid
     */
    KPluginInfo();

    ~KPluginInfo();

    /**
     * @return A list of KPluginInfo objects constructed from a list of
     * KService objects. If you get a trader offer of the plugins you want
     * to use you can just pass them to this function.
     *
     * @param services The list of services to construct the list of KPluginInfo objects from
     * @param config The config group where to save/load whether the plugin is enabled/disabled
     * @deprecated since 5.0, use (Q|K)PluginLoader instead and build the
     * metadata into the plugins using K_PLUGIN_FACTORY_WITH_JSON( ..., "mypluginmetadata.json", ...)
     */
#ifndef KSERVICE_NO_DEPRECATED
    static KSERVICE_DEPRECATED KPluginInfo::List fromServices(const KService::List &services, const KConfigGroup &config = KConfigGroup());
#endif

    /**
     * @return A list of KPluginInfo objects constructed from a list of
     * filenames. If you make a lookup using, for example,
     * KStandardDirs::findAllResources() you pass the list of files to this
     * function.
     *
     * @param files The list of files to construct the list of KPluginInfo objects from
     * @param config The config group where to save/load whether the plugin is enabled/disabled
     */
    static KPluginInfo::List fromFiles(const QStringList &files, const KConfigGroup &config = KConfigGroup());

    /**
     * @return A list of KPluginInfo objects for the KParts plugins of a
     * component.
     *
     * @param componentName Use the component name to look up all KParts plugins for it.
     * @param config The config group where to save/load whether the plugin is enabled/disabled
     */
    static KPluginInfo::List fromKPartsInstanceName(const QString &componentName, const KConfigGroup &config = KConfigGroup());

    /**
     * @return Whether the plugin should be hidden.
     */
    bool isHidden() const;

    /**
     * Set whether the plugin is currently loaded.
     *
     * @see isPluginEnabled()
     * @see save()
     */
    void setPluginEnabled(bool enabled);

    /**
     * @return Whether the plugin is currently loaded.
     *
     * @see setPluginEnabled()
     * @see load()
     */
    bool isPluginEnabled() const;

    /**
     * @return The default value whether the plugin is enabled or not.
     * Defaults to the value set in the desktop file, or if that isn't set
     * to false.
     */
    bool isPluginEnabledByDefault() const;

    /**
     * @return The value associated to the @p key. You can use it if you
     *         want to read custom values. To do this you need to define
     *         your own servicetype and add it to the ServiceTypes keys.
     */
    QVariant property(const QString &key) const;

    /**
     * @return All properties of this object. This can be used to read custom values.
     * @since 5.3
     * @see KPluginInfo::property()
     */
    QVariantMap properties() const;

    /**
     * @return The user visible name of the plugin.
     */
    QString name() const;

    /**
     * @return A comment describing the plugin.
     */
    QString comment() const;

    /**
     * @return The iconname for this plugin
     */
    QString icon() const;

    /**
     * @return The file containing the information about the plugin.
     */
    QString entryPath() const;

    /**
     * @return The author of this plugin.
     */
    QString author() const;

    /**
     * @return The email address of the author.
     */
    QString email() const;

    /**
     * @return The category of this plugin (e.g. playlist/skin).
     */
    QString category() const;

    /**
     * @return The internal name of the plugin (for KParts Plugins this is
     * the same name as set in the .rc file).
     */
    QString pluginName() const;

    /**
     * @return The version of the plugin.
     */
    QString version() const;

    /**
     * @return The website of the plugin/author.
     */
    QString website() const;

    /**
     * @return The license keyword of this plugin.
     */
    QString license() const;

#if 0 // removed in KF5 to avoid dependency on KAboutData. Seems unused anyway.
    /**
     * @return The full license object, according to the license keyword.
     *         It can be used to present friendlier and more detailed
     *         license info to the user, when the license is one of the
     *         widespread within KDE. For other licenses, the license
     *         object will state not very useful, "custom license" info
     *         (this can be identified by KAboutLicense::key() returning
     *          KAboutData::License_Custom).
     *
     * @see KAboutLicense::byKeyword()
     * @since 4.1
     */
    KAboutLicense fullLicense() const;
#endif

    /**
     * @return A list of plugins required for this plugin to be enabled. Use
     *         the pluginName in this list.
     */
    QStringList dependencies() const;

    /**
     * @return A list of ServiceTypes this plugin offers
     * @since 5.0
     */
    QStringList serviceTypes() const;

    /**
     * @return A list of FormFactors this plugin offers, corresponds to the
     * "X-KDE-FormFactors" value in a .desktop service file, or to the "FormFactors"
     * value in the "KPlugin" block of the json metadata. Formfactor values are
     * freestyle, common values are "desktop", "handset", "tablet", "mediacenter".
     * Values are comma-separated.
     * @since 5.14
     */
    QStringList formFactors() const;

    /**
     * @return The absolute path of the plugin on disk. This can be used to load the plugin from, using
     * KPluginLoader or QPluginLoader
     * @since 5.0
     */
    QString libraryPath() const;

    /**
     * @return The KService object for this plugin. You might need it if you
     *         want to read custom values. To do this you need to define
     *         your own servicetype and add it to the ServiceTypes keys.
     *         Then you can use the KService::property() method to read your
     *         keys.
     *
     * @see property()
     */
    KService::Ptr service() const;

    /**
     * @return A list of Service pointers if the plugin installs one or more
     *         KCModule
     */
    QList<KService::Ptr> kcmServices() const;

    /**
     * Set the KConfigGroup to use for load()ing and save()ing the
     * configuration. This will be overridden by the KConfigGroup passed to
     * save() or load() (if one is passed).
     */
    void setConfig(const KConfigGroup &config);

    /**
     * @return If the KPluginInfo object has a KConfig object set return
     * it, else returns an invalid KConfigGroup.
     */
    KConfigGroup config() const;

    /**
     * Save state of the plugin - enabled or not.
     *
     * @param config    The KConfigGroup holding the information whether
     *                  plugin is enabled.
     */
    void save(KConfigGroup config = KConfigGroup());

    /**
     * Load the state of the plugin - enabled or not.
     *
     * @param config    The KConfigGroup holding the information whether
     *                  plugin is enabled.
     */
    void load(const KConfigGroup &config = KConfigGroup());

    /**
     * Restore defaults (enabled or not).
     */
    void defaults();

    /**
     * Returns whether the object is valid. Treat invalid KPluginInfo objects like you would
     * treat a null pointer.
     */
    bool isValid() const;

    /**
     * Creates a KPluginInfo object that shares the data with \p copy.
     */
    KPluginInfo(const KPluginInfo &copy);

    /**
     * Copies the KPluginInfo object to share the data with \p copy.
     */
    KPluginInfo &operator=(const KPluginInfo &rhs);

    /**
     * Compares two objects whether they share the same data.
     */
    bool operator==(const KPluginInfo &rhs) const;

    /**
     * Compares two objects whether they don't share the same data.
     */
    bool operator!=(const KPluginInfo &rhs) const;

    /**
     * Less than relation comparing the categories and if they are the same using the names.
     */
    bool operator<(const KPluginInfo &rhs) const;

    /**
     * Greater than relation comparing the categories and if they are the same using the names.
     */
    bool operator>(const KPluginInfo &rhs) const;

    friend class KPluginTrader;

    /**
     * @return a KPluginMetaData object with equivalent meta data.
     * @since 5.3
     */
    KPluginMetaData toMetaData() const;

    /**
     * @param info the KPluginInfo object to convert
     * @return a KPluginMetaData object with equivalent meta data.
     * @since 5.3
     */
    static KPluginMetaData toMetaData(const KPluginInfo &info);

    /**
     * @param meta the KPluginMetaData to convert
     * @return a KPluginInfo object with equivalent meta data.
     * @since 5.3
     */
    static KPluginInfo fromMetaData(const KPluginMetaData &meta);

    /**
     * @param list the list of KPluginInfo objects to convert
     * @return a list of KPluginMetaData objects with equivalent meta data.
     * @since 5.3
     */
    static QVector<KPluginMetaData> toMetaData(const KPluginInfo::List &list);

    /**
     * @param list the list of KPluginMetaData objects to convert
     * @return a list of KPluginInfo objects with equivalent meta data.
     * @since 5.3
     */
    static KPluginInfo::List fromMetaData(const QVector<KPluginMetaData> &list);

private:
    friend KSERVICE_EXPORT uint qHash(const KPluginInfo &);
    QExplicitlySharedDataPointer<KPluginInfoPrivate> d;
};

KSERVICE_EXPORT uint qHash(const KPluginInfo &);

#endif // KPLUGININFO_H
