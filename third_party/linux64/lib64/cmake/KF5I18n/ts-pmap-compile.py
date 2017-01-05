#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Script that compiles Transcript property maps from text to binary format.
# Binary format greately speeds up loading of property maps at runtime.
# http://techbase.kde.org/Localization/Concepts/Transcript
#
# Usage:
#   ts-pmap-compile.py file.pmap file.pmapc
#
# Works with Python >= 2.6 and >= 3.0.

import locale
import os
import re
import struct
import sys


cmdname = os.path.basename(sys.argv[0])
lenc = locale.getpreferredencoding()

def error (msg, code=1):
    sys.stderr.write(("%s: error: %s\n" % (cmdname, msg)).encode(lenc))
    sys.exit(code)


def count_lines (text, tolen):
    return text.count("\n", 0, tolen) + 1


def norm_keystr (text):
    # Must do the same as normKeystr() in kdelibs/kdecore/ktranscript.cpp
    return re.sub("[\s&]", "", text).lower()


def trim_smart (text):
    return re.sub("^\s*\n|\n\s*$", "", text)


def read_pmap (fname):

    # Adapted directly from C++ code.

    fh = open(fname, "rb")
    s = "".join([l.decode("utf8") for l in fh.readlines()])
    fh.close()

    s_nextEntry, s_nextKey, s_nextValue = 1, 2, 3

    pmap = []

    class END_PROP_PARSE (Exception): pass
    try:
        slen = len(s)
        state = s_nextEntry
        ekeys = [] # holds keys for current entry
        props = [] # holds properties for current entry
        pkey = "" # holds current property key
        i = 0
        while True:
            i_checkpoint = i

            if state == s_nextEntry:
                while s[i].isspace():
                    i += 1
                    if i >= slen: raise END_PROP_PARSE

                if i + 1 >= slen:
                    error("unexpected end of file %s" % fname)

                if s[i] != '#':
                    # Separator characters for this entry.
                    key_sep = s[i]
                    prop_sep = s[i + 1]
                    if key_sep.isalpha() or prop_sep.isalpha():
                        error("separator characters must not be letters "
                              "at %s:%d" % (fname, count_lines(s, i)))

                    # Reset all data for current entry.
                    ekeys = []
                    props = []
                    pkey = ""

                    i += 2
                    state = s_nextKey

                else:
                    # This is a comment, skip to EOL, don't change state.
                    while s[i] != '\n':
                        i += 1
                        if i >= slen: raise END_PROP_PARSE

            elif state == s_nextKey:
                ip = i
                # Proceed up to next key or property separator.
                while s[i] != key_sep and s[i] != prop_sep:
                    i += 1
                    if i >= slen: raise END_PROP_PARSE

                if s[i] == key_sep:
                    # This is a property key,
                    # record for when the value gets parsed.
                    pkey = norm_keystr(s[ip:i])

                    i += 1
                    state = s_nextValue

                else: # if (s[i] == prop_sep
                    # This is an entry key, or end of entry.
                    ekey = norm_keystr(s[ip:i])
                    if ekey:
                        # An entry key.
                        ekeys.append(ekey)

                        i += 1
                        state = s_nextKey

                    else:
                        # End of entry.
                        if len(ekeys) < 1:
                            error("no entry key for entry ending "
                                  "at %s:%d" % (fname, count_lines(s, i)))

                        # Put collected properties into global store.
                        pmap.append((ekeys, props))

                        i += 1
                        state = s_nextEntry
                        # This check covers no newline at end of file.
                        if i >= slen: raise END_PROP_PARSE

            elif state == s_nextValue:
                ip = i
                # Proceed up to next property separator.
                while s[i] != prop_sep:
                    i += 1
                    if i >= slen: raise END_PROP_PARSE
                    if s[i] == key_sep:
                        error("property separator inside property value "
                              "at %s:%d" % (fname, count_lines(s, i)))

                # Extract the property value and store the property.
                pval = trim_smart(s[ip:i])
                props.append((pkey, pval))

                i += 1
                state = s_nextKey

            else:
                error("internal error 10 "
                      "at %s:%d" % (fname, count_lines(s, i)))

            # To avoid infinite looping and stepping out.
            if i == i_checkpoint or i >= slen:
                error("internal error 20 "
                      "at %s:%d" % (fname, count_lines(s, i)))

    except END_PROP_PARSE:
        if state != s_nextEntry:
            error("unexpected end of file in %s" % fname)

    return pmap


