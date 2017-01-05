/* This file is part of the KDE libraries
   Copyright (C) 1999,2000 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef KCOMPLETIONMATCHES_H
#define KCOMPLETIONMATCHES_H

#include <ksortablelist.h>
#include <kcompletion_export.h>

#include <QStringList>
#include <QScopedPointer>

class KCompletionMatchesWrapper;
class KCompletionMatchesPrivate;

typedef KSortableList<QString> KCompletionMatchesList;

/**
 * This structure is returned by KCompletion::allWeightedMatches().
 * It also keeps the weight of the matches, allowing
 * you to modify some matches or merge them with matches
 * from another call to allWeightedMatches(), and sort the matches
 * after that in order to have the matches ordered correctly.
 *
 * Example (a simplified example of what Konqueror's completion does):
 * \code
 * KCompletionMatches matches = completion->allWeightedMatches(location);
 * if(!location.startsWith("www."))
 matches += completion->allWeightedmatches("www." + location");
 * matches.removeDuplicates();
 * QStringList list = matches.list();
 * \endcode
 *
 * @short List for keeping matches returned from KCompletion
 */
class KCOMPLETION_EXPORT KCompletionMatches : public KCompletionMatchesList
{
public:
    Q_DECLARE_PRIVATE(KCompletionMatches)
    /**
     * Default constructor.
     * @param sort if false, the matches won't be sorted before the conversion,
     *             use only if you're sure the sorting is not needed
     */
    KCompletionMatches(bool sort);

    /**
     * copy constructor.
     */
    KCompletionMatches(const KCompletionMatches &);

    /**
     * assignment operator.
     */
    KCompletionMatches &operator=(const KCompletionMatches &);

    /**
     * @internal
     */
    KCompletionMatches(const KCompletionMatchesWrapper &matches);

    /**
     * default destructor.
     */
    ~KCompletionMatches();
    /**
     * Removes duplicate matches. Needed only when you merged several matches
     * results and there's a possibility of duplicates.
     */
    void removeDuplicates();
    /**
     * Returns the matches as a QStringList.
     * @param sort if false, the matches won't be sorted before the conversion,
     *             use only if you're sure the sorting is not needed
     * @return the list of matches
     */
    QStringList list(bool sort = true) const;
    /**
     * If sorting() returns false, the matches aren't sorted by their weight,
     * even if true is passed to list().
     * @return true if the matches won't be sorted
     */
    bool sorting() const;

private:
    const QScopedPointer<KCompletionMatchesPrivate> d_ptr;
};

#endif // KCOMPLETIONMATCHES_H
