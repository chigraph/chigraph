/* This file is part of the KDE libraries
    Copyright (C) 1999 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version
    2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
//-----------------------------------------------------------------------------
// KDE color collection.

#ifndef KDELIBS_KCOLORCOLLECTION_H
#define KDELIBS_KCOLORCOLLECTION_H

#include <kguiaddons_export.h>

#include <QColor>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>

/**
 * Class for handling color collections ("palettes").
 *
 * This class makes it easy to handle color collections, sometimes referred to
 * as "palettes". This class can read and write collections from and to a file.
 *
 * This class uses the "GIMP" palette file format.
 *
 * @author Waldo Bastian (bastian@kde.org)
 **/
class KGUIADDONS_EXPORT KColorCollection
{
public:
    /**
     * Query which KDE color collections are installed.
     *
     * @return A list with installed color collection names.
     */
    static QStringList installedCollections();

    /**
     * KColorCollection constructor. Creates a KColorCollection from a file
     * the filename is derived from the name.
     * @param name The name of collection as returned by installedCollections()
     **/
    explicit KColorCollection(const QString &name = QString());

    /**
     * KColorCollection copy constructor.
     **/
    KColorCollection(const KColorCollection &);

    /**
     * KColorCollection destructor.
     **/
    ~KColorCollection();

    /**
     * KColorCollection assignment operator
     **/
    KColorCollection &operator=(const KColorCollection &);

    /**
     * Save the collection
     *
     * @return 'true' if successful
     **/
    bool save();

    /**
     * Get the description of the collection.
     * @return the description of the collection.
     **/
    QString description() const;

    /**
     * Set the description of the collection.
     * @param desc the new description
     **/
    void setDescription(const QString &desc);

    /**
     * Get the name of the collection.
     * @return the name of the collection
     **/
    QString name() const;

    /**
     * Set the name of the collection.
     * @param name the name of the collection
     **/
    void setName(const QString &name);

    /**
     * Used to specify whether a collection may be edited.
     * @see editable()
     * @see setEditable()
     */
    enum Editable { Yes, ///< Collection may be edited
                    No,  ///< Collection may not be edited
                    Ask  ///< Ask user before editing
                  };

    /**
     * Returns whether the collection may be edited.
     * @return the state of the collection
     **/
    Editable editable() const;

    /**
     * Change whether the collection may be edited.
     * @param editable the state of the collection
     **/
    void setEditable(Editable editable);

    /**
     * Return the number of colors in the collection.
     * @return the number of colors
     **/
    int count() const;

    /**
     * Find color by index.
     * @param index the index of the desired color
     * @return The @p index -th color of the collection, null if not found.
     **/
    QColor color(int index) const;

    /**
     * Find index by @p color.
     * @param color the color to find
     * @return The index of the color in the collection or -1 if the
     * color is not found.
     **/
    int findColor(const QColor &color) const;

    /**
     * Find color name by @p index.
     * @param index the index of the color
     * @return The name of the @p index -th color.
     * Note that not all collections have named the colors. Null is
     * returned if the color does not exist or has no name.
     **/
    QString name(int index) const;

    /**
     * Find color name by @p color.
     * @return The name of color according to this collection.
     * Note that not all collections have named the colors.
     * Note also that each collection can give the same color
     * a different name.
     **/
    QString name(const QColor &color) const;

    /**
     * Add a color.
     * @param newColor The color to add.
     * @param newColorName The name of the color, null to remove
     *                     the name.
     * @return The index of the added color.
     **/
    int addColor(const QColor &newColor,
                 const QString &newColorName = QString());

    /**
     * Change a color.
     * @param index Index of the color to change
     * @param newColor The new color.
     * @param newColorName The new color name, null to remove
     *                     the name.
     * @return The index of the new color or -1 if the color couldn't
     * be changed.
     **/
    int changeColor(int index,
                    const QColor &newColor,
                    const QString &newColorName = QString());

    /**
     * Change a color.
     * @param oldColor The original color
     * @param newColor The new color.
     * @param newColorName The new color name, null to remove
     *                     the name.
     * @return The index of the new color or -1 if the color couldn't
     * be changed.
     **/
    int changeColor(const QColor &oldColor,
                    const QColor &newColor,
                    const QString &newColorName = QString());

private:
    class KColorCollectionPrivate *d;
};

#endif // KDELIBS_KCOLORCOLLECTION_H

