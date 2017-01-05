/* This file is part of the KDE libraries
    Copyright (C) 2009-2012 Dario Freddi <drf@kde.org>

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

#ifndef OBJECTDECORATOR_H
#define OBJECTDECORATOR_H

#include <QObject>
#include <kauthaction.h>

#include <kauth_export.h>

namespace KAuth
{

class ObjectDecoratorPrivate;

/**
 * @brief A decorator to add auth features to a button or an action
 *
 * @author Dario Freddi <drf@kde.org>
 */
class KAUTH_EXPORT ObjectDecorator : public QObject
{
    Q_OBJECT
public:

    /**
     * Instantiate a new decorator attached to an object
     *
     * @param parent The parent object this decorator will be attached to
     */
    explicit ObjectDecorator(QObject *parent);

    /**
     * Destructs the decorator
     */
    ~ObjectDecorator();

    /**
     * Returns the action object associated with this decorator, or 0 if it does not have one
     *
     * @returns the KAuth::Action associated with this decorator.
     */
    KAuth::Action authAction() const;

    /**
     * Sets the action object associated with this decorator
     *
     * By setting a KAuth::Action, this decorator will become associated with it, and
     * whenever the action or button it is attached to gets clicked, it will trigger the
     * authorization and execution process for the action.
     * Pass 0 to this function to disassociate the decorator
     *
     * @param action the KAuth::Action to associate with this decorator.
     */
    void setAuthAction(const KAuth::Action &action);

    /**
    * Sets the action object associated with this decorator
    *
    * Overloaded member to allow creating the action by name
    *
    * @param actionName the name of the action to associate
    */
    void setAuthAction(const QString &actionName);

Q_SIGNALS:
    /**
     * Signal emitted when the action is authorized
     *
     * If the decorator needs authorization, whenever the user triggers it,
     * the authorization process automatically begins.
     * If it succeeds, this signal is emitted. The KAuth::Action object is provided for convenience
     * if you have multiple Action objects, but of course it's always the same set with
     * setAuthAction().
     *
     * WARNING: If your button or action needs authorization you should connect eventual slots
     * processing stuff to this signal, and NOT clicked/triggered. Clicked/triggered will be emitted
     * even if the user has not been authorized
     *
     * @param action The object set with setAuthAction()
     */
    void authorized(const KAuth::Action &action);

private:
    friend class ObjectDecoratorPrivate;
    ObjectDecoratorPrivate *const d;

    Q_PRIVATE_SLOT(d, void slotActivated())
    Q_PRIVATE_SLOT(d, void authStatusChanged(KAuth::Action::AuthStatus))
};

} // namespace KAuth

#endif // OBJECTDECORATOR_H
