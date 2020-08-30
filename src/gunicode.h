/* gunicode.h - Unicode manipulation functions
 *
 *  Copyright (C) 1999, 2000 Tom Tromey
 *  Copyright 2000, 2005 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/** This file does not contain the full original work. It contains only the following
    defitions:
    - gunichar
    - GUnicodeType
    - GUnicodeScript
    - g_unichar_isalnum
    - g_unichar_isalpha
    - g_unichar_isdigit
    - g_unichar_get_script

    Following eight typedefs are added to make the rest of code to be functional
    without brining in the rest of GLib.

    This modification is not part of the original source file from the original
    Copyright holders.

*/
typedef char   gchar;
typedef int    gint;
typedef gint   gboolean;
typedef unsigned char   guchar;
typedef unsigned int    guint;
typedef signed short gint16;
typedef unsigned short guint16;
typedef unsigned int guint32;

/**
 * gunichar:
 *
 * A type which can hold any UTF-32 or UCS-4 character code,
 * also known as a Unicode code point.
 *
 * If you want to produce the UTF-8 representation of a #gunichar,
 * use g_ucs4_to_utf8(). See also g_utf8_to_ucs4() for the reverse
 * process.
 *
 * To print/scan values of this type as integer, use
 * %G_GINT32_MODIFIER and/or %G_GUINT32_FORMAT.
 *
 * The notation to express a Unicode code point in running text is
 * as a hexadecimal number with four to six digits and uppercase
 * letters, prefixed by the string "U+". Leading zeros are omitted,
 * unless the code point would have fewer than four hexadecimal digits.
 * For example, "U+0041 LATIN CAPITAL LETTER A". To print a code point
 * in the U+-notation, use the format string "U+\%04"G_GINT32_FORMAT"X".
 * To scan, use the format string "U+\%06"G_GINT32_FORMAT"X".
 *
 * |[
 * gunichar c;
 * sscanf ("U+0041", "U+%06"G_GINT32_FORMAT"X", &amp;c)
 * g_print ("Read U+%04"G_GINT32_FORMAT"X", c);
 * ]|
 */
typedef guint32 gunichar;

/**
 * GUnicodeType:
 * @G_UNICODE_CONTROL: General category "Other, Control" (Cc)
 * @G_UNICODE_FORMAT: General category "Other, Format" (Cf)
 * @G_UNICODE_UNASSIGNED: General category "Other, Not Assigned" (Cn)
 * @G_UNICODE_PRIVATE_USE: General category "Other, Private Use" (Co)
 * @G_UNICODE_SURROGATE: General category "Other, Surrogate" (Cs)
 * @G_UNICODE_LOWERCASE_LETTER: General category "Letter, Lowercase" (Ll)
 * @G_UNICODE_MODIFIER_LETTER: General category "Letter, Modifier" (Lm)
 * @G_UNICODE_OTHER_LETTER: General category "Letter, Other" (Lo)
 * @G_UNICODE_TITLECASE_LETTER: General category "Letter, Titlecase" (Lt)
 * @G_UNICODE_UPPERCASE_LETTER: General category "Letter, Uppercase" (Lu)
 * @G_UNICODE_SPACING_MARK: General category "Mark, Spacing" (Mc)
 * @G_UNICODE_ENCLOSING_MARK: General category "Mark, Enclosing" (Me)
 * @G_UNICODE_NON_SPACING_MARK: General category "Mark, Nonspacing" (Mn)
 * @G_UNICODE_DECIMAL_NUMBER: General category "Number, Decimal Digit" (Nd)
 * @G_UNICODE_LETTER_NUMBER: General category "Number, Letter" (Nl)
 * @G_UNICODE_OTHER_NUMBER: General category "Number, Other" (No)
 * @G_UNICODE_CONNECT_PUNCTUATION: General category "Punctuation, Connector" (Pc)
 * @G_UNICODE_DASH_PUNCTUATION: General category "Punctuation, Dash" (Pd)
 * @G_UNICODE_CLOSE_PUNCTUATION: General category "Punctuation, Close" (Pe)
 * @G_UNICODE_FINAL_PUNCTUATION: General category "Punctuation, Final quote" (Pf)
 * @G_UNICODE_INITIAL_PUNCTUATION: General category "Punctuation, Initial quote" (Pi)
 * @G_UNICODE_OTHER_PUNCTUATION: General category "Punctuation, Other" (Po)
 * @G_UNICODE_OPEN_PUNCTUATION: General category "Punctuation, Open" (Ps)
 * @G_UNICODE_CURRENCY_SYMBOL: General category "Symbol, Currency" (Sc)
 * @G_UNICODE_MODIFIER_SYMBOL: General category "Symbol, Modifier" (Sk)
 * @G_UNICODE_MATH_SYMBOL: General category "Symbol, Math" (Sm)
 * @G_UNICODE_OTHER_SYMBOL: General category "Symbol, Other" (So)
 * @G_UNICODE_LINE_SEPARATOR: General category "Separator, Line" (Zl)
 * @G_UNICODE_PARAGRAPH_SEPARATOR: General category "Separator, Paragraph" (Zp)
 * @G_UNICODE_SPACE_SEPARATOR: General category "Separator, Space" (Zs)
 *
 * These are the possible character classifications from the
 * Unicode specification.
 * See [Unicode Character Database](http://www.unicode.org/reports/tr44/#General_Category_Values).
 */
