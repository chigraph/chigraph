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

#ifndef KCOMPLETION_H
#define KCOMPLETION_H

#include <kcompletion_export.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QKeySequence>

class KCompTreeNode;
class KCompletionPrivate;
class KCompletionMatchesWrapper;
class KCompletionMatches;

/**
 * @short A generic class for completing QStrings
 *
 * This class offers easy use of "auto completion", "manual completion" or
 * "shell completion" on QString objects. A common use is completing filenames
 * or URLs (see KUrlCompletion()).
 * But it is not limited to URL-completion -- everything should be completable!
 * The user should be able to complete email addresses, telephone numbers,
 * commands, SQL queries...
 * Every time your program knows what the user can type into an edit field, you
 * should offer completion. With KCompletion, this is very easy, and if you are
 * using a line edit widget (KLineEdit), it is even easier.
 * Basically, you tell a KCompletion object what strings should be completable
 * and, whenever completion should be invoked, you call makeCompletion().
 * KLineEdit and (an editable) KComboBox even do this automatically for you.
 *
 * KCompletion offers the completed string via the signal match() and
 * all matching strings (when the result is ambiguous) via the method
 * allMatches().
 *
 * Notice: auto completion, shell completion and manual completion work
 *         slightly differently:
 *
 * @li auto completion always returns a complete item as match.
 *     When more than one matching item is available, it will deliver just
 *     the first one (depending on sorting order). Iterating over all matches
 *     is possible via nextMatch() and previousMatch().
 *
 * @li popup completion works in the same way, the only difference being that
 *     the completed items are not put into the edit widget, but into a
 *     separate popup box.
 *
 * @li manual completion works the same way as auto completion, except that
 *     it is not invoked automatically while the user is typing,
 *     but only when the user presses a special key. The difference
 *     of manual and auto completion is therefore only visible in UI classes.
 *     KCompletion needs to know whether to deliver partial matches
 *     (shell completion) or whole matches (auto/manual completion), therefore
 *     KCompletion::CompletionMan and KCompletion::CompletionAuto have the exact
 *     same effect in KCompletion.
 *
 * @li shell completion works like "tab completion" in a shell:
 *     when multiple matches are available, the longest possible string of all
 *     matches is returned (i.e. only a partial item).
 *     Iterating over all matching items (complete, not partial) is possible
 *     via nextMatch() and previousMatch().
 *
 * As an application programmer, you do not normally have to worry about
 * the different completion modes; KCompletion handles
 * that for you, according to the setting setCompletionMode().
 * The default setting is globally configured by the user and read
 * from completionMode().
 *
 * A short example:
 * \code
 * KCompletion completion;
 * completion.setOrder(KCompletion::Sorted);
 * completion.addItem("pfeiffer@kde.org");
 * completion.addItem("coolo@kde.org");
 * completion.addItem("carpdjih@sp.zrz.tu-berlin.de");
 * completion.addItem("carp@cs.tu-berlin.de");
 *
 * cout << completion.makeCompletion("ca").latin1() << endl;
 * \endcode
 *
 * In shell-completion mode, this will be "carp"; in auto-completion
 * mode it will be "carp\@cs.tu-berlin.de", as that is alphabetically
 * smaller.
 * If setOrder was set to Insertion, "carpdjih\@sp.zrz.tu-berlin.de"
 * would be completed in auto-completion mode, as that was inserted before
 * "carp\@cs.tu-berlin.de".
 *
 * You can dynamically update the completable items by removing and adding them
 * whenever you want.
 * For advanced usage, you could even use multiple KCompletion objects. E.g.
 * imagine an editor like kwrite with multiple open files. You could store
 * items of each file in a different KCompletion object, so that you know (and
 * tell the user) where a completion comes from.
 *
 * Note: KCompletion does not work with strings that contain 0x0 characters
 *       (unicode null), as this is used internally as a delimiter.
 *
 * You may inherit from KCompletion and override makeCompletion() in
 * special cases (like reading directories or urls and then supplying the
 * contents to KCompletion, as KUrlCompletion does), but this is usually
 * not necessary.
 *
 *
 * @author Carsten Pfeiffer <pfeiffer@kde.org>
 */
