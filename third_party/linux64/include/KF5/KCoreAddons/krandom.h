/* This file is part of the KDE libraries
    Copyright (c) 1999 Matthias Kalle Dalheimer <kalle@kde.org>
    Copyright (c) 2000 Charles Samuels <charles@kde.org>
    Copyright (c) 2005 Joseph Wenninger <kde@jowenn.at>

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

#ifndef KRANDOM_H
#define KRANDOM_H

#include <kcoreaddons_export.h>

#include <QtCore/QString>

/**
 * \headerfile krandom.h <KRandom>
 *
 * @short Helper class to create random data
 *
 * This namespace provides methods which generate random data.
 * KRandom is not recommended for serious random-number generation needs,
 * like cryptography.
 */
namespace KRandom
{
/**
 * Generates a uniform random number.
 * @return A random number in the range [0, RAND_MAX). The RNG is seeded
 *   on first use.
 */
KCOREADDONS_EXPORT int random();

/**
 * Generates a random string.  It operates in the range [A-Za-z0-9]
 * @param length Generate a string of this length.
 * @return the random string
 */
KCOREADDONS_EXPORT QString randomString(int length);
}

#endif

