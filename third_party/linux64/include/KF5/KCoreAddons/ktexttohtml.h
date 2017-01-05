/*
  Copyright (c) 2002 Dave Corrie <kde@davecorrie.com>
  Copyright (c) 2014 Daniel Vrátil <dvratil@redhat.com>

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

#ifndef KCOREADDONS_KTEXTTOHTML_H
#define KCOREADDONS_KTEXTTOHTML_H

#include <kcoreaddons_export.h>

#include <QString>
#include <QFlag>

/**
 * @author Dave Corrie \<kde@davecorrie.com\>
 */
namespace KTextToHTML
{

/**
 * @since 5.5.0
 */
enum Option
{
    /**
     * Preserve white-space formatting of the text
     */
    PreserveSpaces  = 1 << 1,

    /**
     * Replace text emoticons smileys by emoticons images.
     *
     * @note
     * This option works only when KEmoticons framework is available at runtime,
     * and requires QGuiApplication, otherwise the flag is simply ignored.
     */
    ReplaceSmileys  = 1 << 2,

    /**
     * Don't parse and replace any URLs.
     */
    IgnoreUrls      = 1 << 3,

    /**
     * Interpret text highlighting markup, like *bold*, _underline_ and /italic/,
     * and wrap them in corresponding HTML entities.
     */
    HighlightText   = 1 << 4
};
Q_DECLARE_FLAGS(Options, Option)

/**
 * Converts plaintext into html. The following characters are converted
 * to HTML entities: & " < >. Newlines are also preserved.
 *
 * @param  plainText  The text to be converted into HTML.
 * @param  flags      The flags to consider when processing @p plainText.
 * @param  maxUrlLen  The maximum length of permitted URLs. The reason for
 *                    this limit is that there may be possible security
 *                    implications in handling URLs of unlimited length.
 * @param  maxAddressLen  The maximum length of permitted email addresses.
 *                    The reason for this limit is that there may be possible
 *                    security implications in handling addresses of unlimited
 *                    length.
 *
 * @return An HTML version of the text supplied in the 'plainText'
 * parameter, suitable for inclusion in the BODY of an HTML document.
 *
 * @since 5.5.0
 */
KCOREADDONS_EXPORT QString convertToHtml(const QString &plainText,
                                         const KTextToHTML::Options &options,
                                         int maxUrlLen = 4096,
                                         int maxAddressLen = 255);

}

Q_DECLARE_OPERATORS_FOR_FLAGS(KTextToHTML::Options)

#endif
