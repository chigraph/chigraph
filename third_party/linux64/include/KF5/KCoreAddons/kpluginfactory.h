/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2007 Bernhard Loos <nhuh.put@web.de>

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

#ifndef KPLUGINFACTORY_H
#define KPLUGINFACTORY_H

#include "kcoreaddons_export.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <kexportplugin.h> // for source compat

class KPluginFactoryPrivate;
namespace KParts
{
class Part;
}

#define KPluginFactory_iid "org.kde.KPluginFactory"

#define K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY_SKEL(name, baseFactory, ...) \
    class name : public KPluginFactory \
    { \
        Q_OBJECT \
        Q_INTERFACES(KPluginFactory) \
        __VA_ARGS__ \
    public: \
        explicit name(); \
        ~name(); \
    private: \
        void init(); \
    };

#define K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY_JSON(name, baseFactory, json) \
    K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY_SKEL(name, baseFactory, Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE json))

#define K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, baseFactory) \
    K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY_SKEL(name, baseFactory, Q_PLUGIN_METADATA(IID KPluginFactory_iid))

#define K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations) \
    name::name() \
    { \
        pluginRegistrations \
    } \
    name::~name() {}

#define K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations) \
    K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, baseFactory) \
    K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations)

#define K_PLUGIN_FACTORY_WITH_BASEFACTORY_JSON(name, baseFactory, jsonFile, pluginRegistrations) \
    K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY_JSON(name, baseFactory, jsonFile) \
    K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations)

/**
 * \relates KPluginFactory
 *
 * Create a KPluginFactory subclass and export it as the root plugin object.
 *
 * \param name The name of the KPluginFactory derived class.
 *
 * \param pluginRegistrations Code to be inserted into the constructor of the
 * class. Usually a series of registerPlugin() calls.
 *
 * Example:
 * \code
 * #include <KPluginFactory>
 * #include <plugininterface.h>
 *
 * class MyPlugin : public PluginInterface
 * {
 * public:
 *     MyPlugin(QObject *parent, const QVariantList &args)
 *         : PluginInterface(parent)
 *     {}
 * };
 *
 * K_PLUGIN_FACTORY(MyPluginFactory,
 *                  registerPlugin<MyPlugin>();
 *                 )
 *
 * #include <myplugin.moc>
 * \endcode
 *
 * If you want to compile a .json file into the plugin, use K_PLUGIN_FACTORY_WITH_JSON.
 *
 * \see K_PLUGIN_FACTORY_WITH_JSON
 * \see K_PLUGIN_FACTORY_DECLARATION
 * \see K_PLUGIN_FACTORY_DEFINITION
 */
#define K_PLUGIN_FACTORY(name, pluginRegistrations) K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)

/**
 * \relates KPluginFactory
 *
 * Create a KPluginFactory subclass and export it as the root plugin object with
 * JSON metadata.
 *
 * This macro does the same as K_PLUGIN_FACTORY, but adds a JSON file as plugin
 * metadata.  See Q_PLUGIN_METADATA() for more information.
 *
 * \param name The name of the KPluginFactory derived class.
 *
 * \param pluginRegistrations Code to be inserted into the constructor of the
 * class. Usually a series of registerPlugin() calls.
 *
 * \param jsonFile Name of the json file to be compiled into the plugin as metadata
 *
 * Example:
 * \code
 * #include <KPluginFactory>
 * #include <plugininterface.h>
 *
 * class MyPlugin : public PluginInterface
 * {
 * public:
 *     MyPlugin(QObject *parent, const QVariantList &args)
 *         : PluginInterface(parent)
 *     {}
 * };
 *
 * K_PLUGIN_FACTORY_WITH_JSON(MyPluginFactory,
 *                  "metadata.json",
 *                  registerPlugin<MyPlugin>();
 *                 )
 *
 * #include <myplugin.moc>
 * \endcode
 *
 * \see K_PLUGIN_FACTORY
 * \see K_PLUGIN_FACTORY_DECLARATION
 * \see K_PLUGIN_FACTORY_DEFINITION
 *
 * @since 5.0
 */
