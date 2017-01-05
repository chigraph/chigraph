/* This file is part of the KDE libraries
   Copyright (C) 1999,2000 Kurt Granroth <granroth@kde.org>
   Copyright (C) 2001,2002 Ellis Whitehead <ellis@kde.org>

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
#ifndef KSTANDARDACTION_H
#define KSTANDARDACTION_H

#include <QAction>
#include <QList>

#include <kconfigwidgets_export.h>
#include <kstandardshortcut.h>
#include <krecentfilesaction.h>
#include <ktoggleaction.h>
#include <ktogglefullscreenaction.h>

#include <type_traits>

class QObject;
class QStringList;
class QWidget;
class QAction;
class KDualAction;
class KToggleAction;
class KToggleFullScreenAction;

/**
 * Convenience methods to access all standard KDE actions.
 *
 * These actions should be used instead of hardcoding menubar and
 * toolbar items.  Using these actions helps your application easily
 * conform to the KDE UI Style Guide
 * @see http://developer.kde.org/documentation/standards/kde/style/basics/index.html .
 *
 * All of the documentation for QAction holds for KStandardAction
 * also.  When in doubt on how things work, check the QAction
 * documention first.
 * Please note that calling any of these methods automatically adds the action
 * to the actionCollection() of the QObject given by the 'parent' parameter.
 *
 * <b>Simple Example:</b>\n
 *
 * In general, using standard actions should be a drop in replacement
 * for regular actions.  For example, if you previously had:
 *
 * \code
 * QAction *newAct = new QAction(i18n("&New"), QIcon::fromTheme("document-new"),
 *                               KStandardShortcut::shortcut(KStandardShortcut::New), this,
 *                               SLOT(fileNew()), actionCollection());
 * \endcode
 *
 * You could drop that and replace it with:
 *
 * \code
 * QAction *newAct = KStandardAction::openNew(this, SLOT(fileNew()),
 *                                       actionCollection());
 * \endcode
 *
 * <b>Non-standard Usages</b>\n
 *
 * It is possible to use the standard actions in various
 * non-recommended ways.  Say, for instance, you wanted to have a
 * standard action (with the associated correct text and icon and
 * accelerator, etc) but you didn't want it to go in the standard
 * place (this is not recommended, by the way).  One way to do this is
 * to simply not use the XML UI framework and plug it into wherever
 * you want.  If you do want to use the XML UI framework (good!), then
 * it is still possible.
 *
 * Basically, the XML building code matches names in the XML code with
 * the internal names of the actions.  You can find out the internal
 * names of each of the standard actions by using the name
 * method like so: KStandardAction::name(KStandardAction::Cut) would return
 * 'edit_cut'.  The XML building code will match 'edit_cut' to the
 * attribute in the global XML file and place your action there.
 *
 * However, you can change the internal name.  In this example, just
 * do something like:
 *
 * \code
 * (void)KStandardAction::cut(this, SLOT(editCut()), actionCollection(), "my_cut");
 * \endcode
 *
 * Now, in your local XML resource file (e.g., yourappui.rc), simply
 * put 'my_cut' where you want it to go.
 *
 * Another non-standard usage concerns getting a pointer to an
 * existing action if, say, you want to enable or disable the action.
 * You could do it the recommended way and just grab a pointer when
 * you instantiate it as in the 'openNew' example above... or you
 * could do it the hard way:
 *
 * \code
 * QAction *cut = actionCollection()->action(KStandardAction::name(KStandardAction::Cut));
 * \endcode
 *
 * Another non-standard usage concerns instantiating the action in the
 * first place.  Usually, you would use the member functions as
 * shown above (e.g., KStandardAction::cut(this, SLOT, parent)).  You
 * may, however, do this using the enums provided.  This author can't
 * think of a reason why you would want to, but, hey, if you do,
 * here's how:
 *
 * \code
 * (void)KStandardAction::action(KStandardAction::New, this, SLOT(fileNew()), actionCollection());
 * (void)KStandardAction::action(KStandardAction::Cut, this, SLOT(editCut()), actionCollection());
 * \endcode
 *
 * @author Kurt Granroth <granroth@kde.org>
 */
