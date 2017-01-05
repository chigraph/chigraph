/*  This file is part of the KDE project
    Copyright (C) 2014 Alex Richardson <arichardson.kde@gmail.com>

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

#ifndef KPLUGINMETADATA_H
#define KPLUGINMETADATA_H

#include "kcoreaddons_export.h"

#include <QExplicitlySharedDataPointer>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include <functional>

class KPluginLoader;
class QPluginLoader;
class QStringList;
class KPluginMetaDataPrivate;
class KAboutPerson;
class QObject;


/** This class allows easily accessing some standardized values from the JSON metadata that
 * can be embedded into Qt plugins. Additional plugin-specific metadata can be retrieved by
 * directly reading from the QJsonObject returned by KPluginMetaData::rawData().
 *
 * This class can be used instead of KPluginInfo from KService for applications that only load
 * Qt C++ plugins.
 *
 * The following keys will be read from an object "KPlugin" inside the metadata JSON:
 *
 * Key                | Accessor function    | JSON Type
 * -------------------| -------------------- | ---------------------
 * Name               | name()               | string
 * Description        | description()        | string
 * ExtraInformation   | extraInformation()   | string
 * Icon               | iconName()           | string
 * Authors            | authors()            | object array (KAboutPerson)
 * Category           | category()           | string
 * License            | license()            | string
 * Copyright          | copyrightText()      | string
 * Id                 | pluginId()           | string
 * Version            | version()            | string
 * Website            | website()            | string
 * EnabledByDefault   | isEnabledByDefault() | bool
 * ServiceTypes       | serviceTypes()       | string array
 * MimeTypes          | mimeTypes()          | string array
 * FormFactors        | formFactors()        | string array
 * Translators        | translators()        | object array (KAboutPerson)
 * OtherContributors  | otherContributors()  | object array (KAboutPerson)
 *
 * The Authors, Translators and OtherContributors keys are expected to be
 * list of objects that match the structure expected by KAboutPerson::fromJSON().
 *
 * An example metadata json file could look like this:
 * @verbatim
   {
     "KPlugin": {
        "Name": "Date and Time",
        "Description": "Date and time by timezone",
        "Icon": "preferences-system-time",
        "Authors": { "Name": "Aaron Seigo", "Email": "aseigo@kde.org" },
        "Category": "Date and Time",
        "Dependencies": [],
        "EnabledByDefault": "true",
        "License": "LGPL",
        "Id": "time",
        "Version": "1.0",
        "Website": "http://plasma.kde.org/",
        "ServiceTypes": ["Plasma/DataEngine"]
     }
   }
   @endverbatim
 *
 * @sa KAboutPerson::fromJSON()
 * @since 5.1
 */
class KCOREADDONS_EXPORT KPluginMetaData
{
public:

    /** Creates an invalid KPluginMetaData instance */
    KPluginMetaData();

    /**
     * Reads the plugin metadata from a KPluginLoader instance. You must call KPluginLoader::setFileName()
     * or use the appropriate constructor on @p loader before calling this.
     */
    KPluginMetaData(const KPluginLoader &loader);

    /**
     * Reads the plugin metadata from a QPluginLoader instance. You must call QPluginLoader::setFileName()
     * or use the appropriate constructor on @p loader before calling this.
     */
    KPluginMetaData(const QPluginLoader &loader);

    /**
     * Reads the plugin metadata from a plugin or .desktop which can be loaded from @p file.
     *
     * For plugins, platform-specific library suffixes may be omitted since @p file will be resolved
     * using the same logic as QPluginLoader.
     *
     * If the file name ends with ".desktop", the .desktop file will be parsed instead of
     * reading the metadata from the QPluginLoader. This is the same as calling
     * KPluginMetaData::fromDesktopFile() without the serviceTypes parameter.
     *
     * If @p file ends with .json, the file will be loaded as the QJsonObject metadata.
     *
     * @see QPluginLoader::setFileName()
     * @see KPluginMetaData::fromDesktopFile()
     */
    KPluginMetaData(const QString &file);

    /**
     * Creates a KPluginMetaData from a QJsonObject holding the metadata and a file name
     * This can be used if the data is not retrieved from a Qt C++ plugin library but from some
     * other source.
     * @see KPluginMetaData(const QJsonObject &, const QString &, const QString &)
     */
    KPluginMetaData(const QJsonObject &metaData, const QString &file);