#define K_PLUGIN_FACTORY_WITH_JSON(name, jsonFile, pluginRegistrations)  K_PLUGIN_FACTORY_WITH_BASEFACTORY_JSON(name, KPluginFactory, jsonFile, pluginRegistrations)

/**
 * \relates KPluginFactory
 *
 * K_PLUGIN_FACTORY_DECLARATION declares the KPluginFactory subclass. This macro
 * can be used in a header file.
 *
 * \param name The name of the KPluginFactory derived class.
 *
 * \see K_PLUGIN_FACTORY
 * \see K_PLUGIN_FACTORY_DEFINITION
 */
#define K_PLUGIN_FACTORY_DECLARATION(name) K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, KPluginFactory)

/**
 * \relates KPluginFactory
 * K_PLUGIN_FACTORY_DEFINITION defines the KPluginFactory subclass. This macro
 * can <b>not</b> be used in a header file.
 *
 * \param name The name of the KPluginFactory derived class.
 *
 * \param pluginRegistrations Code to be inserted into the constructor of the
 * class. Usually a series of registerPlugin() calls.
 *
 * \see K_PLUGIN_FACTORY
 * \see K_PLUGIN_FACTORY_DECLARATION
 */
#define K_PLUGIN_FACTORY_DEFINITION(name, pluginRegistrations) K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)

/**
 * \class KPluginFactory kpluginfactory.h <KPluginFactory>
 *
 * KPluginFactory provides a convenient way to provide factory-style plugins.
 * Qt plugins provide a singleton object, but a common pattern is for plugins
 * to generate as many objects of a particular type as the application requires.
 * By using KPluginFactory, you can avoid implementing the factory pattern
 * yourself.
 *
 * KPluginFactory also allows plugins to provide multiple different object
 * types, indexed by keywords.
 *
 * The objects created by KPluginFactory must inherit QObject, and must have a
 * standard constructor pattern:
 * \li if the object is a KPart::Part, it must be of the form
 * \code
 * T(QWidget *parentWidget, QObject *parent, const QVariantList &args)
 * \endcode
 * \li if it is a QWidget, it must be of the form
 * \code
 * T(QWidget *parent, const QVariantList &args)
 * \endcode
 * \li otherwise it must be of the form
 * \code
 * T(QObject *parent, const QVariantList &args)
 * \endcode
 *
 * You should typically use either K_PLUGIN_FACTORY() or
 * K_PLUGIN_FACTORY_WITH_JSON() in your plugin code to create the factory.  The
 * typical pattern is
 *
 * \code
 * #include <KPluginFactory>
 * #include <plugininterface.h>
 *
 * class MyPlugin : public PluginInterface
 * {
 * public:
 *     MyPlugin(QObject *parent, const QVariantList &args)
 *         : PluginInterface(parent)
 *     {}
 * };
 *
 * K_PLUGIN_FACTORY(MyPluginFactory,
 *                  registerPlugin<MyPlugin>();
 *                 )
 * #include <myplugin.moc>
 * \endcode
 *
 * If you want to write a custom KPluginFactory not using the standard macro(s)
 * you can reimplement the
 * create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword)
 * method.
 *
 * Example:
 * \code
 * class SomeScriptLanguageFactory : public KPluginFactory
 * {
 *     Q_OBJECT
 * public:
 *     SomeScriptLanguageFactory()
 *     {}
 *
 * protected:
 *     virtual QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword)
 *     {
 *         const QString identifier = QLatin1String(iface) + QLatin1Char('_') + keyword;
 *         // load scripting language module from the information in identifier
 *         // and return it:
 *         return object;
 *     }
 * };
 * \endcode
 *
 * If you want to load a library use KPluginLoader.
 * The application that wants to instantiate plugin classes can do the following:
 * \code
 * KPluginFactory *factory = KPluginLoader("libraryname").factory();
 * if (factory) {
 *     PluginInterface *p1 = factory->create<PluginInterface>(parent);
 *     OtherInterface *p2  = factory->create<OtherInterface>(parent);
 *     NextInterface *p3   = factory->create<NextInterface>("keyword1", parent);
 *     NextInterface *p3   = factory->create<NextInterface>("keyword2", parent);
 * }
 * \endcode
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \author Bernhard Loos <nhuh.put@web.de>
 */