# Convert integer to 32-bit big-endian byte sequence.
def int_bin_32 (val):
    return struct.pack(">i", val)[-4:]


# Convert integer to 64-bit big-endian byte sequence.
def int_bin_64 (val):
    return struct.pack(">q", val)[-8:]


# Convert string to UTF-8 byte sequence,
# preceded by its length in 32-bit big-endian.
def str_bin_32 (val):
    val_enc = val.encode("utf8")
    return int_bin_32(len(val_enc)) + val_enc


# Concatenate byte sequence.
def catb (seq):
    return bytes().join(seq)


# Binary map format 00.
def write_map_bin_00 (fh, pmap):

    # Magic bytes.
    fh.write("TSPMAP00".encode("ascii"))

    # Number of entries.
    fh.write(int_bin_32(len(pmap)))

    for ekeys, props in pmap:
        # Number of phrase keys and all phrase keys.
        fh.write(int_bin_32(len(ekeys)))
        for ekey in ekeys:
            fh.write(str_bin_32(ekey))

        # Number of properties and all properties.
        fh.write(int_bin_32(len(props)))
        for pkey, pval in props:
            fh.write(str_bin_32(pkey))
            fh.write(str_bin_32(pval))


# Binary map format 01.
def write_map_bin_01 (fh, pmap):

    offset0 = 0
    binint32len = len(int_bin_32(0))
    binint64len = len(int_bin_64(0))

    # Magic bytes.
    mbytestr = "TSPMAP01".encode("ascii")
    offset0 += len(mbytestr)

    # Compute length of binary representation of all entry keys
    # additionally equipped with offsets to corresponding property blobs.
    offset0 += binint32len
    offset0 += binint64len
    binekeyslen = 0
    for ekeys, d1 in pmap:
        binekeyslen += sum([len(str_bin_32(x)) + binint64len for x in ekeys])
    offset0 += binekeyslen

    # Construct binary representations of all unique property keys.
    offset0 += binint32len
    offset0 += binint64len
    allpkeys = set()
    for d1, props in pmap:
        allpkeys.update([x[0] for x in props])
    binpkeys = catb(map(str_bin_32, sorted(allpkeys)))
    offset0 += len(binpkeys)

    # Construct binary representations of properties for each entry.
    # Compute byte offsets for each of these binary blobs, in the given order.
    binprops = []
    plength = 0
    poffset = offset0 + binint32len
    for d1, props in pmap:
        cbinprops = catb(sum([list(map(str_bin_32, x)) for x in props], []))
        cbinprops = catb([int_bin_32(len(props)), int_bin_32(len(cbinprops)),
                          cbinprops])
        offset = poffset + plength
        binprops.append([cbinprops, offset])
        poffset = offset
        plength = len(cbinprops)

    # Construct binary representations of all entry keys with property offsets.
    allekeys = []
    binekeys = []
    for (ekeys, d1), (d2, offset) in zip(pmap, binprops):
        binoffset = int_bin_64(offset)
        cbinekeys = catb([str_bin_32(x) + binoffset for x in ekeys])
        binekeys.append(cbinekeys)
        allekeys.extend(ekeys)
    binekeys = catb(binekeys)
    assert(binekeyslen == len(binekeys))

    # Write everything out.
    fh.write(mbytestr)
    fh.write(int_bin_32(len(allekeys)))
    fh.write(int_bin_64(len(binekeys)))
    fh.write(binekeys)
    fh.write(int_bin_32(len(allpkeys)))
    fh.write(int_bin_64(len(binpkeys)))
    fh.write(binpkeys)
    fh.write(int_bin_32(len(pmap)))
    for cbinprops, d1 in binprops:
        fh.write(cbinprops)


def main ():

    if len(sys.argv) != 3:
        error("usage: %s INPUT_FILE OUTPUT_FILE" % cmdname)

    try:
        import psyco
        psyco.full()
    except ImportError:
        pass

    ifile = sys.argv[1]
    ofile = sys.argv[2]

    pmap = read_pmap(ifile)
    ofh = open(ofile, "wb")
    write_map_bin_01(ofh, pmap)
    ofh.close()


if __name__ == '__main__':
    main()
