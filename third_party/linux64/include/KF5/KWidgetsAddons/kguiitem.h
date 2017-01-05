/* This file is part of the KDE libraries
    Copyright (C) 2001 Holger Freyther (freyher@yahoo.com)
                  based on ideas from Martijn and Simon

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

    Many thanks to Simon tronical Hausmann
*/

#ifndef kguiitem_h
#define kguiitem_h

#include <QtCore/QString>

#include <QIcon>

#include <kwidgetsaddons_export.h>

class QPushButton;

/**
 * @short An abstract class for GUI data such as ToolTip and Icon.
 *
 * @author Holger Freyther <freyher@yahoo.com>
 * @see KStandardGuiItem
 */
class KWIDGETSADDONS_EXPORT KGuiItem
{
public:
    KGuiItem();

    // This is explicit because it's easy to get subtle bugs otherwise. The
    // icon name, tooltip and whatsthis text get changed behind your back if
    // you do 'setButtonFoo( "Bar" );' It gives the wrong impression that you
    // just change the text.
    explicit KGuiItem(const QString &text,
                      const QString &iconName  = QString(),
                      const QString &toolTip   = QString(),
                      const QString &whatsThis = QString());

    KGuiItem(const QString &text, const QIcon &icon,
             const QString &toolTip   = QString(),
             const QString &whatsThis = QString());

    KGuiItem(const KGuiItem &rhs);
    KGuiItem &operator=(const KGuiItem &rhs);

    ~KGuiItem();

    QString text() const;
    QString plainText() const;

    QIcon icon() const;

    QString iconName() const;
    QString toolTip() const;
    QString whatsThis() const;
    bool isEnabled() const;
    bool hasIcon() const;

    void setText(const QString &text);
    void setIcon(const QIcon &iconset);
    void setIconName(const QString &iconName);
    void setToolTip(const QString &tooltip);
    void setWhatsThis(const QString &whatsThis);
    void setEnabled(bool enable);

    static void assign(QPushButton *button, const KGuiItem &item);
private:
    class KGuiItemPrivate;
    KGuiItemPrivate *d; //krazy:exclude=dpointer (implicitly shared)
};

#endif

