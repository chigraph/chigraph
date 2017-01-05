/* This file is part of the KDE libraries
    Copyright (C) 2001,2002 Ellis Whitehead <ellis@kde.org>
    Copyright (C) 2015 Martin Gräßlin <mgraesslin@kde.org>

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

#ifndef KGLOBALACCEL_INTERFACE_H
#define KGLOBALACCEL_INTERFACE_H

#include <QObject>

#include "kf5globalaccelprivate_export.h"

class GlobalShortcutsRegistry;

/**
 * Abstract interface for plugins to implement
 */
class KF5GLOBALACCELPRIVATE_EXPORT KGlobalAccelInterface : public QObject
{
    Q_OBJECT

public:
    KGlobalAccelInterface(QObject *parent);
    virtual ~KGlobalAccelInterface();

public:
    /**
     * This function registers or unregisters a certain key for global capture,
     * depending on \b grab.
     *
     * Before destruction, every grabbed key will be released, so this
     * object does not need to do any tracking.
     *
     * \param key the Qt keycode to grab or release.
     * \param grab true to grab they key, false to release the key.
     *
     * \return true if successful, otherwise false.
     */
    virtual bool grabKey(int key, bool grab) = 0;

    /*
     * Enable/disable all shortcuts. There will not be any grabbed shortcuts at this point.
     */
    virtual void setEnabled(bool) = 0;

    /**
     * Allows implementing plugins to synchronize with the windowing system.
     * Default implementation does nothing.
     **/
    virtual void syncWindowingSystem();

    void setRegistry(GlobalShortcutsRegistry *registry);

protected:
    /**
     * called by the implementation to inform us about key presses
     * @returns @c true if the key was handled
     **/
    bool keyPressed(int keyQt);
    void grabKeys();
    void ungrabKeys();

private:
    class Private;
    QScopedPointer<Private> d;
};

Q_DECLARE_INTERFACE(KGlobalAccelInterface, "org.kde.kglobalaccel5.KGlobalAccelInterface")

#endif
