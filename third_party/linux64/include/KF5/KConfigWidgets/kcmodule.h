/*
   This file is part of the KDE libraries

   Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#ifndef KCMODULE_H
#define KCMODULE_H

#include <kconfigwidgets_export.h>

#include <kauthaction.h>

#include <QVariant>
#include <QWidget>

class QStringList;
class KAboutData;
class KConfigDialogManager;
class KCoreConfigSkeleton;
class KConfigSkeleton;
class KCModulePrivate;

/**
 * The base class for configuration modules.
 *
 * Configuration modules are realized as plugins that are loaded only when
 * needed.
 *
 * The module in principle is a simple widget displaying the
 * item to be changed. The module has a very small interface.
 *
 * All the necessary glue logic and the GUI bells and whistles
 * are provided by the control center and must not concern
 * the module author.
 *
 * To write a config module, you have to create a library
 * that contains a factory function like the following:
 *
 * \code
 * #include <KPluginFactory>
 *
 * K_PLUGIN_FACTORY(MyKCModuleFactory, registerPlugin<MyKCModule>() )
 * \endcode
 *
 * The constructor of the KCModule then looks like this:
 * \code
 * YourKCModule::YourKCModule( QWidget* parent )
 *   : KCModule( parent )
 * {
 *   KAboutData *about = new KAboutData(
 *     <kcm name>, i18n( "..." ),
 *     KDE_VERSION_STRING, QString(), KAboutLicense::GPL,
 *     i18n( "Copyright 2006 ..." ) );
 *   about->addAuthor( i18n(...) );
 *   setAboutData( about );
 *   .
 *   .
 *   .
 * }
 * \endcode
 *
 * If you want to make the KCModule available only conditionally (i.e. show in
 * the list of available modules only if some test succeeds) then you can use
 * Hidden in the .desktop file. An example:
 * \code
 * Hidden[$e]=$(if test -e /dev/js*; then echo "false"; else echo "true"; fi)
 * \endcode
 * The example executes the given code in a shell and uses the stdout output for
 * the Hidden value (so it's either Hidden=true or Hidden=false).
 *
 * See http://techbase.kde.org/Development/Tutorials/KCM_HowTo
 * for more detailed documentation.
 *
 * @author Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 */
class KCONFIGWIDGETS_EXPORT KCModule : public QWidget
{
    Q_OBJECT

public:

    /**
     * An enumeration type for the buttons used by this module.
     * You should only use Help, Default and Apply. The rest is obsolete.
     * NoAdditionalButton can be used when we do not want have other button that Ok Cancel
     *
     * @see KCModule::buttons @see KCModule::setButtons
     */
    enum Button { NoAdditionalButton = 0, Help = 1, Default = 2, Apply = 4, Export = 8 };
    Q_DECLARE_FLAGS(Buttons, Button)

    /**
     * Base class for all KControlModules.
     *
     * @note do not emit changed signals here, since they are not yet connected
     *       to any slot.
     * @param aboutData becomes owned by the KCModule
     */
    explicit KCModule(const KAboutData *aboutData, QWidget *parent = 0, const QVariantList &args = QVariantList());

    /**
     * Base class for all KControlModules.
     *
     * @note do not emit changed signals here, since they are not yet connected
     *       to any slot.
     */
    explicit KCModule(QWidget *parent = 0, const QVariantList &args = QVariantList());

    /**
     * Destroys the module.
     */
    ~KCModule();

    /**
     * Return a quick-help text.
     *
     * This method is called when the module is docked.
     * The quick-help text should contain a short description of the module and
     * links to the module's help files. You can use QML formatting tags in the text.
     *
     * @note make sure the quick help text gets translated (use i18n()).
     */
    virtual QString quickHelp() const;

    /**
     * This is generally only called for the KBugReport.
     * If you override you should  have it return a pointer to a constant.
     *
     *
     * @returns the KAboutData for this module
     */
    virtual const KAboutData *aboutData() const;

    /**
     * This sets the KAboutData returned by aboutData()
     * The about data is now owned by KCModule.
     */
    void setAboutData(const KAboutData *about);

