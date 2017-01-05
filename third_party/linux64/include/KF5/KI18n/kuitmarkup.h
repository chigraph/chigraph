/*  This file is part of the KDE libraries
    Copyright (C) 2013 Chusslove Illich <caslav.ilic@gmx.net>

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

#ifndef KUITMARKUP_H
#define KUITMARKUP_H

#include <ki18n_export.h>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>

class KuitSetup;

/**
 * Global constants and functions related to KUIT markup.
 */
namespace Kuit
{
/**
 * Visual formats into which KUIT markup can be resolved.
 */
enum VisualFormat {
    /**
     * Visual format not defined.
     * This value can be explicitly set
     * (e.g. through \c KLocalizedString::withFormat)
     * to indicate that the format should be decided
     * by another mechanism (e.g. context UI marker).
     */
    UndefinedFormat = 0,
    /**
     * Plain text.
     */
    PlainText = 10,
    /**
     * Qt rich text (HTML subset).
     */
    RichText = 20,
    /**
     * Terminal escape sequences.
     */
    TermText = 30
};

/**
 * Classification of KUIT tags.
 */
enum TagClass {
    /**
     * Tags wrapping text inserted into running text.
     */
    PhraseTag = 0,
    /**
     * Tags spliting text into paragraph-level blocks.
     */
    StructTag = 1
};

/**
 * Functions accepted by tag formatting functions.
 *
 * \param languages the target languages (by decreasing priority)
 * \param tagName the wrapping tag name
 * \param attributes the attribute name-value pairs in the tag
 * \param text the wrapped text
 * \param tagPath the ordered list of ancestor tag names, parent first
 * \param format the target visual format
 * \return formatted text
 */
typedef QString(*TagFormatter)(const QStringList &languages,
                               const QString &tagName,
                               const QHash<QString, QString> &attributes,
                               const QString &text,
                               const QStringList &tagPath,
                               Kuit::VisualFormat format);

/**
 * Get hold of the KUIT setup object for a given domain.
 *
 * \param domain the translation domain
 * \return pointer to KUIT setup object
 */
KuitSetup &setupForDomain(const char *domain);

KuitSetup &setupForDomain(const QByteArray& domain);
}

class KLocalizedString;
class KuitSetupPrivate;
class KuitFormatterPrivate;

/**
 * Class for modifying KUIT markup in a given domain.
 *
 * Not directly constructed, but obtained through \c Kuit::setupForDomain.
 */
class KI18N_EXPORT KuitSetup
{
    friend KuitSetup &Kuit::setupForDomain(const QByteArray& domain);
    friend class KuitFormatterPrivate;

public:

    /**
     * Destructor.
     */
    ~KuitSetup();

    /**
     * Set the formatting string for a tag with attributes combination.
     *
     * If a new tag name is given, this effectively defines a new tag.
     * The same holds for attribute names.
     *
     * The pattern string \p pattern should contain placeholders
     * for inserting the text and the attribute values.
     * %1 will be replaced with the wrapped text, and %2 and upwards
     * with attribute values in the order given by \p attrNames.
     * Non markup-aware translation call with context (\c ki18nc)
     * should be used to create the pattern string.
     *
     * In addition to the pattern, a formatting function
     * of the type \c TagFormatter can be given.
     * This function receives the full markup parsing context,
     * so that it can do whatever is necessary with the wrapped text.
     * The result of this function is then substituted into the pattern.
     * You can also give an empty pattern (as <tt>KLocalizedString()</tt>)
     * together with the formatting function, in which case the function
     * is assumed to do everything and no substitution is performed.
     *
     * \param tagName the name of the tag
     * \param attribNames the names of the attributes (empty names are ignored)
     * \param format the target visual format
     * \param pattern the pattern string
     * \param leadingNewlines the number of new lines (\\n) to be maintained
     *                        between any preceding text and the text wrapped
     *                        with this tag (for formats where it matters)
     */
    void setTagPattern(const QString &tagName,
                       const QStringList &attribNames,
                       Kuit::VisualFormat format,
                       const KLocalizedString &pattern,
                       Kuit::TagFormatter formatter = NULL,
                       int leadingNewlines = 0);

    /**
     * Set the KUIT class of the tag.
     *
     * \param tagName the name of the tag
     * \param aClass the KUIT tag class
     */
    void setTagClass(const QString &tagName, Kuit::TagClass aClass);

    /**
     * Set the default visual format for a given UI marker.
     *
     * Giving <tt>"@<major>"</tt> for \p marker means to set the format
     * only for standalone <tt>\@\<major\></tt> marker,
     * while <tt>"@<major>:"</tt> (with trailing colon) means to set
     * the same format for all <tt>\@\<major\>:\<minor\></tt> combinations.
     *
     * Defined UI marker major/minor combinations are listed in the section
     * \ref uimark_ctxt. If an UI marker combination outside of the defined
     * is given as \p marker, it will be ignored.
     *
     * Setting \c Kuit::UndefinedFormat as \p format
     * means to fall back to default format for the given UI marker.
     *
     * \param marker the UI marker
     * \param format the visual format
     */
    void setFormatForMarker(const QString &marker, Kuit::VisualFormat format);

private:

    KuitSetup(const QByteArray &domain);
    Q_DISABLE_COPY(KuitSetup)

    KuitSetupPrivate *const d;
};

#endif // KUITMARKUP_H
