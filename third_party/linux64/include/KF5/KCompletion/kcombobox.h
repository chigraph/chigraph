/* This file is part of the KDE libraries

   Copyright (c) 2000,2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (c) 2000,2001 Carsten Pfeiffer <pfeiffer@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License (LGPL) as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KCOMBOBOX_H
#define KCOMBOBOX_H

#include <kcompletion.h>

#include <kcompletionbase.h>
#include <kcompletion_export.h>

#include <QComboBox>

class KCompletionBox;
class KComboBoxPrivate;

class QLineEdit;
class QMenu;

/**
 * @short A combo box with completion support.
 *
 * This widget inherits from QComboBox and implements the following
 * additional features:
 *   @li a completion object that provides both automatic
 * and manual text completion as well as text rotation
 *   @li configurable key bindings to activate these features
 *   @li a popup menu item that can be used to allow the user to change
 * the text completion mode on the fly.
 *
 * To support these new features, KComboBox emits a few additional signals
 * such as completion(const QString&) and textRotation(KeyBindingType).
 * The completion signal can be connected to a slot that will assist the user in
 * filling out the remaining text while the rotation signal can be used to traverse
 * through all possible matches whenever text completion results in multiple matches.
 * Additionally, the returnPressed() and returnPressed(const QString&)
 * signals are emitted when the user presses the Enter/Return key.
 *
 * KCombobox by default creates a completion object when you invoke the
 * completionObject(bool) member function for the first time or
 * explicitly use setCompletionObject(KCompletion*, bool) to assign your
 * own completion object. Additionally, to make this widget more functional,
 * KComboBox will by default handle text rotation and completion events
 * internally whenever a completion object is created through either one of the
 * methods mentioned above. If you do not need this functionality, simply use
 * KCompletionBase::setHandleSignals(bool) or alternatively set the boolean
 * parameter in the @p setCompletionObject call to false.
 *
 * Beware: The completion object can be deleted on you, especially if a call
 * such as setEditable(false) is made. Store the pointer at your own risk,
 * and consider using QPointer<KCompletion>.
 *
 * The default key bindings for completion and rotation are determined from the
 * global settings in KStandardShortcut. These values, however, can be overridden
 * locally by invoking KCompletionBase::setKeyBinding(). The values can
 * easily be reverted back to the default settings by calling
 * useGlobalSettings(). An alternate method would be to default individual
 * key bindings by using setKeyBinding() with the default second argument.
 *
 * A non-editable combo box only has one completion mode, @p CompletionAuto.
 * Unlike an editable combo box, the CompletionAuto mode works by matching
 * any typed key with the first letter of entries in the combo box. Please note
 * that if you call setEditable(false) to change an editable combo box to a
 * non-editable one, the text completion object associated with the combo box will
 * no longer exist unless you created the completion object yourself and assigned
 * it to this widget or you called setAutoDeleteCompletionObject(false). In other
 * words do not do the following:
 *
 * \code
 * KComboBox* combo = new KComboBox(true, this);
 * KCompletion* comp = combo->completionObject();
 * combo->setEditable(false);
 * comp->clear(); // CRASH: completion object does not exist anymore.
 * \endcode
 *
 *
 * A read-only KComboBox will have the same background color as a
 * disabled KComboBox, but its foreground color will be the one used for
 * the editable mode. This differs from QComboBox's implementation
 * and is done to give visual distinction between the three different modes:
 * disabled, read-only, and editable.
 *
 * \b Usage \n
 *
 * To enable the basic completion feature:
 *
 * \code
 * KComboBox *combo = new KComboBox(true, this);
 * KCompletion *comp = combo->completionObject();
 * // Connect to the return pressed signal - optional
 * connect(combo,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&)));
 *
 * // Provide the to be completed strings. Note that those are separate from the combo's
 * // contents.
 * comp->insertItems(someQStringList);
 * \endcode
 *
 * To use your own completion object:
 *
 * \code
 * KComboBox *combo = new KComboBox(this);
 * KUrlCompletion *comp = new KUrlCompletion();
 * combo->setCompletionObject(comp);
 * // Connect to the return pressed signal - optional
 * connect(combo,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&)));
 * \endcode
 *
 * Note that you have to either delete the allocated completion object
 * when you don't need it anymore, or call
 * setAutoDeleteCompletionObject(true);
 *
 * Miscellaneous function calls:
 *
 * \code
 * // Tell the widget not to handle completion and rotation
 * combo->setHandleSignals(false);
 * // Set your own completion key for manual completions.
 * combo->setKeyBinding(KCompletionBase::TextCompletion, Qt::End);
 * \endcode
 *
 * \image html kcombobox.png "KDE Combo Boxes, one non-editable, one editable with KUrlCompletion"
 *
 * @author Dawit Alemayehu <adawit@kde.org>
 */
