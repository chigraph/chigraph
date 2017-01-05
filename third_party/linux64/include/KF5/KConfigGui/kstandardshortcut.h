/* This file is part of the KDE libraries
    Copyright (C) 1997 Stefan Taferner (taferner@kde.org)
    Copyright (C) 2000 Nicolas Hadacek (hadacek@kde.org)
    Copyright (C) 2001,2002 Ellis Whitehead (ellis@kde.org)

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
#ifndef KSTANDARDSHORTCUT_H
#define KSTANDARDSHORTCUT_H

#include <QtCore/QString>
#include <QKeySequence>

#include <kconfiggui_export.h>

/**
 * \namespace KStandardShortcut
 * Convenient methods for access to the common accelerator keys in
 * the key configuration. These are the standard keybindings that should
 * be used in all KDE applications. They will be configurable,
 * so do not hardcode the default behavior.
 */
namespace KStandardShortcut
{
// STUFF WILL BREAK IF YOU DON'T READ THIS!!!
/*
 *Always add new std-accels to the end of this enum, never in the middle!
 *Don't forget to add the corresponding entries in g_infoStandardShortcut[] in kstandardshortcut.cpp, too.
 *Values of elements here and positions of the corresponding entries in
 *the big array g_infoStandardShortcut[] ABSOLUTELY MUST BE THE SAME.
 * !!!    !!!!   !!!!!    !!!!
 *    !!!!    !!!     !!!!    !!!!
 * Remember to also update kdoctools/genshortcutents.cpp.
 *
 * Other Rules:
 *
 * - Never change the name of an existing shortcut
 * - Never translate the name of a shortcut
 */

/**
 * Defines the identifier of all standard accelerators.
 */
enum StandardShortcut {
    //C++ requires that the value of an enum symbol be one more than the previous one.
    //This means that everything will be well-ordered from here on.
    AccelNone = 0,
    // File menu
    Open, New, Close, Save,
    // The Print item
    Print,
    Quit,
    // Edit menu
    Undo, Redo, Cut, Copy, Paste,     PasteSelection,
    SelectAll, Deselect, DeleteWordBack, DeleteWordForward,
    Find, FindNext, FindPrev, Replace,
    // Navigation
    Home, Begin, End, Prior, Next,
    Up, Back, Forward, Reload,
    // Text Navigation
    BeginningOfLine, EndOfLine, GotoLine,
    BackwardWord, ForwardWord,
    // View parameters
    AddBookmark, ZoomIn, ZoomOut, FullScreen,
    ShowMenubar,
    // Tabular navigation
    TabNext, TabPrev,
    // Help menu
    Help, WhatsThis,
    // Text completion
    TextCompletion, PrevCompletion, NextCompletion, SubstringCompletion,

    RotateUp, RotateDown,

    OpenRecent,
    SaveAs,
    Revert,
    PrintPreview,
    Mail,
    Clear,
    ActualSize,
    FitToPage,
    FitToWidth,
    FitToHeight,
    Zoom,
    Goto,
    GotoPage,
    DocumentBack,
    DocumentForward,
    EditBookmarks,
    Spelling,
    ShowToolbar,
    ShowStatusbar,
    SaveOptions,
    KeyBindings,
    Preferences,
    ConfigureToolbars,
    ConfigureNotifications,
    TipofDay,
    ReportBug,
    SwitchApplicationLanguage,
    AboutApp,
    AboutKDE,

    DeleteFile,     // @since 5.25
    RenameFile,     // @since 5.25
    MoveToTrash,    // @since 5.25

    Donate,         // @since 5.26
    // Insert new items here!

