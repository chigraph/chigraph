/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
   Copyright (c) 2001-2002 Marc Mutz <mutz@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

   The quoted-printable codec as described in RFC 2045, section 6.7. is by
   Rik Hemsley (C) 2001.
*/

#ifndef KCODECS_H
#define KCODECS_H

#define KBase64 KCodecs

#include <kcodecs_export.h>
#include <QtCore/QString>

template<typename T, typename U> class QHash;

class QByteArray;
class QIODevice;

/**
 * A wrapper class for the most commonly used encoding and
 * decoding algorithms.  Currently there is support for encoding
 * and decoding input using base64, uu and the quoted-printable
 * specifications.
 *
 * \b Usage:
 *
 * \code
 * QByteArray input = "Aladdin:open sesame";
 * QByteArray result = KCodecs::base64Encode(input);
 * cout << "Result: " << result.data() << endl;
 * \endcode
 *
 * <pre>
 * Output should be
 * Result: QWxhZGRpbjpvcGVuIHNlc2FtZQ==
 * </pre>
 *
 * The above example makes use of the convenience functions
 * (ones that accept/return null-terminated strings) to encode/decode
 * a string.  If what you need is to encode or decode binary data, then
 * it is highly recommended that you use the functions that take an input
 * and output QByteArray as arguments.  These functions are specifically
 * tailored for encoding and decoding binary data.
 *
 * @short A collection of commonly used encoding and decoding algorithms.
 * @author Dawit Alemayehu <adawit@kde.org>
 * @author Rik Hemsley <rik@kde.org>
 */
