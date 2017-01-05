/*
 *   Copyright (C) 1997  Michael Roth <mroth@wirlweb.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef KSEPARATOR_H
#define KSEPARATOR_H

#include <kwidgetsaddons_export.h>
#include <QFrame>

/**
 * Standard horizontal or vertical separator.
 *
 * \image html kseparator-horizontal.png "KDE Separator with horizontal orientation"
 * \image html kseparator-vertical.png "KDE Separator with vertical orientation"
 *
 * @author Michael Roth <mroth@wirlweb.de>
 */
class KWIDGETSADDONS_EXPORT KSeparator : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

public:
    /**
     * Constructor.
     * @param parent parent object.
     * @param f extra QWidget flags.
     **/
    explicit KSeparator(QWidget *parent = 0, Qt::WindowFlags f = 0);

    /**
     * Constructor.
     * @param orientation Set the orientation of the separator.
     * Possible values are Horizontal or Vertical.
     * @param parent parent object.
     * @param f extra QWidget flags.
     **/
    explicit KSeparator(Qt::Orientation orientation, QWidget *parent = 0, Qt::WindowFlags f = 0);

    /**
     * Returns the orientation of the separator.
     * @return int Possible values Horizontal or Vertical.
     **/
    Qt::Orientation orientation() const;

    /**
     * Set the orientation of the separator to @p orientation
     *
     * @param orientation Possible values are Vertical and Horizontal.
     */
    void setOrientation(Qt::Orientation orientation);

private:
    class KSeparatorPrivate *d;
};

#endif // KSEPARATOR_H