    // TODO: KF6: merge with the above and make metaDataFile default to QString()
    /**
     * Creates a KPluginMetaData
     * @param metaData the JSON metadata to use for this object
     * @param pluginFile the file that the plugin can be loaded from
     * @param metaDataFile the file that the JSON metadata was read from
     *
     * This can be used if the data is not retrieved from a Qt C++ plugin library but from some
     * other source.
     *
     * @since 5.5
     */
    KPluginMetaData(const QJsonObject &metaData, const QString &pluginFile, const QString &metaDataFile);

    /**
     * Copy contructor
     */
    KPluginMetaData(const KPluginMetaData &);
    /**
     * Copy assignment
     */
    KPluginMetaData &operator=(const KPluginMetaData &);
    /**
     * Destructor
     */
    ~KPluginMetaData();

    /**
     * Load a KPluginMetaData instace from a .desktop file. Unlike the constructor which takes
     * a single file parameter this method allows you to specify which service type files should
     * be parsed to determine the correct type for a given .desktop property.
     * This ensures that a e.g. comma-separated string list field in the .desktop file will correctly
     * be converted to a JSON string array.
     *
     * @note This function mostly exists for backwards-compatibility. It is recommended
     * that new applications load JSON files directly instead of using .desktop files for plugin metadata.
     *
     * @param file the .desktop file to load
     * @param serviceTypes a list of files to parse If one of these paths is a relative path it
     * will be resolved relative to the "kservicetypes5" subdirectory in QStandardPaths::GenericDataLocation.
     * If the list is empty only the default set of properties will be treated specially and all other entries
     * will be read as the JSON string type.
     *
     * @since 5.16
     */
    static KPluginMetaData fromDesktopFile(const QString &file, const QStringList &serviceTypes = QStringList());

    /**
     * @return whether this object holds valid information about a plugin.
     * If this is @c true pluginId() will return a non-empty string.
     */
    bool isValid() const;

    /**
     * @return whether this object should be hidden, this is usually not used for binary
     * plugins, when loading a KPluginMetaData from a .desktop file, this will reflect
     * the value of the "Hidden" key.
     *
     * @since 5.8
     */
    bool isHidden() const;

    /**
     * @return the path to the plugin. This string can be passed to the KPluginLoader
     * or QPluginLoader constructors in order to attempt to load this plugin.
     * @note It is not guaranteed that this is a valid path to a shared library (i.e. loadable
     * by QPluginLoader) since the metadata could also refer to a non-C++ plugin.
     */
    QString fileName() const;

    /**
     * @return the file that the metadata was read from. This is not necessarily the same as
     * fileName(), since not all plugins have the metadata embedded. The metadata could also be
     * stored in a separate .desktop file.
     *
     * @since 5.5
     */
    QString metaDataFileName() const;

    /**
     * @return the full metadata stored inside the plugin file.
     */
    QJsonObject rawData() const;


    /**
     * Tries to instantiate this plugin using KPluginMetaData::fileName().
     * @note The value of KPluginMetaData::dependencies() is not used here, dependencies must be
     * resolved manually.
     *
     * @return The plugin root object or @c nullptr if it could not be loaded
     * @see QPluginLoader::instance(), KPluginLoader::instance()
     */
    QObject *instantiate() const;

    /**
     * @return the user visible name of the plugin.
     */
    QString name() const;

    /**
     * @return a short description of the plugin.
     */
    QString description() const;

    /**
     * @return additional information about this plugin (e.g. for use in an "about plugin" dialog)
     *
     * @since 5.18
     */
    QString extraInformation() const;

    /**
     * @return the author(s) of this plugin.
     */
    QList<KAboutPerson> authors() const;

    /**
     * @return the translator(s) of this plugin.
     *
     * @since 5.18
     */
    QList<KAboutPerson> translators() const;

    /**
     * @return a list of people that contributed to this plugin (other than the authors and translators).
     *
     * @since 5.18
     */
    QList<KAboutPerson> otherContributors() const;

    /**
     * @return the categories of this plugin (e.g. "playlist/skin").
     */
    QString category() const;