namespace KCodecs
{

/**
 * Encodes the given data using the quoted-printable algorithm.
 *
 * @param in      data to be encoded.
 * @param useCRLF if true the input data is expected to have
 *                CRLF line breaks and the output will have CRLF line
 *                breaks, too.
 * @return        quoted-printable encoded string.
 */
KCODECS_EXPORT QByteArray quotedPrintableEncode(const QByteArray &in,
        bool useCRLF = true);

/**
 * Encodes the given data using the quoted-printable algorithm.
 *
 * Use this function if you want the result of the encoding
 * to be placed in another array which cuts down the number
 * of copy operation that have to be performed in the process.
 * This is also the preferred method for encoding binary data.
 *
 * NOTE: the output array is first reset and then resized
 * appropriately before use, hence, all data stored in the
 * output array will be lost.
 *
 * @param in      data to be encoded.
 * @param out     encoded data.
 * @param useCRLF if true the input data is expected to have
 *                CRLF line breaks and the output will have CRLF line
 *                breaks, too.
 */
KCODECS_EXPORT void quotedPrintableEncode(const QByteArray &in, QByteArray &out,
        bool useCRLF);

/**
 * Decodes a quoted-printable encoded data.
 *
 * Accepts data with CRLF or standard unix line breaks.
 *
 * @param in  data to be decoded.
 * @return    decoded string.
 * @since 5.5
 */
KCODECS_EXPORT QByteArray quotedPrintableDecode(const QByteArray &in);

/**
 * Decodes a quoted-printable encoded data.
 *
 * Accepts data with CRLF or standard unix line breaks.
 * Use this function if you want the result of the decoding
 * to be placed in another array which cuts down the number
 * of copy operation that have to be performed in the process.
 * This is also the preferred method for decoding an encoded
 * binary data.
 *
 * NOTE: the output array is first reset and then resized
 * appropriately before use, hence, all data stored in the
 * output array will be lost.
 *
 * @param in   data to be decoded.
 * @param out  decoded data.
 */
KCODECS_EXPORT void quotedPrintableDecode(const QByteArray &in, QByteArray &out);

/**
 * Encodes the given data using the uuencode algorithm.
 *
 * The output is split into lines starting with the number of
 * encoded octets in the line and ending with a newline.  No
 * line is longer than 45 octets (60 characters), excluding the
 * line terminator.
 *
 * @param in   data to be uuencoded
 * @return     uuencoded string.
 */
KCODECS_EXPORT QByteArray uuencode(const QByteArray &in);

/**
 * Encodes the given data using the uuencode algorithm.
 *
 * Use this function if you want the result of the encoding
 * to be placed in another array and cut down the number of
 * copy operation that have to be performed in the process.
 * This is the preffered method for encoding binary data.
 *
 * NOTE: the output array is first reset and then resized
 * appropriately before use, hence, all data stored in the
 * output array will be lost.
 *
 * @param in   data to be uuencoded.
 * @param out  uudecoded data.
 */
KCODECS_EXPORT void uuencode(const QByteArray &in, QByteArray &out);

/**
 * Decodes the given data using the uudecode algorithm.
 *
 * Any 'begin' and 'end' lines like those generated by
 * the utilities in unix and unix-like OS will be
 * automatically ignored.
 *
 * @param in   data to be decoded.
 * @return     decoded string.
 */
KCODECS_EXPORT QByteArray uudecode(const QByteArray &in);

/**
 * Decodes the given data using the uudecode algorithm.
 *
 * Use this function if you want the result of the decoding
 * to be placed in another array which cuts down the number
 * of copy operation that have to be performed in the process.
 * This is the preferred method for decoding binary data.
 *
 * Any 'begin' and 'end' lines like those generated by
 * the utilities in unix and unix-like OS will be
 * automatically ignored.
 *
 * NOTE: the output array is first reset and then resized
 * appropriately before use, hence, all data stored in the
 * output array will be lost.
 *
 * @param in   data to be decoded.
 * @param out  uudecoded data.
 */
KCODECS_EXPORT void uudecode(const QByteArray &in, QByteArray &out);

/**
 * Encodes the given data using the base64 algorithm.
 *
 * The boolean argument determines if the encoded data is
 * going to be restricted to 76 characters or less per line
 * as specified by RFC 2045.  If @p insertLFs is true, then
 * there will be 76 characters or less per line.
 *
 * @param in         data to be encoded.
 * @param insertLFs  limit the number of characters per line.
 *
 * @return           base64 encoded string.
 * @since 5.5
 */
KCODECS_EXPORT QByteArray base64Encode(const QByteArray &in);

/**
 * @copydoc
 * KCodecs::base64Encode(QByteArray)
 * @deprecated Use KCodecs::base64Encode(QByteArray) instead.
 */
KCODECS_DEPRECATED_EXPORT QByteArray base64Encode(const QByteArray &in, bool insertLFs);

/**
 * Encodes the given data using the base64 algorithm.
 *
 * Use this function if you want the result of the encoding
 * to be placed in another array which cuts down the number
 * of copy operation that have to be performed in the process.
 * This is also the preferred method for encoding binary data.
 *
 * The boolean argument determines if the encoded data is going
 * to be restricted to 76 characters or less per line as specified
 * by RFC 2045.  If @p insertLFs is true, then there will be 76
 * characters or less per line.
 *
 * NOTE: the output array is first reset and then resized
 * appropriately before use, hence, all data stored in the
 * output array will be lost.
 *
 * @param in        data to be encoded.
 * @param out       encoded data.
 * @param insertLFs limit the number of characters per line.
 */
KCODECS_EXPORT void base64Encode(const QByteArray &in, QByteArray &out,
                                 bool insertLFs = false);

/**
 * Decodes the given data that was encoded using the
 * base64 algorithm.
 *
 * @param in   data to be decoded.
 * @return     decoded string.
 */
KCODECS_EXPORT QByteArray base64Decode(const QByteArray &in);

/**
 * Decodes the given data that was encoded with the base64
 * algorithm.
 *
 * Use this function if you want the result of the decoding
 * to be placed in another array which cuts down the number
 * of copy operation that have to be performed in the process.
 * This is also the preferred method for decoding an encoded
 * binary data.
 *
 * NOTE: the output array is first reset and then resized
 * appropriately before use, hence, all data stored in the
 * output array will be lost.
 *
 * @param in   data to be decoded.
 * @param out  decoded data.
 */
KCODECS_EXPORT void base64Decode(const QByteArray &in, QByteArray &out);

/**
 * Decodes string @p text according to RFC2047,
 * i.e., the construct =?charset?[qb]?encoded?=
 *
 * @param text source string
 * @returns the decoded string
 */
KCODECS_EXPORT QString decodeRFC2047String(const QString &text);

/**
 * Charset options for RFC2047 encoder
 * @since 5.5
 */
enum CharsetOption
{
    NoOption = 0,              /// No special option
    ForceDefaultCharset = 1    /// Force use of the default charset
};

/**
 * Decodes string @p src according to RFC2047, i.e. the construct
 *  =?charset?[qb]?encoded?=
 *
 * @param src       source string.
 * @param usedCS    the detected charset is returned here
 * @param defaultCS the charset to use in case the detected
 *                  one isn't known to us.
 * @param option    options for the encoder
 *
 * @return the decoded string.
 * @since 5.5
 */
KCODECS_EXPORT QString decodeRFC2047String(const QByteArray &src, QByteArray *usedCS,
                                           const QByteArray &defaultCS = QByteArray(),
                                           CharsetOption option = NoOption);

/**
 * Encodes string @p src according to RFC2047 using charset @p charset.
 *
 * This function also makes commas, quotes and other characters part of the encoded name, for example
 * the string "Jöhn Döe" <john@example.com"> would be encoded as <encoded word for "Jöhn Döe"> <john@example.com>,
 * i.e. the opening and closing quote mark would be part of the encoded word.
 * Therefore don't use this function for input strings that contain semantically meaningful characters,
 * like the quoting marks in this example.
 *
 * @param src           source string.
 * @param charset       charset to use. If it can't encode the string, UTF-8 will be used instead.
 * @return the encoded string.
 * @since 5.5
 */
KCODECS_EXPORT QByteArray encodeRFC2047String(const QString &src, const QByteArray &charset);


class Encoder;
class EncoderPrivate;
class Decoder;
class DecoderPrivate;

/**
  @glossary @anchor MIME @anchor mime @b MIME:
  <b>Multipurpose Internet Mail Extensions</b> or @acronym MIME is an
  Internet Standard that extends the format of e-mail to support text in
  character sets other than US-ASCII, non-text attachments, multi-part message
  bodies, and header information in non-ASCII character sets. Virtually all
  human-written Internet e-mail and a fairly large proportion of automated
  e-mail is transmitted via @acronym SMTP in MIME format. Internet e-mail is
  so closely associated with the SMTP and MIME standards that it is sometimes
  called SMTP/MIME e-mail. The content types defined by MIME standards are
  also of growing importance outside of e-mail, such as in communication
  protocols like @acronym HTTP for the World Wide Web. MIME is also a
  fundamental component of communication protocols such as  HTTP, which
  requires that data be transmitted in the context of e-mail-like messages,
  even though the data may not actually be e-mail.

  @glossary @anchor codec @anchor codecs @anchor Codec @anchor Codecs @b codec:
  a program capable of performing encoding and decoding on a digital data
  stream. Codecs encode data for storage or encryption and decode it for
  viewing or editing.

  @glossary @anchor CRLF @b CRLF: a "Carriage Return (0x0D)" followed by a
  "Line Feed (0x0A)", two ASCII control characters used to represent a
  newline on some operating systems, notably DOS and Microsoft Windows.

  @glossary @anchor LF @b LF: a "Line Feed (0x0A)" ASCII control character used
  to represent a newline on some operating systems, notably Unix, Unix-like,
  and Linux.

  @brief An abstract base class of @ref codecs for common mail transfer encodings.

  Provides an abstract base class of @ref codecs like base64 and quoted-printable.
  Implemented as a singleton.

  @authors Marc Mutz \<mutz@kde.org\>
  @since 5.5
*/
class KCODECS_EXPORT Codec
{
public:
    enum NewlineType {
        NewlineLF,
        NewlineCRLF
    };

