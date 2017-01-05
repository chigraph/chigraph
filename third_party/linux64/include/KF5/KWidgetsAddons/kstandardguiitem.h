/* This file is part of the KDE libraries
   Copyright (C) 2001 Holger Freyther <freyther@kde.org>

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

#ifndef kstandardguiitem_h
#define kstandardguiitem_h

#include <kwidgetsaddons_export.h>

#include <QtCore/QPair>

#include <kguiitem.h>

class QString;

/**
 * @short Provides a set of standardized KGuiItems.
 *
 * The various KWIDGETSADDONS_EXPORT methods returns standardized @ref KGuiItem's
 * conforming to the KDE UI Standards. Use them instead of creating
 * your own.
 *
 * @author Holger Freyther <freyther@kde.org>
 */
namespace KStandardGuiItem
{
/**
 * The back and forward items by default use the RTL settings for Hebrew
 * and Arab countries. If you want those actions to ignore the RTL value
 * and force 'Western' behavior instead, use the IgnoreRTL value instead.
 */
enum BidiMode { UseRTL = 0, IgnoreRTL };

enum StandardItem {
    Ok = 1, Cancel, Yes, No, Discard, Save, DontSave, SaveAs, Apply, Clear,
    Help, Defaults, Close, Back, Forward, Print, Continue, Open, Quit,
    AdminMode, Reset, Delete, Insert, Configure, Find, Stop, Add, Remove,
    Test, Properties, Overwrite, CloseWindow, CloseDocument
};

/**
 * Returns the gui item for the given identifier @param id.
 *
 * @param id the identifier to search for
 */
KWIDGETSADDONS_EXPORT KGuiItem guiItem(StandardItem id);

/**
 * Returns the name of the gui item for the given identifier @param id.
 *
 * @param id the identifier to search for
 */
KWIDGETSADDONS_EXPORT QString standardItem(StandardItem id);

/**
 * Returns the 'Ok' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem ok();

/**
 * Returns the 'Cancel' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem cancel();

/**
 * Returns the 'Yes' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem yes();

/**
 * Returns the 'No' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem no();

/**
 * Returns the 'Insert' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem insert();

/**
 * Returns the 'Discard' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem discard();

/**
 * Returns the 'Save' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem save();

/**
 * Returns the 'Help' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem help();

/**
 * Returns the 'DontSave' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem dontSave();

/**
 * Returns the 'SaveAs' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem saveAs();

/**
 * Returns the 'Apply' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem apply();

/**
 * Returns the 'Clear' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem clear();

/**
 * Returns the 'Defaults' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem defaults();

/**
 * Returns the 'Close' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem close();

/**
 * Returns the 'CloseWindow' gui item.
 * @since 4.3
 */
KWIDGETSADDONS_EXPORT KGuiItem closeWindow();

/**
 * Returns the 'CloseDocument' gui item.
 * @since 4.3
 */
KWIDGETSADDONS_EXPORT KGuiItem closeDocument();

/**
 * Returns the 'Print' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem print();

/**
 * Returns the 'Properties' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem properties();

/**
 * Returns the 'Reset' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem reset();

/**
 * Returns the 'Overwrite' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem overwrite();

/**
 * Returns a KGuiItem suiting for cases where code or functionality
 * runs under root privileges. Used in conjunction with KConfig Modules.
 */
KWIDGETSADDONS_EXPORT KGuiItem adminMode();

/**
 * Returns the 'Continue' gui item. The short name is due to 'continue' being a
 * reserved word in the C++ language.
 */
KWIDGETSADDONS_EXPORT KGuiItem cont();

/**
 * Returns the 'Delete' gui item. The short name is due to 'delete' being a
 * reserved word in the C++ language.
 */
KWIDGETSADDONS_EXPORT KGuiItem del();

/**
 * Returns the 'Open' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem open();

/**
 * Returns the 'Back' gui item, like Konqueror's back button.
 * This GUI item can optionally honor the user's setting for BiDi, so the
 * icon for right-to-left languages (Hebrew and Arab) has the arrow
 * pointing in the opposite direction.
 * By default the arrow points in the Western 'back' direction (i.e.
 * to the left). This is because usually you only want the Bidi aware
 * GUI item if you also want the 'forward' item. Those two are available
 * in the separate backAndForward() method.
 */
KWIDGETSADDONS_EXPORT KGuiItem back(BidiMode useBidi = IgnoreRTL);

/**
 * Returns the 'Forward' gui item, like Konqueror's forward
 * button. This GUI item can optionally honor the user's setting for BiDi,
 * so the icon for right-to-left languages (Hebrew and Arab) has the arrow
 * pointing in the opposite direction.
 * By default the arrow points in the Western 'forward' direction (i.e.
 * to the right). This is because usually you only want the Bidi aware
 * GUI item if you also want the 'back' item. Those two are available
 * in the separate backAndForward() method.
 */
KWIDGETSADDONS_EXPORT KGuiItem forward(BidiMode useBidi = IgnoreRTL);

/**
 * Returns the 'Configure' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem configure();

/**
 * Return both a back and a forward gui item. This function always returns
 * items that are aware of the Right-to-Left setting for Arab and Hebrew
 * locales. If you have a reason for wanting the 'Western' back/forward
 * buttons, please use the back() and forward() items instead.
 */
KWIDGETSADDONS_EXPORT QPair<KGuiItem, KGuiItem> backAndForward();

/**
 * Returns the 'Quit' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem quit();

/**
 * Returns the 'Find' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem find();

/**
 * Returns the 'Stop' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem stop();

/**
 * Returns the 'Add' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem add();

/**
 * Returns the 'Remove' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem remove();

/**
 * Returns the 'Test' gui item.
 */
KWIDGETSADDONS_EXPORT KGuiItem test();

KWIDGETSADDONS_EXPORT void assign(QPushButton *button, StandardItem item);
}

#endif