class KCOMPLETION_EXPORT KCompletion : public QObject
{
    Q_ENUMS(CompOrder)
    Q_PROPERTY(CompOrder order READ order WRITE setOrder)
    Q_PROPERTY(bool ignoreCase READ ignoreCase WRITE setIgnoreCase)
    Q_PROPERTY(QStringList items READ items WRITE setItems)
    Q_OBJECT
    Q_DECLARE_PRIVATE(KCompletion)

public:
    /**
     * This enum describes the completion mode used for by the KCompletion class.
     * See <a href="http://developer.kde.org/documentation/standards/kde/style/keys/completion.html">
     * the styleguide</a>.
     *
     * @since 5.0
     **/
    enum CompletionMode {
        /**
         * No completion is used.
         */
        CompletionNone = 1,
        /**
         * Text is automatically filled in whenever possible.
         */
        CompletionAuto,
        /**
         * Same as automatic, but shortest match is used for completion.
         */
        CompletionMan,
        /**
         * Completes text much in the same way as a typical *nix shell would.
         */
        CompletionShell,
        /**
         * Lists all possible matches in a popup list box to choose from.
         */
        CompletionPopup,
        /**
         * Lists all possible matches in a popup list box to choose from, and automatically
         * fills the result whenever possible.
         */
        CompletionPopupAuto
    };

    /**
     * Constants that represent the order in which KCompletion performs
     * completion lookups.
     */
    enum CompOrder { Sorted,    ///< Use alphabetically sorted order
                     Insertion, ///< Use order of insertion
                     Weighted   ///< Use weighted order
                   };

    /**
     * Constructor, nothing special here :)
     */
    KCompletion();

    /**
     * Destructor, nothing special here, either.
     */
    virtual ~KCompletion();

    /**
     * Returns a list of all completion items that contain the given @p string.
     * @param string the string to complete
     * @return a list of items which contain @p text as a substring,
     * i.e. not necessarily at the beginning.
     *
     * @see makeCompletion
     */
    QStringList substringCompletion(const QString &string) const;

    /**
     * Returns the last match. Might be useful if you need to check whether
     * a completion is different from the last one.
     * @return the last match. QString() is returned when there is no
     *         last match.
     */
    virtual const QString &lastMatch() const;

    /**
     * Returns a list of all items inserted into KCompletion. This is useful
     * if you need to save the state of a KCompletion object and restore it
     * later.
     *
     * Important note: when order() == Weighted, then every item in the
     * stringlist has its weight appended, delimited by a colon. E.g. an item
     * "www.kde.org" might look like "www.kde.org:4", where 4 is the weight.
     *
     * This is necessary so that you can save the items along with its
     * weighting on disk and load them back with setItems(), restoring its
     * weight as well. If you really don't want the appended weightings, call
     * setOrder( KCompletion::Insertion ) before calling items().
     *
     * @return a list of all items
     * @see setItems
     */
    QStringList items() const;

    /**
     * Returns true if the completion object contains no entries.
     */
    bool isEmpty() const;

    /**
     * Sets the completion mode.
     * @param mode the completion mode
     * @see CompletionMode
     */
    virtual void setCompletionMode(CompletionMode mode);

    /**
     * Returns the current completion mode.
     *
     * @return the current completion mode, default is CompletionPopup
     * @see setCompletionMode
     * @see CompletionMode
     */
    CompletionMode completionMode() const;

    /**
     * KCompletion offers three different ways in which it offers its items:
     * @li in the order of insertion
     * @li sorted alphabetically
     * @li weighted
     *
     * Choosing weighted makes KCompletion perform an implicit weighting based
     * on how often an item is inserted. Imagine a web browser with a location
     * bar, where the user enters URLs. The more often a URL is entered, the
     * higher priority it gets.
     *
     * Note: Setting the order to sorted only affects new inserted items,
     * already existing items will stay in the current order. So you probably
     * want to call setOrder(Sorted) before inserting items if you want
     * everything sorted.
     *
     * Default is insertion order.
     * @param order the new order
     * @see order
     */
    virtual void setOrder(CompOrder order);

    /**
     * Returns the completion order.
     * @return the current completion order.
     * @see setOrder
     */
    CompOrder order() const;

