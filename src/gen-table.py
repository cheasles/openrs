#
# gen-table.py: Generates pre-computed value table for Whirlpool.
#
# Author(s): objectx
#
import sys, re

from optparse import OptionParser
from string import Template

options = None

def main ():
    global options

    def create_option_parser ():
        p = OptionParser ("Usage: %prog [-options] [<output>]")
        p.set_defaults (verbose = False)
        p.add_option ("-v", "--verbose", dest = "verbose", action = "store_true", help = "Be verbose.")
        return p

    p = create_option_parser ()
    (options, args) = p.parse_args ()
    output = sys.stdout
    if 0 < len (args):
        output = open (args [0], "w", newline = '\n')
    try:
        generate (output)
    except:
        return 1
    finally:
        if output and output is not sys.stdout:
            output.close ()
    return 0

def verbose (format, *args):
    if options.verbose:
        sys.stderr.write (format % args)
        sys.stderr.flush ()


def generate (output):
    S = generate_sbox ()
    C = [1, 1, 4, 1, 8, 5, 2, 9]

    def mod_8 (v):
        """Computes modulo with polynomial x^8 + x^4 + x^3 + x^2 + 1"""
        poly = 0x1D
        v &= 0xFFFF
        poly <<= 8
        mask = 0x8000

        for i in range (8):
            if v & mask:
                v = (v << 1) ^ poly
            else:
                v = (v << 1)
        return (v >> 8) & 0xFF

    def mul_gf2 (a, b):
        """Computes (a * b) mod (x^8 + x^4 + x^3 + x^2 + 1)"""
        if 16 <= b:
            raise ValueError ("Multiplier must be fit into 4bits.")
        result = 0
        for i in range (4):
            if b & (1 << i):
                result ^= a
            a <<= 1
        return mod_8 (result)

    def gen_table (circ):
        result = []
        for x in range (256):
            Sx = S [x]
            v = 0
            for k in range (8):
                v = (v << 8) | mul_gf2 (Sx, circ [k])
            result.append ("0x%016XuLL" % v)
        return result

    def gen_rc_table ():
        result = ["0x%016XuLL" % 0]
        for r in range (1,11):
            v = ((S [8 * (r - 1) + 0] << 56) |
                 (S [8 * (r - 1) + 1] << 48) |
                 (S [8 * (r - 1) + 2] << 40) |
                 (S [8 * (r - 1) + 3] << 32) |
                 (S [8 * (r - 1) + 4] << 24) |
                 (S [8 * (r - 1) + 5] << 16) |
                 (S [8 * (r - 1) + 6] <<  8) |
                 (S [8 * (r - 1) + 7] <<  0))
            result.append ("0x%016XuLL" % v)
        return result

    def gen_small_table ():
        return gen_table (C)

    def gen_full_table ():
        c = C [:]
        result = []
        for i in range (8):
            result.extend (gen_table (c))
            c.insert (0, c.pop ())
        return result

    tmpl = """/* --- DO NOT EDIT --- */
#pragma once
#include <stdint.h>
static const uint64_t\tRC_ [11] = {
${rc_table}
} ;
#if defined (${cpp_symbol}) && (${cpp_symbol} != 0)
static const uint64_t\tCIR_ [8 * 256] = {
${full_table}
} ;\t/* end of CIR_ [8 * 256] */
#else
static const uint64_t\tCIR_ [256] = {
${small_table}
} ;\t/* end of CIR_ [256] */
#endif\t/* ${cpp_symbol} */
"""
    output.write (Template (tmpl).substitute (cpp_symbol = "WHIRLPOOL_USE_FULL_TABLE",
                                              rc_table = fold (gen_rc_table ()),
                                              full_table = fold (gen_full_table ()),
                                              small_table = fold (gen_small_table ())))

def fold (values, prefix = "    ", columns = 4):
    tmp = []
    rows = []
    for v in values:
        tmp.append (v)
        if columns <= len (tmp):
            rows.append (prefix + ", ".join (tmp))
            tmp = []
    if 0 < len (tmp):
        rows.append (prefix + ", ".join (tmp))
    return ",\n".join (rows)

def generate_sbox ():
    def invert (arg):
        result = list ((0,) * 16)
        for i, x in enumerate (arg):
            result [x] = i
        return result

    E = [0x1, 0xB, 0x9, 0xC, 0xD, 0x6, 0xF, 0x3, 0xE, 0x8, 0x7, 0x4, 0xA, 0x2, 0x5, 0x0]
    Einv = invert (E)
    R = [0x7, 0xC, 0xB, 0xD, 0xE, 0x4, 0x9, 0xF, 0x6, 0x3, 0x8, 0xA, 0x2, 0x5, 0x1, 0x0]

    sbox = []

    for u in range (0, 16):
        for l in range (0, 16):
            vu = E [u]
            vl = Einv [l]
            r = R [vu ^ vl]
            vu = E [vu ^ r]
            vl = Einv [vl ^ r]
            sbox.append (vu << 4 | vl)
    return sbox

if __name__ == "__main__":
    sys.exit (main ())