    /**
     * @return the icon name for this plugin
     * @see QIcon::fromTheme()
     */
    QString iconName() const;

    /**
     * @return the short license identifier (e.g. LGPL).
     * @see KAboutLicense::byKeyword() for retrieving the full license information
     */
    QString license() const;

    /**
     * @return a short copyright statement
     *
     * @since 5.18
     */
    QString copyrightText() const;

    /**
     * @return the internal name of the plugin (for KParts Plugins this is
     * the same name as set in the .rc file). If the plugin name property is not set in
     * the metadata this will return the plugin file name without the file extension.
     */
    QString pluginId() const;

    /**
     * @return the version of the plugin.
     */
    QString version() const;

    /**
     * @return the website of the plugin.
     */
    QString website() const;

    /**
     * @return a list of plugins that this plugin depends on so that it can function properly
     * @see KJsonPluginInfo::pluginId()
     */
    QStringList dependencies() const;

    /**
     * @note Unlike KService this does not contain the MIME types. To get the handled MIME types
     * use the KPluginMetaData::mimeTypes() function.
     * @return a list of service types this plugin implements (e.g. "Plasma/DataEngine")
     */
    QStringList serviceTypes() const;

    /**
     * @return a list of MIME types this plugin can handle (e.g. "application/pdf", "image/png", etc.)
     * @since 5.16
     */
    QStringList mimeTypes() const;

    /**
     * @return A string list of formfactors this plugin is useful for, e.g. desktop, tablet,
     * handset, mediacenter, etc.
     * The keys for this are not formally defined.
     *
     * @since 5.12
     */
    QStringList formFactors() const;

    /**
     * @return whether the plugin should be enabled by default.
     * This is only a recommendation, applications can ignore this value if they want to.
     */
    bool isEnabledByDefault() const;

    /**
     * @return the value for @p key from the metadata or @p defaultValue if the key does not exist
     * or the value for @p key is not of type string
     *
     * @see KPluginMetaData::rawData() if QString is not the correct type for @p key
     */
    QString value(const QString &key, const QString &defaultValue = QString()) const;

    /** @return the value for @p key inside @p jo as a string list. If the type of @p key is string, a list with containing
     * just that string will be returned, if it is an array the list will contain one entry for each array member.
     * If the key cannot be found an empty list will be returned.
     */
    static QStringList readStringList(const QJsonObject &jo, const QString &key);

    /**
     * Reads a value from @p jo but unlike QJsonObject::value() it allows different entries for each locale
     * This is done by appending the locale identifier in brackets to the key (e.g. "[de_DE]" or "[es]")
     * When looking for a key "foo" with German (Germany) locale we will first attempt to read "foo[de_DE]",
     * if that does not exist "foo[de]", finally falling back to "foo" if that also doesn't exist.
     * @return the translated value for @p key from @p jo or @p defaultValue if @p key was not found
     */
    static QJsonValue readTranslatedValue(const QJsonObject &jo, const QString &key, const QJsonValue &defaultValue = QJsonValue());

    /**
     * @return the translated value of @p key from @p jo as a string or @p defaultValue if @p key was not found
     * or the value for @p key is not of type string
     * @see KPluginMetaData::readTranslatedValue(const QJsonObject &jo, const QString &key)
     */
    static QString readTranslatedString(const QJsonObject &jo, const QString &key, const QString &defaultValue = QString());

    /**
     * @return @c true if this object is equal to @p other, otherwise @c false
     */
    bool operator==(const KPluginMetaData &other) const;

    /**
     * @return @c true if this object is not equal to @p other, otherwise @c false.
     */
    inline bool operator!=(const KPluginMetaData &other) const
    {
        return !(*this == other);
    }
private:
    QJsonObject rootObject() const;
    void loadFromDesktopFile(const QString &file, const QStringList &serviceTypes);
private:
    QJsonObject m_metaData;
    QString m_fileName;
    QExplicitlySharedDataPointer<KPluginMetaDataPrivate> d; // for future binary compatible extensions
};

inline uint qHash(const KPluginMetaData &md, uint seed)
{
    return qHash(md.pluginId(), seed);
}

#endif // KPLUGINMETADATA_H
