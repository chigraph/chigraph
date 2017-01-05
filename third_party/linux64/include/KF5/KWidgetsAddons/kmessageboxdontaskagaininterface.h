/*  This file is part of the KDE libraries
 *  Copyright 2012 David Faure <faure+bluesystems@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KMESSAGEBOXDONTASKAGAININTERFACE_H
#define KMESSAGEBOXDONTASKAGAININTERFACE_H

#include "kmessagebox.h"
#include <qmetatype.h>

/**
 * @internal
 * Used internally by KMessageBox, implemented by plugin, for dynamic dependency on KConfig.
 */
class KMessageBoxDontAskAgainInterface
{
public:
    KMessageBoxDontAskAgainInterface() {}
    virtual ~KMessageBoxDontAskAgainInterface() {}

    virtual bool shouldBeShownYesNo(const QString &dontShowAgainName, KMessageBox::ButtonCode &result) = 0;
    virtual bool shouldBeShownContinue(const QString &dontShowAgainName) = 0;
    virtual void saveDontShowAgainYesNo(const QString &dontShowAgainName, KMessageBox::ButtonCode result) = 0;
    virtual void saveDontShowAgainContinue(const QString &dontShowAgainName) = 0;
    virtual void enableAllMessages() = 0;
    virtual void enableMessage(const QString &dontShowAgainName) = 0;
    virtual void setConfig(KConfig *) = 0;
};

Q_DECLARE_METATYPE(KMessageBoxDontAskAgainInterface *)

#define KMESSAGEBOXDONTASKAGAIN_PROPERTY "KMessageBoxDontAskAgain"

#endif
