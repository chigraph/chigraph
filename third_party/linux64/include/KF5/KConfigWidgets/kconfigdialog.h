/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Benjamin C Meyer (ben+kdelibs at meyerhome dot net)
 *  Copyright (C) 2003 Waldo Bastian <bastian@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */
#ifndef KCONFIGDIALOG_H
#define KCONFIGDIALOG_H

#include <kpagedialog.h>

#include "kconfigwidgets_export.h"

class KConfig;
class KCoreConfigSkeleton;
class KConfigDialogManager;

/**
 * \short Standard %KDE configuration dialog class
 *
 * The KConfigDialog class provides an easy and uniform means of displaying
 * a settings dialog using KPageDialog, KConfigDialogManager and a
 * KConfigSkeleton derived settings class.
 *
 * KConfigDialog handles the enabling and disabling of buttons, creation
 * of the dialog, and deletion of the widgets.  Because of
 * KConfigDialogManager, this class also manages: restoring
 * the settings, reseting them to the default values, and saving them. This
 * requires that the names of the widgets corresponding to configuration entries
 * have to have the same name plus an additional "kcfg_" prefix. For example the
 * widget named "kcfg_MyOption" would be associated with the configuration entry
 * "MyOption".
 *
 * Here is an example usage of KConfigDialog:
 *
 * \code
 * void KCoolApp::showSettings(){
 *   if(KConfigDialog::showDialog("settings"))
 *     return;
 *   KConfigDialog *dialog = new KConfigDialog(this, "settings", MySettings::self());
 *   dialog->setFaceType(KPageDialog::List);
 *   dialog->addPage(new General(0, "General"), i18n("General") );
 *   dialog->addPage(new Appearance(0, "Style"), i18n("Appearance") );
 *   connect(dialog, SIGNAL(settingsChanged(const QString&)), mainWidget, SLOT(loadSettings()));
 *   connect(dialog, SIGNAL(settingsChanged(const QString&)), this, SLOT(loadSettings()));
 *   dialog->show();
 * }
 * \endcode
 *
 * Other than the above code, each class that has settings in the dialog should
 * have a loadSettings() type slot to read settings and perform any
 * necessary changes.
 *
 * For dialog appearance options (like buttons, default button, ...) please see
 * @see KPageDialog
 *
 * @see KConfigSkeleton
 * @author Waldo Bastian <bastian@kde.org>
 */
class KCONFIGWIDGETS_EXPORT KConfigDialog : public KPageDialog
{
    Q_OBJECT

Q_SIGNALS:
    /**
     * A widget in the dialog was modified.
     */
    void widgetModified();

    /**
     * One or more of the settings have been permanently changed such as if
     * the user clicked on the Apply or Ok button.
     * @param dialogName the name of the dialog.
     */
    void settingsChanged(const QString &dialogName);

public:
    /**
     * @param parent - The parent of this object.  Even though the class
     * deletes itself the parent should be set so the dialog can be centered
     * with the application on the screen.
     *
     * @param name - The name of this object.  The name is used in determining if
     * there can be more than one dialog at a time.  Use names such as:
     * "Font Settings" or "Color Settings" and not just "Settings" in
     * applications where there is more than one dialog.
     *
     * @param config - Config object containing settings.
     */
    KConfigDialog(QWidget *parent, const QString &name,
                  KCoreConfigSkeleton *config);

    /**
     * Deconstructor, removes name from the list of open dialogs.
     * Deletes private class.
     * @see exists()
     */
    ~KConfigDialog();

    /**
     * Adds page to the dialog and to KConfigDialogManager.  When an
     * application is done adding pages show() should be called to
     * display the dialog.
     * @param page - Pointer to the page that is to be added to the dialog.
     * This object is reparented.
     * @param itemName - Name of the page.
     * @param pixmapName - Name of the icon that should be used, if needed, when
     *        displaying the page. The string may either be the name of a themed
     *        icon (e.g. "document-save"), which the internal icon loader will be
     *        used to retrieve, or an absolute path to the pixmap on disk.
     * @param header - Header text use in the list modes. Ignored in Tabbed
     *        mode. If empty, the itemName text is used when needed.
     * @param manage - Whether KConfigDialogManager should manage the page or not.
     * @returns The KPageWidgetItem associated with the page.
     */
    KPageWidgetItem *addPage(QWidget *page, const QString &itemName,
                             const QString &pixmapName = QString(),
                             const QString &header = QString(),
                             bool manage = true);

