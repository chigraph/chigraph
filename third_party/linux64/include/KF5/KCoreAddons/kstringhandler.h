/* This file is part of the KDE libraries
   Copyright (C) 1999 Ian Zepp (icszepp@islc.net)
   Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>
   Copyright (C) 2006 by Dominic Battre <dominic@battre.de>
   Copyright (C) 2006 by Martin Pool <mbp@canonical.com>

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
#ifndef KSTRINGHANDLER_H
#define KSTRINGHANDLER_H

#include <kcoreaddons_export.h>

#include <QtCore/qnamespace.h>

class QChar;
class QRegExp;
class QString;
class QStringList;

/**
 * This namespace contains utility functions for handling strings.
 *
 * The functions here are intended to provide an easy way to
 * cut/slice/splice words inside sentences in whatever order desired.
 * While the main focus of KStringHandler is words (ie characters
 * separated by spaces/tabs), the two core functions here (split()
 * and join()) will allow you to use any character as a separator
 * This will make it easy to redefine what a 'word' means in the
 * future if needed.
 *
 * The function names and calling styles are based on python and mIRC's
 * scripting support.
 *
 * The ranges are a fairly powerful way of getting/stripping words from
 * a string. These ranges function, for the large part, as they would in
 * python. See the word(const QString&, int) and remword(const QString&, int)
 * functions for more detail.
 *
 * The methods here are completely stateless.  All strings are cut
 * on the fly and returned as new qstrings/qstringlists.
 *
 * @short Namespace for manipulating words and sentences in strings
 * @author Ian Zepp <icszepp@islc.net>
 * @see KShell
 */
namespace KStringHandler
{

/** Capitalizes each word in the string
  * "hello there" becomes "Hello There"        (string)
  * @param text the text to capitalize
  * @return the resulting string
  */
KCOREADDONS_EXPORT QString capwords(const QString &text);

/** Capitalizes each word in the list
  * [hello, there] becomes [Hello, There]    (list)
  * @param list the list to capitalize
  * @return the resulting list
  */
KCOREADDONS_EXPORT QStringList capwords(const QStringList &list);

/** Substitute characters at the beginning of a string by "...".
 * @param str is the string to modify
 * @param maxlen is the maximum length the modified string will have
 * If the original string is shorter than "maxlen", it is returned verbatim
 * @return the modified string
 */
KCOREADDONS_EXPORT QString lsqueeze(const QString &str, int maxlen = 40);

/** Substitute characters at the middle of a string by "...".
 * @param str is the string to modify
 * @param maxlen is the maximum length the modified string will have
 * If the original string is shorter than "maxlen", it is returned verbatim
 * @return the modified string
 */
KCOREADDONS_EXPORT QString csqueeze(const QString &str, int maxlen = 40);

/** Substitute characters at the end of a string by "...".
 * @param str is the string to modify
 * @param maxlen is the maximum length the modified string will have
 * If the original string is shorter than "maxlen", it is returned verbatim
 * @return the modified string
 */
KCOREADDONS_EXPORT QString rsqueeze(const QString &str, int maxlen = 40);

/**
 * Split a QString into a QStringList in a similar fashion to the static
 * QStringList function in Qt, except you can specify a maximum number
 * of tokens. If max is specified (!= 0) then only that number of tokens
 * will be extracted. The final token will be the remainder of the string.
 *
 * Example:
 * \code
 * perlSplit("__", "some__string__for__you__here", 4)
 * QStringList contains: "some", "string", "for", "you__here"
 * \endcode
 *
 * @param sep is the string to use to delimit s.
 * @param s is the input string
 * @param max is the maximum number of extractions to perform, or 0.
 * @return A QStringList containing tokens extracted from s.
 */
KCOREADDONS_EXPORT QStringList perlSplit(const QString &sep,
        const QString &s,
        int max = 0);

/**
 * Split a QString into a QStringList in a similar fashion to the static
 * QStringList function in Qt, except you can specify a maximum number
 * of tokens. If max is specified (!= 0) then only that number of tokens
 * will be extracted. The final token will be the remainder of the string.
 *
 * Example:
 * \code
 * perlSplit(' ', "kparts reaches the parts other parts can't", 3)
 * QStringList contains: "kparts", "reaches", "the parts other parts can't"
 * \endcode
 *
 * @param sep is the character to use to delimit s.
 * @param s is the input string
 * @param max is the maximum number of extractions to perform, or 0.
 * @return A QStringList containing tokens extracted from s.
 */
KCOREADDONS_EXPORT QStringList perlSplit(const QChar &sep,
        const QString &s,
        int max = 0);

/**
 * Split a QString into a QStringList in a similar fashion to the static
 * QStringList function in Qt, except you can specify a maximum number
 * of tokens. If max is specified (!= 0) then only that number of tokens
 * will be extracted. The final token will be the remainder of the string.
 *
 * Example:
 * \code
 * perlSplit(QRegExp("[! ]"), "Split me up ! I'm bored ! OK ?", 3)
 * QStringList contains: "Split", "me", "up ! I'm bored ! OK ?"
 * \endcode
 *
 * @param sep is the regular expression to use to delimit s.
 * @param s is the input string
 * @param max is the maximum number of extractions to perform, or 0.
 * @return A QStringList containing tokens extracted from s.
 */
KCOREADDONS_EXPORT QStringList perlSplit(const QRegExp &sep,
        const QString &s,
        int max = 0);

/**
 * This method auto-detects URLs in strings, and adds HTML markup to them
 * so that richtext or HTML-enabled widgets will display the URL correctly.
 * @param text the string which may contain URLs
 * @return the resulting text
 */
KCOREADDONS_EXPORT QString tagUrls(const QString &text);

/**
  Obscure string by using a simple symmetric encryption. Applying the
  function to a string obscured by this function will result in the original
  string.

  The function can be used to obscure passwords stored to configuration
  files. Note that this won't give you any more security than preventing
  that the password is directly copied and pasted.

  @param str string to be obscured
  @return obscured string
*/
KCOREADDONS_EXPORT QString obscure(const QString &str);

/**
  Guess whether a string is UTF8 encoded.

  @param str the string to check
  @return true if UTF8. If false, the string is probably in Local8Bit.
 */
KCOREADDONS_EXPORT bool isUtf8(const char *str);

/**
  Construct QString from a c string, guessing whether it is UTF8- or
  Local8Bit-encoded.

  @param str the input string
  @return the (hopefully correctly guessed) QString representation of @p str
  @see KEncodingProber

 */
KCOREADDONS_EXPORT QString from8Bit(const char *str);

/**
  Preprocesses the given string in order to provide additional line breaking
  opportunities for QTextLayout.

  This is done by inserting ZWSP (Zero-width space) characters in the string
  at points that wouldn't normally be considered word boundaries by QTextLayout,
  but where wrapping the text will produce good results.

  Examples of such points includes after punctuation signs, underscores and
  dashes, that aren't followed by spaces.

  @since 4.4
*/
KCOREADDONS_EXPORT QString preProcessWrap(const QString &text);
}
#endif
