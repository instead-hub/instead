/* guniprop.c - Unicode character properties.
 *
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/** This file contains only parts of the original souce file from the original
    Copyright holders in order to make the rest of this code to be functional
    without brining in the rest of GLib.
*/

#include "gunicode.h"
#include "gunichartables.h"
#include "gscripttable.h"

/* These two defintions do not belong to the original source file. */
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (!FALSE)
#endif

/* Count the number of elements in an array. The array must be defined
 * as such; using this with a dynamically allocated array will give
 * incorrect results.
 */
#define G_N_ELEMENTS(arr)   (sizeof (arr) / sizeof ((arr)[0]))

#define TTYPE_PART1(Page, Char) \
  ((type_table_part1[Page] >= G_UNICODE_MAX_TABLE_INDEX) \
   ? (type_table_part1[Page] - G_UNICODE_MAX_TABLE_INDEX) \
   : (type_data[type_table_part1[Page]][Char]))

#define TTYPE_PART2(Page, Char) \
  ((type_table_part2[Page] >= G_UNICODE_MAX_TABLE_INDEX) \
   ? (type_table_part2[Page] - G_UNICODE_MAX_TABLE_INDEX) \
   : (type_data[type_table_part2[Page]][Char]))

#define TYPE(Char) \
  (((Char) <= G_UNICODE_LAST_CHAR_PART1) \
   ? TTYPE_PART1 ((Char) >> 8, (Char) & 0xff) \
   : (((Char) >= 0xe0000 && (Char) <= G_UNICODE_LAST_CHAR) \
      ? TTYPE_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
      : G_UNICODE_UNASSIGNED))

#define IS(Type, Class) (((guint)1 << (Type)) & (Class))
#define OR(Type, Rest)  (((guint)1 << (Type)) | (Rest))

#define ISALPHA(Type) IS ((Type),       \
          OR (G_UNICODE_LOWERCASE_LETTER, \
          OR (G_UNICODE_UPPERCASE_LETTER, \
          OR (G_UNICODE_TITLECASE_LETTER, \
          OR (G_UNICODE_MODIFIER_LETTER,  \
          OR (G_UNICODE_OTHER_LETTER,   0))))))

#define ISALDIGIT(Type) IS ((Type),       \
          OR (G_UNICODE_DECIMAL_NUMBER, \
          OR (G_UNICODE_LETTER_NUMBER,  \
          OR (G_UNICODE_OTHER_NUMBER,   \
          OR (G_UNICODE_LOWERCASE_LETTER, \
          OR (G_UNICODE_UPPERCASE_LETTER, \
          OR (G_UNICODE_TITLECASE_LETTER, \
          OR (G_UNICODE_MODIFIER_LETTER,  \
          OR (G_UNICODE_OTHER_LETTER,   0)))))))))

#define ISMARK(Type)  IS ((Type),       \
          OR (G_UNICODE_NON_SPACING_MARK, \
          OR (G_UNICODE_SPACING_MARK, \
          OR (G_UNICODE_ENCLOSING_MARK, 0))))

#define ISZEROWIDTHTYPE(Type) IS ((Type),     \
          OR (G_UNICODE_NON_SPACING_MARK, \
          OR (G_UNICODE_ENCLOSING_MARK, \
          OR (G_UNICODE_FORMAT,   0))))

#define G_SCRIPT_TABLE_MIDPOINT (G_N_ELEMENTS (g_script_table) / 2)

static inline GUnicodeScript
g_unichar_get_script_bsearch (gunichar ch)
{
  int lower = 0;
  int upper = G_N_ELEMENTS (g_script_table) - 1;
  static int saved_mid = G_SCRIPT_TABLE_MIDPOINT;
  int mid = saved_mid;

  do 
    {
      if (ch < g_script_table[mid].start)
  upper = mid - 1;
      else if (ch >= g_script_table[mid].start + g_script_table[mid].chars)
  lower = mid + 1;
      else
        {
          return g_script_table[saved_mid = mid].script;
        }

      mid = (lower + upper) / 2;
    }
  while (lower <= upper);

  return G_UNICODE_SCRIPT_UNKNOWN;
}

/**
 * g_unichar_get_script:
 * @ch: a Unicode character
 * 
 * Looks up the #GUnicodeScript for a particular character (as defined 
 * by Unicode Standard Annex \#24). No check is made for @ch being a
 * valid Unicode character; if you pass in invalid character, the
 * result is undefined.
 *
 * This function is equivalent to pango_script_for_unichar() and the
 * two are interchangeable.
 * 
 * Returns: the #GUnicodeScript for the character.
 *
 * Since: 2.14
 */
GUnicodeScript
g_unichar_get_script (gunichar ch)
{
  if (ch < G_EASY_SCRIPTS_RANGE)
    return g_script_easy_table[ch];
  else 
    return g_unichar_get_script_bsearch (ch); 
}

/**
 * g_unichar_isalnum:
 * @c: a Unicode character
 * 
 * Determines whether a character is alphanumeric.
 * Given some UTF-8 text, obtain a character value
 * with g_utf8_get_char().
 * 
 * Returns: %TRUE if @c is an alphanumeric character
 **/
gboolean
g_unichar_isalnum (gunichar c)
{
  return ISALDIGIT (TYPE (c)) ? TRUE : FALSE;
}

/**
 * g_unichar_isalpha:
 * @c: a Unicode character
 * 
 * Determines whether a character is alphabetic (i.e. a letter).
 * Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 * 
 * Returns: %TRUE if @c is an alphabetic character
 **/
gboolean
g_unichar_isalpha (gunichar c)
{
  return ISALPHA (TYPE (c)) ? TRUE : FALSE;
}

/**
 * g_unichar_isdigit:
 * @c: a Unicode character
 * 
 * Determines whether a character is numeric (i.e. a digit).  This
 * covers ASCII 0-9 and also digits in other languages/scripts.  Given
 * some UTF-8 text, obtain a character value with g_utf8_get_char().
 * 
 * Returns: %TRUE if @c is a digit
 **/
gboolean
g_unichar_isdigit (gunichar c)
{
  return TYPE (c) == G_UNICODE_DECIMAL_NUMBER;
}