typedef enum
{
  G_UNICODE_CONTROL,
  G_UNICODE_FORMAT,
  G_UNICODE_UNASSIGNED,
  G_UNICODE_PRIVATE_USE,
  G_UNICODE_SURROGATE,
  G_UNICODE_LOWERCASE_LETTER,
  G_UNICODE_MODIFIER_LETTER,
  G_UNICODE_OTHER_LETTER,
  G_UNICODE_TITLECASE_LETTER,
  G_UNICODE_UPPERCASE_LETTER,
  G_UNICODE_SPACING_MARK,
  G_UNICODE_ENCLOSING_MARK,
  G_UNICODE_NON_SPACING_MARK,
  G_UNICODE_DECIMAL_NUMBER,
  G_UNICODE_LETTER_NUMBER,
  G_UNICODE_OTHER_NUMBER,
  G_UNICODE_CONNECT_PUNCTUATION,
  G_UNICODE_DASH_PUNCTUATION,
  G_UNICODE_CLOSE_PUNCTUATION,
  G_UNICODE_FINAL_PUNCTUATION,
  G_UNICODE_INITIAL_PUNCTUATION,
  G_UNICODE_OTHER_PUNCTUATION,
  G_UNICODE_OPEN_PUNCTUATION,
  G_UNICODE_CURRENCY_SYMBOL,
  G_UNICODE_MODIFIER_SYMBOL,
  G_UNICODE_MATH_SYMBOL,
  G_UNICODE_OTHER_SYMBOL,
  G_UNICODE_LINE_SEPARATOR,
  G_UNICODE_PARAGRAPH_SEPARATOR,
  G_UNICODE_SPACE_SEPARATOR
} GUnicodeType;