class KCOMPLETION_EXPORT KComboBox : public QComboBox, public KCompletionBase //krazy:exclude=qclasses
{
    Q_OBJECT
    Q_PROPERTY(bool autoCompletion READ autoCompletion WRITE setAutoCompletion)
#ifndef KCOMPLETION_NO_DEPRECATED
    Q_PROPERTY(bool urlDropsEnabled READ urlDropsEnabled WRITE setUrlDropsEnabled)
#endif
    Q_PROPERTY(bool trapReturnKey READ trapReturnKey WRITE setTrapReturnKey)
    Q_DECLARE_PRIVATE(KComboBox)

public:

    /**
    * Constructs a read-only (or rather select-only) combo box.
    *
    * @param parent The parent object of this widget
    */
    explicit KComboBox(QWidget *parent = 0);

    /**
    * Constructs an editable or read-only combo box.
    *
    * @param rw When @p true, widget will be editable.
    * @param parent The parent object of this widget.
    */
    explicit KComboBox(bool rw, QWidget *parent = 0);

    /**
    * Destructor.
    */
    virtual ~KComboBox();

    /**
     * Deprecated to reflect Qt api changes
     * @deprecated since 4.5
     */
#ifndef KCOMPLETION_NO_DEPRECATED
    KCOMPLETION_DEPRECATED void insertURL(const QUrl &url, int index = -1)
    {
        insertUrl(index < 0 ? count() : index, url);
    }
    KCOMPLETION_DEPRECATED void insertURL(const QPixmap &pixmap, const QUrl &url, int index = -1)
    {
        insertUrl(index < 0 ? count() : index, QIcon(pixmap), url);
    }
    KCOMPLETION_DEPRECATED void changeURL(const QUrl &url, int index)
    {
        changeUrl(index, url);
    }
    KCOMPLETION_DEPRECATED void changeURL(const QPixmap &pixmap, const QUrl &url, int index)
    {
        changeUrl(index, QIcon(pixmap), url);
    }
#endif

    /**
     * Sets @p url into the edit field of the combo box.
     *
     * It uses QUrl::toDisplayString() so that the url is properly decoded for
     * displaying.
     */
    void setEditUrl(const QUrl &url);

    /**
     * Appends @p url to the combo box.
     *
     * QUrl::toDisplayString() is used so that the url is properly decoded
     * for displaying.
     */
    void addUrl(const QUrl &url);

    /**
     * Appends @p url with the @p icon to the combo box.
     *
     * QUrl::toDisplayString() is used so that the url is properly decoded
     * for displaying.
     */
    void addUrl(const QIcon &icon,  const QUrl &url);

    /**
     * Inserts @p url at position @p index into the combo box.
     *
     * QUrl::toDisplayString() is used so that the url is properly decoded
     * for displaying.
     */
    void insertUrl(int index, const QUrl &url);

    /**
     * Inserts @p url with the @p icon at position @p index into
     * the combo box.
     *
     * QUrl::toDisplayString() is used so that the url is
     * properly decoded for displaying.
     */
    void insertUrl(int index, const QIcon &icon, const QUrl &url);

    /**
     * Replaces the item at position @p index with @p url.
     *
     * QUrl::toDisplayString() is used so that the url is properly decoded
     * for displaying.
     */
    void changeUrl(int index, const QUrl &url);

    /**
     * Replaces the item at position @p index with @p url and @p icon.
     *
     * QUrl::toDisplayString() is used so that the url is properly decoded
     * for displaying.
     */
    void changeUrl(int index, const QIcon &icon, const QUrl &url);

