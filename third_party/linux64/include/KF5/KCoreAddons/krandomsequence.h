/* This file is part of the KDE libraries
   Copyright (c) 1999 Sean Harmer <sh@astro.keele.ac.uk>

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
*/
#ifndef K_RANDOM_SEQUENCE_H
#define K_RANDOM_SEQUENCE_H

#include <kcoreaddons_export.h>
#include <QtCore/QList>

/**
 * \class KRandomSequence krandomsequence.h <KRandomSequence>
 *
 * A class to create a pseudo-random sequence
 *
 * Given a seed number, this class will produce a sequence of
 * pseudo-random numbers.  This would typically be used in
 * applications like games.
 *
 * In general, you should instantiate a KRandomSequence object and
 * pass along your seed number in the constructor.  From then on,
 * simply call getDouble or getLong to obtain the next
 * number in the sequence.
 *
 * @author Sean Harmer <sh@astro.keele.ac.uk>
 */
class KCOREADDONS_EXPORT KRandomSequence
{
public:
    /**
     * Creates a pseudo-random sequence based on the seed lngSeed.
     *
     * A Pseudo-random sequence is different for each seed but can be
     * reproduced by starting the sequence with the same seed.
     *
     * If you need a single value which needs to be unpredictable,
     * you need to use KRandom::random() instead.
     *
     * @param intSeed Seed to initialize the sequence with.
     * If lngSeed is 0, the sequence is initialized with a value from
     * KRandom::random().
     *
     * Do not use methods working with long type because on 64-bit
     * their size is different.
     */
    explicit KRandomSequence(int intSeed = 0);
    explicit KRandomSequence(long lngSeed);

    /**
     * Standard destructor
     */
    virtual ~KRandomSequence();

    /**
     * Copy constructor
     */
    KRandomSequence(const KRandomSequence &a);

    /**
     * Assignment
     */
    KRandomSequence &operator=(const KRandomSequence &a);

    /**
     * Restart the sequence based on lngSeed.
     * @param intSeed Seed to initialize the sequence with.
     * If lngSeed is 0, the sequence is initialized with a value from
     * KRandom::random().
     */
    void setSeed(int intSeed = 0);
    void setSeed(long lngSeed = 0);

    /**
     * Get the next number from the pseudo-random sequence.
     *
     * @return a pseudo-random double value between [0,1)
     */
    double getDouble();

    /**
     * Get the next number from the pseudo-random sequence.
     *
     * @return a pseudo-random integer value between [0, max)
     * with 0 <= max < 1.000.000
     */
    unsigned int getInt(unsigned int max);
    unsigned long getLong(unsigned long max);

    /**
     * Get a boolean from the pseudo-random sequence.
     *
     * @return a boolean which is either true or false
     */
    bool getBool();

    /**
     * Put a list in random order.
     *
     * Since kdelibs 4.11, this function uses a more efficient
     * algorithm (Fisher-Yates). Therefore, the order of the items in
     * the randomized list is different from the one in earlier
     * versions if the same seed value is used for the random
     * sequence.
     *
     * @param list the list whose order will be modified
     * @note modifies the list in place
     */
    template<typename T> void randomize(QList<T> &list)
    {
        // Fisher-Yates algorithm
        for (int index = list.count() - 1; index > 0; --index) {
            const int swapIndex = getInt(index + 1);
            qSwap(list[index], list[swapIndex]);
        }
    }

    /**
     * Modulate the random sequence.
     *
     * If S(i) is the sequence of numbers that will follow
     * given the current state after calling modulate(i),
     * then S(i) != S(j) for i != j and
     *      S(i) == S(j) for i == j.
     *
     * This can be useful in game situation where "undo" restores
     * the state of the random sequence. If the game modulates the
     * random sequence with the move chosen by the player, the
     * random sequence will be identical whenever the player "redo"-s
     * his or hers original move, but different when the player
     * chooses another move.
     *
     * With this scenario "undo" can no longer be used to repeat a
     * certain move over and over again until the computer reacts
     * with a favorable response or to predict the response for a
     * certain move based on the response to another move.
     * @param i the sequence identified
     */
    void modulate(int i);

private:
    class Private;
    Private *const d;
};

#endif
