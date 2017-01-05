/* This file is part of the KDE libraries
   Copyright (C) 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2006 David Faure <faure@kde.org>
   Copyright 2013 Sebastian Kügler <sebas@kde.org>

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
#ifndef __kplugintrader_h__
#define __kplugintrader_h__

#include "kplugininfo.h"
class KPluginTraderPrivate;
/**
 * \class KPluginTrader kplugintrader.h <KPluginTrader>
 *
 * A trader interface which provides a way to query specific subdirectories in the Qt
 * plugin paths for plugins. KPluginTrader provides an easy way to load a plugin
 * instance from a KPluginFactory, or just querying for existing plugins.
 *
 * KPluginTrader provides a way for an application to query directories in the
 * Qt plugin paths, accessed through QCoreApplication::libraryPaths().
 * Plugins may match a specific set of requirements. This allows to find
 * specific plugins at run-time without having to hard-code their names and/or
 * paths. KPluginTrader does not search recursively, you are rather encouraged
 * to install plugins into specific subdirectories to further speed searching.
 *
 * KPluginTrader exclusively searches within the plugin binaries' metadata
 * (via QPluginLoader::metaData()). It does not search these directories recursively.
 *
 * KPluginTrader does not use KServiceTypeTrader or KSyCoCa. As such, it will
 * only find binary plugins. If you are looking for a generic way to query for
 * services, use KServiceTypeTrader. For anything relating to mimetypes (type
 * of files), use KMimeTypeTrader.
 *
 * \par Example
 *
 * If you want to find all plugins for your application,
 * you would define a KMyApp/Plugin servicetype, and then you can query
 * the trader for it:
 * \code
 * KPluginInfo::List offers =
 *     KPluginTrader::self()->query("KMyApp/Plugin", "kf5");
 * \endcode
 *
 * You can add a constraint in the "trader query language". For instance:
 * \code
 * KPluginTrader::self()->query("KMyApp/Plugin", "kf5",
 *                                   "[X-KMyApp-InterfaceVersion] > 15");
 * \endcode
 *
 * Please note that when including property names containing arithmetic operators like - or +, then you have
 * to put brackets around the property name, in order to correctly separate arithmetic operations from
 * the name. So for example a constraint expression like
 * \code
 * X-KMyApp-InterfaceVersion > 4 // wrong!
 * \endcode
 * needs to be written as
 * \code
 * [X-KMyApp-InterfaceVersion] > 4
 * \endcode
 * otherwise it could also be interpreted as
 * Subtract the numeric value of the property "KMyApp" and "InterfaceVersion" from the
 * property "X" and make sure it is greater than 4.\n
 * Instead of the other meaning, make sure that the numeric value of "X-KMyApp-InterfaceVersion" is
 * greater than 4.
 *
 * @see KMimeTypeTrader, KServiceTypeTrader, KPluginInfo
 * @see QCoreApplication::libraryPaths
 * @see QT_PLUGIN_PATH (env variable)
 * @see KPluginFactory
 * @see kservice_desktop_to_json (Cmake macro)
 * @see K_PLUGIN_FACTORY_WITH_JSON (macro defined in KPluginFactory)
 *
 * @since 5.0
 */
class KSERVICE_EXPORT KPluginTrader
{
public:
    /**
     * Standard destructor
     */
    ~KPluginTrader();

    /**
     * The main function in the KPluginTrader class.
     *
     * It will return a list of plugins that match your specifications. Required parameter is the
     * service type and subdirectory. This method will append the subDirectory to every path found
     * in QCoreApplication::libraryPaths(), append the subDirectory parameter, and search through
     * the plugin's metadata
     *
     * KPluginTrader exclusively searches within the plugin binaries' metadata
     * (via QPluginLoader::metaData()). It does not search these directories recursively.
     *
     * The constraint parameter is used to limit the possible choices returned based on the
     * constraints you give it.
     *
     * The @p constraint language is rather full.  The most common
     * keywords are AND, OR, NOT, IN, and EXIST, all used in an
     * almost spoken-word form.  An example is:
     * \code
     * (Type == 'Service') and (('KParts/ReadOnlyPart' in ServiceTypes) or (exist Exec))
     * \endcode
     *
     * If you want to load a list of plugins from a specific subdirectory, you can do the following:
     *
     * \code
     *
     * KPluginInfo::List plugins = KPluginTrader::self()->query("plasma/engines");
     *
     * foreach (const KPluginInfo &info, plugins) {
     *      KPluginLoader loader(info.libraryPath());
     *      const QVariantList argsWithMetaData = QVariantList() << loader.metaData().toVariantMap();
     *      // In many cases, plugins are actually based on KPluginFactory, this is how that works:
     *      KPluginFactory* factory = loader.factory();
     *      if (factory) {
     *          Engine* component = factory->create<Engine>(parent, argsWithMetaData);
     *          if (component) {
     *              // Do whatever you want to do with the resulting object
     *          }
     *      }
     *      // Otherwise, just use the normal QPluginLoader methods
     *      Engine *myengine = qobject_cast<Engine*>(loader.instance());
     *      if (myengine) {
     *          // etc. ...
     *      }
     * }
     * \endcode
     *
     * If you have a specific query for just one plugin, use the createInstanceFromQuery method.
     *
     * The keys used in the query (Type, ServiceType, Exec) are all fields found in the .json files
     * which are compiled into the plugin binaries.
     *
     * @param subDirectory The subdirectory under the Qt plugin path
     * @param servicetype A service type like 'KMyApp/Plugin' or 'KFilePlugin'
     * @param constraint  A constraint to limit the choices returned, QString() to
     *                    get all services of the given @p servicetype
     *
     * @return A list of services that satisfy the query
     * @see http://techbase.kde.org/Development/Tutorials/Services/Traders#The_KTrader_Query_Language
     */
    KPluginInfo::List query(const QString &subDirectory, const QString &serviceType = QString(),
                            const QString &constraint = QString());