    /**
     * Indicate which buttons will be used.
     *
     * The return value is a value or'ed together from
     * the Button enumeration type.
     *
     * @see KCModule::setButtons
     */
    Buttons buttons() const;

    /**
     * Get the RootOnly message for this module.
     *
     * When the module must be run as root, or acts differently
     * for root and a normal user, it is sometimes useful to
     * customize the message that appears at the top of the module
     * when used as a normal user. This function returns this
     * customized message. If none has been set, a default message
     * will be used.
     *
     * @see KCModule::setRootOnlyMessage
     */
    QString rootOnlyMessage() const;

    /**
     * Tell if KControl should show a RootOnly message when run as
     * a normal user.
     *
     * In some cases, the module don't want a RootOnly message to
     * appear (for example if it has already one). This function
     * tells KControl if a RootOnly message should be shown
     *
     * @see KCModule::setUseRootOnlyMessage
     */
    bool useRootOnlyMessage() const;

    KAboutData componentData() const;

    /**
     * @return a list of @ref KConfigDialogManager's in use, if any.
     */
    QList<KConfigDialogManager *> configs() const;

    /**
     * @brief Set if the module's save() method requires authorization to be executed.
     *
     * The module can set this property to @c true if it requires authorization.
     * It will still have to execute the action itself using the KAuth library, so
     * this method is not technically needed to perform the action, but
     * using this and/or the setAuthAction() method will ensure that hosting
     * applications like System Settings or kcmshell behave correctly.
     *
     * Called with @c true, this method will set the action to  "org.kde.kcontrol.name.save" where
     * "name" is aboutData()->appName() return value. This default action won't be set if
     * the aboutData() object is not valid.
     *
     * Note that called with @c false, this method will reset the action name set with setAuthAction().
     *
     * @param needsAuth Tells if the module's save() method requires authorization to be executed.
     */
    void setNeedsAuthorization(bool needsAuth);

    /**
     * Returns the value previously set with setNeedsAuthorization() or setAuthAction(). By default it's @c false.
     *
     * @return @c true if the module's save() method requires authorization, @c false otherwise
     */
    bool needsAuthorization() const;

    /**
     * @brief Set if the module's save() method requires authorization to be executed
     *
     * It will still have to execute the action itself using the KAuth library, so
     * this method is not technically needed to perform the action, but
     * using this method will ensure that hosting
     * applications like System Settings or kcmshell behave correctly.
     *
     * @param action the action that will be used by this KCModule
     */
    void setAuthAction(const KAuth::Action &action);

    /**
     * Returns the action previously set with setAuthAction(). By default its an invalid action.
     *
     * @return The action that has to be authorized to execute the save() method.
     */
    KAuth::Action authAction() const;

    /**
     * Returns the value set by setExportText();
     * @deprecated since 5.0, obsolete feature
     */
    KCONFIGWIDGETS_DEPRECATED QString exportText() const;

    /**
     * Sets the export QString value, used for exporting data.
     * @deprecated since 5.0, obsolete feature
     */
    KCONFIGWIDGETS_DEPRECATED void setExportText(const QString &);

public Q_SLOTS:
    /**
     * Load the configuration data into the module.
     *
     * The load method sets the user interface elements of the
     * module to reflect the current settings stored in the
     * configuration files.
     *
     * This method is invoked whenever the module should read its configuration
     * (most of the times from a config file) and update the user interface.
     * This happens when the user clicks the "Reset" button in the control
     * center, to undo all of his changes and restore the currently valid
     * settings. It is also called right after construction.
     */
    virtual void load();

    /**
     * Save the configuration data.
     *
     * The save method stores the config information as shown
     * in the user interface in the config files.
     *
     * If necessary, this method also updates the running system,
     * e.g. by restarting applications. This normally does not apply for
     * KSettings::Dialog modules where the updating is taken care of by
     * KSettings::Dispatcher.
     *
     * save is called when the user clicks "Apply" or "Ok".
     *
     * If you use KConfigXT, saving is taken care off automatically and
     * you do not need to load manually. However, if you for some reason reimplement it and
     * also are using KConfigXT, you must call this function, otherwise the saving of KConfigXT
     * options will not work. Call it at the very end of your reimplementation, to avoid
     * changed() signals getting emitted when you modify widgets.
     */
    virtual void save();

