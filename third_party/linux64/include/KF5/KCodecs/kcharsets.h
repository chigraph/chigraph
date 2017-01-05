/* This file is part of the KDE libraries
    Copyright (C) 1999 Lars Knoll (knoll@kde.org)

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
#ifndef KCHARSETS_H
#define KCHARSETS_H

#include <kcodecs_export.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QList>

class KCharsets;
class KCharsetsPrivate;

class QChar;
class QString;
class QStringList;
class QTextCodec;

/**
 * Charset font and encoder/decoder handling.
 *
 * This is needed, because Qt's encoding name matching in
 * QTextCodec::codecForName matches only closely-related encoded names
 * but not alternate names, e.g. found in the reality of the Internet.
 */
class KCODECS_EXPORT KCharsets
{
    Q_DECLARE_TR_FUNCTIONS(KCharsets)

protected:
    /** Protected constructor. If you need the kcharsets object, use
        KCharsets::charsets() instead.
    */
    KCharsets();

public:

    /**
     * Destructor.
     */
    virtual ~KCharsets();

    /**
     * Provided for compatibility.
     * @param name the name of the codec
     * @return the QTextCodec. If the desired codec could not be found,
     *         it returns a default (ISO 8859-1) codec
     */
    QTextCodec *codecForName(const QString &name) const;

    /**
     * Tries to find a QTextCodec to convert the given encoding from and to
     * Unicode.
     *
     * If no codec could be found, the ISO 8859-1 codec will be returned an
     * and @p ok will be set to false.
     *
     * @param n the name of the codec
     * @param ok true if a matching codec has been found, false if not
     * @return the QTextCodec. If the desired codec could not be found,
     *         it returns a default (ISO 8859-1) codec
     */
    QTextCodec *codecForName(const QString &n, bool &ok) const;

    /**
     * The global charset manager.
     * @return the global charset manager
     */
    static KCharsets *charsets();

    /**
     * @brief Converts an entity to a character.
     *
     * The string must contain only the
     * entity without the trailing ';'.
     * @param str the entity
     * @return QChar::Null if the entity could not be decoded.
     */
    static QChar fromEntity(const QString &str);

    /**
     * Overloaded member function. Tries to find an entity in the
     * QString str.
     * @param str the string containing entified
     * @param len is a return value, that gives the length of the decoded
     * entity.
     * @return a decoded entity if one could be found, QChar::null
     * otherwise
     */
    static QChar fromEntity(const QString &str, int &len);

    /**
     * Converts a QChar to an entity. The returned string does already
     * contain the leading '&' and the trailing ';'.
     * @param ch the char to convert
     * @return the entity
     */
    static QString toEntity(const QChar &ch);

    /**
     * Scans the given string for entities (like &amp;amp;) and resolves them
     * using fromEntity.
     * @param text the string containing the entities
     * @return the clean string
     */
    static QString resolveEntities(const QString &text);

    /**
     * Lists all available encodings as names.
     * @return the list of all encodings
     */
    QStringList availableEncodingNames() const;

    /**
     * Lists the available encoding names together with a more descriptive language.
     * @return the list of descriptive encoding names
     */
    QStringList descriptiveEncodingNames() const;

    /**
     * Lists the available encoding names grouped by script (or language that uses them).
     * @returns the list of lists consisting of description followed by encoding names (i.e. encodingsByScript().at(i).at(0) is a description for encodingsByScript().at(i).at(k), k>0)
     */
    QList<QStringList> encodingsByScript() const;

    /**
     * @brief Returns a long description for an encoding name.
     * @param encoding the encoding for the language
     * @return the long description for the encoding
     */
    QString descriptionForEncoding(const QString &encoding) const;

    /**
     * Returns the encoding for a string obtained with descriptiveEncodingNames().
     * @param descriptiveName the descriptive name for the encoding
     * @return the name of the encoding
     */
    QString encodingForName(const QString &descriptiveName) const;

private:
    KCharsetsPrivate *const d;
    friend struct KCharsetsSingletonPrivate;

    /**
     * @brief Get the QTextCodec for the name or return NULL
     *
     * This function is similar to KCharsets::codecForName except that it
     * can return a NULL value when the name was not found.
     *
     * @param n name of the text codec
     * @return pointer to the QTextCodec or NULL
     * @todo Make this function public when it is clear what API is needed.
     */
    QTextCodec *codecForNameOrNull(const QByteArray &n) const;
};

#endif
