/* This file is part of the KDE libraries
   Copyright (C) 1999,2000 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef KCOMPLETIONBASE_H
#define KCOMPLETIONBASE_H

#include <kcompletion.h>
#include <kcompletion_export.h>

#include <QMap>

class KCompletionBasePrivate;

/**
 * An abstract base class for adding a completion feature
 * into widgets.
 *
 * This is a convenience class that provides the basic functions
 * needed to add text completion support into widgets. All that
 * is required is an implementation for the pure virtual function
 * setCompletedText(). Refer to KLineEdit or KComboBox
 * to see how easily such support can be added using this as a base
 * class.
 *
 * @short An abstract class for adding text completion support to widgets.
 * @author Dawit Alemayehu <adawit@kde.org>
 */

class KCOMPLETION_EXPORT KCompletionBase
{
public:
    Q_DECLARE_PRIVATE(KCompletionBase)
    /**
     * Constants that represent the items whose shortcut
     * key binding is programmable. The default key bindings
     * for these items are defined in KStandardShortcut.
     */
    enum KeyBindingType {
        /**
         * Text completion (by default Ctrl-E).
         */
        TextCompletion,
        /**
         * Switch to previous completion (by default Ctrl-Up).
         */
        PrevCompletionMatch,
        /**
         * Switch to next completion (by default Ctrl-Down).
         */
        NextCompletionMatch,
        /**
         * Substring completion (by default Ctrl-T).
         */
        SubstringCompletion
    };

    // Map for the key binding types mentioned above.
    typedef QMap<KeyBindingType, QList<QKeySequence> > KeyBindingMap;

    /**
     * Default constructor.
     */
    KCompletionBase();

    /**
     * Destructor.
     */
    virtual ~KCompletionBase();

    /**
     * Returns a pointer to the current completion object.
     *
     * If the completion object does not exist, it is automatically created and
     * by default handles all the completion signals internally unless @p handleSignals
     * is set to false. It is also automatically destroyed when the destructor
     * is called. You can change this default behavior using the
     * @ref setAutoDeleteCompletionObject and @ref setHandleSignals member
     * functions.
     *
     * See also @ref compObj.
     *
     * @param handleSignals if true, handles completion signals internally.
     * @return a pointer to the completion object.
     */
    KCompletion *completionObject(bool handleSignals = true);

    /**
     * Sets up the completion object to be used.
     *
     * This method assigns the completion object and sets it up to automatically
     * handle the completion and rotation signals internally. You should use
     * this function if you want to share one completion object among your
     * widgets or need to use a customized completion object.
     *
     * The object assigned through this method is not deleted when this object's
     * destructor is invoked unless you explicitly call @ref setAutoDeleteCompletionObject
     * after calling this method. Be sure to set the bool argument to false, if
     * you want to handle the completion signals yourself.
     *
     * @param completionObject a KCompletion or a derived child object.
     * @param handleCompletionSignals if true, handles completion signals internally.
     */
    virtual void setCompletionObject(KCompletion *completionObject, bool handleSignals = true);

    /**
     * Enables this object to handle completion and rotation
     * events internally.
     *
     * This function simply assigns a boolean value that
     * indicates whether it should handle rotation and
     * completion events or not. Note that this does not
     * stop the object from emitting signals when these
     * events occur.
     *
     * @param handle if true, it handles completion and rotation internally.
     */
    virtual void setHandleSignals(bool handle);

    /**
     * Returns true if the completion object is deleted
     * upon this widget's destruction.
     *
     * See setCompletionObject() and enableCompletion()
     * for details.
     *
     * @return true if the completion object will be deleted
     *              automatically
     */
    bool isCompletionObjectAutoDeleted() const;

    /**
     * Sets the completion object when this widget's destructor
     * is called.
     *
     * If the argument is set to true, the completion object
     * is deleted when this widget's destructor is called.
     *
     * @param autoDelete if true, delete completion object on destruction.
     */
    void setAutoDeleteCompletionObject(bool autoDelete);

    /**
     * Sets the widget's ability to emit text completion and
     * rotation signals.
     *
     * Invoking this function with @p enable set to @p false will
     * cause the completion and rotation signals not to be emitted.
     * However, unlike setting the completion object to @p NULL
     * using setCompletionObject, disabling the emission of
     * the signals through this method does not affect the current
     * completion object.
     *
     * There is no need to invoke this function by default. When a
     * completion object is created through completionObject or
     * setCompletionObject, these signals are set to emit
     * automatically. Also note that disabling this signals will not
     * necessarily interfere with the objects' ability to handle these
     * events internally. See setHandleSignals.
     *
     * @param enable if false, disables the emission of completion and rotation signals.
     */
    void setEnableSignals(bool enable);

    /**
     * Returns true if the object handles the signals.
     *
     * @return true if this signals are handled internally.
     */
    bool handleSignals() const;

    /**
     * Returns true if the object emits the signals.
     *
     * @return true if signals are emitted
     */
    bool emitSignals() const;

    /**
     * Sets whether the object emits rotation signals.
     *
     * @param emitRotationSignals if false, disables the emission of rotation signals.
     */
    void setEmitSignals(bool emitRotationSignals);

