/* This file is part of the KDE project
   Copyright (C) 2002 Anders Lund <anders.lund@lund.tdcadsl.dk>

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

#ifndef _KACTION_SELECTOR_H_
#define _KACTION_SELECTOR_H_

#include <kwidgetsaddons_export.h>
#include <QWidget>

class QListWidget;
class QListWidgetItem;
class QKeyEvent;
class QEvent;
class QIcon;

class KActionSelectorPrivate;

/**
    @short A widget for selecting and arranging actions/objects

    This widget allows the user to select from a set of objects and arrange
    the order of the selected ones using two list boxes labeled "Available"
    and "Used" with horizontal arrows in between to move selected objects between
    the two, and vertical arrows on the right to arrange the order of the selected
    objects.

    The widget moves objects to the other listbox when doubleclicked if
    the property moveOnDoubleClick is set to true (default). See moveOnDoubleClick()
    and setMoveOnDoubleClick().

    The user control the widget using the keyboard if enabled (default),
    see keyboardEnabled.

    Note that this may conflict with keyboard selection in the selected list box,
    if you set that to anything else than QListWidget::Single (which is the default).

    To use it, simply construct an instance and then add items to the two listboxes,
    available through lbAvailable() and lbSelected(). Whenever you want, you can retrieve
    the selected options using QListWidget methods on lbSelected().

    This way, you can use your own QListWidgetItem class, allowing you to easily
    store object data in those.

    When an item is moved to a listbox, it is placed below the current item
    of that listbox.

    Standard arrow icons are used, but you can use icons of your own choice if desired,
    see setButtonIcon(). It is also possible to set tooltips and whatsthis help
    for the buttons. See setButtonTooltip() and setButtonWhatsThis().

    To set whatsthis or tooltips for the listboxes, access them through
    availableListWidget() and selectedListWidget().

    All the moving buttons are automatically set enabled as expected.

    Signals are sent each time an item is moved, allowing you to follow the
    users actions if you need to. See addedToSelection(), removedFromSelection(),
    movedUp() and movedDown()

    \image html kactionselector.png "KDE Action Selector"

    @author Anders Lund <anders@alweb.dk>
*/

class KWIDGETSADDONS_EXPORT KActionSelector : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool moveOnDoubleClick READ moveOnDoubleClick WRITE setMoveOnDoubleClick)
    Q_PROPERTY(bool keyboardEnabled READ keyboardEnabled WRITE setKeyboardEnabled)
    Q_PROPERTY(QString availableLabel READ availableLabel WRITE setAvailableLabel)
    Q_PROPERTY(QString selectedLabel READ selectedLabel WRITE setSelectedLabel)
    Q_PROPERTY(InsertionPolicy availableInsertionPolicy READ availableInsertionPolicy WRITE setAvailableInsertionPolicy)
    Q_PROPERTY(InsertionPolicy selectedInsertionPolicy READ selectedInsertionPolicy WRITE setSelectedInsertionPolicy)
    Q_PROPERTY(bool showUpDownButtons READ showUpDownButtons WRITE setShowUpDownButtons)