    /**
    * Returns the current cursor position.
    *
    * This method always returns a -1 if the combo box is @em not
    * editable (read-only).
    *
    * @return Current cursor position.
    */
    int cursorPosition() const;

    /**
    * Reimplemented from QComboBox.
    *
    * If @p true, the completion mode will be set to automatic.
    * Otherwise, it is defaulted to the global setting. This
    * method has been replaced by the more comprehensive
    * setCompletionMode().
    *
    * @param autocomplete Flag to enable/disable automatic completion mode.
    */
    virtual void setAutoCompletion(bool autocomplete);

    /**
    * Reimplemented from QComboBox.
    *
    * Returns @p true if the current completion mode is set
    * to automatic. See its more comprehensive replacement
    * completionMode().
    *
    * @return @p true when completion mode is automatic.
    */
    bool autoCompletion() const;

    /**
    * Enables or disables the popup (context) menu.
    *
    * This method only works if this widget is editable, and
    * allows you to enable/disable the context menu. It does nothing if invoked
    * for a non-editable combo box.
    *
    * By default, the context menu is created if this widget is editable.
    * Call this function with the argument set to false to disable the popup
    * menu.
    *
    * @param showMenu If @p true, show the context menu.
    * @deprecated since 4.5, use setContextMenuPolicy instead
    */
#ifndef KCOMPLETION_NO_DEPRECATED
    virtual KCOMPLETION_DEPRECATED void setContextMenuEnabled(bool showMenu);
#endif

    /**
     * Enables/Disables handling of URL drops.
     *
     * If enabled and the user drops an URL, the decoded URL will
     * be inserted. Otherwise the default behavior of QComboBox is used,
     * which inserts the encoded URL.
     *
     * @param enable If @p true, insert decoded URLs
     * @deprecated since 5.0. Use lineEdit()->installEventFilter with a LineEditUrlDropEventFilter
     */
#ifndef KCOMPLETION_NO_DEPRECATED
    KCOMPLETION_DEPRECATED void setUrlDropsEnabled(bool enable);
#endif

    /**
     * Returns @p true when decoded URL drops are enabled
     */
    bool urlDropsEnabled() const;

    /**
     * Convenience method which iterates over all items and checks if
     * any of them is equal to @p text.
     *
     * If @p text is an empty string, @p false
     * is returned.
     *
     * @return @p true if an item with the string @p text is in the combo box.
     */
    bool contains(const QString &text) const;

    /**
     * By default, KComboBox recognizes Key_Return and Key_Enter
     * and emits the returnPressed() signals, but it also lets the
     * event pass, for example causing a dialog's default button to
     * be called.
     *
     * Call this method with @p trap equal to true to make KComboBox
     * stop these events. The signals will still be emitted of course.
     *
     * Only affects editable combo boxes.
     *
     * @see setTrapReturnKey()
     */
    void setTrapReturnKey(bool trap);

    /**
     * @return @p true if key events of Key_Return or Key_Enter will
     * be stopped; @p false if they will be propagated.
     *
     * @see setTrapReturnKey ()
     */
    bool trapReturnKey() const;

    /**
     * @returns the completion box that is used in completion mode
     * CompletionPopup and CompletionPopupAuto.
     *
     * This method will create a completion box by calling
     * KLineEdit::completionBox, if none is there yet.
     *
     * @param create Set this to false if you don't want the box to be created
     *               i.e. to test if it is available.
     */
    KCompletionBox *completionBox(bool create = true);

    /**
     * Reimplemented for internal reasons. API remains unaffected.
     * Note that QComboBox::setLineEdit is not virtual in Qt4, do not
     * use a KComboBox in a QComboBox pointer.
     *
     * NOTE: Only editable combo boxes can have a line editor. As such
     * any attempt to assign a line edit to a non-editable combo box will
     * simply be ignored.
     */
    virtual void setLineEdit(QLineEdit *);

    /**
     * Reimplemented so that setEditable(true) creates a KLineEdit
     * instead of QLineEdit.
     *
     * Note that QComboBox::setEditable is not virtual, so do not
     * use a KComboBox in a QComboBox pointer.
     */
    void setEditable(bool editable);

Q_SIGNALS:
    /**
    * Emitted when the user presses the Enter key.
    *
    * Note that this signal is only emitted when the widget is editable.
    */
    void returnPressed();