    /**
      Returns a codec associated with the specified @p name.

      @param name points to a character string containing a valid codec name.
    */
    static Codec *codecForName(const char *name);

    /**
      Returns a codec associated with the specified @p name.

      @param name is a QByteArray containing a valid codec name.
    */
    static Codec *codecForName(const QByteArray &name);

    /**
      Computes the maximum size, in characters, needed for the encoding.

      @param insize is the number of input characters to be encoded.
      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).

      @return the maximum number of characters in the encoding.
    */
    virtual int maxEncodedSizeFor(int insize, NewlineType newline = NewlineLF) const = 0;

    /**
      Computes the maximum size, in characters, needed for the deccoding.

      @param insize is the number of input characters to be decoded.
      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).

      @return the maximum number of characters in the decoding.
    */
    virtual int maxDecodedSizeFor(int insize, NewlineType newline = NewlineLF) const = 0;

    /**
      Creates the encoder for the codec.

      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).

      @return a pointer to an instance of the codec's encoder.
    */
    virtual Encoder *makeEncoder(NewlineType newline = NewlineLF) const = 0;

    /**
      Creates the decoder for the codec.

      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).

      @return a pointer to an instance of the codec's decoder.
    */
    virtual Decoder *makeDecoder(NewlineType newline = NewlineLF) const = 0;