namespace KStandardAction
{
/**
 * The standard menubar and toolbar actions.
 */
enum StandardAction {
    ActionNone,

    // File Menu
    New, Open, OpenRecent, Save, SaveAs, Revert, Close,
    Print, PrintPreview, Mail, Quit,

    // Edit Menu
    Undo, Redo, Cut, Copy, Paste, SelectAll, Deselect, Find, FindNext, FindPrev,
    Replace,

    // View Menu
    ActualSize, FitToPage, FitToWidth, FitToHeight, ZoomIn, ZoomOut,
    Zoom, Redisplay,

    // Go Menu
    Up, Back, Forward, Home /*Home page*/, Prior, Next, Goto, GotoPage, GotoLine,
    FirstPage, LastPage, DocumentBack, DocumentForward,

    // Bookmarks Menu
    AddBookmark, EditBookmarks,

    // Tools Menu
    Spelling,

    // Settings Menu
    ShowMenubar, ShowToolbar, ShowStatusbar,
    SaveOptions, KeyBindings,
    Preferences, ConfigureToolbars,

    // Help Menu
    Help, HelpContents, WhatsThis, ReportBug, AboutApp, AboutKDE,
    TipofDay,

    // Other standard actions
    ConfigureNotifications,
    FullScreen,
    Clear,
    PasteText,
    SwitchApplicationLanguage,
    DeleteFile, //< @since 5.25
    RenameFile, //< @since 5.25
    MoveToTrash, //< @since 5.25
    Donate //< @since 5.26
};

/**
 * Creates an action corresponding to one of the
 * KStandardAction::StandardAction actions, which is connected to the given
 * object and @p slot, and is owned by @p parent.
 *
 * The signal that is connected to @p slot is triggered(bool), except for the case of
 * OpenRecent standard action, which uses the urlSelected(const QUrl &) signal of
 * KRecentFilesAction.
 *
 * @param id The StandardAction identifier to create a QAction for.
 * @param recvr The QObject to receive the signal, or 0 if no notification
 *              is needed.
 * @param slot  The slot to connect the signal to (remember to use the SLOT() macro).
 * @param parent The QObject that should own the created QAction, or 0 if no parent will
 *               own the QAction returned (ensure you delete it manually in this case).
 */
KCONFIGWIDGETS_EXPORT QAction *create(StandardAction id, const QObject *recvr, const char *slot,
                                      QObject *parent);

/**
 * @internal
 */
KCONFIGWIDGETS_EXPORT QAction *_k_createInternal(StandardAction id, QObject *parent);

/**
 * This overloads create() to allow using the new connect syntax
 * @note if you use @c OpenRecent as @p id, you should manually connect to the urlSelected(const QUrl &)
 * signal of the returned KRecentFilesAction instead or use KStandardAction::openRecent(Receiver *, Func).
 * @see create(StandardAction, const QObject *, const char *, QObject *)
 * @since 5.23
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
inline QAction *create(StandardAction id, const QObject *recvr, Func slot, QObject* parent);
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, QAction>::type *create(StandardAction id, const Receiver *recvr, Func slot, QObject *parent)
#endif
{
    QAction *action = _k_createInternal(id, parent);
    Qt::ConnectionType connectionType = Qt::AutoConnection;
    if (id == ConfigureToolbars) { // bug #200815
        connectionType = Qt::QueuedConnection;
    }
    QObject::connect(action, &QAction::triggered, recvr, slot, connectionType);
    return action;
}


/**
 * This will return the internal name of a given standard action.
 */
KCONFIGWIDGETS_EXPORT const char *name(StandardAction id);

/// @deprecated use name()
#ifndef KDE_NO_DEPRECATED
inline KCONFIGWIDGETS_DEPRECATED const char *stdName(StandardAction act_enum)
{
    return name(act_enum);
}
#endif

/**
 * Returns a list of all standard names. Used by KAccelManager
 * to give those heigher weight.
 */
KCONFIGWIDGETS_EXPORT QStringList stdNames();

/**
 * Returns a list of all actionIds.
 *
 * @since 4.2
 */
KCONFIGWIDGETS_EXPORT QList<StandardAction> actionIds();

/**
 * Returns the standardshortcut associated with @a actionId.
 *
 * @param actionId    The actionId whose associated shortcut is wanted.
 *
 * @since 4.2
 */
KCONFIGWIDGETS_EXPORT KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id);