    /**
     * Sets the configuration to sensible default values.
     *
     * This method is called when the user clicks the "Default"
     * button. It should set the display to useful values.
     *
     * If you use KConfigXT, you do not have to reimplement this function since
     * the fetching and settings of default values is done automatically. However, if you
     * reimplement and also are using KConfigXT, remember to call the base function at the
     * very end of your reimplementation.
     */
    virtual void defaults();

protected:
    /**
     * Adds a KCoreConfigskeleton @p config to watch the widget @p widget
     *
     * This function is useful if you need to handle multiple configuration files.
     *
     * @return a pointer to the KCoreConfigDialogManager in use
     * @param config the KCoreConfigSkeleton to use
     * @param widget the widget to watch
     */
    KConfigDialogManager *addConfig(KCoreConfigSkeleton *config, QWidget *widget);

    /**
     * Adds a KConfigskeleton @p config to watch the widget @p widget
     *
     * This function is useful if you need to handle multiple configuration files.
     *
     * @return a pointer to the KConfigDialogManager in use
     * @param config the KConfigSkeleton to use
     * @param widget the widget to watch
     */
    KConfigDialogManager *addConfig(KConfigSkeleton *config, QWidget *widget);

    /**
     * Sets the quick help.
     */
    void setQuickHelp(const QString &help);

    void showEvent(QShowEvent *ev) Q_DECL_OVERRIDE;

    friend class KCModuleProxy;

Q_SIGNALS:

    /**
     * Indicate that the state of the modules contents has changed.
     *
     * This signal is emitted whenever the state of the configuration
     * shown in the module changes. It allows the module container to
     * keep track of unsaved changes.
     */
    void changed(bool state);

    /**
     * Indicate that the module's quickhelp has changed.
     *
     * Emit this signal whenever the module's quickhelp changes.
     * Modules implemented as tabbed dialogs might want to implement
     * per-tab quickhelp for example.
     *
     */
    void quickHelpChanged();

    /**
     * Indicate that the module's root message has changed.
     *
     * Emits this signal whenever the module's root message changes.
     *
     * @since 4.4
     *
     */
    void rootOnlyMessageChanged(bool use, QString message);

protected Q_SLOTS:

    /**
     * Calling this slot is equivalent to emitting changed(true).
     */
    void changed();

    /**
     * A managed widget was changed, the widget settings and the current
     * settings are compared and a corresponding changed() signal is emitted
     */
    void widgetChanged();

    /**
     * The status of the auth action, if one, has changed
     */
    void authStatusChanged(KAuth::Action::AuthStatus status);

protected:

    /**
     * Sets the buttons to display.
     *
     * Help: shows a "Help" button.
     *
     * Default: shows a "Use Defaults" button.
     *
     * Apply: in kcontrol this will show an "Apply" and "Reset" button,
     *        in kcmshell this will show an "Ok", "Apply" and "Cancel" button.
     *
     * If Apply is not specified, kcmshell will show a "Close" button.
     *
     * @see KCModule::buttons
     */
    void setButtons(Buttons btn);

    /**
     * Sets the RootOnly message.
     *
     * This message will be shown at the top of the module if useRootOnlyMessage is
     * set. If no message is set, a default one will be used.
     *
     * @see KCModule::rootOnlyMessage
     */
    void setRootOnlyMessage(const QString &message);

    /**
     * Change whether or not the RootOnly message should be shown.
     *
     * Following the value of @p on, the RootOnly message will be
     * shown or not.
     *
     * @see KCModule::useRootOnlyMessage
     */
    void setUseRootOnlyMessage(bool on);

    /**
     * Returns the changed state of automatically managed widgets in this dialog
     */
    bool managedWidgetChangeState() const;

    /**
     * Call this method when your manually managed widgets change state between
     * changed and not changed
     */
    void unmanagedWidgetChangeState(bool);

private:
    KCModulePrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KCModule::Buttons)

#endif //KCMODULE_H