    /**
      Convenience wrapper that can be used for small chunks of data
      when you can provide a large enough buffer. The default
      implementation creates an Encoder and uses it.

      Encodes a chunk of bytes starting at @p scursor and extending to
      @p send into the buffer described by @p dcursor and @p dend.

      This function doesn't support chaining of blocks. The returned
      block cannot be added to, but you don't need to finalize it, too.

      Example usage (@p in contains the input data):
      <pre>
      KCodecs::Codec *codec = KCodecs::Codec::codecForName("base64");
      if (!codec) {
          qFatal() << "no base64 codec found!?";
      }
      QByteArray out(in.size() * 1.4); // crude maximal size of b64 encoding
      QByteArray::Iterator iit = in.begin();
      QByteArray::Iterator oit = out.begin();
      if (!codec->encode(iit, in.end(), oit, out.end())) {
          qDebug() << "output buffer too small";
          return;
      }
      qDebug() << "Size of encoded data:" << oit - out.begin();
      </pre>

      @param scursor is a pointer to the start of the input buffer.
      @param send is a pointer to the end of the input buffer.
      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).

      @return false if the encoded data didn't fit into the output buffer;
      true otherwise.
    */
    virtual bool encode(const char *&scursor, const char *const send,
                        char *&dcursor, const char *const dend,
                        NewlineType newline = NewlineLF) const;

    /**
      Convenience wrapper that can be used for small chunks of data
      when you can provide a large enough buffer. The default
      implementation creates a Decoder and uses it.

      Decodes a chunk of bytes starting at @p scursor and extending to
      @p send into the buffer described by @p dcursor and @p dend.

      This function doesn't support chaining of blocks. The returned
      block cannot be added to, but you don't need to finalize it, too.

      Example usage (@p in contains the input data):
      <pre>
      KCodecs::Codec *codec = KCodecs::Codec::codecForName("base64");
      if (!codec) {
          qFatal() << "no base64 codec found!?";
      }
      QByteArray out(in.size()); // good guess for any encoding...
      QByteArray::Iterator iit = in.begin();
      QByteArray::Iterator oit = out.begin();
      if (!codec->decode(iit, in.end(), oit, out.end())) {
          qDebug() << "output buffer too small";
          return;
      }
      qDebug() << "Size of decoded data:" << oit - out.begin();
      </pre>

      @param scursor is a pointer to the start of the input buffer.
      @param send is a pointer to the end of the input buffer.
      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).

      @return false if the decoded data didn't fit into the output buffer;
      true otherwise.
    */
    virtual bool decode(const char *&scursor, const char *const send,
                        char *&dcursor, const char *const dend,
                        NewlineType newline = NewlineLF) const;

    /**
      Even more convenient, but also a bit slower and more memory
      intensive, since it allocates storage for the worst case and then
      shrinks the result QByteArray to the actual size again.

      For use with small @p src.

      @param src is a QByteArray containing the data to encode.
      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).
    */
    virtual QByteArray encode(const QByteArray &src,
                              NewlineType newline = NewlineLF) const;

    /**
      Even more convenient, but also a bit slower and more memory
      intensive, since it allocates storage for the worst case and then
      shrinks the result QByteArray to the actual size again.

      For use with small @p src.

      @param src is a QByteArray containing the data to decode.
      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).
    */
    virtual QByteArray decode(const QByteArray &src,
                              NewlineType newline = NewlineLF) const;

    /**
      Returns the name of the encoding. Guaranteed to be lowercase.
    */
    virtual const char *name() const = 0;

    /**
      Destroys the codec.
    */
    virtual ~Codec() {}

protected:
    /**
      Constructs the codec.
    */
    Codec() {}
};

/**
  @brief Stateful CTE decoder class

  Stateful decoder class, modelled after QTextDecoder.

  @section Overview

  KCodecs decoders are designed to be able to process encoded data in
  chunks of arbitrary size and to work with output buffers of also
  arbitrary size. They maintain any state necessary to go on where
  the previous call left off.

  The class consists of only two methods of interest: see decode,
  which decodes an input block and finalize, which flushes any
  remaining data to the output stream.

  Typically, you will create a decoder instance, call decode as
  often as necessary, then call finalize (most often a single
  call suffices, but it might be that during that call the output
  buffer is filled, so you should be prepared to call finalize
  as often as necessary, ie. until it returns @p true).

  @section Return Values

  Both methods return @p true to indicate that they've finished their
  job. For decode, a return value of @p true means that the
  current input block has been finished (@p false most often means
  that the output buffer is full, but that isn't required
  behavior. The decode call is free to return at arbitrary
  times during processing).

  For finalize, a return value of @p true means that all data
  implicitly or explicitly stored in the decoder instance has been
  flushed to the output buffer. A @p false return value should be
  interpreted as "check if the output buffer is full and call me
  again", just as with decode.

  @section Usage Pattern

  Since the decoder maintains state, you can only use it once. After
  a sequence of input blocks has been processed, you finalize
  the output and then delete the decoder instance. If you want to
  process another input block sequence, you create a new instance.

  Typical usage (@p in contains the (base64-encoded) input data),
  taking into account all the conventions detailed above:

  <pre>
  KCodecs::Codec *codec = KCodecs::Codec::codecForName("base64");
  if (!codec) {
      qFatal() << "No codec found for base64!";
  }
  KCodecs::Decoder *dec = codec->makeDecoder();
  Q_ASSERT(dec); // should not happen
  QByteArray out(256); // small buffer is enough ;-)
  QByteArray::Iterator iit = in.begin();
  QByteArray::Iterator oit = out.begin();
  // decode the chunk
  while (!dec->decode(iit, in.end(), oit, out.end()))
    if (oit == out.end()) { // output buffer full, process contents
      do_something_with(out);
      oit = out.begin();
    }
  // repeat while loop for each input block
  // ...
  // finish (flush remaining data from decoder):
  while (!dec->finish(oit, out.end()))
    if (oit == out.end()) { // output buffer full, process contents
      do_something_with(out);
      oit = out.begin();
    }
  // now process last chunk:
  out.resize(oit - out.begin());
  do_something_with(out);
  // _delete_ the decoder, but not the codec:
  delete dec;
  </pre>

  @since 5.5
*/
class KCODECS_EXPORT Decoder
{
protected:
    friend class Codec;
    friend class DecoderPrivate;