class KCOREADDONS_EXPORT KPluginFactory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(KPluginFactory)
public:
    /**
     * This constructor creates a factory for a plugin.
     */
    explicit KPluginFactory();

    /**
     * This destroys the PluginFactory.
     */
    virtual ~KPluginFactory();

    /**
     * Use this method to create an object. It will try to create an object which inherits
     * \p T. If it has multiple choices, you will get a fatal error (kFatal()), so be careful
     * to request a unique interface or use keywords.
     *
     * \tparam T The interface for which an object should be created. The object will inherit \p T.
     * \param parent The parent of the object. If \p parent is a widget type, it will also passed
     *               to the parentWidget argument of the CreateInstanceFunction for the object.
     * \param args Additional arguments which will be passed to the object.
     * \returns A pointer to the created object is returned, or 0 if an error occurred.
     */
    template<typename T>
    T *create(QObject *parent = 0, const QVariantList &args = QVariantList());

    /**
     * Use this method to create an object. It will try to create an object which inherits
     * \p T and was registered with \p keyword.
     *
     * \tparam T The interface for which an object should be created. The object will inherit \p T.
     * \param keyword The keyword of the object.
     * \param parent The parent of the object. If \p parent is a widget type, it will also passed
     *               to the parentWidget argument of the CreateInstanceFunction for the object.
     * \param args Additional arguments which will be passed to the object.
     * \returns A pointer to the created object is returned, or 0 if an error occurred.
     */
    template<typename T>
    T *create(const QString &keyword, QObject *parent = 0, const QVariantList &args = QVariantList());

    /**
     * Use this method to create an object. It will try to create an object which inherits
     * \p T and was registered with \p keyword.
     * This overload has an additional \p parentWidget argument, which is used by some plugins (e.g. Parts).

     * \tparam T The interface for which an object should be created. The object will inherit \p T.
     * \param parentWidget An additional parent widget.
     * \param parent The parent of the object. If \p parent is a widget type, it will also passed
     *               to the parentWidget argument of the CreateInstanceFunction for the object.
     * \param keyword The keyword of the object.
     * \param args Additional arguments which will be passed to the object.
     * \returns A pointer to the created object is returned, or 0 if an error occurred.
     */
    template<typename T>
    T *create(QWidget *parentWidget, QObject *parent, const QString &keyword = QString(), const QVariantList &args = QVariantList());

    /**
     * @deprecated
     */
#ifndef KCOREADDONS_NO_DEPRECATED
    template<typename T>
    KCOREADDONS_DEPRECATED
    T *create(QObject *parent, const QStringList &args)
    {
        return create<T>(parent, stringListToVariantList(args));
    }
#endif

    /**
     * @deprecated
     */
#ifndef KCOREADDONS_NO_DEPRECATED
    KCOREADDONS_DEPRECATED QObject *create(QObject *parent = 0, const char *classname = "QObject", const QStringList &args = QStringList())
    {
        return create(classname, 0, parent, stringListToVariantList(args), QString());
    }
#endif

    /**
     * \internal
     * Converts a QStringList to a QVariantList
     */
    static QVariantList stringListToVariantList(const QStringList &list);

    /**
     * \internal
     * Converts a QVariantList of strings to a QStringList
     */
    static QStringList variantListToStringList(const QVariantList &list);

Q_SIGNALS:
    void objectCreated(QObject *object);