public:
    explicit KActionSelector(QWidget *parent = 0);
    ~KActionSelector();

    /**
       @return The QListWidget holding the available actions
    */
    QListWidget *availableListWidget() const;

    /**
       @return The QListWidget holding the selected actions
    */
    QListWidget *selectedListWidget() const;

    /**
      This enum identifies the moving buttons
    */
    enum MoveButton {
        ButtonAdd,
        ButtonRemove,
        ButtonUp,
        ButtonDown
    };
    Q_ENUM(MoveButton)

    /**
      This enum defines policies for where to insert moved items in a listbox.
      The following policies are currently defined:
      @li BelowCurrent - The item is inserted below the listbox'
          currentItem() or at the end if there is no current item.
      @li Sorted - The listbox is sort()ed after one or more items are inserted.
      @li AtTop - The item is inserted at index 0 in the listbox.
      @li AtBottom - The item is inserted at the end of the listbox.

      @sa availableInsertionPolicy(), setAvailableInsertionPolicy(),
      selectedInsertionPolicy(), setSelectedInsertionPolicy().
    */
    enum InsertionPolicy {
        BelowCurrent,
        Sorted,
        AtTop,
        AtBottom
    };
    Q_ENUM(InsertionPolicy)

    /**
      @return Whether moveOnDoubleClcik is enabled.

      If enabled, an item in any listbox will be moved to the other one whenever
      double-clicked.
      This feature is enabled by default.
      @sa setMoveOnDoubleClick()
    */
    bool moveOnDoubleClick() const;

    /**
      Sets moveOnDoubleClick to @p enable
      @sa moveOnDoubleClick()
    */
    void setMoveOnDoubleClick(bool enable);

    /**
      @return Whether keyboard control is enabled.

      When Keyboard control is enabled, the widget will react to
      the following keyboard actions:
      @li CTRL + Right - simulate clicking the add button
      @li CTRL + Left - simulate clicking the remove button
      @li CTRL + Up - simulate clicking the up button
      @li CTRL + Down - simulate clicking the down button

      Additionally, pressing RETURN or ENTER on one of the list boxes
      will cause the current item of that listbox to be moved to the other
      listbox.

      The keyboard actions are enabled by default.

      @sa setKeyboardEnabled()
    */
    bool keyboardEnabled() const;

    /**
      Sets the keyboard enabled depending on @p enable.
      @sa keyboardEnabled()
    */
    void setKeyboardEnabled(bool enable);

    /**
      @return The text of the label for the available items listbox.
    */
    QString availableLabel() const;

    /**
      Sets the label for the available items listbox to @p text.
      Note that this label has the listbox as its @e buddy, so that
      if you have a single ampersand in the text, the following character
      will become the accelerator to focus the listbox.
    */
    void setAvailableLabel(const QString &text);

    /**
      @return the label of the selected items listbox.
    */
    QString selectedLabel() const;

    /**
      Sets the label for the selected items listbox to @p text.
      Note that this label has the listbox as its @e buddy, so that
      if you have a single ampersand in the text, the following character
      will become the accelerator to focus the listbox.
    */
    void setSelectedLabel(const QString &text);

    /**
      @return The current insertion policy for the available listbox.
      The default policy for the available listbox is Sorted.
      See also InsertionPolicy, setAvailableInsertionPolicy().
    */
    InsertionPolicy availableInsertionPolicy() const;

    /**
      Sets the insertion policy for the available listbox.
      See also InsertionPolicy, availableInsertionPolicy().
    */
    void setAvailableInsertionPolicy(InsertionPolicy policy);

    /**
      @return The current insertion policy for the selected listbox.
      The default policy for the selected listbox is BelowCurrent.
      See also InsertionPolicy, setSelectedInsertionPolicy().
    */
    InsertionPolicy selectedInsertionPolicy() const;

    /**
      Sets the insertion policy for the selected listbox.
      See also InsertionPolicy, selectedInsertionPolicy().
    */
    void setSelectedInsertionPolicy(InsertionPolicy policy);

    /**
      @return whether the Up and Down buttons should be displayed.
    */
    bool showUpDownButtons() const;

    /**
      Sets whether the Up and Down buttons should be displayed
      according to @p show
    */
    void setShowUpDownButtons(bool show);

    /**
      Sets the pixmap of the button @p button to @p icon.
      It calls SmallIconSet(pm) to generate the icon set.
    */
    void setButtonIcon(const QString &icon, MoveButton button);

    /**
      Sets the iconset for button @p button to @p iconset.
      You can use this method to set a custom icon set. Either
      created by QIconSet, or use the application instance of
      KIconLoader (recommended).
    */
    void setButtonIconSet(const QIcon &iconset, MoveButton button);

    /**
      Sets the tooltip for the button @p button to @p tip.
    */
    void setButtonTooltip(const QString &tip, MoveButton button);

    /**
      Sets the whatsthis help for button @p button to @p text.
    */
    void setButtonWhatsThis(const QString &text, MoveButton button);

Q_SIGNALS:
    /**
      Emitted when an item is moved to the "selected" listbox.
    */
    void added(QListWidgetItem *item);

    /**
      Emitted when an item is moved out of the "selected" listbox.
    */
    void removed(QListWidgetItem *item);

    /**
      Emitted when an item is moved upwards in the "selected" listbox.
    */
    void movedUp(QListWidgetItem *item);

    /**
      Emitted when an item is moved downwards in the "selected" listbox.
    */
    void movedDown(QListWidgetItem *item);

    /**
      Emitted when an item is moved to the "selected" listbox.
    */
//  void addedToSelection( QListWidgetItem *item );

public Q_SLOTS:
    /**
       Sets the enabled state of all moving buttons to reflect the current
       options.

       Be sure to call this if you add or removes items to either listbox after the
       widget is shown
    */
    void setButtonsEnabled();

protected:
    /**
      Reimplemented for internal reasons.
    */
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

    /**
      Reimplemented for internal reasons.
    */
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

private:
    /**
      Move selected item from available box to the selected box
    */
    Q_PRIVATE_SLOT(d, void buttonAddClicked())

    /**
      Move selected item from selected box to available box
    */
    Q_PRIVATE_SLOT(d, void buttonRemoveClicked())

    /**
      Move selected item in selected box upwards
    */
    Q_PRIVATE_SLOT(d, void buttonUpClicked())

    /**
      Move seleted item in selected box downwards
    */
    Q_PRIVATE_SLOT(d, void buttonDownClicked())

    /**
      Moves the item @p item to the other listbox if moveOnDoubleClick is enabled.
    */
    Q_PRIVATE_SLOT(d, void itemDoubleClicked(QListWidgetItem *item))

    /**
      connected to both list boxes to set the buttons enabled
    */
    Q_PRIVATE_SLOT(d, void slotCurrentChanged(QListWidgetItem *))

private:

    /** @private
      Private data storage
    */
    friend class KActionSelectorPrivate;
    KActionSelectorPrivate *const d;

    Q_DISABLE_COPY(KActionSelector)
};

#endif // _KACTION_SELECTOR_H_