    /**
      Protected constructor. Use KCodecs::Codec::makeDecoder to create an
      instance.

      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).
    */
    Decoder(Codec::NewlineType newline = Codec::NewlineLF);

public:
    /**
      Destroys the decoder.
    */
    virtual ~Decoder();

    /**
      Decodes a chunk of data, maintaining state information between
      calls. See class decumentation for calling conventions.

      @param scursor is a pointer to the start of the input buffer.
      @param send is a pointer to the end of the input buffer.
      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
    */
    virtual bool decode(const char *&scursor, const char *const send,
                        char *&dcursor, const char *const dend) = 0;

    /**
      Call this method to finalize the output stream. Writes all
      remaining data and resets the decoder. See KCodecs::Codec for
      calling conventions.

      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
    */
    virtual bool finish(char *&dcursor, const char *const dend) = 0;

protected:
    //@cond PRIVATE
    DecoderPrivate * const d;
    //@endcond
};

/**
  @brief Stateful encoder class.

  Stateful encoder class, modeled after QTextEncoder.

  @since 5.5
*/
class KCODECS_EXPORT Encoder
{

protected:
    friend class Codec;
    friend class EncoderPrivate;

    /**
      Protected constructor. Use KCodecs::Codec::makeEncoder if you want one.

      @param newline whether make new lines using @ref CRLF, or @ref LF (default is @ref LF).
    */
    explicit Encoder(Codec::NewlineType newline = Codec::NewlineLF);

public:
    /**
      Destroys the encoder.
    */
    virtual ~Encoder();

    /**
      Encodes a chunk of data, maintaining state information between
      calls. See KCodecs::Codec for calling conventions.

      @param scursor is a pointer to the start of the input buffer.
      @param send is a pointer to the end of the input buffer.
      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
    */
    virtual bool encode(const char *&scursor, const char *const send,
                        char *&dcursor, const char *const dend) = 0;

    /**
      Call this method to finalize the output stream. Writes all remaining
      data and resets the encoder. See KCodecs::Codec for calling conventions.

      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
    */
    virtual bool finish(char *&dcursor, const char *const dend) = 0;

protected:
    /**
      The maximum number of characters permitted in the output buffer.
    */
    enum {
        maxBufferedChars = 8  /**< Eight */
    };

    /**
      Writes character @p ch to the output stream or the output buffer,
      depending on whether or not the output stream has space left.

      @param ch is the character to write.
      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.

      @return true if written to the output stream; else false if buffered.
    */
    bool write(char ch, char *&dcursor, const char *const dend);

    /**
      Writes characters from the output buffer to the output stream.
      Implementations of encode and finish should call this
      at the very beginning and for each iteration of the while loop.

      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.

      @return true if all chars could be written, false otherwise
    */
    bool flushOutputBuffer(char *&dcursor, const char *const dend);

    /**
      Convenience function. Outputs @ref LF or @ref CRLF, based on the
      state of mWithCRLF.

      @param dcursor is a pointer to the start of the output buffer.
      @param dend is a pointer to the end of the output buffer.
    */
    bool writeCRLF(char *&dcursor, const char *const dend);

protected:
    //@cond PRIVATE
    EncoderPrivate * const d;
    //@endcond
};

} // namespace KCodecs

#endif // KCODECS_H
