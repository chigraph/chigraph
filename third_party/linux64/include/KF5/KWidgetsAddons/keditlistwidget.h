/* This file is part of the KDE libraries
    Copyright (C) 2000 David Faure <faure@kde.org>, Alexander Neundorf <neundorf@kde.org>
              (C) 2010 Sebastian Trueg <trueg@kde.org>

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

#ifndef KEDITLISTWIDGET_H
#define KEDITLISTWIDGET_H

#include <kwidgetsaddons_export.h>

#include <QWidget>

class QLineEdit;
class QComboBox;
class QListView;
class QPushButton;
class QItemSelection;

class KEditListWidgetPrivate;
/**
 * An editable listbox
 *
 * This class provides an editable listbox, this means
 * a listbox which is accompanied by a line edit to enter new
 * items into the listbox and pushbuttons to add and remove
 * items from the listbox and two buttons to move items up and down.
 *
 * \image html keditlistbox.png "KDE Edit List Box Widget"
 *
 * @since 4.6
 */
class KWIDGETSADDONS_EXPORT KEditListWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Buttons buttons READ buttons WRITE setButtons)
    Q_PROPERTY(QStringList items READ items WRITE setItems NOTIFY changed USER true)
    Q_PROPERTY(bool checkAtEntering READ checkAtEntering WRITE setCheckAtEntering)

public:
    class CustomEditorPrivate;

    /**
     * Custom editor class
     **/
    class KWIDGETSADDONS_EXPORT CustomEditor
    {
    public:
        CustomEditor();
        CustomEditor(QWidget *repWidget, QLineEdit *edit);
        CustomEditor(QComboBox *combo);
        virtual ~CustomEditor();

        void setRepresentationWidget(QWidget *repWidget);
        void setLineEdit(QLineEdit *edit);

        virtual QWidget *representationWidget() const;
        virtual QLineEdit *lineEdit() const;

    private:
        friend class CustomEditorPrivate;
        CustomEditorPrivate *const d;

        Q_DISABLE_COPY(CustomEditor)
    };

public:

    /**
     * Enumeration of the buttons, the listbox offers. Specify them in the
     * constructor in the buttons parameter, or in setButtons.
     */
    enum Button {
        Add = 0x0001,
        Remove = 0x0002,
        UpDown = 0x0004,
        All = Add | Remove | UpDown
    };

    Q_DECLARE_FLAGS(Buttons, Button)
    Q_FLAG(Buttons)

    /**
     * Create an editable listbox.
     */
    explicit KEditListWidget(QWidget *parent = 0);

    /**
     * Constructor which allows to use a custom editing widget
     * instead of the standard QLineEdit widget. E.g. you can use a
     * KUrlRequester or a QComboBox as input widget. The custom
     * editor must consist of a lineedit and optionally another widget that
     * is used as representation. A QComboBox or a KUrlRequester have a
     * QLineEdit as child-widget for example, so the QComboBox is used as
     * the representation widget.
     *
     * @see KUrlRequester::customEditor(), setCustomEditor
     */
    KEditListWidget(const CustomEditor &customEditor,
                    QWidget *parent = 0,
                    bool checkAtEntering = false,
                    Buttons buttons = All);

    virtual ~KEditListWidget();

    /**
     * @returns a pointer to the embedded QListView.
     */
    QListView *listView() const;
    /**
     * @returns a pointer to the embedded QLineEdit.
     */
    QLineEdit *lineEdit() const;
    /**
     * @returns a pointer to the Add button
     */
    QPushButton *addButton() const;
    /**
     * @returns a pointer to the Remove button
     */
    QPushButton *removeButton() const;
    /**
     * @returns a pointer to the Up button
     */
    QPushButton *upButton() const;
    /**
     * @returns a pointer to the Down button
     */
    QPushButton *downButton() const;

    /**
     * @returns the count of elements in the list
     */
    int count() const;

    /**
     * Inserts a @p list of elements from the @p index element
     * If @p index is negative, the elements will be appended
     */
    void insertStringList(const QStringList &list, int index = -1);

    /**
     * Inserts a @p text element at the @p index position
     * If @p index is negative, the element will be appended
     */
    void insertItem(const QString &text, int index = -1);

    /**
     * Clears both the listbox and the line edit.
     */
    void clear();

    /**
     * @returns the text at the @p index position
     */
    QString text(int index) const;

    /**
     * @returns the currently selected item
     */
    int currentItem() const;

    /**
     * @returns the currently selected item's text
     */
    QString currentText() const;

    /**
     * @returns a list with the text of all items in the listbox
     */
    QStringList items() const;

    /**
     * Clears the listbox and sets the contents to @p items
     */
    void setItems(const QStringList &items);

    /**
     * @returns which buttons are visible
     */
    Buttons buttons() const;

    /**
     * Specifies which @p buttons are visible
     */
    void setButtons(Buttons buttons);

    /**
     * If @p check is true, after every character you type
     * in the line edit KEditListWidget will enable or disable
     * the Add-button, depending whether the current content of the
     * line edit is already in the listbox. Maybe this can become a
     * performance hit with large lists on slow machines.
     * If @p check is false,
     * it will be checked if you press the Add-button. It is not
     * possible to enter items twice into the listbox.
     * Default is false.
     */
    void setCheckAtEntering(bool check);

    /**
     * @returns true if check at entering is enabled.
     */
    bool checkAtEntering();

    /**
     * Allows to use a custom editing widget
     * instead of the standard QLineEdit widget. E.g. you can use a
     * KUrlRequester or a QComboBox as input widget. The custom
     * editor must consist of a lineedit and optionally another widget that
     * is used as representation. A QComboBox or a KUrlRequester have a
     * QLineEdit as child-widget for example, so the QComboBox is used as
     * the representation widget.
     */
    void setCustomEditor(const CustomEditor &editor);

    /**
     * Reimplented for interal reasons. The API is not affected.
     */
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void changed();

    /**
     * This signal is emitted when the user adds a new string to the list,
     * the parameter is the added string.
     */
    void added(const QString &text);

    /**
     * This signal is emitted when the user removes a string from the list,
     * the parameter is the removed string.
     */
    void removed(const QString &text);

private Q_SLOTS:
    void moveItemUp();
    void moveItemDown();
    void addItem();
    void removeItem();
    void enableMoveButtons(const QModelIndex &, const QModelIndex &);
    void typedSomething(const QString &text);
    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    friend class KEditListWidgetPrivate;
    KEditListWidgetPrivate *const d;

    Q_DISABLE_COPY(KEditListWidget)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KEditListWidget::Buttons)

#endif
