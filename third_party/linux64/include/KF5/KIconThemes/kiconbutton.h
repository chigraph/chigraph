/* vi: ts=8 sts=4 sw=4
 *
 * This file is part of the KDE project, module kfile.
 * Copyright (C) 2000 Geert Jansen <jansen@kde.org>
 *           (C) 2000 Kurt Granroth <granroth@kde.org>
 *           (C) 1997 Christoph Neerfeld <chris@kde.org>
 *           (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>
 *
 * This is free software; it comes under the GNU Library General
 * Public License, version 2. See the file "COPYING.LIB" for the
 * exact licensing terms.
 */

#ifndef KICONBUTTON_H
#define KICONBUTTON_H

#include "kiconthemes_export.h"

#include <QPushButton>

#include <kiconloader.h>

/**
 * A pushbutton for choosing an icon. Pressing on the button will open a
 * KIconDialog for the user to select an icon. The current icon will be
 * displayed on the button.
 *
 * @see KIconDialog
 * @short A push button that allows selection of an icon.
 */
class KICONTHEMES_EXPORT KIconButton: public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QString icon READ icon WRITE setIcon RESET resetIcon NOTIFY iconChanged USER true)
    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(bool strictIconSize READ strictIconSize WRITE setStrictIconSize)

public:
    /**
     * Constructs a KIconButton using the global icon loader.
     *
     * @param parent The parent widget.
     */
    explicit KIconButton(QWidget *parent = 0L);

    /**
     * Constructs a KIconButton using a specific icon loader.
     *
     * @param loader The icon loader to use.
     * @param parent The parent widget.
     */
    KIconButton(KIconLoader *loader, QWidget *parent);

    /**
     * Destructs the button.
     */
    ~KIconButton();

    /**
     * Sets a strict icon size policy for allowed icons. When true,
     * only icons of the specified group's size in setIconType() are allowed,
     * and only icons of that size will be shown in the icon dialog.
     */
    void setStrictIconSize(bool b);
    /**
     * Returns true if a strict icon size policy is set.
     */
    bool strictIconSize() const;

    /**
     * Sets the icon group and context. Use KIconLoader::NoGroup if you want to
     * allow icons for any group in the given context.
     */
    void setIconType(KIconLoader::Group group, KIconLoader::Context context, bool user = false);

    /**
     * Sets the button's initial icon.
     */
    void setIcon(const QString &icon);

    void setIcon(const QIcon &icon);

    /**
     * Resets the icon (reverts to an empty button).
     */
    void resetIcon();

    /**
     * Returns the name of the selected icon.
     */
    const QString &icon() const;

    /**
     * Sets the size of the icon to be shown / selected.
     * @see KIconLoader::StdSizes
     * @see iconSize
     */
    void setIconSize(int size);
    /**
     * Returns the icon size set via setIconSize() or 0, if the default
     * icon size will be used.
     */
    int iconSize() const;

    /**
     * Sets the size of the icon to be shown on the button.
     * @see KIconLoader::StdSizes
     * @see buttonIconSize
     * @since 4.1
     */
    void setButtonIconSize(int size);
    /**
     * Returns the button's icon size.
     * @since 4.1
     */
    int buttonIconSize() const;

Q_SIGNALS:
    /**
     * Emitted when the icon has changed.
     */
    void iconChanged(const QString &icon);

private:
    class KIconButtonPrivate;
    KIconButtonPrivate *const d;

    Q_DISABLE_COPY(KIconButton)

    Q_PRIVATE_SLOT(d, void _k_slotChangeIcon())
    Q_PRIVATE_SLOT(d, void _k_newIconName(const QString &))
};

#endif // KICONBUTTON_H
