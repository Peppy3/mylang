/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf ../utils/keywords.gperf  */
/* Computed positions: -k'1-2' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 9 "../utils/keywords.gperf"


#include <stddef.h>
#include <string.h>
#include "Token.h"

#include "Keywords.h"

#line 19 "../utils/keywords.gperf"
struct KeywordResult;

#define TOTAL_KEYWORDS 43
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 73
/* maximum key range = 72, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
KeywordHash (register const char *str, register size_t len)
{
  static unsigned char asso_values[] =
    {
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 60,
      74, 45, 74, 74, 30, 74, 20, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74,  0, 25, 10,
      10, 15, 10, 74, 15,  5, 74, 74, 16,  5,
       0, 10, 15, 74,  0,  0, 20,  0, 30,  0,
      10, 10, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74
    };
  return len + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]];
}

struct KeywordResult *
KeywordLookup (register const char *str, register size_t len)
{
  static struct KeywordResult wordlist[] =
    {
      {(char*)0}, {(char*)0},
#line 44 "../utils/keywords.gperf"
      {"as",			TOKEN_as},
#line 46 "../utils/keywords.gperf"
      {"and",		TOKEN_bool_and},
#line 39 "../utils/keywords.gperf"
      {"null",		TOKEN_null},
#line 37 "../utils/keywords.gperf"
      {"union",		TOKEN_union},
#line 23 "../utils/keywords.gperf"
      {"switch",		TOKEN_switch},
      {(char*)0},
#line 59 "../utils/keywords.gperf"
      {"int",		TOKEN_int},
#line 60 "../utils/keywords.gperf"
      {"uint",		TOKEN_uint},
#line 31 "../utils/keywords.gperf"
      {"macro",		TOKEN_macro},
#line 34 "../utils/keywords.gperf"
      {"inline",		TOKEN_inline},
#line 47 "../utils/keywords.gperf"
      {"or",			TOKEN_bool_or},
#line 45 "../utils/keywords.gperf"
      {"not",		TOKEN_bool_not},
#line 24 "../utils/keywords.gperf"
      {"case",		TOKEN_case},
#line 41 "../utils/keywords.gperf"
      {"false",		TOKEN_false},
#line 43 "../utils/keywords.gperf"
      {"import",		TOKEN_import},
#line 21 "../utils/keywords.gperf"
      {"if",			TOKEN_if},
#line 33 "../utils/keywords.gperf"
      {"pub",		TOKEN_pub},
#line 38 "../utils/keywords.gperf"
      {"enum",		TOKEN_enum},
#line 26 "../utils/keywords.gperf"
      {"while",		TOKEN_while},
#line 35 "../utils/keywords.gperf"
      {"return",		TOKEN_return},
#line 52 "../utils/keywords.gperf"
      {"u8",			TOKEN_u8},
#line 27 "../utils/keywords.gperf"
      {"for",		TOKEN_for},
#line 40 "../utils/keywords.gperf"
      {"true",		TOKEN_true},
#line 30 "../utils/keywords.gperf"
      {"const",		TOKEN_const},
#line 36 "../utils/keywords.gperf"
      {"struct",		TOKEN_struct},
#line 48 "../utils/keywords.gperf"
      {"i8",			TOKEN_i8},
#line 28 "../utils/keywords.gperf"
      {"continue",	TOKEN_continue},
#line 42 "../utils/keywords.gperf"
      {"linkname",	TOKEN_linkname},
#line 29 "../utils/keywords.gperf"
      {"break",		TOKEN_break},
#line 32 "../utils/keywords.gperf"
      {"extern",		TOKEN_extern},
#line 25 "../utils/keywords.gperf"
      {"default",	TOKEN_default},
#line 55 "../utils/keywords.gperf"
      {"u64",		TOKEN_u64},
#line 63 "../utils/keywords.gperf"
      {"type",		TOKEN_type},
#line 22 "../utils/keywords.gperf"
      {"else",		TOKEN_else},
      {(char*)0}, {(char*)0},
#line 51 "../utils/keywords.gperf"
      {"i64",		TOKEN_i64},
#line 62 "../utils/keywords.gperf"
      {"bool",		TOKEN_bool},
      {(char*)0}, {(char*)0}, {(char*)0},
#line 58 "../utils/keywords.gperf"
      {"f64",		TOKEN_f64},
#line 61 "../utils/keywords.gperf"
      {"void",		TOKEN_void},
      {(char*)0}, {(char*)0}, {(char*)0},
#line 54 "../utils/keywords.gperf"
      {"u32",		TOKEN_u32},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 50 "../utils/keywords.gperf"
      {"i32",		TOKEN_i32},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 57 "../utils/keywords.gperf"
      {"f32",		TOKEN_f32},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 53 "../utils/keywords.gperf"
      {"u16",		TOKEN_u16},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 49 "../utils/keywords.gperf"
      {"i16",		TOKEN_i16},
      {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 56 "../utils/keywords.gperf"
      {"f16",		TOKEN_f16}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = KeywordHash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (s && *str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 64 "../utils/keywords.gperf"