    /**
     * Sets the type of completion to be used.
     *
     * @param mode Completion type
     * @see CompletionMode
     */
    virtual void setCompletionMode(KCompletion::CompletionMode mode);

    /**
     * Returns the current completion mode.
     *
     * @return the completion mode.
     */
    KCompletion::CompletionMode completionMode() const;

    /**
     * Sets the key binding to be used for manual text
     * completion, text rotation in a history list as
     * well as a completion list.
     *
     *
     * When the keys set by this function are pressed, a
     * signal defined by the inheriting widget will be activated.
     * If the default value or 0 is specified by the second
     * parameter, then the key binding as defined in the global
     * setting should be used. This method returns false
     * when @p key is negative or the supplied key binding conflicts
     * with another one set for another feature.
     *
     * NOTE: To use a modifier key (Shift, Ctrl, Alt) as part of
     * the key binding simply @p sum up the values of the
     * modifier and the actual key. For example, to use CTRL+E, supply
     * @p "Qt::CtrlButton + Qt::Key_E" as the second argument to this
     * function.
     *
     * @param item the feature whose key binding needs to be set:
     *   @li TextCompletion the manual completion key binding.
     *   @li PrevCompletionMatch the previous match key for multiple completion.
     *   @li NextCompletionMatch the next match key for for multiple completion.
     *   @li SubstringCompletion the key for substring completion
     * @param key key binding used to rotate down in a list.
     * @return true if key binding is successfully set.
     * @see keyBinding
     */
    bool setKeyBinding(KeyBindingType item, const QList<QKeySequence> &key);

    /**
     * Returns the key binding used for the specified item.
     *
     * This method returns the key binding used to activate
     * the feature given by @p item. If the binding
     * contains modifier key(s), the sum of the modifier key
     * and the actual key code is returned.
     *
     * @param item the item to check
     * @return the key binding used for the feature given by @p item.
     * @since 5.0
     * @see setKeyBinding
     */
    QList<QKeySequence> keyBinding(KeyBindingType item) const;

    /**
     * @deprecated since 5.0, use keyBinding instead
     */
#ifndef KCOMPLETION_NO_DEPRECATED
    KCOMPLETION_DEPRECATED QList<QKeySequence> getKeyBinding(KeyBindingType item) const
    {
        return keyBinding(item);
    }
#endif

    /**
     * Sets this object to use global values for key bindings.
     *
     * This method changes the values of the key bindings for
     * rotation and completion features to the default values
     * provided in KGlobalSettings.
     *
     * NOTE: By default, inheriting widgets should use the
     * global key bindings so that there is no need to
     * call this method.
     */
    void useGlobalKeyBindings();

    /**
     * A pure virtual function that must be implemented by
     * all inheriting classes.
     *
     * This function is intended to allow external completion
     * implementations to set completed text appropriately. It
     * is mostly relevant when the completion mode is set to
     * CompletionAuto and CompletionManual modes. See
     * KCompletionBase::setCompletedText.
     * Does nothing in CompletionPopup mode, as all available
     * matches will be shown in the popup.
     *
     * @param text the completed text to be set in the widget.
     */
    virtual void setCompletedText(const QString &text) = 0;

    /**
     * A pure virtual function that must be implemented by
     * all inheriting classes.
     * @param items the list of completed items
     * @param autoSuggest if @c true, the first element of @p items
     *        is automatically completed (i.e. preselected).
     */
    virtual void setCompletedItems(const QStringList &items, bool autoSuggest = true) = 0;

    /**
     * Returns a pointer to the completion object.
     *
     * This method is only different from completionObject()
     * in that it does not create a new KCompletion object even if
     * the internal pointer is @c NULL. Use this method to get the
     * pointer to a completion object when inheriting so that you
     * will not inadvertently create it.
     *
     * @return the completion object or @c NULL if one does not exist.
     */
    KCompletion *compObj() const;

protected:
    /**
     * Returns a key binding map.
     *
     * This method is the same as getKeyBinding(), except that it
     * returns the whole keymap containing the key bindings.
     *
     * @return the key binding used for the feature given by @p item.
     * @since 5.0
     */
    KeyBindingMap keyBindingMap() const;

    /**
     * @deprecated since 5.0, use keyBindingMap instead
     */
#ifndef KCOMPLETION_NO_DEPRECATED
    KCOMPLETION_DEPRECATED KeyBindingMap getKeyBindings() const
    {
        return keyBindingMap();
    }
#endif

    /**
     * Sets the keymap.
     *
     * @param keyBindingMap
     */
    void setKeyBindingMap(KeyBindingMap keyBindingMap);

    /**
     * Sets or removes the delegation object. If a delegation object is
     * set, all function calls will be forwarded to the delegation object.
     * @param delegate the delegation object, or 0 to remove it
     */
    void setDelegate(KCompletionBase *delegate);

    /**
     * Returns the delegation object.
     * @return the delegation object, or 0 if there is none
     * @see setDelegate()
     */
    KCompletionBase *delegate() const;

    /** Virtual hook, used to add new "virtual" functions while maintaining
    binary compatibility. Unused in this class.
    */
    virtual void virtual_hook(int id, void *data);

private:
    Q_DISABLE_COPY(KCompletionBase)
    const QScopedPointer<KCompletionBasePrivate> d_ptr;
};

#endif // KCOMPLETIONBASE_H