/**
 * GUnicodeScript:
 * @G_UNICODE_SCRIPT_INVALID_CODE:
 *                               a value never returned from g_unichar_get_script()
 * @G_UNICODE_SCRIPT_COMMON:     a character used by multiple different scripts
 * @G_UNICODE_SCRIPT_INHERITED:  a mark glyph that takes its script from the
 *                               base glyph to which it is attached
 * @G_UNICODE_SCRIPT_ARABIC:     Arabic
 * @G_UNICODE_SCRIPT_ARMENIAN:   Armenian
 * @G_UNICODE_SCRIPT_BENGALI:    Bengali
 * @G_UNICODE_SCRIPT_BOPOMOFO:   Bopomofo
 * @G_UNICODE_SCRIPT_CHEROKEE:   Cherokee
 * @G_UNICODE_SCRIPT_COPTIC:     Coptic
 * @G_UNICODE_SCRIPT_CYRILLIC:   Cyrillic
 * @G_UNICODE_SCRIPT_DESERET:    Deseret
 * @G_UNICODE_SCRIPT_DEVANAGARI: Devanagari
 * @G_UNICODE_SCRIPT_ETHIOPIC:   Ethiopic
 * @G_UNICODE_SCRIPT_GEORGIAN:   Georgian
 * @G_UNICODE_SCRIPT_GOTHIC:     Gothic
 * @G_UNICODE_SCRIPT_GREEK:      Greek
 * @G_UNICODE_SCRIPT_GUJARATI:   Gujarati
 * @G_UNICODE_SCRIPT_GURMUKHI:   Gurmukhi
 * @G_UNICODE_SCRIPT_HAN:        Han
 * @G_UNICODE_SCRIPT_HANGUL:     Hangul
 * @G_UNICODE_SCRIPT_HEBREW:     Hebrew
 * @G_UNICODE_SCRIPT_HIRAGANA:   Hiragana
 * @G_UNICODE_SCRIPT_KANNADA:    Kannada
 * @G_UNICODE_SCRIPT_KATAKANA:   Katakana
 * @G_UNICODE_SCRIPT_KHMER:      Khmer
 * @G_UNICODE_SCRIPT_LAO:        Lao
 * @G_UNICODE_SCRIPT_LATIN:      Latin
 * @G_UNICODE_SCRIPT_MALAYALAM:  Malayalam
 * @G_UNICODE_SCRIPT_MONGOLIAN:  Mongolian
 * @G_UNICODE_SCRIPT_MYANMAR:    Myanmar
 * @G_UNICODE_SCRIPT_OGHAM:      Ogham
 * @G_UNICODE_SCRIPT_OLD_ITALIC: Old Italic
 * @G_UNICODE_SCRIPT_ORIYA:      Oriya
 * @G_UNICODE_SCRIPT_RUNIC:      Runic
 * @G_UNICODE_SCRIPT_SINHALA:    Sinhala
 * @G_UNICODE_SCRIPT_SYRIAC:     Syriac
 * @G_UNICODE_SCRIPT_TAMIL:      Tamil
 * @G_UNICODE_SCRIPT_TELUGU:     Telugu
 * @G_UNICODE_SCRIPT_THAANA:     Thaana
 * @G_UNICODE_SCRIPT_THAI:       Thai
 * @G_UNICODE_SCRIPT_TIBETAN:    Tibetan
 * @G_UNICODE_SCRIPT_CANADIAN_ABORIGINAL:
 *                               Canadian Aboriginal
 * @G_UNICODE_SCRIPT_YI:         Yi
 * @G_UNICODE_SCRIPT_TAGALOG:    Tagalog
 * @G_UNICODE_SCRIPT_HANUNOO:    Hanunoo
 * @G_UNICODE_SCRIPT_BUHID:      Buhid
 * @G_UNICODE_SCRIPT_TAGBANWA:   Tagbanwa
 * @G_UNICODE_SCRIPT_BRAILLE:    Braille
 * @G_UNICODE_SCRIPT_CYPRIOT:    Cypriot
 * @G_UNICODE_SCRIPT_LIMBU:      Limbu
 * @G_UNICODE_SCRIPT_OSMANYA:    Osmanya
 * @G_UNICODE_SCRIPT_SHAVIAN:    Shavian
 * @G_UNICODE_SCRIPT_LINEAR_B:   Linear B
 * @G_UNICODE_SCRIPT_TAI_LE:     Tai Le
 * @G_UNICODE_SCRIPT_UGARITIC:   Ugaritic
 * @G_UNICODE_SCRIPT_NEW_TAI_LUE:
 *                               New Tai Lue
 * @G_UNICODE_SCRIPT_BUGINESE:   Buginese
 * @G_UNICODE_SCRIPT_GLAGOLITIC: Glagolitic
 * @G_UNICODE_SCRIPT_TIFINAGH:   Tifinagh
 * @G_UNICODE_SCRIPT_SYLOTI_NAGRI:
 *                               Syloti Nagri
 * @G_UNICODE_SCRIPT_OLD_PERSIAN:
 *                               Old Persian
 * @G_UNICODE_SCRIPT_KHAROSHTHI: Kharoshthi
 * @G_UNICODE_SCRIPT_UNKNOWN:    an unassigned code point
 * @G_UNICODE_SCRIPT_BALINESE:   Balinese
 * @G_UNICODE_SCRIPT_CUNEIFORM:  Cuneiform
 * @G_UNICODE_SCRIPT_PHOENICIAN: Phoenician
 * @G_UNICODE_SCRIPT_PHAGS_PA:   Phags-pa
 * @G_UNICODE_SCRIPT_NKO:        N'Ko
 * @G_UNICODE_SCRIPT_KAYAH_LI:   Kayah Li. Since 2.16.3
 * @G_UNICODE_SCRIPT_LEPCHA:     Lepcha. Since 2.16.3
 * @G_UNICODE_SCRIPT_REJANG:     Rejang. Since 2.16.3
 * @G_UNICODE_SCRIPT_SUNDANESE:  Sundanese. Since 2.16.3
 * @G_UNICODE_SCRIPT_SAURASHTRA: Saurashtra. Since 2.16.3
 * @G_UNICODE_SCRIPT_CHAM:       Cham. Since 2.16.3
 * @G_UNICODE_SCRIPT_OL_CHIKI:   Ol Chiki. Since 2.16.3
 * @G_UNICODE_SCRIPT_VAI:        Vai. Since 2.16.3
 * @G_UNICODE_SCRIPT_CARIAN:     Carian. Since 2.16.3
 * @G_UNICODE_SCRIPT_LYCIAN:     Lycian. Since 2.16.3
 * @G_UNICODE_SCRIPT_LYDIAN:     Lydian. Since 2.16.3
 * @G_UNICODE_SCRIPT_AVESTAN:    Avestan. Since 2.26
 * @G_UNICODE_SCRIPT_BAMUM:      Bamum. Since 2.26
 * @G_UNICODE_SCRIPT_EGYPTIAN_HIEROGLYPHS:
 *                               Egyptian Hieroglpyhs. Since 2.26
 * @G_UNICODE_SCRIPT_IMPERIAL_ARAMAIC:
 *                               Imperial Aramaic. Since 2.26
 * @G_UNICODE_SCRIPT_INSCRIPTIONAL_PAHLAVI:
 *                               Inscriptional Pahlavi. Since 2.26
 * @G_UNICODE_SCRIPT_INSCRIPTIONAL_PARTHIAN:
 *                               Inscriptional Parthian. Since 2.26
 * @G_UNICODE_SCRIPT_JAVANESE:   Javanese. Since 2.26
 * @G_UNICODE_SCRIPT_KAITHI:     Kaithi. Since 2.26
 * @G_UNICODE_SCRIPT_LISU:       Lisu. Since 2.26
 * @G_UNICODE_SCRIPT_MEETEI_MAYEK:
 *                               Meetei Mayek. Since 2.26
 * @G_UNICODE_SCRIPT_OLD_SOUTH_ARABIAN:
 *                               Old South Arabian. Since 2.26
 * @G_UNICODE_SCRIPT_OLD_TURKIC: Old Turkic. Since 2.28
 * @G_UNICODE_SCRIPT_SAMARITAN:  Samaritan. Since 2.26
 * @G_UNICODE_SCRIPT_TAI_THAM:   Tai Tham. Since 2.26
 * @G_UNICODE_SCRIPT_TAI_VIET:   Tai Viet. Since 2.26
 * @G_UNICODE_SCRIPT_BATAK:      Batak. Since 2.28
 * @G_UNICODE_SCRIPT_BRAHMI:     Brahmi. Since 2.28
 * @G_UNICODE_SCRIPT_MANDAIC:    Mandaic. Since 2.28
 * @G_UNICODE_SCRIPT_CHAKMA:               Chakma. Since: 2.32
 * @G_UNICODE_SCRIPT_MEROITIC_CURSIVE:     Meroitic Cursive. Since: 2.32
 * @G_UNICODE_SCRIPT_MEROITIC_HIEROGLYPHS: Meroitic Hieroglyphs. Since: 2.32
 * @G_UNICODE_SCRIPT_MIAO:                 Miao. Since: 2.32
 * @G_UNICODE_SCRIPT_SHARADA:              Sharada. Since: 2.32
 * @G_UNICODE_SCRIPT_SORA_SOMPENG:         Sora Sompeng. Since: 2.32
 * @G_UNICODE_SCRIPT_TAKRI:                Takri. Since: 2.32
 * @G_UNICODE_SCRIPT_BASSA_VAH:            Bassa. Since: 2.42
 * @G_UNICODE_SCRIPT_CAUCASIAN_ALBANIAN:   Caucasian Albanian. Since: 2.42
 * @G_UNICODE_SCRIPT_DUPLOYAN:             Duployan. Since: 2.42
 * @G_UNICODE_SCRIPT_ELBASAN:              Elbasan. Since: 2.42
 * @G_UNICODE_SCRIPT_GRANTHA:              Grantha. Since: 2.42
 * @G_UNICODE_SCRIPT_KHOJKI:               Kjohki. Since: 2.42
 * @G_UNICODE_SCRIPT_KHUDAWADI:            Khudawadi, Sindhi. Since: 2.42
 * @G_UNICODE_SCRIPT_LINEAR_A:             Linear A. Since: 2.42
 * @G_UNICODE_SCRIPT_MAHAJANI:             Mahajani. Since: 2.42
 * @G_UNICODE_SCRIPT_MANICHAEAN:           Manichaean. Since: 2.42
 * @G_UNICODE_SCRIPT_MENDE_KIKAKUI:        Mende Kikakui. Since: 2.42
 * @G_UNICODE_SCRIPT_MODI:                 Modi. Since: 2.42
 * @G_UNICODE_SCRIPT_MRO:                  Mro. Since: 2.42
 * @G_UNICODE_SCRIPT_NABATAEAN:            Nabataean. Since: 2.42
 * @G_UNICODE_SCRIPT_OLD_NORTH_ARABIAN:    Old North Arabian. Since: 2.42
 * @G_UNICODE_SCRIPT_OLD_PERMIC:           Old Permic. Since: 2.42
 * @G_UNICODE_SCRIPT_PAHAWH_HMONG:         Pahawh Hmong. Since: 2.42
 * @G_UNICODE_SCRIPT_PALMYRENE:            Palmyrene. Since: 2.42
 * @G_UNICODE_SCRIPT_PAU_CIN_HAU:          Pau Cin Hau. Since: 2.42
 * @G_UNICODE_SCRIPT_PSALTER_PAHLAVI:      Psalter Pahlavi. Since: 2.42
 * @G_UNICODE_SCRIPT_SIDDHAM:              Siddham. Since: 2.42
 * @G_UNICODE_SCRIPT_TIRHUTA:              Tirhuta. Since: 2.42
 * @G_UNICODE_SCRIPT_WARANG_CITI:          Warang Citi. Since: 2.42
 * @G_UNICODE_SCRIPT_AHOM:                 Ahom. Since: 2.48
 * @G_UNICODE_SCRIPT_ANATOLIAN_HIEROGLYPHS: Anatolian Hieroglyphs. Since: 2.48
 * @G_UNICODE_SCRIPT_HATRAN:               Hatran. Since: 2.48
 * @G_UNICODE_SCRIPT_MULTANI:              Multani. Since: 2.48
 * @G_UNICODE_SCRIPT_OLD_HUNGARIAN:        Old Hungarian. Since: 2.48
 * @G_UNICODE_SCRIPT_SIGNWRITING:          Signwriting. Since: 2.48
 * @G_UNICODE_SCRIPT_ADLAM:                Adlam. Since: 2.50
 * @G_UNICODE_SCRIPT_BHAIKSUKI:            Bhaiksuki. Since: 2.50
 * @G_UNICODE_SCRIPT_MARCHEN:              Marchen. Since: 2.50
 * @G_UNICODE_SCRIPT_NEWA:                 Newa. Since: 2.50
 * @G_UNICODE_SCRIPT_OSAGE:                Osage. Since: 2.50
 * @G_UNICODE_SCRIPT_TANGUT:               Tangut. Since: 2.50
 * @G_UNICODE_SCRIPT_MASARAM_GONDI:        Masaram Gondi. Since: 2.54
 * @G_UNICODE_SCRIPT_NUSHU:                Nushu. Since: 2.54
 * @G_UNICODE_SCRIPT_SOYOMBO:              Soyombo. Since: 2.54
 * @G_UNICODE_SCRIPT_ZANABAZAR_SQUARE:     Zanabazar Square. Since: 2.54
 * @G_UNICODE_SCRIPT_DOGRA:                Dogra. Since: 2.58
 * @G_UNICODE_SCRIPT_GUNJALA_GONDI:        Gunjala Gondi. Since: 2.58
 * @G_UNICODE_SCRIPT_HANIFI_ROHINGYA:      Hanifi Rohingya. Since: 2.58
 * @G_UNICODE_SCRIPT_MAKASAR:              Makasar. Since: 2.58
 * @G_UNICODE_SCRIPT_MEDEFAIDRIN:          Medefaidrin. Since: 2.58
 * @G_UNICODE_SCRIPT_OLD_SOGDIAN:          Old Sogdian. Since: 2.58
 * @G_UNICODE_SCRIPT_SOGDIAN:              Sogdian. Since: 2.58
 * @G_UNICODE_SCRIPT_ELYMAIC:              Elym. Since: 2.62
 * @G_UNICODE_SCRIPT_NANDINAGARI:          Nand. Since: 2.62
 * @G_UNICODE_SCRIPT_NYIAKENG_PUACHUE_HMONG: Rohg. Since: 2.62
 * @G_UNICODE_SCRIPT_WANCHO:               Wcho. Since: 2.62
 * @G_UNICODE_SCRIPT_CHORASMIAN:           Chorasmian. Since: 2.66
 * @G_UNICODE_SCRIPT_DIVES_AKURU:          Dives Akuru. Since: 2.66
 * @G_UNICODE_SCRIPT_KHITAN_SMALL_SCRIPT:  Khitan small script. Since: 2.66
 * @G_UNICODE_SCRIPT_YEZIDI:               Yezidi. Since: 2.66
 *
 * The #GUnicodeScript enumeration identifies different writing
 * systems. The values correspond to the names as defined in the
 * Unicode standard. The enumeration has been added in GLib 2.14,
 * and is interchangeable with #PangoScript.
 *
 * Note that new types may be added in the future. Applications
 * should be ready to handle unknown values.
 * See [Unicode Standard Annex #24: Script names](http://www.unicode.org/reports/tr24/).
 */