    /**
     * This is a static pointer to the KPluginTrader singleton.
     *
     * You will need to use this to access the KPluginTrader functionality since the
     * constructors are protected.
     *
     * @return Static KPluginTrader instance
     */
    static KPluginTrader *self();

    /**
     * Get a plugin from a trader query
     *
     * Example:
     * \code
     * KMyAppPlugin* plugin = KPluginTrader::createInstanceFromQuery<KMyAppPlugin>(subDirectory, serviceType, QString(), parentObject );
     * if ( plugin ) {
     *     ....
     * }
     * \endcode
     *
     * @param subDirectory The subdirectory under the Qt plugin pathes to search in
     * @param serviceType The type of service for which to find a plugin
     * @param constraint An optional constraint to pass to the trader (see KTrader)
     * @param parent The parent object for the part itself
     * @param args A list of arguments passed to the service component
     * @param error The string passed here will contain an error description.
     * @return A pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type.
     */
    template <class T>
    static T *createInstanceFromQuery(const QString &subDirectory,
                                      const QString &serviceType = QString(),
                                      const QString &constraint = QString(),
                                      QObject *parent = 0,
                                      const QVariantList &args = QVariantList(),
                                      QString *error = 0)
    {
        return createInstanceFromQuery<T>(subDirectory, serviceType, constraint, parent, 0, args, error);
    }

    /**
     * Get a plugin from a trader query
     *
     * This method works like
     * createInstanceFromQuery(const QString&, const QString& ,const QString&, QObject*,
     * const QVariantList&, QString*),
     * but you can specify an additional parent widget.  This is important for a KPart, for example.
     *
     * @param subDirectory The subdirectory under the Qt plugin pathes to search in
     * @param serviceType the type of service for which to find a plugin
     * @param constraint an optional constraint to pass to the trader (see KTrader)
     * @param parent the parent object for the part itself
     * @param parentWidget the parent widget for the plugin
     * @param args A list of arguments passed to the service component
     * @param error The string passed here will contain an error description.
     * @return A pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type.
     */
    template <class T>
    static T *createInstanceFromQuery(const QString &subDirectory,
                                      const QString &serviceType,
                                      const QString &constraint,
                                      QObject *parent,
                                      QWidget *parentWidget,
                                      const QVariantList &args = QVariantList(),
                                      QString *error = 0)
    {
        Q_UNUSED(parentWidget)
        Q_UNUSED(args)
        if (error) {
            error->clear();
        }
        const KPluginInfo::List offers = self()->query(subDirectory, serviceType, constraint);

        Q_FOREACH (const KPluginInfo &info, offers) {
            KPluginLoader loader(info.libraryPath());
            const QVariantList argsWithMetaData = QVariantList() << loader.metaData().toVariantMap();
            KPluginFactory *factory = loader.factory();
            if (factory) {
                T *component = factory->create<T>(parent, argsWithMetaData);
                if (component) {
                    return component;
                }
            }
        }
        if (error && error->isEmpty()) {
            *error = QCoreApplication::translate("", "No service matching the requirements was found");
        }
        return 0;
    }

    static void applyConstraints(KPluginInfo::List &lst,
                                 const QString &constraint);

private:
    /**
     * @internal
     */
    KPluginTrader();

    // disallow copy ctor and assignment operator
    KPluginTrader(const KPluginTrader &other);
    KPluginTrader &operator=(const KPluginTrader &rhs);

    KPluginTraderPrivate *const d;

    friend class KPluginTraderSingleton;
};

#endif