// we have to disable the templated function for const char* as Func, since it is ambiguous otherwise
// TODO: KF6: unify const char* version and new style by removing std::enable_if
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(name, enumValue) \
    inline QAction *name(const QObject *recvr, Func slot, QObject *parent);
#else
#define KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(name, enumValue) \
    template<class Receiver, class Func> \
    inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, QAction>::type *name(const Receiver *recvr, Func slot, QObject *parent) \
    { return create(enumValue, recvr, slot, parent); }
#endif

/**
 * Create a new document or window.
 */
KCONFIGWIDGETS_EXPORT QAction *openNew(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Create a new document or window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(openNew, New)

/**
 * Open an existing file.
 */
KCONFIGWIDGETS_EXPORT QAction *open(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Open an existing file.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(open, Open)

/**
 * Open a recently used document. The signature of the slot being called
 * is of the form slotURLSelected( const QUrl & ).
 * @param recvr object to receive slot
 * @param slot The SLOT to invoke when a URL is selected. The slot's
 * signature is slotURLSelected( const QUrl & ).
 * @param parent parent widget
 */
KCONFIGWIDGETS_EXPORT KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent);

/**
 * The same as openRecent(const QObject *, const char *, QObject *), but using new-style connect syntax
 * @see openRecent(const QObject *, const char *, QObject *)
 * @since 5.23
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
inline KRecentFilesAction *openRecent(const QObject *recvr, Func slot, QObject *parent)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, KRecentFilesAction>::type *openRecent(const Receiver *recvr, Func slot, QObject *parent)
#endif
{
    QAction* action = _k_createInternal(OpenRecent, parent);
    KRecentFilesAction* recentAction = qobject_cast<KRecentFilesAction*>(action);
    Q_ASSERT(recentAction);
    QObject::connect(recentAction, &KRecentFilesAction::urlSelected, recvr, slot);
    return recentAction;
}

/**
 * Save the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *save(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Save the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(save, Save)

/**
 * Save the current document under a different name.
 */
KCONFIGWIDGETS_EXPORT QAction *saveAs(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Save the current document under a different name.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(saveAs, SaveAs)

/**
 * Revert the current document to the last saved version
 * (essentially will undo all changes).
 */
KCONFIGWIDGETS_EXPORT QAction *revert(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Revert the current document to the last saved version
 * (essentially will undo all changes).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(revert, Revert)

/**
 * Close the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *close(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Close the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(close, Close)

/**
 * Print the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *print(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Print the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(print, Print)

/**
 * Show a print preview of the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *printPreview(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Show a print preview of the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(printPreview, PrintPreview)

/**
 * Mail this document.
 */
KCONFIGWIDGETS_EXPORT QAction *mail(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Mail this document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(mail, Mail)

/**
 * Quit the program.
 *
 * Note that you probably want to connect this action to either QWidget::close()
 * or QApplication::closeAllWindows(), but not QApplication::quit(), so that
 * KMainWindow::queryClose() is called on any open window (to warn the user
 * about unsaved changes for example).
 */
KCONFIGWIDGETS_EXPORT QAction *quit(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Quit the program.
 * @see quit(const QObject *recvr, const char *slot, QObject *parent)
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(quit, Quit)

/**
 * Undo the last operation.
 */
KCONFIGWIDGETS_EXPORT QAction *undo(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Undo the last operation.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(undo, Undo)

/**
 * Redo the last operation.
 */
KCONFIGWIDGETS_EXPORT QAction *redo(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Redo the last operation.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(redo, Redo)

/**
 * Cut selected area and store it in the clipboard.
 * Calls cut() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *cut(QObject *parent);

/**
 * Copy selected area and store it in the clipboard.
 * Calls copy() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *copy(QObject *parent);

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * Calls paste() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *paste(QObject *parent);

/**
 * Clear selected area.  Calls clear() on the widget with the current focus.
 * Note that for some widgets, this may not provide the intended behavior.  For
 * example if you make use of the code above and a K3ListView has the focus, clear()
 * will clear all of the items in the list.  If this is not the intened behavior
 * and you want to make use of this slot, you can subclass K3ListView and reimplement
 * this slot.  For example the following code would implement a K3ListView without this
 * behavior:
 *
 * \code
 * class MyListView : public K3ListView {
 *   Q_OBJECT
 * public:
 *   MyListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 ) : K3ListView( parent, name, f ) {}
 *   virtual ~MyListView() {}
 * public Q_SLOTS:
 *   virtual void clear() {}
 * };
 * \endcode
 */
KCONFIGWIDGETS_EXPORT QAction *clear(QObject *parent);

/**
 * Calls selectAll() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *selectAll(QObject *parent);

/**
 * Cut selected area and store it in the clipboard.
 */
KCONFIGWIDGETS_EXPORT QAction *cut(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Cut selected area and store it in the clipboard.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(cut, Cut)

/**
 * Copy the selected area into the clipboard.
 */
KCONFIGWIDGETS_EXPORT QAction *copy(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Copy the selected area into the clipboard.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(copy, Copy)

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * position.
 */
KCONFIGWIDGETS_EXPORT QAction *paste(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * position.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(paste, Paste)

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * position. Provide a button on the toolbar with the clipboard history
 * menu if Klipper is running.
 */
KCONFIGWIDGETS_EXPORT QAction *pasteText(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * position. Provide a button on the toolbar with the clipboard history
 * menu if Klipper is running.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(pasteText, PasteText)

/**
 * Clear the content of the focus widget
 */
KCONFIGWIDGETS_EXPORT QAction *clear(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Clear the content of the focus widget
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(clear, Clear)

/**
 * Select all elements in the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *selectAll(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Select all elements in the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(selectAll, SelectAll)

/**
 * Deselect any selected elements in the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *deselect(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Deselect any selected elements in the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(deselect, Deselect)

/**
 * Initiate a 'find' request in the current document.
 */
KCONFIGWIDGETS_EXPORT QAction *find(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Initiate a 'find' request in the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(find, Find)

/**
 * Find the next instance of a stored 'find'.
 */
KCONFIGWIDGETS_EXPORT QAction *findNext(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Find the next instance of a stored 'find'.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(findNext, FindNext)

/**
 * Find a previous instance of a stored 'find'.
 */
KCONFIGWIDGETS_EXPORT QAction *findPrev(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Find a previous instance of a stored 'find'.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(findPrev, FindPrev)

/**
 * Find and replace matches.
 */
KCONFIGWIDGETS_EXPORT QAction *replace(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Find and replace matches.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(replace, Replace)

/**
 * View the document at its actual size.
 */
KCONFIGWIDGETS_EXPORT QAction *actualSize(const QObject *recvr, const char *slot, QObject *parent);

/**
 * View the document at its actual size.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(actualSize, ActualSize)

/**
 * Fit the document view to the size of the current window.
 */
KCONFIGWIDGETS_EXPORT QAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Fit the document view to the size of the current window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(fitToPage, FitToPage)

/**
 * Fit the document view to the width of the current window.
 */
KCONFIGWIDGETS_EXPORT QAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Fit the document view to the width of the current window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(fitToWidth, FitToWidth)

/**
 * Fit the document view to the height of the current window.
 */
KCONFIGWIDGETS_EXPORT QAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Fit the document view to the height of the current window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(fitToHeight, FitToHeight)

/**
 * Zoom in.
 */
KCONFIGWIDGETS_EXPORT QAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Zoom in.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(zoomIn, ZoomIn)

/**
 * Zoom out.
 */
KCONFIGWIDGETS_EXPORT QAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Zoom out.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(zoomOut, ZoomOut)

/**
 * Popup a zoom dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *zoom(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Popup a zoom dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(zoom, Zoom)

/**
 * Redisplay or redraw the document.
 */
KCONFIGWIDGETS_EXPORT QAction *redisplay(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Redisplay or redraw the document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(redisplay, Redisplay)

/**
 * Move up (web style menu).
 */
KCONFIGWIDGETS_EXPORT QAction *up(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Move up (web style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(up, Up)

/**
 * Move back (web style menu).
 */
KCONFIGWIDGETS_EXPORT QAction *back(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Move back (web style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(back, Back)

/**
 * Move forward (web style menu).
 */
KCONFIGWIDGETS_EXPORT QAction *forward(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Move forward (web style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(forward, Forward)

/**
 * Go to the "Home" position or document.
 */
KCONFIGWIDGETS_EXPORT QAction *home(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Go to the "Home" position or document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(home, Home)

/**
 * Scroll up one page.
 */
KCONFIGWIDGETS_EXPORT QAction *prior(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Scroll up one page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(prior, Prior)

/**
 * Scroll down one page.
 */
KCONFIGWIDGETS_EXPORT QAction *next(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Scroll down one page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(next, Next)

/**
 * Go to somewhere in general.
 */
KCONFIGWIDGETS_EXPORT QAction *goTo(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Go to somewhere in general.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(goTo, Goto)

/**
 * Go to a specific page (dialog).
 */
KCONFIGWIDGETS_EXPORT QAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Go to a specific page (dialog).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(gotoPage, GotoPage)

/**
 * Go to a specific line (dialog).
 */
KCONFIGWIDGETS_EXPORT QAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Go to a specific line (dialog).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(gotoLine, GotoLine)

/**
 * Jump to the first page.
 */
KCONFIGWIDGETS_EXPORT QAction *firstPage(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Jump to the first page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(firstPage, FirstPage)

/**
 * Jump to the last page.
 */
KCONFIGWIDGETS_EXPORT QAction *lastPage(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Jump to the last page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(lastPage, LastPage)

/**
 * Move back (document style menu).
 */
KCONFIGWIDGETS_EXPORT QAction *documentBack(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Move back (document style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(documentBack, DocumentBack)

/**
 * Move forward (document style menu).
 */
KCONFIGWIDGETS_EXPORT QAction *documentForward(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Move forward (document style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(documentForward, DocumentForward)

/**
 * Add the current page to the bookmarks tree.
 */
KCONFIGWIDGETS_EXPORT QAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Add the current page to the bookmarks tree.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(addBookmark, AddBookmark)

/**
 * Edit the application bookmarks.
 */
KCONFIGWIDGETS_EXPORT QAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Edit the application bookmarks.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(editBookmarks, EditBookmarks)

/**
 * Pop up the spell checker.
 */
KCONFIGWIDGETS_EXPORT QAction *spelling(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Pop up the spell checker.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(spelling, Spelling)

/**
 * Show/Hide the menubar.
 */
KCONFIGWIDGETS_EXPORT KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent);

template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, KToggleAction>::type *showMenubar(const Receiver *recvr, Func slot, QObject *parent) {
    QAction* ret = create(ShowMenubar, recvr, slot, parent);
    Q_ASSERT(qobject_cast<KToggleAction *>(ret));
    return static_cast<KToggleAction *>(ret);
}

/**
 * Show/Hide the statusbar.
 */
KCONFIGWIDGETS_EXPORT KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Show/Hide the statusbar.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
inline KToggleAction *showStatusbar(const QObject *recvr, Func slot, QObject *parent)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, KToggleAction>::type *showStatusbar(const Receiver *recvr, Func slot, QObject *parent)
#endif
{
    QAction* ret = create(ShowStatusbar, recvr, slot, parent);
    Q_ASSERT(qobject_cast<KToggleAction *>(ret));
    return static_cast<KToggleAction *>(ret);
}

/**
 * Switch to/from full screen mode
 */
KCONFIGWIDGETS_EXPORT KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent);

/**
 * Switch to/from full screen mode
 * @since 5.23
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
inline KToggleFullScreenAction *fullScreen(const QObject *recvr, Func slot, QWidget *window, QObject *parent);
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, KToggleFullScreenAction>::type *fullScreen(const Receiver *recvr, Func slot, QWidget *window, QObject *parent)
#endif
{
    QAction* a = create(FullScreen, recvr, slot, parent);
    Q_ASSERT(qobject_cast<KToggleFullScreenAction *>(a));
    KToggleFullScreenAction *ret = static_cast<KToggleFullScreenAction *>(a);
    ret->setWindow(window);
    return ret;
}

/**
 * Display the save options dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the save options dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(saveOptions, SaveOptions)

/**
 * Display the configure key bindings dialog.
 *
 *  Note that you might be able to use the pre-built KXMLGUIFactory's function:
 *  KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());
 */
KCONFIGWIDGETS_EXPORT QAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the configure key bindings dialog.
 * @see keyBindings(const QObject *recvr, const char *slot, QObject *parent)
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(keyBindings, KeyBindings)

/**
 * Display the preferences/options dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *preferences(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the preferences/options dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(preferences, Preferences)

/**
 * The Customize Toolbar dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent);

/**
 * The Customize Toolbar dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(configureToolbars, ConfigureToolbars)

/**
 * The Configure Notifications dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent);

/**
 * The Configure Notifications dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(configureNotifications, ConfigureNotifications)

/**
 * Display the help.
 */
KCONFIGWIDGETS_EXPORT QAction *help(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the help.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(help, Help)

/**
 * Display the help contents.
 */
KCONFIGWIDGETS_EXPORT QAction *helpContents(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the help contents.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(helpContents, HelpContents)

/**
 * Trigger the What's This cursor.
 */
KCONFIGWIDGETS_EXPORT QAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Trigger the What's This cursor.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(whatsThis, WhatsThis)

/**
 * Display "Tip of the Day"
 */
KCONFIGWIDGETS_EXPORT QAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display "Tip of the Day"
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(tipOfDay, TipofDay)

/**
 * Open up the Report Bug dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *reportBug(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Open up the Report Bug dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(reportBug, ReportBug)

/**
 * Display the application's About box.
 */
KCONFIGWIDGETS_EXPORT QAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the application's About box.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(aboutApp, AboutApp)

/**
 * Display the About KDE dialog.
 */
KCONFIGWIDGETS_EXPORT QAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Display the About KDE dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(aboutKDE, AboutKDE)

/**
 * Delete file.
 * @since 5.25
 */
KCONFIGWIDGETS_EXPORT QAction *deleteFile(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Delete file.
 * @since 5.25
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(deleteFile, DeleteFile)

/**
 * Rename file.
 * @since 5.25
 */
KCONFIGWIDGETS_EXPORT QAction *renameFile(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Rename file.
 * @since 5.25
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(renameFile, RenameFile)

/**
 * Move to Trash.
 * @since 5.25
 */
KCONFIGWIDGETS_EXPORT QAction *moveToTrash(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Move to Trash.
 * @since 5.25
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(moveToTrash, MoveToTrash)

/**
 * Open donation page on kde.org.
 * @since 5.26
 */
KCONFIGWIDGETS_EXPORT QAction *donate(const QObject *recvr, const char *slot, QObject *parent);

/**
 * Open donation page on kde.org.
 * @since 5.26
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(donate, Donate)

}

#endif // KSTDACTION_H
