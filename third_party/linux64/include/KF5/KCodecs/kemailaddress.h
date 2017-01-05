/*
  Copyright (c) 2004 Matt Douhan <matt@fruitsalad.org>

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

/**
  @file
  This file provides static methods for email address validation.

  @brief Email address validation methods.

  @author Matt Douhan \<matt@fruitsalad.org\>
 */

#ifndef KCODECS_EMAILADDRESS_H
#define KCODECS_EMAILADDRESS_H

#include <QUrl>

#include <QStringList>
#include <QByteArray>

#include <kcodecs_export.h>

/**
 * @since 5.5.0
 */
namespace KEmailAddress
{

/**
  @defgroup emailvalidation Email Validation Functions

  This collection of methods that can validate email addresses as supplied
  by the user (typically, user input from a text box). There are also
  functions for splitting an RFC2822 address into its component parts.

  @{
*/

/**
  Email validation result. The only 'success' code in
  this enumeration is AddressOK; all the other values
  indicate some specific problem with the address which
  is being validated.

  Result type for splitAddress(), isValidAddress()
  and isValidSimpleAddress().
*/
enum EmailParseResult {
    AddressOk,          /**< Email is valid */
    AddressEmpty,       /**< The address is empty */
    UnexpectedEnd,      /**< Something is unbalanced */
    UnbalancedParens,   /**< Unbalanced ( ) */
    MissingDomainPart,  /**< No domain in address */
    UnclosedAngleAddr,  /**< \< with no matching \> */
    UnopenedAngleAddr,  /**< \> with no preceding \< */
    TooManyAts,         /**< More than one \@ in address */
    UnexpectedComma,    /**< Comma not allowed here */
    TooFewAts,          /**< Missing \@ in address */
    MissingLocalPart,   /**< No address specified, only domain */
    UnbalancedQuote,    /**< Quotes (single or double) not matched */
    NoAddressSpec,
    DisallowedChar,     /**< An invalid character detected in address */
    InvalidDisplayName,  /**< An invalid displayname detected in address */
    TooFewDots /**< Missing \. in address */
};

/** Split a comma separated list of email addresses.

@param aStr a single string representing a list of addresses
@return a list of strings, where each string is one address
from the original list
*/
KCODECS_EXPORT
QStringList splitAddressList(const QString &aStr);

/**
  Splits the given address into display name, email address and comment.
  Returns AddressOk if no error was encountered. Otherwise an appropriate
  error code is returned. In case of an error the values of displayName,
  addrSpec and comment are undefined.

  @param address      a single email address,
  example: Joe User (comment1) <joe.user@example.org> (comment2)
  @param displayName  only out: the display-name of the email address, i.e.
  "Joe User" in the example; in case of an error the
  return value is undefined
  @param addrSpec     only out: the addr-spec, i.e. "joe.user@example.org"
  in the example; in case of an error the return value is undefined
  @param comment      only out: the space-separated comments, i.e.
  "comment1 comment2" in the example; in case of an
  error the return value is undefined

  @return             AddressOk if no error was encountered. Otherwise an
  appropriate error code is returned.
*/
KCODECS_EXPORT
EmailParseResult splitAddress(const QByteArray &address,
                              QByteArray &displayName,
                              QByteArray &addrSpec,
                              QByteArray &comment);

/**
  This is an overloaded member function, provided for convenience.
  It behaves essentially like the above function.

  Splits the given address into display name, email address and comment.
  Returns AddressOk if no error was encountered. Otherwise an appropriate
  error code is returned. In case of an error the values of displayName,
  addrSpec and comment are undefined.

  @param address      a single email address,
  example: Joe User (comment1) <joe.user@example.org> (comment2)
  @param displayName  only out: the display-name of the email address, i.e.
  "Joe User" in the example; in case of an error the
  return value is undefined
  @param addrSpec     only out: the addr-spec, i.e. "joe.user@example.org"
  in the example; in case of an error the return value is undefined
  @param comment      only out: the space-separated comments, i.e.
  "comment1 comment2" in the example; in case of an
  error the return value is undefined

  @return             AddressOk if no error was encountered. Otherwise an
  appropriate error code is returned.
*/
KCODECS_EXPORT
EmailParseResult splitAddress(const QString &address,
                              QString &displayName,
                              QString &addrSpec,
                              QString &comment);

/**
  Validates an email address in the form of "Joe User" <joe@example.org>.
  Returns AddressOk if no error was encountered. Otherwise an appropriate
  error code is returned.

  @param aStr         a single email address,
  example: Joe User (comment1) <joe.user@example.org>
  @return             AddressOk if no error was encountered. Otherwise an
  appropriate error code is returned.
*/
KCODECS_EXPORT
EmailParseResult isValidAddress(const QString &aStr);

/**
  Validates a list of email addresses, and also allow aliases and
  distribution lists to be expanded before validation.

  @param aStr         a string containing a list of email addresses.
  @param badAddr      a string to hold the address that was faulty.

  @return AddressOk if no error was encountered. Otherwise an
  appropriate error code is returned.
*/
KCODECS_EXPORT
EmailParseResult isValidAddressList(const QString &aStr,
                                    QString &badAddr);

/**
  Translate the enum errorcodes from emailParseResult
  into i18n'd strings that can be used for msg boxes.

  @param errorCode an @em error code returned from one of the
  email validation functions. Do not pass
  AddressOk as a value, since that will yield
  a misleading error message

  @return human-readable and already translated message describing
  the validation error.
*/
KCODECS_EXPORT
QString emailParseResultToString(EmailParseResult errorCode);

/**
  Validates an email address in the form of joe@example.org.
  Returns true if no error was encountered.
  This method should be used when the input field should not
  allow a "full" email address with comments and other special
  cases that normally are valid in an email address.

  @param aStr         a single email address,
  example: joe.user@example.org

  @return             true if no error was encountered.

  @note This method differs from calling isValidAddress()
  and checking that that returns AddressOk in two ways:
  it is faster, and it does @em not allow fancy addresses.
*/
KCODECS_EXPORT
bool isValidSimpleAddress(const QString &aStr);

/**
  Returns a i18n string to be used in msgboxes. This allows for error
  messages to be the same across the board.

  @return             An i18n ready string for use in msgboxes.
*/

KCODECS_EXPORT
QString simpleEmailAddressErrorMsg();

/** @}  */

/** @defgroup emailextraction Email Extraction Functions
    @{
*/

/**
  Returns the pure email address (addr-spec in RFC2822) of the given address
  (mailbox in RFC2822).

  @param address  an email address, e.g. "Joe User <joe.user@example.org>"
  @return         the addr-spec of @a address, i.e. joe.user@example.org
  in the example
*/
KCODECS_EXPORT
QByteArray extractEmailAddress(const QByteArray &address);

/*KF6 merge with above*/

/**
  Returns the pure email address (addr-spec in RFC2822) of the given address
  (mailbox in RFC2822).

  @param address  an email address, e.g. "Joe User <joe.user@example.org>"
  @param errorMessage return error message when we can't parse email
  @return         the addr-spec of @a address, i.e. joe.user@example.org
  in the example
  @since 5.11.0

*/
KCODECS_EXPORT
QByteArray extractEmailAddress(const QByteArray &address, QString &errorMessage);

/**
  This is an overloaded member function, provided for convenience.
  It behaves essentially like the above function.

  Returns the pure email address (addr-spec in RFC2822) of the given
  address (mailbox in RFC2822).

  @param address  an email address, e.g. "Joe User <joe.user@example.org>"
  @return         the addr-spec of @a address, i.e. joe.user@example.org
  in the example
*/
KCODECS_EXPORT
QString extractEmailAddress(const QString &address);

/**
  Returns the pure email address (addr-spec in RFC2822) of the first
  email address of a list of addresses.

  @param addresses an email address, e.g. "Joe User <joe.user@example.org>"
  @param errorMessage return error message when we can't parse email
  @return          the addr-spec of @a addresses, i.e. joe.user@example.org
  in the example
  @since 5.11
*/

KCODECS_EXPORT
QString extractEmailAddress(const QString &address, QString &errorMessage);

/**
  Returns the pure email address (addr-spec in RFC2822) of the first
  email address of a list of addresses.

  @param addresses an email address, e.g. "Joe User <joe.user@example.org>"
  @return          the addr-spec of @a addresses, i.e. joe.user@example.org
  in the example
*/

/*KF6 merge with above*/
KCODECS_EXPORT
QByteArray firstEmailAddress(const QByteArray &addresses);

/**
  Returns the pure email address (addr-spec in RFC2822) of the first
  email address of a list of addresses.

  @param addresses an email address, e.g. "Joe User <joe.user@example.org>"
  @param errorMessage return error message when we can't parse email
  @return          the addr-spec of @a addresses, i.e. joe.user@example.org
  in the example
  @since 5.11.0
*/

KCODECS_EXPORT
QByteArray firstEmailAddress(const QByteArray &addresses, QString &errorMessage);

/**
  This is an overloaded member function, provided for convenience.
  It behaves essentially like the above function.

  Returns the pure email address (addr-spec in RFC2822) of the first
  email address of a list of addresses.

  @param addresses an email address, e.g. "Joe User <joe.user@example.org>"
  @return          the addr-spec of @a addresses, i.e. joe.user@example.org
  in the example
*/
KCODECS_EXPORT
QString firstEmailAddress(const QString &addresses);

/**
  This is an overloaded member function, provided for convenience.
  It behaves essentially like the above function.

  Returns the pure email address (addr-spec in RFC2822) of the first
  email address of a list of addresses.

  @param addresses an email address, e.g. "Joe User <joe.user@example.org>"
  @param errorMessage return error message when we can't parse email
  @return          the addr-spec of @a addresses, i.e. joe.user@example.org
  in the example
  @since 5.11.0
*/
KCODECS_EXPORT
QString firstEmailAddress(const QString &addresses, QString &errorMessage);


/**
  Return email address and name from string.
  Examples:
    "Stefan Taferner <taferner@example.org>" returns "taferner@example.org"
    and "Stefan Taferner". "joe@example.com" returns "joe@example.com"
    and "". Note that this only returns the first address.

  Also note that the return value is true if both the name and the
  mail are not empty: this does NOT tell you if mail contains a
  valid email address or just some rubbish.

  @param aStr  an email address, e.g "Joe User <joe.user@example.org>"
  @param name  only out: returns the displayname, "Joe User" in the example
  @param mail  only out: returns the email address "joe.user@example.org"
  in the example

  @return true if both name and email address are not empty
*/
KCODECS_EXPORT
bool extractEmailAddressAndName(const QString &aStr, QString &mail,
                                QString &name);

/**
  Compare two email addresses. If matchName is false, it just checks
  the email address, and returns true if this matches. If matchName
  is true, both the name and the email must be the same.

  @param email1  the first email address to use for comparison
  @param email2  the second email address to use for comparison
  @param matchName  if set to true email address and displayname must match

  @return true if the comparison matches true in all other cases
*/
KCODECS_EXPORT
bool compareEmail(const QString &email1, const QString &email2,
                  bool matchName);

/**
  Returns a normalized address built from the given parts. The normalized
  address is of one the following forms:
    - displayName (comment) &lt;addrSpec&gt;
    - displayName &lt;addrSpec&gt;
    - comment &lt;addrSpec&gt;
    - addrSpec

  @param displayName  the display name of the address
  @param addrSpec     the actual email address (addr-spec in RFC 2822)
  @param comment      a comment

  @return             a normalized address built from the given parts
*/
KCODECS_EXPORT
QString normalizedAddress(const QString &displayName,
                          const QString &addrSpec,
                          const QString &comment = QString());

/** @} */

/** @defgroup emailidn Email IDN (punycode) handling
    @{
*/

/**
  Decodes the punycode domain part of the given addr-spec if it's an IDN.

  @param addrSpec  a pure 7-bit email address (addr-spec in RFC2822)
  @return          the email address with Unicode domain
*/
KCODECS_EXPORT
QString fromIdn(const QString &addrSpec);

/**
  Encodes the domain part of the given addr-spec in punycode if it's an IDN.

  @param addrSpec  a pure email address with Unicode domain
  @return          the email address with domain in punycode
*/
KCODECS_EXPORT
QString toIdn(const QString &addrSpec);

/**
  Normalizes all email addresses in the given list and decodes all IDNs.

  @param addresses  a list of email addresses with punycoded IDNs
  @return           the email addresses in normalized form with Unicode IDNs
*/
KCODECS_EXPORT
QString normalizeAddressesAndDecodeIdn(const QString &addresses);

/**
  Normalizes all email addresses in the given list and encodes all IDNs
  in punycode.

  @param str  a list of email addresses
  @return     the email addresses in normalized form
*/
KCODECS_EXPORT
QString normalizeAddressesAndEncodeIdn(const QString &str);

/** @} */

/** @ingroup emailextraction

  Add quote characters around the given string if it contains a
  character that makes that necessary, in an email name, such as ",".

  @param str  a string that may need quoting
  @return     the string quoted if necessary
*/
KCODECS_EXPORT
QString quoteNameIfNecessary(const QString &str);

/**
  * Creates a valid mailto: URL from the given mailbox.
  * @param mailbox The mailbox, which means the display name and the address specification, for
  *                example "Thomas McGuire" <thomas@domain.com>. The display name is optional.
  * @return a valid mailto: URL for the given mailbox.
  */
KCODECS_EXPORT
QUrl encodeMailtoUrl(const QString &mailbox);

/**
  * Extracts the mailbox out of the mailto: URL.
  * @param mailtoUrl the URL with the mailto protocol, which contains the mailbox to be extracted
  * @return the mailbox, which means the display name and the address specification.
  */
KCODECS_EXPORT
QString decodeMailtoUrl(const QUrl &mailtoUrl);

} // namespace KEmailAddress

#endif