    StandardShortcutCount // number of standard shortcuts
};

/**
 * Returns the keybinding for @p accel.
 * On X11, if QApplication was initialized with GUI disabled, the
 * default keybinding will always be returned.
 * @param id the id of the accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &shortcut(StandardShortcut id);

/**
 * Returns a unique name for the given accel.
 * @param id the id of the accelerator
 * @return the unique name of the accelerator
 */
KCONFIGGUI_EXPORT QString name(StandardShortcut id);

/**
 * Returns a localized label for user-visible display.
 * @param id the id of the accelerator
 * @return a localized label for the accelerator
 */
KCONFIGGUI_EXPORT QString label(StandardShortcut id);

/**
 * Returns an extended WhatsThis description for the given accelerator.
 * @param id the id of the accelerator
 * @return a localized description of the accelerator
 */
KCONFIGGUI_EXPORT QString whatsThis(StandardShortcut id);

/**
 * Return the StandardShortcut id of the standard accel action which
 * uses this key sequence, or AccelNone if none of them do.
 * This is used by class KKeyChooser.
 * @param keySeq the key sequence to search
 * @return the id of the standard accelerator, or AccelNone if there
 *          is none
 */
KCONFIGGUI_EXPORT StandardShortcut find(const QKeySequence &keySeq);

/**
 * Return the StandardShortcut id of the standard accel action which
 * has \a keyName as its name, or AccelNone if none of them do.
 * This is used by class KKeyChooser.
 * @param keyName the key sequence to search
 * @return the id of the standard accelerator, or AccelNone if there
 *          is none
 */
KCONFIGGUI_EXPORT StandardShortcut find(const char *keyName);

/**
 * Returns the hardcoded default shortcut for @p id.
 * This does not take into account the user's configuration.
 * @param id the id of the accelerator
 * @return the default shortcut of the accelerator
 */
KCONFIGGUI_EXPORT QList<QKeySequence> hardcodedDefaultShortcut(StandardShortcut id);

/**
 * Saves the new shortcut \a cut for standard accel \a id.
 */
KCONFIGGUI_EXPORT void saveShortcut(StandardShortcut id, const QList<QKeySequence> &newShortcut);

/**
 * Open file. Default: Ctrl-o
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &open();

/**
 * Create a new document (or whatever). Default: Ctrl-n
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &openNew();

/**
 * Close current document. Default: Ctrl-w
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &close();

/**
 * Save current document. Default: Ctrl-s
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &save();

/**
 * Print current document. Default: Ctrl-p
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &print();

/**
 * Quit the program. Default: Ctrl-q
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &quit();

/**
 * Undo last operation. Default: Ctrl-z
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &undo();

/**
 * Redo. Default: Shift-Ctrl-z
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &redo();

/**
 * Cut selected area and store it in the clipboard. Default: Ctrl-x
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &cut();

/**
 * Copy selected area into the clipboard. Default: Ctrl-c
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &copy();

/**
 * Paste contents of clipboard at mouse/cursor position. Default: Ctrl-v
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &paste();

/**
 * Paste the selection at mouse/cursor position. Default: Ctrl-Shift-Insert
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &pasteSelection();

/**
 * Select all. Default: Ctrl-A
 * @return the shortcut of the standard accelerator
 **/
KCONFIGGUI_EXPORT const QList<QKeySequence> &selectAll();

/**
 * Delete a word back from mouse/cursor position. Default: Ctrl-Backspace
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &deleteWordBack();

/**
 * Delete a word forward from mouse/cursor position. Default: Ctrl-Delete
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &deleteWordForward();

/**
 * Find, search. Default: Ctrl-f
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &find();

/**
 * Find/search next. Default: F3
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &findNext();

/**
 * Find/search previous. Default: Shift-F3
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &findPrev();

/**
 * Find and replace matches. Default: Ctrl-r
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &replace();

/**
 * Zoom in. Default: Ctrl-Plus
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &zoomIn();

/**
 * Zoom out. Default: Ctrl-Minus
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &zoomOut();

/**
 * Goto home page. Default: Alt-Home
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &home();

/**
 * Goto beginning of the document. Default: Ctrl-Home
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &begin();

/**
 * Goto end of the document. Default: Ctrl-End
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &end();

/**
 * Goto beginning of current line. Default: Home
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &beginningOfLine();

/**
 * Goto end of current line. Default: End
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &endOfLine();

/**
 * Scroll up one page. Default: Prior
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &prior();

/**
 * Scroll down one page. Default: Next
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &next();

/**
 * Go to line. Default: Ctrl+G
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &gotoLine();

/**
 * Add current page to bookmarks. Default: Ctrl+B
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &addBookmark();

/**
 * Next Tab. Default: Ctrl-<
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &tabNext();

/**
 * Previous Tab. Default: Ctrl->
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &tabPrev();

/**
 * Full Screen Mode. Default: Ctrl+Shift+F
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &fullScreen();

/**
 * Help the user in the current situation. Default: F1
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &help();

/**
 * Complete text in input widgets. Default Ctrl+E
 * @return the shortcut of the standard accelerator
 **/
KCONFIGGUI_EXPORT const QList<QKeySequence> &completion();

/**
 * Iterate through a list when completion returns
 * multiple items. Default: Ctrl+Up
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &prevCompletion();

/**
 * Iterate through a list when completion returns
 * multiple items. Default: Ctrl+Down
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &nextCompletion();

/**
 * Find a string within another string or list of strings.
 * Default: Ctrl-T
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &substringCompletion();

/**
 * Help users iterate through a list of entries. Default: Up
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &rotateUp();

/**
 * Help users iterate through a list of entries. Default: Down
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &rotateDown();

/**
 * What's This button. Default: Shift+F1
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &whatsThis();

/**
 * Reload. Default: F5
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &reload();

/**
 * Up. Default: Alt+Up
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &up();

/**
 * Back. Default: Alt+Left
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &back();

/**
 * Forward. Default: ALT+Right
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &forward();

/**
 * BackwardWord. Default: Ctrl+Left
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &backwardWord();

/**
 * ForwardWord. Default: Ctrl+Right
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &forwardWord();

/**
 * Show Menu Bar.  Default: Ctrl-M
 * @return the shortcut of the standard accelerator
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &showMenubar();

/**
 * Delete File. Default: Shift+Delete
 * @return the shortcut of the standard accelerator
 * @since 5.25
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &deleteFile();

/**
 * Rename File. Default: F2
 * @return the shortcut of the standard accelerator
 * @since 5.25
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &renameFile();

/**
 * Move to Trash. Default: Delete
 * @return the shortcut of the standard accelerator
 * @since 5.25
 */
KCONFIGGUI_EXPORT const QList<QKeySequence> &moveToTrash();

}

#endif // KSTANDARDSHORTCUT_H