    /**
     * Setting this to true makes KCompletion behave case insensitively.
     * E.g. makeCompletion("CA"); might return "carp\@cs.tu-berlin.de".
     * Default is false (case sensitive).
     * @param ignoreCase true to ignore the case
     * @see ignoreCase
     */
    virtual void setIgnoreCase(bool ignoreCase);

    /**
     * Returns whether KCompletion acts case insensitively or not.
     * Default is false (case sensitive).
     * @return true if the case will be ignored
     * @see setIgnoreCase
     */
    bool ignoreCase() const;

    /**
     * Returns a list of all items matching the last completed string.
     * It might take some time if you have a @em lot of items.
     * @return a list of all matches for the last completed string.
     * @see substringCompletion
     */
    QStringList allMatches();

    /**
     * Returns a list of all items matching @p string.
     * @param string the string to match
     * @return the list of all matches
     */
    QStringList allMatches(const QString &string);

    /**
     * Returns a list of all items matching the last completed string.
     * It might take some time if you have a @em lot of items.
     * The matches are returned as KCompletionMatches, which also
     * keeps the weight of the matches, allowing
     * you to modify some matches or merge them with matches
     * from another call to allWeightedMatches(), and sort the matches
     * after that in order to have the matches ordered correctly.
     *
     * @return a list of all completion matches
     * @see substringCompletion
     */
    KCompletionMatches allWeightedMatches();

    /**
     * Returns a list of all items matching @p string.
     * @param string the string to match
     * @return a list of all matches
     */
    KCompletionMatches allWeightedMatches(const QString &string);

    /**
     * Enables/disables emitting a sound when
     * @li makeCompletion() can't find a match
     * @li there is a partial completion (= multiple matches in
     *     Shell-completion mode)
     * @li nextMatch() or previousMatch() hit the last possible
     *     match and the list is rotated
     *
     * KNotifyClient() is used to emit the sounds.
     *
     * @param enable true to enable sounds
     * @see soundsEnabled
     */
    virtual void setSoundsEnabled(bool enable);

    /**
     * Tells you whether KCompletion will emit sounds on certain occasions.
     * Default is enabled.
     * @return true if sounds are enabled
     * @see setSoundsEnabled
     */
    bool soundsEnabled() const;

    /**
     * Returns true when more than one match is found.
     * @return true if there is more than one match
     * @see multipleMatches
     */
    bool hasMultipleMatches() const;

public Q_SLOTS:
    /**
     * Attempts to find an item in the list of available completions
     * that begins with @p string. Will either return the first matching item
     * (if there is more than one match) or QString(), if no match is
     * found.
     *
     * In the latter case, a sound will be emitted, depending on
     * soundsEnabled().
     * If a match is found, it will be emitted via the signal
     * match().
     *
     * If this is called twice or more with the same string while no
     * items were added or removed in the meantime, all available completions
     * will be emitted via the signal matches().
     * This happens only in shell-completion mode.
     *
     * @param string the string to complete
     * @return the matching item, or QString() if there is no matching
     * item.
     * @see substringCompletion
     */
    virtual QString makeCompletion(const QString &string);

    /**
     * Returns the next item from the list of matching items.
     * When reaching the beginning, the list is rotated so it will return the
     * last match and a sound is emitted (depending on soundsEnabled()).
     * @return the next item from the list of matching items.
     * When there is no match, QString() is returned and
     * a sound is emitted.
     */
    QString previousMatch();

    /**
     * Returns the next item from the list of matching items.
     * When reaching the last item, the list is rotated, so it will return
     * the first match and a sound is emitted (depending on
     * soundsEnabled()).
     * @return the next item from the list of matching items. When there is no
     * match, QString() is returned and a sound is emitted.
     */
    QString nextMatch();

    /**
     * Attempts to complete "string" and emits the completion via match().
     * Same as makeCompletion(), but in this case as a slot.
     * @param string the string to complete
     * @see makeCompletion
     * @deprecated since 5.0, use makeCompletion() instead
     */
#ifndef KCOMPLETION_NO_DEPRECATED
    KCOMPLETION_DEPRECATED void slotMakeCompletion(const QString &string)     //inline (redirect)
    {
        (void) makeCompletion(string);
    }

