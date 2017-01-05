/*
 * kregexpeditorinterface.h - KDE RegExp Editor Interface
 *
 * Copyright (c) 2002 Jesper K. Pedersen <blackie@kdab.net>
 * Copyright (c) 2002 Simon Hausmann <hausmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __kregexpeditorinterface_h__
#define __kregexpeditorinterface_h__

#include <QtCore/QString>
#include <QtCore/QObject>

/**
 *  A graphical editor for regular expressions.
 *
 * @author Jesper K. Pedersen blackie@kde.org
 *
 * The actual editor is located in kdeutils, with an interface in
 * kdelibs. This means that it is a bit more comlicated to create an
 * instance of the editor, but only a little bit more complicated.
 *
 * To check if kregexpeditor in kdeutils is installed and available use this line:
 *
 * \code
 * bool installed=!KTrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty();
 * \endcode
 *
 * The following is a template for what you need to do to create an instance of the
 * regular expression dialog:
 *
 * \code
 * QDialog *editorDialog = KPluginTrader::createInstanceFromQuery<QDialog>(QLatin1String("kregexpeditor"),
                                                                           QLatin1String("KRegExpEditor/KRegExpEditor"));
 * if ( editorDialog ) {
 *   // kdeutils was installed, so the dialog was found fetch the editor interface
 *   KRegExpEditorInterface *editor = static_cast<KRegExpEditorInterface *>( editorDialog->qt_cast( "KRegExpEditorInterface" ) );
 *   Q_ASSERT( editor ); // This should not fail!
 *
 *   // now use the editor.
 *   editor->setRegExp("^kde$");
 *
 *   // Finally exec the dialog
 *   editorDialog->exec();
 * }
 * else {
 *   // Don't offer the dialog.
 * }
 * \endcode
 *
 * Note: signals and slots must be connected to the editorDialog object, not to the editor object:
 * \code
 * connect( editorDialog, SIGNAL( canUndo( bool ) ), undoBut, SLOT( setEnabled( bool ) ) );
 * \endcode
 *
 * If you want to create an instance of the editor widget, i.e. not the
 * dialog, then you must do it in the following way:
 *
 * \code
 * QWidget *editorWidget =
 * KServiceTypeTrader::createInstanceFromQuery<QWidget>(
 *     "KRegExpEditor/KRegExpEditor", QString(), parent );
 * if ( editorWidget ) {
 *   // kdeutils was installed, so the widget was found fetch the editor interface
 *   KRegExpEditorInterface *editor = static_cast<KRegExpEditorInterface *>( editorWidget->qt_cast( "KRegExpEditorInterface" ) );
 *   Q_ASSERT( editor ); // This should not fail!
 *
 *   // now use the editor.
 *   editor->setRegExp("^kde$");

 *   // Finally insert the widget into the layout of its parent
 *   layout->addWidget( editorWidget );
 * }
 * else {
 *   // Don't offer the editor widget.
 * }
 * \endcode
 *
 */
class KRegExpEditorInterface
{
public:
    /**
     * returns the regular expression of the editor in Qt3 QRegExp
     * syntax. Note, there is also a 'regexp' Qt property available.
     */
    virtual QString regExp() const = 0;

    virtual ~KRegExpEditorInterface() {}

protected:
// These are Q_SIGNALS: in classes that actually implement the interface.

    /**
     * This signal tells whether undo is available.
     */
    virtual void canUndo(bool) = 0;

    /**
     * This signal tells whether redo is available.
     */
    virtual void canRedo(bool) = 0;

    /**
     * This signal is emitted whenever the regular expression changes.
     * The argument is true when the regular expression is different from
     * the loaded regular expression and false when it is equal to the
     * loaded regular expression.
     */
    virtual void changes(bool) = 0;

public:
// These are public Q_SLOTS: in classes that implement the interface.

    /**
     * Set the regular expression for the editor. The syntax must be Qt3
     * QRegExp syntax.
     */
    virtual void setRegExp(const QString &regexp) = 0;
    virtual void redo() = 0;
    virtual void undo() = 0;

    /**
     * Set text to use when showing matches. NOT IMPLEMENTED YET!
     *
     * This method is not yet implemented. In later version of the widget
     * this method will be used to give the widget a text to show matches of
     * the regular expression on.
     */
    virtual void setMatchText(const QString &) = 0;

    /**
     * This method allows for future changes that will not break binary
     * compatibility. DO NOT USE!
     *
     * See http://techbase.kde.org/Policies/Binary_Compatibility_Issues_With_C++
     */
    virtual void doSomething(const QString& method, void *arguments) = 0;
};

Q_DECLARE_INTERFACE(KRegExpEditorInterface, "org.kde.KRegExpEditorInterface/1.0")

#endif