    /**
     * Adds page to the dialog that is managed by a custom KConfigDialogManager.
     * This is useful for dialogs that contain settings spread over more than
     * one configuration file and thus have/need more than one KConfigSkeleton.
     * When an application is done adding pages show() should be called to
     * display the dialog.
     * @param page - Pointer to the page that is to be added to the dialog.
     * This object is reparented.
     * @param config - Config object containing corresponding settings.
     * @param itemName - Name of the page.
     * @param pixmapName - Name of the icon that should be used, if needed, when
     *        displaying the page. The string may either be the name of a themed
     *        icon (e.g. "document-save"), which the internal icon loader will be
     *        used to retrieve, or an absolute path to the pixmap on disk.
     * @param header - Header text use in the list modes. Ignored in Tabbed
     *        mode. If empty, the itemName text is used when needed.
     * @returns The KPageWidgetItem associated with the page.
     */
    KPageWidgetItem *addPage(QWidget *page, KCoreConfigSkeleton *config,
                             const QString &itemName,
                             const QString &pixmapName = QString(),
                             const QString &header = QString());

    /**
     * See if a dialog with the name 'name' already exists.
     * @see showDialog()
     * @param name - Dialog name to look for.
     * @return Pointer to widget or NULL if it does not exist.
     */
    static KConfigDialog *exists(const QString &name);

    /**
     * Attempts to show the dialog with the name 'name'.
     * @see exists()
     * @param name - The name of the dialog to show.
     * @return True if the dialog 'name' exists and was shown.
     */
    static bool showDialog(const QString &name);

protected Q_SLOTS:
    /**
     * Update the settings from the dialog.
     * Virtual function for custom additions.
     *
     * Example use: User clicks Ok or Apply button in a configure dialog.
     */
    virtual void updateSettings();

    /**
     * Update the dialog based on the settings.
     * Virtual function for custom additions.
     *
     * Example use: Initialisation of dialog.
     * Example use: User clicks Reset button in a configure dialog.
     */
    virtual void updateWidgets();

    /**
     * Update the dialog based on the default settings.
     * Virtual function for custom additions.
     *
     * Example use: User clicks Defaults button in a configure dialog.
     */
    virtual void updateWidgetsDefault();

    /**
     * Updates the Apply and Default buttons.
     * Connect to this slot if you implement your own hasChanged()
     * or isDefault() methods for widgets not managed by KConfig.
     * @since 4.3
     */
    void updateButtons();

    /**
     * Some setting was changed. Emit the signal with the dialogs name.
     * Connect to this slot if there are widgets not managed by KConfig.
     * @since 4.3
     */
    void settingsChangedSlot();

    /**
     * Sets the help path and topic.
     *
     * The HTML file will be found using the X-DocPath entry in the application's desktop file.
     * It can be either a relative path, or a website URL.
     *
     * @param anchor      This has to be a defined anchor in your
     *                    docbook sources or website. If empty the main index
     *                    is loaded.
     * @param appname     This allows you to specify the .desktop file to get the help path from.
     *                    If empty the QCoreApplication::applicationName() is used.
     */
    void setHelp(const QString &anchor, const QString &appname = QString());


    /**
     * Displays help for this config dialog.
     * @since 5.0
     */
    virtual void showHelp();

protected:

    /**
     * Returns whether the current state of the dialog is
     * different from the current configuration.
     * Virtual function for custom additions.
     */
    virtual bool hasChanged();

    /**
     * Returns whether the current state of the dialog is
     * the same as the default configuration.
     */
    virtual bool isDefault();

    /**
     * @internal
     */
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;

private Q_SLOTS:
    /**
     * Slot which cleans up the KConfigDialogManager of the page.
     * */
    void onPageRemoved(KPageWidgetItem *item);

private:
    class KConfigDialogPrivate;
    friend class KConfigDialogPrivate;

    KConfigDialogPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_updateButtons())
    Q_PRIVATE_SLOT(d, void _k_settingsChangedSlot())

    Q_DISABLE_COPY(KConfigDialog)
};

#endif //KCONFIGDIALOG_H