    /**
     * Searches the previous matching item and emits it via match().
     * Same as previousMatch(), but in this case as a slot.
     * @see previousMatch
     * @deprecated since 5.0, use previousMatch() instead
     */
    KCOMPLETION_DEPRECATED void slotPreviousMatch()   //inline (redirect)
    {
        (void) previousMatch();
    }

    /**
     * Searches the next matching item and emits it via match().
     * Same as nextMatch(), but in this case as a slot.
     * @see nextMatch
     * @deprecated since 5.0, use nextMatch() instead
     */
    KCOMPLETION_DEPRECATED void slotNextMatch()   //inline (redirect)
    {
        (void) nextMatch();
    }
#endif

    /**
     * Inserts @p items into the list of possible completions.
     * It does the same as setItems(), but without calling clear() before.
     * @param items the items to insert
     */
    void insertItems(const QStringList &items);

    /**
     * Sets the list of items available for completion. Removes all previous
     * items.
     *
     * Notice: when order() == Weighted, then the weighting is looked up for
     * every item in the stringlist. Every item should have ":number" appended,
     * where number is an unsigned integer, specifying the weighting.
     *
     * If you don't like this, call
     * setOrder(KCompletion::Insertion)
     * before calling setItems().
     *
     * @param itemList the list of items that are available for completion
     * @see items
     */
    virtual void setItems(const QStringList &itemList);

    /**
     * Adds an item to the list of available completions.
     * Resets the current item state (previousMatch() and nextMatch()
     * won't work the next time they are called).
     * @param item the item to add
     */
    void addItem(const QString &item);

    /**
     * Adds an item to the list of available completions.
     * Resets the current item state (previousMatch() and nextMatch()
     * won't work the next time they are called).
     *
     * Sets the weight of the item to @p weight or adds it to the current
     * weight if the item is already available. The weight has to be greater
     * than 1 to take effect (default weight is 1).
     * @param item the item to add
     * @param weight the weight of the item, default is 1
     */
    void addItem(const QString &item, uint weight);

    /**
     * Removes an item from the list of available completions.
     * Resets the current item state (previousMatch() and nextMatch()
     * won't work the next time they are called).
     * @param item the item to remove
     */
    void removeItem(const QString &item);

    /**
     * Removes all inserted items.
     */
    virtual void clear();

Q_SIGNALS:
    /**
     * This signal is emitted when a match is found.
     *
     * In particular, makeCompletion(), previousMatch() and nextMatch()
     * all emit this signal; makeCompletion() will only emit it when a
     * match is found, but the other methods will alwasy emit it (and so
     * may emit it with an empty string).
     *
     * @param item the matching item, or QString() if there were no more
     * matching items.
     */
    void match(const QString &item);

    /**
     * This signal is emitted by makeCompletion() in shell-completion mode
     * when the same string is passed to makeCompletion() multiple times in
     * a row.
     * @param matchlist the list of all matching items
     */
    void matches(const QStringList &matchlist);

    /**
     * This signal is emitted when calling makeCompletion() and more than
     * one matching item is found.
     * @see hasMultipleMatches
     */
    void multipleMatches();

protected:
    /**
     * This method is called after a completion is found and before the
     * matching string is emitted. You can override this method to modify the
     * string that will be emitted.
     * This is necessary e.g. in KUrlCompletion(), where files with spaces
     * in their names are shown escaped ("filename\ with\ spaces"), but stored
     * unescaped inside KCompletion.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @param match the match to process
     * @see postProcessMatches
     */
    virtual void postProcessMatch(QString *match) const;

    /**
     * This method is called before a list of all available completions is
     * emitted via matches(). You can override this method to modify the
     * found items before match() or matches() are emitted.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @param matchList the matches to process
     * @see postProcessMatch
     */
    virtual void postProcessMatches(QStringList *matchList) const;

    /**
     * This method is called before a list of all available completions is
     * emitted via #matches(). You can override this method to modify the
     * found items before #match() or #matches() are emitted.
     * Never delete that pointer!
     *
     * Default implementation does nothing.
     * @param matches the matches to process
     * @see postProcessMatch
     */
    virtual void postProcessMatches(KCompletionMatches *matches) const;

private:
    Q_DISABLE_COPY(KCompletion)
    const QScopedPointer<KCompletionPrivate> d_ptr;
};

#endif // KCOMPLETION_H