protected:
    /**
     * Function pointer type to a function that instantiates a plugin.
     */
    typedef QObject *(*CreateInstanceFunction)(QWidget *, QObject *, const QVariantList &);

    /**
     * This is used to detect the arguments need for the constructor of plugin classes.
     * You can inherit it, if you want to add new classes and still keep support for the old ones.
     */
    template<class impl>
    struct InheritanceChecker {
        CreateInstanceFunction createInstanceFunction(KParts::Part *)
        {
            return &createPartInstance<impl>;
        }
        CreateInstanceFunction createInstanceFunction(QWidget *)
        {
            return &createInstance<impl, QWidget>;
        }
        CreateInstanceFunction createInstanceFunction(...)
        {
            return &createInstance<impl, QObject>;
        }
    };

    explicit KPluginFactory(KPluginFactoryPrivate &dd);

    /**
     * Registers a plugin with the factory. Call this function from the constructor of the
     * KPluginFactory subclass to make the create function able to instantiate the plugin when asked
     * for an interface the plugin implements.
     *
     * You can register as many plugin classes as you want as long as either the plugin interface or
     * the \p keyword makes it unique. E.g. it is possible to register a KCModule and a
     * KParts::Part without having to specify keywords since their interfaces differ.
     *
     * \tparam T the name of the plugin class
     *
     * \param keyword An optional keyword as unique identifier for the plugin. This allows you to
     * put more than one plugin with the same interface into the same library using the same
     * factory. X-KDE-PluginKeyword is a convenient way to specify the keyword in a desktop file.
     *
     * \param instanceFunction A function pointer to a function that creates an instance of the
     * plugin. The default function that will be used depends on the type of interface. If the
     * interface inherits from
     * \li \c KParts::Part the function will call
     * \code
     * new T(QWidget *parentWidget, QObject *parent, const QVariantList &args)
     * \endcode
     * \li \c QWidget the function will call
     * \code
     * new T(QWidget *parent, const QVariantList &args)
     * \endcode
     * \li else the function will call
     * \code
     * new T(QObject *parent, const QVariantList &args)
     * \endcode
     */
    template<class T>
    void registerPlugin(const QString &keyword = QString(), CreateInstanceFunction instanceFunction
                        = InheritanceChecker<T>().createInstanceFunction(reinterpret_cast<T *>(0)))
    {
        registerPlugin(keyword, &T::staticMetaObject, instanceFunction);
    }

    KPluginFactoryPrivate *const d_ptr;

    /**
     * @deprecated
     */
#ifndef KCOREADDONS_NO_DEPRECATED
    virtual KCOREADDONS_DEPRECATED QObject *createObject(QObject *parent, const char *className, const QStringList &args);
#endif

    /**
     * @deprecated
     */
#ifndef KCOREADDONS_NO_DEPRECATED
    virtual KCOREADDONS_DEPRECATED KParts::Part *createPartObject(QWidget *parentWidget, QObject *parent, const char *classname, const QStringList &args);
#endif

    /**
     * This function is called when the factory asked to create an Object.
     *
     * You may reimplement it to provide a very flexible factory. This is especially useful to
     * provide generic factories for plugins implemeted using a scripting language.
     *
     * \param iface The staticMetaObject::className() string identifying the plugin interface that
     * was requested. E.g. for KCModule plugins this string will be "KCModule".
     * \param parentWidget Only used if the requested plugin is a KPart.
     * \param parent The parent object for the plugin object.
     * \param args A plugin specific list of arbitrary arguments.
     * \param keyword A string that uniquely identifies the plugin. If a KService is used this
     * keyword is read from the X-KDE-PluginKeyword entry in the .desktop file.
     */
    virtual QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword);

    template<class impl, class ParentType>
    static QObject *createInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        Q_UNUSED(parentWidget);
        ParentType *p = 0;
        if (parent) {
            p = qobject_cast<ParentType *>(parent);
            Q_ASSERT(p);
        }
        return new impl(p, args);
    }

    template<class impl>
    static QObject *createPartInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        return new impl(parentWidget, parent, args);
    }

private:
    void registerPlugin(const QString &keyword, const QMetaObject *metaObject, CreateInstanceFunction instanceFunction);
};

typedef KPluginFactory KLibFactory;

template<typename T>
inline T *KPluginFactory::create(QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent) : 0, parent, args, QString());

    T *t = qobject_cast<T *>(o);
    if (!t) {
        delete o;
    }
    return t;
}

template<typename T>
inline T *KPluginFactory::create(const QString &keyword, QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent) : 0, parent, args, keyword);

    T *t = qobject_cast<T *>(o);
    if (!t) {
        delete o;
    }
    return t;
}

template<typename T>
inline T *KPluginFactory::create(QWidget *parentWidget, QObject *parent, const QString &keyword, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parentWidget, parent, args, keyword);

    T *t = qobject_cast<T *>(o);
    if (!t) {
        delete o;
    }
    return t;
}

Q_DECLARE_INTERFACE(KPluginFactory, KPluginFactory_iid)

#endif // KPLUGINFACTORY_H