typedef enum
{                         /* ISO 15924 code */
  G_UNICODE_SCRIPT_INVALID_CODE = -1,
  G_UNICODE_SCRIPT_COMMON       = 0,   /* Zyyy */
  G_UNICODE_SCRIPT_INHERITED,          /* Zinh (Qaai) */
  G_UNICODE_SCRIPT_ARABIC,             /* Arab */
  G_UNICODE_SCRIPT_ARMENIAN,           /* Armn */
  G_UNICODE_SCRIPT_BENGALI,            /* Beng */
  G_UNICODE_SCRIPT_BOPOMOFO,           /* Bopo */
  G_UNICODE_SCRIPT_CHEROKEE,           /* Cher */
  G_UNICODE_SCRIPT_COPTIC,             /* Copt (Qaac) */
  G_UNICODE_SCRIPT_CYRILLIC,           /* Cyrl (Cyrs) */
  G_UNICODE_SCRIPT_DESERET,            /* Dsrt */
  G_UNICODE_SCRIPT_DEVANAGARI,         /* Deva */
  G_UNICODE_SCRIPT_ETHIOPIC,           /* Ethi */
  G_UNICODE_SCRIPT_GEORGIAN,           /* Geor (Geon, Geoa) */
  G_UNICODE_SCRIPT_GOTHIC,             /* Goth */
  G_UNICODE_SCRIPT_GREEK,              /* Grek */
  G_UNICODE_SCRIPT_GUJARATI,           /* Gujr */
  G_UNICODE_SCRIPT_GURMUKHI,           /* Guru */
  G_UNICODE_SCRIPT_HAN,                /* Hani */
  G_UNICODE_SCRIPT_HANGUL,             /* Hang */
  G_UNICODE_SCRIPT_HEBREW,             /* Hebr */
  G_UNICODE_SCRIPT_HIRAGANA,           /* Hira */
  G_UNICODE_SCRIPT_KANNADA,            /* Knda */
  G_UNICODE_SCRIPT_KATAKANA,           /* Kana */
  G_UNICODE_SCRIPT_KHMER,              /* Khmr */
  G_UNICODE_SCRIPT_LAO,                /* Laoo */
  G_UNICODE_SCRIPT_LATIN,              /* Latn (Latf, Latg) */
  G_UNICODE_SCRIPT_MALAYALAM,          /* Mlym */
  G_UNICODE_SCRIPT_MONGOLIAN,          /* Mong */
  G_UNICODE_SCRIPT_MYANMAR,            /* Mymr */
  G_UNICODE_SCRIPT_OGHAM,              /* Ogam */
  G_UNICODE_SCRIPT_OLD_ITALIC,         /* Ital */
  G_UNICODE_SCRIPT_ORIYA,              /* Orya */
  G_UNICODE_SCRIPT_RUNIC,              /* Runr */
  G_UNICODE_SCRIPT_SINHALA,            /* Sinh */
  G_UNICODE_SCRIPT_SYRIAC,             /* Syrc (Syrj, Syrn, Syre) */
  G_UNICODE_SCRIPT_TAMIL,              /* Taml */
  G_UNICODE_SCRIPT_TELUGU,             /* Telu */
  G_UNICODE_SCRIPT_THAANA,             /* Thaa */
  G_UNICODE_SCRIPT_THAI,               /* Thai */
  G_UNICODE_SCRIPT_TIBETAN,            /* Tibt */
  G_UNICODE_SCRIPT_CANADIAN_ABORIGINAL, /* Cans */
  G_UNICODE_SCRIPT_YI,                 /* Yiii */
  G_UNICODE_SCRIPT_TAGALOG,            /* Tglg */
  G_UNICODE_SCRIPT_HANUNOO,            /* Hano */
  G_UNICODE_SCRIPT_BUHID,              /* Buhd */
  G_UNICODE_SCRIPT_TAGBANWA,           /* Tagb */

  /* Unicode-4.0 additions */
  G_UNICODE_SCRIPT_BRAILLE,            /* Brai */
  G_UNICODE_SCRIPT_CYPRIOT,            /* Cprt */
  G_UNICODE_SCRIPT_LIMBU,              /* Limb */
  G_UNICODE_SCRIPT_OSMANYA,            /* Osma */
  G_UNICODE_SCRIPT_SHAVIAN,            /* Shaw */
  G_UNICODE_SCRIPT_LINEAR_B,           /* Linb */
  G_UNICODE_SCRIPT_TAI_LE,             /* Tale */
  G_UNICODE_SCRIPT_UGARITIC,           /* Ugar */

  /* Unicode-4.1 additions */
  G_UNICODE_SCRIPT_NEW_TAI_LUE,        /* Talu */
  G_UNICODE_SCRIPT_BUGINESE,           /* Bugi */
  G_UNICODE_SCRIPT_GLAGOLITIC,         /* Glag */
  G_UNICODE_SCRIPT_TIFINAGH,           /* Tfng */
  G_UNICODE_SCRIPT_SYLOTI_NAGRI,       /* Sylo */
  G_UNICODE_SCRIPT_OLD_PERSIAN,        /* Xpeo */
  G_UNICODE_SCRIPT_KHAROSHTHI,         /* Khar */

  /* Unicode-5.0 additions */
  G_UNICODE_SCRIPT_UNKNOWN,            /* Zzzz */
  G_UNICODE_SCRIPT_BALINESE,           /* Bali */
  G_UNICODE_SCRIPT_CUNEIFORM,          /* Xsux */
  G_UNICODE_SCRIPT_PHOENICIAN,         /* Phnx */
  G_UNICODE_SCRIPT_PHAGS_PA,           /* Phag */
  G_UNICODE_SCRIPT_NKO,                /* Nkoo */

  /* Unicode-5.1 additions */
  G_UNICODE_SCRIPT_KAYAH_LI,           /* Kali */
  G_UNICODE_SCRIPT_LEPCHA,             /* Lepc */
  G_UNICODE_SCRIPT_REJANG,             /* Rjng */
  G_UNICODE_SCRIPT_SUNDANESE,          /* Sund */
  G_UNICODE_SCRIPT_SAURASHTRA,         /* Saur */
  G_UNICODE_SCRIPT_CHAM,               /* Cham */
  G_UNICODE_SCRIPT_OL_CHIKI,           /* Olck */
  G_UNICODE_SCRIPT_VAI,                /* Vaii */
  G_UNICODE_SCRIPT_CARIAN,             /* Cari */
  G_UNICODE_SCRIPT_LYCIAN,             /* Lyci */
  G_UNICODE_SCRIPT_LYDIAN,             /* Lydi */

  /* Unicode-5.2 additions */
  G_UNICODE_SCRIPT_AVESTAN,                /* Avst */
  G_UNICODE_SCRIPT_BAMUM,                  /* Bamu */
  G_UNICODE_SCRIPT_EGYPTIAN_HIEROGLYPHS,   /* Egyp */
  G_UNICODE_SCRIPT_IMPERIAL_ARAMAIC,       /* Armi */
  G_UNICODE_SCRIPT_INSCRIPTIONAL_PAHLAVI,  /* Phli */
  G_UNICODE_SCRIPT_INSCRIPTIONAL_PARTHIAN, /* Prti */
  G_UNICODE_SCRIPT_JAVANESE,               /* Java */
  G_UNICODE_SCRIPT_KAITHI,                 /* Kthi */
  G_UNICODE_SCRIPT_LISU,                   /* Lisu */
  G_UNICODE_SCRIPT_MEETEI_MAYEK,           /* Mtei */
  G_UNICODE_SCRIPT_OLD_SOUTH_ARABIAN,      /* Sarb */
  G_UNICODE_SCRIPT_OLD_TURKIC,             /* Orkh */
  G_UNICODE_SCRIPT_SAMARITAN,              /* Samr */
  G_UNICODE_SCRIPT_TAI_THAM,               /* Lana */
  G_UNICODE_SCRIPT_TAI_VIET,               /* Tavt */

  /* Unicode-6.0 additions */
  G_UNICODE_SCRIPT_BATAK,                  /* Batk */
  G_UNICODE_SCRIPT_BRAHMI,                 /* Brah */
  G_UNICODE_SCRIPT_MANDAIC,                /* Mand */

  /* Unicode-6.1 additions */
  G_UNICODE_SCRIPT_CHAKMA,                 /* Cakm */
  G_UNICODE_SCRIPT_MEROITIC_CURSIVE,       /* Merc */
  G_UNICODE_SCRIPT_MEROITIC_HIEROGLYPHS,   /* Mero */
  G_UNICODE_SCRIPT_MIAO,                   /* Plrd */
  G_UNICODE_SCRIPT_SHARADA,                /* Shrd */
  G_UNICODE_SCRIPT_SORA_SOMPENG,           /* Sora */
  G_UNICODE_SCRIPT_TAKRI,                  /* Takr */

  /* Unicode 7.0 additions */
  G_UNICODE_SCRIPT_BASSA_VAH,              /* Bass */
  G_UNICODE_SCRIPT_CAUCASIAN_ALBANIAN,     /* Aghb */
  G_UNICODE_SCRIPT_DUPLOYAN,               /* Dupl */
  G_UNICODE_SCRIPT_ELBASAN,                /* Elba */
  G_UNICODE_SCRIPT_GRANTHA,                /* Gran */
  G_UNICODE_SCRIPT_KHOJKI,                 /* Khoj */
  G_UNICODE_SCRIPT_KHUDAWADI,              /* Sind */
  G_UNICODE_SCRIPT_LINEAR_A,               /* Lina */
  G_UNICODE_SCRIPT_MAHAJANI,               /* Mahj */
  G_UNICODE_SCRIPT_MANICHAEAN,             /* Mani */
  G_UNICODE_SCRIPT_MENDE_KIKAKUI,          /* Mend */
  G_UNICODE_SCRIPT_MODI,                   /* Modi */
  G_UNICODE_SCRIPT_MRO,                    /* Mroo */
  G_UNICODE_SCRIPT_NABATAEAN,              /* Nbat */
  G_UNICODE_SCRIPT_OLD_NORTH_ARABIAN,      /* Narb */
  G_UNICODE_SCRIPT_OLD_PERMIC,             /* Perm */
  G_UNICODE_SCRIPT_PAHAWH_HMONG,           /* Hmng */
  G_UNICODE_SCRIPT_PALMYRENE,              /* Palm */
  G_UNICODE_SCRIPT_PAU_CIN_HAU,            /* Pauc */
  G_UNICODE_SCRIPT_PSALTER_PAHLAVI,        /* Phlp */
  G_UNICODE_SCRIPT_SIDDHAM,                /* Sidd */
  G_UNICODE_SCRIPT_TIRHUTA,                /* Tirh */
  G_UNICODE_SCRIPT_WARANG_CITI,            /* Wara */

  /* Unicode 8.0 additions */
  G_UNICODE_SCRIPT_AHOM,                   /* Ahom */
  G_UNICODE_SCRIPT_ANATOLIAN_HIEROGLYPHS,  /* Hluw */
  G_UNICODE_SCRIPT_HATRAN,                 /* Hatr */
  G_UNICODE_SCRIPT_MULTANI,                /* Mult */
  G_UNICODE_SCRIPT_OLD_HUNGARIAN,          /* Hung */
  G_UNICODE_SCRIPT_SIGNWRITING,            /* Sgnw */

  /* Unicode 9.0 additions */
  G_UNICODE_SCRIPT_ADLAM,                  /* Adlm */
  G_UNICODE_SCRIPT_BHAIKSUKI,              /* Bhks */
  G_UNICODE_SCRIPT_MARCHEN,                /* Marc */
  G_UNICODE_SCRIPT_NEWA,                   /* Newa */
  G_UNICODE_SCRIPT_OSAGE,                  /* Osge */
  G_UNICODE_SCRIPT_TANGUT,                 /* Tang */

  /* Unicode 10.0 additions */
  G_UNICODE_SCRIPT_MASARAM_GONDI,          /* Gonm */
  G_UNICODE_SCRIPT_NUSHU,                  /* Nshu */
  G_UNICODE_SCRIPT_SOYOMBO,                /* Soyo */
  G_UNICODE_SCRIPT_ZANABAZAR_SQUARE,       /* Zanb */

  /* Unicode 11.0 additions */
  G_UNICODE_SCRIPT_DOGRA,                  /* Dogr */
  G_UNICODE_SCRIPT_GUNJALA_GONDI,          /* Gong */
  G_UNICODE_SCRIPT_HANIFI_ROHINGYA,        /* Rohg */
  G_UNICODE_SCRIPT_MAKASAR,                /* Maka */
  G_UNICODE_SCRIPT_MEDEFAIDRIN,            /* Medf */
  G_UNICODE_SCRIPT_OLD_SOGDIAN,            /* Sogo */
  G_UNICODE_SCRIPT_SOGDIAN,                /* Sogd */

  /* Unicode 12.0 additions */
  G_UNICODE_SCRIPT_ELYMAIC,                /* Elym */
  G_UNICODE_SCRIPT_NANDINAGARI,            /* Nand */
  G_UNICODE_SCRIPT_NYIAKENG_PUACHUE_HMONG, /* Rohg */
  G_UNICODE_SCRIPT_WANCHO,                 /* Wcho */

  /* Unicode 13.0 additions */
  G_UNICODE_SCRIPT_CHORASMIAN,             /* Chrs */
  G_UNICODE_SCRIPT_DIVES_AKURU,            /* Diak */
  G_UNICODE_SCRIPT_KHITAN_SMALL_SCRIPT,    /* Kits */
  G_UNICODE_SCRIPT_YEZIDI                  /* Yezi */
} GUnicodeScript;


/** Following four function declarions are slightly modified to make the rest of
    the code to be functional without brining in the rest of GLib. The original
    definition is like this:

    GLIB_AVAILABLE_IN_ALL
    gboolean g_unichar_isalnum   (gunichar c) G_GNUC_CONST;

    GLIB_AVAILABLE_IN_ALL and G_GNUC_CONST are removed and commented out.

    This modification is not part of the original source file from the original
    Copyright holders.
*/

/* These are all analogs of the <ctype.h> functions.*/
gboolean g_unichar_isalnum   (gunichar c) /*G_GNUC_CONST*/;
gboolean g_unichar_isalpha   (gunichar c) /*G_GNUC_CONST*/;
gboolean g_unichar_isdigit   (gunichar c) /*G_GNUC_CONST*/;
GUnicodeScript g_unichar_get_script (gunichar ch) /*G_GNUC_CONST*/;