    /**
    * Emitted when the user presses the Enter key.
    *
    * The argument is the current text being edited. This signal is just like
    * returnPressed() except that it contains the current text as its argument.
    *
    * Note that this signal is only emitted when the
    * widget is editable.
    */
    void returnPressed(const QString &);

    /**
    * Emitted when the completion key is pressed.
    *
    * The argument is the current text being edited.
    *
    * Note that this signal is @em not available when the widget is non-editable
    * or the completion mode is set to @p CompletionNone.
    */
    void completion(const QString &);

    /**
     * Emitted when the shortcut for substring completion is pressed.
     */
    void substringCompletion(const QString &);

    /**
     * Emitted when the text rotation key bindings are pressed.
     *
     * The argument indicates which key binding was pressed. In this case this
     * can be either one of four values: @p PrevCompletionMatch,
     * @p NextCompletionMatch, @p RotateUp or @p RotateDown.
     *
     * Note that this signal is @em not emitted if the completion
     * mode is set to CompletionNone.
     *
     * @see KCompletionBase::setKeyBinding() for details
     */
    void textRotation(KCompletionBase::KeyBindingType);

    /**
     * Emitted whenever the completion mode is changed by the user
     * through the context menu.
     */
    void completionModeChanged(KCompletion::CompletionMode);

    /**
     * Emitted before the context menu is displayed.
     *
     * The signal allows you to add your own entries into the context menu.
     * Note that you <em>must not</em> store the pointer to the QPopupMenu since it is
     * created and deleted on demand. Otherwise, you can crash your app.
     *
     * @param contextMenu the context menu about to be displayed
     */
    void aboutToShowContextMenu(QMenu *contextMenu);

public Q_SLOTS:

    /**
    * Iterates through all possible matches of the completed text
    * or the history list.
    *
    * Depending on the value of the argument, this function either
    * iterates through the history list of this widget or all the
    * possible matches in whenever multiple matches result from a
    * text completion request. Note that the all-possible-match
    * iteration will not work if there are no previous matches, i.e.
    * no text has been completed and the *nix shell history list
    * rotation is only available if the insertion policy for this
    * widget is set either @p QComobBox::AtTop or @p QComboBox::AtBottom.
    * For other insertion modes whatever has been typed by the user
    * when the rotation event was initiated will be lost.
    *
    * @param type The key binding invoked.
    */
    void rotateText(KCompletionBase::KeyBindingType type);

    /**
     * Sets the completed text in the line edit appropriately.
     *
     * This function is an implementation for
     * KCompletionBase::setCompletedText.
     */
    void setCompletedText(const QString &) Q_DECL_OVERRIDE;

    /**
     * Sets @p items into the completion box if completionMode() is
     * CompletionPopup. The popup will be shown immediately.
     */
    void setCompletedItems(const QStringList &items, bool autosubject = true) Q_DECL_OVERRIDE;

    /**
     * Selects the first item that matches @p item.
     *
     * If there is no such item, it is inserted at position @p index
     * if @p insert is true. Otherwise, no item is selected.
     */
    void setCurrentItem(const QString &item, bool insert = false, int index = -1);

protected Q_SLOTS:

    /**
    * Completes text according to the completion mode.
    *
    * Note: this method is not invoked if the completion mode is
    * set to @p CompletionNone. Also if the mode is set to @p CompletionShell
    * and multiple matches are found, this method will complete the
    * text to the first match with a beep to indicate that there are
    * more matches. Then any successive completion key event iterates
    * through the remaining matches. This way the rotation functionality
    * is left to iterate through the list as usual.
    */
    virtual void makeCompletion(const QString &);

protected:
    /**
    * This function sets the line edit text and
    * highlights the text appropriately if the boolean
    * value is set to true.
    *
    * @param text The text to be set in the line edit
    * @param marked Whether the text inserted should be highlighted
    */
    virtual void setCompletedText(const QString &text, bool marked);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

private:
    const QScopedPointer<KComboBoxPrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _k_lineEditDeleted())
};

#endif

