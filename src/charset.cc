/**
 * \file src/charset.cc
 * Implementation of internal character set for dictionary.
 */

#include <shg/charset.h>
#include <cassert>
#include <bitset>
#include <iomanip>
#include <map>
#include <shg/encoding.h>

namespace SHG::PLP {

namespace {

struct Unicode_character {
     char32_t const code;
     char const* const name;
};

/*
 * The table character_table is constructed in the following way:
 *
 * 1. The initial table includes ISO/IEC 8859-2:1999 codes 00-FF
 * supplemented with those codes from ISO/IEC 8859-1:1998 which
 * are missing in ISO/IEC 8859-2:1999.
 *
 * 2. From the initial table the following characters are removed:
 *
 * 00A1 INVERTED EXCLAMATION MARK
 * 00A2 CENT SIGN
 * 00A3 POUND SIGN
 * 00A4 CURRENCY SIGN
 * 00A5 YEN SIGN
 * 00A6 BROKEN BAR
 * 00A8 DIAERESIS
 * 00AA FEMININE ORDINAL INDICATOR
 * 00AC NOT SIGN
 * 00AD SOFT HYPHEN
 * 00AF MACRON
 * 00B2 SUPERSCRIPT TWO
 * 00B3 SUPERSCRIPT THREE
 * 00B4 ACUTE ACCENT
 * 00B5 MICRO SIGN
 * 00B6 PILCROW SIGN
 * 00B7 MIDDLE DOT
 * 00B8 CEDILLA
 * 00B9 SUPERSCRIPT ONE
 * 00BA MASCULINE ORDINAL INDICATOR
 * 00BC VULGAR FRACTION ONE QUARTER
 * 00BD VULGAR FRACTION ONE HALF
 * 00BE VULGAR FRACTION THREE QUARTERS
 * 00BF INVERTED QUESTION MARK
 * 00C6 LATIN CAPITAL LETTER AE
 * 00D0 LATIN CAPITAL LETTER ETH
 * 00D7 MULTIPLICATION SIGN
 * 00DE LATIN CAPITAL LETTER THORN
 * 00E6 LATIN SMALL LETTER AE
 * 00F0 LATIN SMALL LETTER ETH
 * 00F7 DIVISION SIGN
 * 00FE LATIN SMALL LETTER THORN
 * 02C7 CARON
 * 02D8 BREVE
 * 02D9 DOT ABOVE
 * 02DB OGONEK
 * 02DD DOUBLE ACUTE ACCENT
 *
 * 3. The following characters are added:
 *
 * 0178 LATIN CAPITAL LETTER Y WITH DIAERESIS
 * 2010 HYPHEN (dywiz, lacznik)
 * 2013 EN DASH (myslnik, polpauza)
 * 2014 EM DASH (myslnik, pauza)
 * 2018 LEFT SINGLE QUOTATION MARK
 * 2019 RIGHT SINGLE QUOTATION MARK
 * 201B SINGLE HIGH-REVERSED-9 QUOTATION MARK
 * 201D RIGHT DOUBLE QUOTATION MARK (prawy cudzyslow)
 * 201E DOUBLE LOW-9 QUOTATION MARK (lewy cudzyslow)
 * 2026 HORIZONTAL ELLIPSIS (wielokropek)
 * 2212 MINUS SIGN (znak minus)
 * FFFD REPLACEMENT CHARACTER
 *
 * The character 0178 was added to have uppercase of 00FF LATIN SMALL
 * LETTER Y WITH DIAERESIS.
 */
Unicode_character constexpr character_table[256] = {
     {0x0000, "NULL"},                                        // 0x00
     {0x0001, "START OF HEADING"},                            // 0x01
     {0x0002, "START OF TEXT"},                               // 0x02
     {0x0003, "END OF TEXT"},                                 // 0x03
     {0x0004, "END OF TRANSMISSION"},                         // 0x04
     {0x0005, "ENQUIRY"},                                     // 0x05
     {0x0006, "ACKNOWLEDGE"},                                 // 0x06
     {0x0007, "BELL"},                                        // 0x07
     {0x0008, "BACKSPACE"},                                   // 0x08
     {0x0009, "CHARACTER TABULATION"},                        // 0x09
     {0x000a, "LINE FEED"},                                   // 0x0a
     {0x000b, "LINE TABULATION"},                             // 0x0b
     {0x000c, "FORM FEED"},                                   // 0x0c
     {0x000d, "CARRIAGE RETURN"},                             // 0x0d
     {0x000e, "SHIFT OUT"},                                   // 0x0e
     {0x000f, "SHIFT IN"},                                    // 0x0f
     {0x0010, "DATA LINK ESCAPE"},                            // 0x10
     {0x0011, "DEVICE CONTROL ONE"},                          // 0x11
     {0x0012, "DEVICE CONTROL TWO"},                          // 0x12
     {0x0013, "DEVICE CONTROL THREE"},                        // 0x13
     {0x0014, "DEVICE CONTROL FOUR"},                         // 0x14
     {0x0015, "NEGATIVE ACKNOWLEDGE"},                        // 0x15
     {0x0016, "SYNCHRONOUS IDLE"},                            // 0x16
     {0x0017, "END OF TRANSMISSION BLOCK"},                   // 0x17
     {0x0018, "CANCEL"},                                      // 0x18
     {0x0019, "END OF MEDIUM"},                               // 0x19
     {0x001a, "SUBSTITUTE"},                                  // 0x1a
     {0x001b, "ESCAPE"},                                      // 0x1b
     {0x001c, "INFORMATION SEPARATOR FOUR"},                  // 0x1c
     {0x001d, "INFORMATION SEPARATOR THREE"},                 // 0x1d
     {0x001e, "INFORMATION SEPARATOR TWO"},                   // 0x1e
     {0x001f, "INFORMATION SEPARATOR ONE"},                   // 0x1f
     {0x0020, "SPACE"},                                       // 0x20
     {0x0021, "EXCLAMATION MARK"},                            // 0x21
     {0x0022, "QUOTATION MARK"},                              // 0x22
     {0x0023, "NUMBER SIGN"},                                 // 0x23
     {0x0024, "DOLLAR SIGN"},                                 // 0x24
     {0x0025, "PERCENT SIGN"},                                // 0x25
     {0x0026, "AMPERSAND"},                                   // 0x26
     {0x0027, "APOSTROPHE"},                                  // 0x27
     {0x0028, "LEFT PARENTHESIS"},                            // 0x28
     {0x0029, "RIGHT PARENTHESIS"},                           // 0x29
     {0x002a, "ASTERISK"},                                    // 0x2a
     {0x002b, "PLUS SIGN"},                                   // 0x2b
     {0x002c, "COMMA"},                                       // 0x2c
     {0x002d, "HYPHEN-MINUS"},                                // 0x2d
     {0x002e, "FULL STOP"},                                   // 0x2e
     {0x002f, "SOLIDUS"},                                     // 0x2f
     {0x0030, "DIGIT ZERO"},                                  // 0x30
     {0x0031, "DIGIT ONE"},                                   // 0x31
     {0x0032, "DIGIT TWO"},                                   // 0x32
     {0x0033, "DIGIT THREE"},                                 // 0x33
     {0x0034, "DIGIT FOUR"},                                  // 0x34
     {0x0035, "DIGIT FIVE"},                                  // 0x35
     {0x0036, "DIGIT SIX"},                                   // 0x36
     {0x0037, "DIGIT SEVEN"},                                 // 0x37
     {0x0038, "DIGIT EIGHT"},                                 // 0x38
     {0x0039, "DIGIT NINE"},                                  // 0x39
     {0x003a, "COLON"},                                       // 0x3a
     {0x003b, "SEMICOLON"},                                   // 0x3b
     {0x003c, "LESS-THAN SIGN"},                              // 0x3c
     {0x003d, "EQUALS SIGN"},                                 // 0x3d
     {0x003e, "GREATER-THAN SIGN"},                           // 0x3e
     {0x003f, "QUESTION MARK"},                               // 0x3f
     {0x0040, "COMMERCIAL AT"},                               // 0x40
     {0x0041, "LATIN CAPITAL LETTER A"},                      // 0x41
     {0x0042, "LATIN CAPITAL LETTER B"},                      // 0x42
     {0x0043, "LATIN CAPITAL LETTER C"},                      // 0x43
     {0x0044, "LATIN CAPITAL LETTER D"},                      // 0x44
     {0x0045, "LATIN CAPITAL LETTER E"},                      // 0x45
     {0x0046, "LATIN CAPITAL LETTER F"},                      // 0x46
     {0x0047, "LATIN CAPITAL LETTER G"},                      // 0x47
     {0x0048, "LATIN CAPITAL LETTER H"},                      // 0x48
     {0x0049, "LATIN CAPITAL LETTER I"},                      // 0x49
     {0x004a, "LATIN CAPITAL LETTER J"},                      // 0x4a
     {0x004b, "LATIN CAPITAL LETTER K"},                      // 0x4b
     {0x004c, "LATIN CAPITAL LETTER L"},                      // 0x4c
     {0x004d, "LATIN CAPITAL LETTER M"},                      // 0x4d
     {0x004e, "LATIN CAPITAL LETTER N"},                      // 0x4e
     {0x004f, "LATIN CAPITAL LETTER O"},                      // 0x4f
     {0x0050, "LATIN CAPITAL LETTER P"},                      // 0x50
     {0x0051, "LATIN CAPITAL LETTER Q"},                      // 0x51
     {0x0052, "LATIN CAPITAL LETTER R"},                      // 0x52
     {0x0053, "LATIN CAPITAL LETTER S"},                      // 0x53
     {0x0054, "LATIN CAPITAL LETTER T"},                      // 0x54
     {0x0055, "LATIN CAPITAL LETTER U"},                      // 0x55
     {0x0056, "LATIN CAPITAL LETTER V"},                      // 0x56
     {0x0057, "LATIN CAPITAL LETTER W"},                      // 0x57
     {0x0058, "LATIN CAPITAL LETTER X"},                      // 0x58
     {0x0059, "LATIN CAPITAL LETTER Y"},                      // 0x59
     {0x005a, "LATIN CAPITAL LETTER Z"},                      // 0x5a
     {0x005b, "LEFT SQUARE BRACKET"},                         // 0x5b
     {0x005c, "REVERSE SOLIDUS"},                             // 0x5c
     {0x005d, "RIGHT SQUARE BRACKET"},                        // 0x5d
     {0x005e, "CIRCUMFLEX ACCENT"},                           // 0x5e
     {0x005f, "LOW LINE"},                                    // 0x5f
     {0x0060, "GRAVE ACCENT"},                                // 0x60
     {0x0061, "LATIN SMALL LETTER A"},                        // 0x61
     {0x0062, "LATIN SMALL LETTER B"},                        // 0x62
     {0x0063, "LATIN SMALL LETTER C"},                        // 0x63
     {0x0064, "LATIN SMALL LETTER D"},                        // 0x64
     {0x0065, "LATIN SMALL LETTER E"},                        // 0x65
     {0x0066, "LATIN SMALL LETTER F"},                        // 0x66
     {0x0067, "LATIN SMALL LETTER G"},                        // 0x67
     {0x0068, "LATIN SMALL LETTER H"},                        // 0x68
     {0x0069, "LATIN SMALL LETTER I"},                        // 0x69
     {0x006a, "LATIN SMALL LETTER J"},                        // 0x6a
     {0x006b, "LATIN SMALL LETTER K"},                        // 0x6b
     {0x006c, "LATIN SMALL LETTER L"},                        // 0x6c
     {0x006d, "LATIN SMALL LETTER M"},                        // 0x6d
     {0x006e, "LATIN SMALL LETTER N"},                        // 0x6e
     {0x006f, "LATIN SMALL LETTER O"},                        // 0x6f
     {0x0070, "LATIN SMALL LETTER P"},                        // 0x70
     {0x0071, "LATIN SMALL LETTER Q"},                        // 0x71
     {0x0072, "LATIN SMALL LETTER R"},                        // 0x72
     {0x0073, "LATIN SMALL LETTER S"},                        // 0x73
     {0x0074, "LATIN SMALL LETTER T"},                        // 0x74
     {0x0075, "LATIN SMALL LETTER U"},                        // 0x75
     {0x0076, "LATIN SMALL LETTER V"},                        // 0x76
     {0x0077, "LATIN SMALL LETTER W"},                        // 0x77
     {0x0078, "LATIN SMALL LETTER X"},                        // 0x78
     {0x0079, "LATIN SMALL LETTER Y"},                        // 0x79
     {0x007a, "LATIN SMALL LETTER Z"},                        // 0x7a
     {0x007b, "LEFT CURLY BRACKET"},                          // 0x7b
     {0x007c, "VERTICAL LINE"},                               // 0x7c
     {0x007d, "RIGHT CURLY BRACKET"},                         // 0x7d
     {0x007e, "TILDE"},                                       // 0x7e
     {0x007f, "DELETE"},                                      // 0x7f
     {0x00a0, "NO-BREAK SPACE"},                              // 0x80
     {0x00a7, "SECTION SIGN"},                                // 0x81
     {0x00a9, "COPYRIGHT SIGN"},                              // 0x82
     {0x00ab, "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK"},   // 0x83
     {0x00ae, "REGISTERED SIGN"},                             // 0x84
     {0x00b0, "DEGREE SIGN"},                                 // 0x85
     {0x00b1, "PLUS-MINUS SIGN"},                             // 0x86
     {0x00bb, "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK"},  // 0x87
     {0x00c0, "LATIN CAPITAL LETTER A WITH GRAVE"},           // 0x88
     {0x00c1, "LATIN CAPITAL LETTER A WITH ACUTE"},           // 0x89
     {0x00c2, "LATIN CAPITAL LETTER A WITH CIRCUMFLEX"},      // 0x8a
     {0x00c3, "LATIN CAPITAL LETTER A WITH TILDE"},           // 0x8b
     {0x00c4, "LATIN CAPITAL LETTER A WITH DIAERESIS"},       // 0x8c
     {0x00c5, "LATIN CAPITAL LETTER A WITH RING ABOVE"},      // 0x8d
     {0x00c7, "LATIN CAPITAL LETTER C WITH CEDILLA"},         // 0x8e
     {0x00c8, "LATIN CAPITAL LETTER E WITH GRAVE"},           // 0x8f
     {0x00c9, "LATIN CAPITAL LETTER E WITH ACUTE"},           // 0x90
     {0x00ca, "LATIN CAPITAL LETTER E WITH CIRCUMFLEX"},      // 0x91
     {0x00cb, "LATIN CAPITAL LETTER E WITH DIAERESIS"},       // 0x92
     {0x00cc, "LATIN CAPITAL LETTER I WITH GRAVE"},           // 0x93
     {0x00cd, "LATIN CAPITAL LETTER I WITH ACUTE"},           // 0x94
     {0x00ce, "LATIN CAPITAL LETTER I WITH CIRCUMFLEX"},      // 0x95
     {0x00cf, "LATIN CAPITAL LETTER I WITH DIAERESIS"},       // 0x96
     {0x00d1, "LATIN CAPITAL LETTER N WITH TILDE"},           // 0x97
     {0x00d2, "LATIN CAPITAL LETTER O WITH GRAVE"},           // 0x98
     {0x00d3, "LATIN CAPITAL LETTER O WITH ACUTE"},           // 0x99
     {0x00d4, "LATIN CAPITAL LETTER O WITH CIRCUMFLEX"},      // 0x9a
     {0x00d5, "LATIN CAPITAL LETTER O WITH TILDE"},           // 0x9b
     {0x00d6, "LATIN CAPITAL LETTER O WITH DIAERESIS"},       // 0x9c
     {0x00d8, "LATIN CAPITAL LETTER O WITH STROKE"},          // 0x9d
     {0x00d9, "LATIN CAPITAL LETTER U WITH GRAVE"},           // 0x9e
     {0x00da, "LATIN CAPITAL LETTER U WITH ACUTE"},           // 0x9f
     {0x00db, "LATIN CAPITAL LETTER U WITH CIRCUMFLEX"},      // 0xa0
     {0x00dc, "LATIN CAPITAL LETTER U WITH DIAERESIS"},       // 0xa1
     {0x00dd, "LATIN CAPITAL LETTER Y WITH ACUTE"},           // 0xa2
     {0x00df, "LATIN SMALL LETTER SHARP S"},                  // 0xa3
     {0x00e0, "LATIN SMALL LETTER A WITH GRAVE"},             // 0xa4
     {0x00e1, "LATIN SMALL LETTER A WITH ACUTE"},             // 0xa5
     {0x00e2, "LATIN SMALL LETTER A WITH CIRCUMFLEX"},        // 0xa6
     {0x00e3, "LATIN SMALL LETTER A WITH TILDE"},             // 0xa7
     {0x00e4, "LATIN SMALL LETTER A WITH DIAERESIS"},         // 0xa8
     {0x00e5, "LATIN SMALL LETTER A WITH RING ABOVE"},        // 0xa9
     {0x00e7, "LATIN SMALL LETTER C WITH CEDILLA"},           // 0xaa
     {0x00e8, "LATIN SMALL LETTER E WITH GRAVE"},             // 0xab
     {0x00e9, "LATIN SMALL LETTER E WITH ACUTE"},             // 0xac
     {0x00ea, "LATIN SMALL LETTER E WITH CIRCUMFLEX"},        // 0xad
     {0x00eb, "LATIN SMALL LETTER E WITH DIAERESIS"},         // 0xae
     {0x00ec, "LATIN SMALL LETTER I WITH GRAVE"},             // 0xaf
     {0x00ed, "LATIN SMALL LETTER I WITH ACUTE"},             // 0xb0
     {0x00ee, "LATIN SMALL LETTER I WITH CIRCUMFLEX"},        // 0xb1
     {0x00ef, "LATIN SMALL LETTER I WITH DIAERESIS"},         // 0xb2
     {0x00f1, "LATIN SMALL LETTER N WITH TILDE"},             // 0xb3
     {0x00f2, "LATIN SMALL LETTER O WITH GRAVE"},             // 0xb4
     {0x00f3, "LATIN SMALL LETTER O WITH ACUTE"},             // 0xb5
     {0x00f4, "LATIN SMALL LETTER O WITH CIRCUMFLEX"},        // 0xb6
     {0x00f5, "LATIN SMALL LETTER O WITH TILDE"},             // 0xb7
     {0x00f6, "LATIN SMALL LETTER O WITH DIAERESIS"},         // 0xb8
     {0x00f8, "LATIN SMALL LETTER O WITH STROKE"},            // 0xb9
     {0x00f9, "LATIN SMALL LETTER U WITH GRAVE"},             // 0xba
     {0x00fa, "LATIN SMALL LETTER U WITH ACUTE"},             // 0xbb
     {0x00fb, "LATIN SMALL LETTER U WITH CIRCUMFLEX"},        // 0xbc
     {0x00fc, "LATIN SMALL LETTER U WITH DIAERESIS"},         // 0xbd
     {0x00fd, "LATIN SMALL LETTER Y WITH ACUTE"},             // 0xbe
     {0x00ff, "LATIN SMALL LETTER Y WITH DIAERESIS"},         // 0xbf
     {0x0102, "LATIN CAPITAL LETTER A WITH BREVE"},           // 0xc0
     {0x0103, "LATIN SMALL LETTER A WITH BREVE"},             // 0xc1
     {0x0104, "LATIN CAPITAL LETTER A WITH OGONEK"},          // 0xc2
     {0x0105, "LATIN SMALL LETTER A WITH OGONEK"},            // 0xc3
     {0x0106, "LATIN CAPITAL LETTER C WITH ACUTE"},           // 0xc4
     {0x0107, "LATIN SMALL LETTER C WITH ACUTE"},             // 0xc5
     {0x010c, "LATIN CAPITAL LETTER C WITH CARON"},           // 0xc6
     {0x010d, "LATIN SMALL LETTER C WITH CARON"},             // 0xc7
     {0x010e, "LATIN CAPITAL LETTER D WITH CARON"},           // 0xc8
     {0x010f, "LATIN SMALL LETTER D WITH CARON"},             // 0xc9
     {0x0110, "LATIN CAPITAL LETTER D WITH STROKE"},          // 0xca
     {0x0111, "LATIN SMALL LETTER D WITH STROKE"},            // 0xcb
     {0x0118, "LATIN CAPITAL LETTER E WITH OGONEK"},          // 0xcc
     {0x0119, "LATIN SMALL LETTER E WITH OGONEK"},            // 0xcd
     {0x011a, "LATIN CAPITAL LETTER E WITH CARON"},           // 0xce
     {0x011b, "LATIN SMALL LETTER E WITH CARON"},             // 0xcf
     {0x0139, "LATIN CAPITAL LETTER L WITH ACUTE"},           // 0xd0
     {0x013a, "LATIN SMALL LETTER L WITH ACUTE"},             // 0xd1
     {0x013d, "LATIN CAPITAL LETTER L WITH CARON"},           // 0xd2
     {0x013e, "LATIN SMALL LETTER L WITH CARON"},             // 0xd3
     {0x0141, "LATIN CAPITAL LETTER L WITH STROKE"},          // 0xd4
     {0x0142, "LATIN SMALL LETTER L WITH STROKE"},            // 0xd5
     {0x0143, "LATIN CAPITAL LETTER N WITH ACUTE"},           // 0xd6
     {0x0144, "LATIN SMALL LETTER N WITH ACUTE"},             // 0xd7
     {0x0147, "LATIN CAPITAL LETTER N WITH CARON"},           // 0xd8
     {0x0148, "LATIN SMALL LETTER N WITH CARON"},             // 0xd9
     {0x0150, "LATIN CAPITAL LETTER O WITH DOUBLE ACUTE"},    // 0xda
     {0x0151, "LATIN SMALL LETTER O WITH DOUBLE ACUTE"},      // 0xdb
     {0x0154, "LATIN CAPITAL LETTER R WITH ACUTE"},           // 0xdc
     {0x0155, "LATIN SMALL LETTER R WITH ACUTE"},             // 0xdd
     {0x0158, "LATIN CAPITAL LETTER R WITH CARON"},           // 0xde
     {0x0159, "LATIN SMALL LETTER R WITH CARON"},             // 0xdf
     {0x015a, "LATIN CAPITAL LETTER S WITH ACUTE"},           // 0xe0
     {0x015b, "LATIN SMALL LETTER S WITH ACUTE"},             // 0xe1
     {0x015e, "LATIN CAPITAL LETTER S WITH CEDILLA"},         // 0xe2
     {0x015f, "LATIN SMALL LETTER S WITH CEDILLA"},           // 0xe3
     {0x0160, "LATIN CAPITAL LETTER S WITH CARON"},           // 0xe4
     {0x0161, "LATIN SMALL LETTER S WITH CARON"},             // 0xe5
     {0x0162, "LATIN CAPITAL LETTER T WITH CEDILLA"},         // 0xe6
     {0x0163, "LATIN SMALL LETTER T WITH CEDILLA"},           // 0xe7
     {0x0164, "LATIN CAPITAL LETTER T WITH CARON"},           // 0xe8
     {0x0165, "LATIN SMALL LETTER T WITH CARON"},             // 0xe9
     {0x016e, "LATIN CAPITAL LETTER U WITH RING ABOVE"},      // 0xea
     {0x016f, "LATIN SMALL LETTER U WITH RING ABOVE"},        // 0xeb
     {0x0170, "LATIN CAPITAL LETTER U WITH DOUBLE ACUTE"},    // 0xec
     {0x0171, "LATIN SMALL LETTER U WITH DOUBLE ACUTE"},      // 0xed
     {0x0178, "LATIN CAPITAL LETTER Y WITH DIAERESIS"},       // 0xee
     {0x0179, "LATIN CAPITAL LETTER Z WITH ACUTE"},           // 0xef
     {0x017a, "LATIN SMALL LETTER Z WITH ACUTE"},             // 0xf0
     {0x017b, "LATIN CAPITAL LETTER Z WITH DOT ABOVE"},       // 0xf1
     {0x017c, "LATIN SMALL LETTER Z WITH DOT ABOVE"},         // 0xf2
     {0x017d, "LATIN CAPITAL LETTER Z WITH CARON"},           // 0xf3
     {0x017e, "LATIN SMALL LETTER Z WITH CARON"},             // 0xf4
     {0x2010, "HYPHEN"},                                      // 0xf5
     {0x2013, "EN DASH"},                                     // 0xf6
     {0x2014, "EM DASH"},                                     // 0xf7
     {0x2018, "LEFT SINGLE QUOTATION MARK"},                  // 0xf8
     {0x2019, "RIGHT SINGLE QUOTATION MARK"},                 // 0xf9
     {0x201b, "SINGLE HIGH-REVERSED-9 QUOTATION MARK"},       // 0xfa
     {0x201d, "RIGHT DOUBLE QUOTATION MARK"},                 // 0xfb
     {0x201e, "DOUBLE LOW-9 QUOTATION MARK"},                 // 0xfc
     {0x2026, "HORIZONTAL ELLIPSIS"},                         // 0xfd
     {0x2212, "MINUS SIGN"},                                  // 0xfe
     {0xfffd, "REPLACEMENT CHARACTER"},                       // 0xff
};

std::bitset<12> constexpr ctype_table[256] = {
     //              XUSPPLGDCBAA
     0x008,  // 0x00 000000001000
     0x008,  // 0x01 000000001000
     0x008,  // 0x02 000000001000
     0x008,  // 0x03 000000001000
     0x008,  // 0x04 000000001000
     0x008,  // 0x05 000000001000
     0x008,  // 0x06 000000001000
     0x008,  // 0x07 000000001000
     0x008,  // 0x08 000000001000
     0x20c,  // 0x09 001000001100
     0x208,  // 0x0a 001000001000
     0x208,  // 0x0b 001000001000
     0x208,  // 0x0c 001000001000
     0x208,  // 0x0d 001000001000
     0x008,  // 0x0e 000000001000
     0x008,  // 0x0f 000000001000
     0x008,  // 0x10 000000001000
     0x008,  // 0x11 000000001000
     0x008,  // 0x12 000000001000
     0x008,  // 0x13 000000001000
     0x008,  // 0x14 000000001000
     0x008,  // 0x15 000000001000
     0x008,  // 0x16 000000001000
     0x008,  // 0x17 000000001000
     0x008,  // 0x18 000000001000
     0x008,  // 0x19 000000001000
     0x008,  // 0x1a 000000001000
     0x008,  // 0x1b 000000001000
     0x008,  // 0x1c 000000001000
     0x008,  // 0x1d 000000001000
     0x008,  // 0x1e 000000001000
     0x008,  // 0x1f 000000001000
     0x284,  // 0x20 001010000100
     0x1a0,  // 0x21 000110100000
     0x1a0,  // 0x22 000110100000
     0x1a0,  // 0x23 000110100000
     0x1a0,  // 0x24 000110100000
     0x1a0,  // 0x25 000110100000
     0x1a0,  // 0x26 000110100000
     0x1a0,  // 0x27 000110100000
     0x1a0,  // 0x28 000110100000
     0x1a0,  // 0x29 000110100000
     0x1a0,  // 0x2a 000110100000
     0x1a0,  // 0x2b 000110100000
     0x1a0,  // 0x2c 000110100000
     0x1a0,  // 0x2d 000110100000
     0x1a0,  // 0x2e 000110100000
     0x1a0,  // 0x2f 000110100000
     0x8b1,  // 0x30 100010110001
     0x8b1,  // 0x31 100010110001
     0x8b1,  // 0x32 100010110001
     0x8b1,  // 0x33 100010110001
     0x8b1,  // 0x34 100010110001
     0x8b1,  // 0x35 100010110001
     0x8b1,  // 0x36 100010110001
     0x8b1,  // 0x37 100010110001
     0x8b1,  // 0x38 100010110001
     0x8b1,  // 0x39 100010110001
     0x1a0,  // 0x3a 000110100000
     0x1a0,  // 0x3b 000110100000
     0x1a0,  // 0x3c 000110100000
     0x1a0,  // 0x3d 000110100000
     0x1a0,  // 0x3e 000110100000
     0x1a0,  // 0x3f 000110100000
     0x1a0,  // 0x40 000110100000
     0xca3,  // 0x41 110010100011
     0xca3,  // 0x42 110010100011
     0xca3,  // 0x43 110010100011
     0xca3,  // 0x44 110010100011
     0xca3,  // 0x45 110010100011
     0xca3,  // 0x46 110010100011
     0x4a3,  // 0x47 010010100011
     0x4a3,  // 0x48 010010100011
     0x4a3,  // 0x49 010010100011
     0x4a3,  // 0x4a 010010100011
     0x4a3,  // 0x4b 010010100011
     0x4a3,  // 0x4c 010010100011
     0x4a3,  // 0x4d 010010100011
     0x4a3,  // 0x4e 010010100011
     0x4a3,  // 0x4f 010010100011
     0x4a3,  // 0x50 010010100011
     0x4a3,  // 0x51 010010100011
     0x4a3,  // 0x52 010010100011
     0x4a3,  // 0x53 010010100011
     0x4a3,  // 0x54 010010100011
     0x4a3,  // 0x55 010010100011
     0x4a3,  // 0x56 010010100011
     0x4a3,  // 0x57 010010100011
     0x4a3,  // 0x58 010010100011
     0x4a3,  // 0x59 010010100011
     0x4a3,  // 0x5a 010010100011
     0x1a0,  // 0x5b 000110100000
     0x1a0,  // 0x5c 000110100000
     0x1a0,  // 0x5d 000110100000
     0x1a0,  // 0x5e 000110100000
     0x1a0,  // 0x5f 000110100000
     0x1a0,  // 0x60 000110100000
     0x8e3,  // 0x61 100011100011
     0x8e3,  // 0x62 100011100011
     0x8e3,  // 0x63 100011100011
     0x8e3,  // 0x64 100011100011
     0x8e3,  // 0x65 100011100011
     0x8e3,  // 0x66 100011100011
     0x0e3,  // 0x67 000011100011
     0x0e3,  // 0x68 000011100011
     0x0e3,  // 0x69 000011100011
     0x0e3,  // 0x6a 000011100011
     0x0e3,  // 0x6b 000011100011
     0x0e3,  // 0x6c 000011100011
     0x0e3,  // 0x6d 000011100011
     0x0e3,  // 0x6e 000011100011
     0x0e3,  // 0x6f 000011100011
     0x0e3,  // 0x70 000011100011
     0x0e3,  // 0x71 000011100011
     0x0e3,  // 0x72 000011100011
     0x0e3,  // 0x73 000011100011
     0x0e3,  // 0x74 000011100011
     0x0e3,  // 0x75 000011100011
     0x0e3,  // 0x76 000011100011
     0x0e3,  // 0x77 000011100011
     0x0e3,  // 0x78 000011100011
     0x0e3,  // 0x79 000011100011
     0x0e3,  // 0x7a 000011100011
     0x1a0,  // 0x7b 000110100000
     0x1a0,  // 0x7c 000110100000
     0x1a0,  // 0x7d 000110100000
     0x1a0,  // 0x7e 000110100000
     0x008,  // 0x7f 000000001000
     0x1a0,  // 0x80 000110100000
     0x1a0,  // 0x81 000110100000
     0x1a0,  // 0x82 000110100000
     0x1a0,  // 0x83 000110100000
     0x1a0,  // 0x84 000110100000
     0x1a0,  // 0x85 000110100000
     0x1a0,  // 0x86 000110100000
     0x1a0,  // 0x87 000110100000
     0x4a3,  // 0x88 010010100011
     0x4a3,  // 0x89 010010100011
     0x4a3,  // 0x8a 010010100011
     0x4a3,  // 0x8b 010010100011
     0x4a3,  // 0x8c 010010100011
     0x4a3,  // 0x8d 010010100011
     0x4a3,  // 0x8e 010010100011
     0x4a3,  // 0x8f 010010100011
     0x4a3,  // 0x90 010010100011
     0x4a3,  // 0x91 010010100011
     0x4a3,  // 0x92 010010100011
     0x4a3,  // 0x93 010010100011
     0x4a3,  // 0x94 010010100011
     0x4a3,  // 0x95 010010100011
     0x4a3,  // 0x96 010010100011
     0x4a3,  // 0x97 010010100011
     0x4a3,  // 0x98 010010100011
     0x4a3,  // 0x99 010010100011
     0x4a3,  // 0x9a 010010100011
     0x4a3,  // 0x9b 010010100011
     0x4a3,  // 0x9c 010010100011
     0x4a3,  // 0x9d 010010100011
     0x4a3,  // 0x9e 010010100011
     0x4a3,  // 0x9f 010010100011
     0x4a3,  // 0xa0 010010100011
     0x4a3,  // 0xa1 010010100011
     0x4a3,  // 0xa2 010010100011
     0x0e3,  // 0xa3 000011100011
     0x0e3,  // 0xa4 000011100011
     0x0e3,  // 0xa5 000011100011
     0x0e3,  // 0xa6 000011100011
     0x0e3,  // 0xa7 000011100011
     0x0e3,  // 0xa8 000011100011
     0x0e3,  // 0xa9 000011100011
     0x0e3,  // 0xaa 000011100011
     0x0e3,  // 0xab 000011100011
     0x0e3,  // 0xac 000011100011
     0x0e3,  // 0xad 000011100011
     0x0e3,  // 0xae 000011100011
     0x0e3,  // 0xaf 000011100011
     0x0e3,  // 0xb0 000011100011
     0x0e3,  // 0xb1 000011100011
     0x0e3,  // 0xb2 000011100011
     0x0e3,  // 0xb3 000011100011
     0x0e3,  // 0xb4 000011100011
     0x0e3,  // 0xb5 000011100011
     0x0e3,  // 0xb6 000011100011
     0x0e3,  // 0xb7 000011100011
     0x0e3,  // 0xb8 000011100011
     0x0e3,  // 0xb9 000011100011
     0x0e3,  // 0xba 000011100011
     0x0e3,  // 0xbb 000011100011
     0x0e3,  // 0xbc 000011100011
     0x0e3,  // 0xbd 000011100011
     0x0e3,  // 0xbe 000011100011
     0x0e3,  // 0xbf 000011100011
     0x4a3,  // 0xc0 010010100011
     0x0e3,  // 0xc1 000011100011
     0x4a3,  // 0xc2 010010100011
     0x0e3,  // 0xc3 000011100011
     0x4a3,  // 0xc4 010010100011
     0x0e3,  // 0xc5 000011100011
     0x4a3,  // 0xc6 010010100011
     0x0e3,  // 0xc7 000011100011
     0x4a3,  // 0xc8 010010100011
     0x0e3,  // 0xc9 000011100011
     0x4a3,  // 0xca 010010100011
     0x0e3,  // 0xcb 000011100011
     0x4a3,  // 0xcc 010010100011
     0x0e3,  // 0xcd 000011100011
     0x4a3,  // 0xce 010010100011
     0x0e3,  // 0xcf 000011100011
     0x4a3,  // 0xd0 010010100011
     0x0e3,  // 0xd1 000011100011
     0x4a3,  // 0xd2 010010100011
     0x0e3,  // 0xd3 000011100011
     0x4a3,  // 0xd4 010010100011
     0x0e3,  // 0xd5 000011100011
     0x4a3,  // 0xd6 010010100011
     0x0e3,  // 0xd7 000011100011
     0x4a3,  // 0xd8 010010100011
     0x0e3,  // 0xd9 000011100011
     0x4a3,  // 0xda 010010100011
     0x0e3,  // 0xdb 000011100011
     0x4a3,  // 0xdc 010010100011
     0x0e3,  // 0xdd 000011100011
     0x4a3,  // 0xde 010010100011
     0x0e3,  // 0xdf 000011100011
     0x4a3,  // 0xe0 010010100011
     0x0e3,  // 0xe1 000011100011
     0x4a3,  // 0xe2 010010100011
     0x0e3,  // 0xe3 000011100011
     0x4a3,  // 0xe4 010010100011
     0x0e3,  // 0xe5 000011100011
     0x4a3,  // 0xe6 010010100011
     0x0e3,  // 0xe7 000011100011
     0x4a3,  // 0xe8 010010100011
     0x0e3,  // 0xe9 000011100011
     0x4a3,  // 0xea 010010100011
     0x0e3,  // 0xeb 000011100011
     0x4a3,  // 0xec 010010100011
     0x0e3,  // 0xed 000011100011
     0x4a3,  // 0xee 010010100011
     0x4a3,  // 0xef 010010100011
     0x0e3,  // 0xf0 000011100011
     0x4a3,  // 0xf1 010010100011
     0x0e3,  // 0xf2 000011100011
     0x4a3,  // 0xf3 010010100011
     0x0e3,  // 0xf4 000011100011
     0x1a0,  // 0xf5 000110100000
     0x1a0,  // 0xf6 000110100000
     0x1a0,  // 0xf7 000110100000
     0x1a0,  // 0xf8 000110100000
     0x1a0,  // 0xf9 000110100000
     0x1a0,  // 0xfa 000110100000
     0x1a0,  // 0xfb 000110100000
     0x1a0,  // 0xfc 000110100000
     0x1a0,  // 0xfd 000110100000
     0x1a0,  // 0xfe 000110100000
     0x1a0,  // 0xff 000110100000
};

unsigned char constexpr toupper_table[256] = {
     0x00,  // 0x00
     0x01,  // 0x01
     0x02,  // 0x02
     0x03,  // 0x03
     0x04,  // 0x04
     0x05,  // 0x05
     0x06,  // 0x06
     0x07,  // 0x07
     0x08,  // 0x08
     0x09,  // 0x09
     0x0a,  // 0x0a
     0x0b,  // 0x0b
     0x0c,  // 0x0c
     0x0d,  // 0x0d
     0x0e,  // 0x0e
     0x0f,  // 0x0f
     0x10,  // 0x10
     0x11,  // 0x11
     0x12,  // 0x12
     0x13,  // 0x13
     0x14,  // 0x14
     0x15,  // 0x15
     0x16,  // 0x16
     0x17,  // 0x17
     0x18,  // 0x18
     0x19,  // 0x19
     0x1a,  // 0x1a
     0x1b,  // 0x1b
     0x1c,  // 0x1c
     0x1d,  // 0x1d
     0x1e,  // 0x1e
     0x1f,  // 0x1f
     0x20,  // 0x20
     0x21,  // 0x21
     0x22,  // 0x22
     0x23,  // 0x23
     0x24,  // 0x24
     0x25,  // 0x25
     0x26,  // 0x26
     0x27,  // 0x27
     0x28,  // 0x28
     0x29,  // 0x29
     0x2a,  // 0x2a
     0x2b,  // 0x2b
     0x2c,  // 0x2c
     0x2d,  // 0x2d
     0x2e,  // 0x2e
     0x2f,  // 0x2f
     0x30,  // 0x30
     0x31,  // 0x31
     0x32,  // 0x32
     0x33,  // 0x33
     0x34,  // 0x34
     0x35,  // 0x35
     0x36,  // 0x36
     0x37,  // 0x37
     0x38,  // 0x38
     0x39,  // 0x39
     0x3a,  // 0x3a
     0x3b,  // 0x3b
     0x3c,  // 0x3c
     0x3d,  // 0x3d
     0x3e,  // 0x3e
     0x3f,  // 0x3f
     0x40,  // 0x40
     0x41,  // 0x41
     0x42,  // 0x42
     0x43,  // 0x43
     0x44,  // 0x44
     0x45,  // 0x45
     0x46,  // 0x46
     0x47,  // 0x47
     0x48,  // 0x48
     0x49,  // 0x49
     0x4a,  // 0x4a
     0x4b,  // 0x4b
     0x4c,  // 0x4c
     0x4d,  // 0x4d
     0x4e,  // 0x4e
     0x4f,  // 0x4f
     0x50,  // 0x50
     0x51,  // 0x51
     0x52,  // 0x52
     0x53,  // 0x53
     0x54,  // 0x54
     0x55,  // 0x55
     0x56,  // 0x56
     0x57,  // 0x57
     0x58,  // 0x58
     0x59,  // 0x59
     0x5a,  // 0x5a
     0x5b,  // 0x5b
     0x5c,  // 0x5c
     0x5d,  // 0x5d
     0x5e,  // 0x5e
     0x5f,  // 0x5f
     0x60,  // 0x60
     0x41,  // 0x61
     0x42,  // 0x62
     0x43,  // 0x63
     0x44,  // 0x64
     0x45,  // 0x65
     0x46,  // 0x66
     0x47,  // 0x67
     0x48,  // 0x68
     0x49,  // 0x69
     0x4a,  // 0x6a
     0x4b,  // 0x6b
     0x4c,  // 0x6c
     0x4d,  // 0x6d
     0x4e,  // 0x6e
     0x4f,  // 0x6f
     0x50,  // 0x70
     0x51,  // 0x71
     0x52,  // 0x72
     0x53,  // 0x73
     0x54,  // 0x74
     0x55,  // 0x75
     0x56,  // 0x76
     0x57,  // 0x77
     0x58,  // 0x78
     0x59,  // 0x79
     0x5a,  // 0x7a
     0x7b,  // 0x7b
     0x7c,  // 0x7c
     0x7d,  // 0x7d
     0x7e,  // 0x7e
     0x7f,  // 0x7f
     0x80,  // 0x80
     0x81,  // 0x81
     0x82,  // 0x82
     0x83,  // 0x83
     0x84,  // 0x84
     0x85,  // 0x85
     0x86,  // 0x86
     0x87,  // 0x87
     0x88,  // 0x88
     0x89,  // 0x89
     0x8a,  // 0x8a
     0x8b,  // 0x8b
     0x8c,  // 0x8c
     0x8d,  // 0x8d
     0x8e,  // 0x8e
     0x8f,  // 0x8f
     0x90,  // 0x90
     0x91,  // 0x91
     0x92,  // 0x92
     0x93,  // 0x93
     0x94,  // 0x94
     0x95,  // 0x95
     0x96,  // 0x96
     0x97,  // 0x97
     0x98,  // 0x98
     0x99,  // 0x99
     0x9a,  // 0x9a
     0x9b,  // 0x9b
     0x9c,  // 0x9c
     0x9d,  // 0x9d
     0x9e,  // 0x9e
     0x9f,  // 0x9f
     0xa0,  // 0xa0
     0xa1,  // 0xa1
     0xa2,  // 0xa2
     0xa3,  // 0xa3
     0x88,  // 0xa4
     0x89,  // 0xa5
     0x8a,  // 0xa6
     0x8b,  // 0xa7
     0x8c,  // 0xa8
     0x8d,  // 0xa9
     0x8e,  // 0xaa
     0x8f,  // 0xab
     0x90,  // 0xac
     0x91,  // 0xad
     0x92,  // 0xae
     0x93,  // 0xaf
     0x94,  // 0xb0
     0x95,  // 0xb1
     0x96,  // 0xb2
     0x97,  // 0xb3
     0x98,  // 0xb4
     0x99,  // 0xb5
     0x9a,  // 0xb6
     0x9b,  // 0xb7
     0x9c,  // 0xb8
     0x9d,  // 0xb9
     0x9e,  // 0xba
     0x9f,  // 0xbb
     0xa0,  // 0xbc
     0xa1,  // 0xbd
     0xa2,  // 0xbe
     0xee,  // 0xbf
     0xc0,  // 0xc0
     0xc0,  // 0xc1
     0xc2,  // 0xc2
     0xc2,  // 0xc3
     0xc4,  // 0xc4
     0xc4,  // 0xc5
     0xc6,  // 0xc6
     0xc6,  // 0xc7
     0xc8,  // 0xc8
     0xc8,  // 0xc9
     0xca,  // 0xca
     0xca,  // 0xcb
     0xcc,  // 0xcc
     0xcc,  // 0xcd
     0xce,  // 0xce
     0xce,  // 0xcf
     0xd0,  // 0xd0
     0xd0,  // 0xd1
     0xd2,  // 0xd2
     0xd2,  // 0xd3
     0xd4,  // 0xd4
     0xd4,  // 0xd5
     0xd6,  // 0xd6
     0xd6,  // 0xd7
     0xd8,  // 0xd8
     0xd8,  // 0xd9
     0xda,  // 0xda
     0xda,  // 0xdb
     0xdc,  // 0xdc
     0xdc,  // 0xdd
     0xde,  // 0xde
     0xde,  // 0xdf
     0xe0,  // 0xe0
     0xe0,  // 0xe1
     0xe2,  // 0xe2
     0xe2,  // 0xe3
     0xe4,  // 0xe4
     0xe4,  // 0xe5
     0xe6,  // 0xe6
     0xe6,  // 0xe7
     0xe8,  // 0xe8
     0xe8,  // 0xe9
     0xea,  // 0xea
     0xea,  // 0xeb
     0xec,  // 0xec
     0xec,  // 0xed
     0xee,  // 0xee
     0xef,  // 0xef
     0xef,  // 0xf0
     0xf1,  // 0xf1
     0xf1,  // 0xf2
     0xf3,  // 0xf3
     0xf3,  // 0xf4
     0xf5,  // 0xf5
     0xf6,  // 0xf6
     0xf7,  // 0xf7
     0xf8,  // 0xf8
     0xf9,  // 0xf9
     0xfa,  // 0xfa
     0xfb,  // 0xfb
     0xfc,  // 0xfc
     0xfd,  // 0xfd
     0xfe,  // 0xfe
     0xff,  // 0xff
};

unsigned char constexpr tolower_table[256] = {
     0x00,  // 0x00
     0x01,  // 0x01
     0x02,  // 0x02
     0x03,  // 0x03
     0x04,  // 0x04
     0x05,  // 0x05
     0x06,  // 0x06
     0x07,  // 0x07
     0x08,  // 0x08
     0x09,  // 0x09
     0x0a,  // 0x0a
     0x0b,  // 0x0b
     0x0c,  // 0x0c
     0x0d,  // 0x0d
     0x0e,  // 0x0e
     0x0f,  // 0x0f
     0x10,  // 0x10
     0x11,  // 0x11
     0x12,  // 0x12
     0x13,  // 0x13
     0x14,  // 0x14
     0x15,  // 0x15
     0x16,  // 0x16
     0x17,  // 0x17
     0x18,  // 0x18
     0x19,  // 0x19
     0x1a,  // 0x1a
     0x1b,  // 0x1b
     0x1c,  // 0x1c
     0x1d,  // 0x1d
     0x1e,  // 0x1e
     0x1f,  // 0x1f
     0x20,  // 0x20
     0x21,  // 0x21
     0x22,  // 0x22
     0x23,  // 0x23
     0x24,  // 0x24
     0x25,  // 0x25
     0x26,  // 0x26
     0x27,  // 0x27
     0x28,  // 0x28
     0x29,  // 0x29
     0x2a,  // 0x2a
     0x2b,  // 0x2b
     0x2c,  // 0x2c
     0x2d,  // 0x2d
     0x2e,  // 0x2e
     0x2f,  // 0x2f
     0x30,  // 0x30
     0x31,  // 0x31
     0x32,  // 0x32
     0x33,  // 0x33
     0x34,  // 0x34
     0x35,  // 0x35
     0x36,  // 0x36
     0x37,  // 0x37
     0x38,  // 0x38
     0x39,  // 0x39
     0x3a,  // 0x3a
     0x3b,  // 0x3b
     0x3c,  // 0x3c
     0x3d,  // 0x3d
     0x3e,  // 0x3e
     0x3f,  // 0x3f
     0x40,  // 0x40
     0x61,  // 0x41
     0x62,  // 0x42
     0x63,  // 0x43
     0x64,  // 0x44
     0x65,  // 0x45
     0x66,  // 0x46
     0x67,  // 0x47
     0x68,  // 0x48
     0x69,  // 0x49
     0x6a,  // 0x4a
     0x6b,  // 0x4b
     0x6c,  // 0x4c
     0x6d,  // 0x4d
     0x6e,  // 0x4e
     0x6f,  // 0x4f
     0x70,  // 0x50
     0x71,  // 0x51
     0x72,  // 0x52
     0x73,  // 0x53
     0x74,  // 0x54
     0x75,  // 0x55
     0x76,  // 0x56
     0x77,  // 0x57
     0x78,  // 0x58
     0x79,  // 0x59
     0x7a,  // 0x5a
     0x5b,  // 0x5b
     0x5c,  // 0x5c
     0x5d,  // 0x5d
     0x5e,  // 0x5e
     0x5f,  // 0x5f
     0x60,  // 0x60
     0x61,  // 0x61
     0x62,  // 0x62
     0x63,  // 0x63
     0x64,  // 0x64
     0x65,  // 0x65
     0x66,  // 0x66
     0x67,  // 0x67
     0x68,  // 0x68
     0x69,  // 0x69
     0x6a,  // 0x6a
     0x6b,  // 0x6b
     0x6c,  // 0x6c
     0x6d,  // 0x6d
     0x6e,  // 0x6e
     0x6f,  // 0x6f
     0x70,  // 0x70
     0x71,  // 0x71
     0x72,  // 0x72
     0x73,  // 0x73
     0x74,  // 0x74
     0x75,  // 0x75
     0x76,  // 0x76
     0x77,  // 0x77
     0x78,  // 0x78
     0x79,  // 0x79
     0x7a,  // 0x7a
     0x7b,  // 0x7b
     0x7c,  // 0x7c
     0x7d,  // 0x7d
     0x7e,  // 0x7e
     0x7f,  // 0x7f
     0x80,  // 0x80
     0x81,  // 0x81
     0x82,  // 0x82
     0x83,  // 0x83
     0x84,  // 0x84
     0x85,  // 0x85
     0x86,  // 0x86
     0x87,  // 0x87
     0xa4,  // 0x88
     0xa5,  // 0x89
     0xa6,  // 0x8a
     0xa7,  // 0x8b
     0xa8,  // 0x8c
     0xa9,  // 0x8d
     0xaa,  // 0x8e
     0xab,  // 0x8f
     0xac,  // 0x90
     0xad,  // 0x91
     0xae,  // 0x92
     0xaf,  // 0x93
     0xb0,  // 0x94
     0xb1,  // 0x95
     0xb2,  // 0x96
     0xb3,  // 0x97
     0xb4,  // 0x98
     0xb5,  // 0x99
     0xb6,  // 0x9a
     0xb7,  // 0x9b
     0xb8,  // 0x9c
     0xb9,  // 0x9d
     0xba,  // 0x9e
     0xbb,  // 0x9f
     0xbc,  // 0xa0
     0xbd,  // 0xa1
     0xbe,  // 0xa2
     0xa3,  // 0xa3
     0xa4,  // 0xa4
     0xa5,  // 0xa5
     0xa6,  // 0xa6
     0xa7,  // 0xa7
     0xa8,  // 0xa8
     0xa9,  // 0xa9
     0xaa,  // 0xaa
     0xab,  // 0xab
     0xac,  // 0xac
     0xad,  // 0xad
     0xae,  // 0xae
     0xaf,  // 0xaf
     0xb0,  // 0xb0
     0xb1,  // 0xb1
     0xb2,  // 0xb2
     0xb3,  // 0xb3
     0xb4,  // 0xb4
     0xb5,  // 0xb5
     0xb6,  // 0xb6
     0xb7,  // 0xb7
     0xb8,  // 0xb8
     0xb9,  // 0xb9
     0xba,  // 0xba
     0xbb,  // 0xbb
     0xbc,  // 0xbc
     0xbd,  // 0xbd
     0xbe,  // 0xbe
     0xbf,  // 0xbf
     0xc1,  // 0xc0
     0xc1,  // 0xc1
     0xc3,  // 0xc2
     0xc3,  // 0xc3
     0xc5,  // 0xc4
     0xc5,  // 0xc5
     0xc7,  // 0xc6
     0xc7,  // 0xc7
     0xc9,  // 0xc8
     0xc9,  // 0xc9
     0xcb,  // 0xca
     0xcb,  // 0xcb
     0xcd,  // 0xcc
     0xcd,  // 0xcd
     0xcf,  // 0xce
     0xcf,  // 0xcf
     0xd1,  // 0xd0
     0xd1,  // 0xd1
     0xd3,  // 0xd2
     0xd3,  // 0xd3
     0xd5,  // 0xd4
     0xd5,  // 0xd5
     0xd7,  // 0xd6
     0xd7,  // 0xd7
     0xd9,  // 0xd8
     0xd9,  // 0xd9
     0xdb,  // 0xda
     0xdb,  // 0xdb
     0xdd,  // 0xdc
     0xdd,  // 0xdd
     0xdf,  // 0xde
     0xdf,  // 0xdf
     0xe1,  // 0xe0
     0xe1,  // 0xe1
     0xe3,  // 0xe2
     0xe3,  // 0xe3
     0xe5,  // 0xe4
     0xe5,  // 0xe5
     0xe7,  // 0xe6
     0xe7,  // 0xe7
     0xe9,  // 0xe8
     0xe9,  // 0xe9
     0xeb,  // 0xea
     0xeb,  // 0xeb
     0xed,  // 0xec
     0xed,  // 0xed
     0xbf,  // 0xee
     0xf0,  // 0xef
     0xf0,  // 0xf0
     0xf2,  // 0xf1
     0xf2,  // 0xf2
     0xf4,  // 0xf3
     0xf4,  // 0xf4
     0xf5,  // 0xf5
     0xf6,  // 0xf6
     0xf7,  // 0xf7
     0xf8,  // 0xf8
     0xf9,  // 0xf9
     0xfa,  // 0xfa
     0xfb,  // 0xfb
     0xfc,  // 0xfc
     0xfd,  // 0xfd
     0xfe,  // 0xfe
     0xff,  // 0xff
};

using Character_map = std::map<char16_t, unsigned char>;

Character_map init_character_map() {
     std::map<char16_t, unsigned char> m;
     char32_t prev_code;
     for (std::size_t i = 0; i < std::size(character_table); i++) {
          char32_t const code = character_table[i].code;
          assert(i == 0 || code > prev_code);
          m[code] = i;
          prev_code = code;
     }
     return m;
}

Character_map const& character_map() {
     static auto const m = init_character_map();
     return m;
}

}  // anonymous namespace

namespace Charset {

Invalid_character_error::Invalid_character_error()
     : std::runtime_error("invalid character in Charset") {}

char32_t unicode(char c) {
     return character_table[static_cast<unsigned char>(c)].code;
}

char const* name(char c) {
     return character_table[static_cast<unsigned char>(c)].name;
}

char unicode_to_char(char32_t c) {
     if (c < 0x10000u) {
          auto const it = character_map().find(c);
          if (it != character_map().end())
               return it->second;
     }
     throw Invalid_character_error();
}

std::string utf8_to_charset(std::string const& s) {
     std::string t;
     std::u32string const u = SHG::Encoding::utf8_to_utf32(s);
     auto i = u.cbegin();
     for (; i != u.cend(); ++i) {
          if (*i > 0xffff)
               break;
          auto const j = character_map().find(*i);
          if (j == character_map().end())
               break;
          t += j->second;
     }
     if (i != u.cend())
          throw Invalid_character_error();
     return t;
}

std::string charset_to_utf8(std::string const& s) {
     std::string t;
     for (auto c : s)
          t += SHG::Encoding::utf32_to_utf8(
               character_table[static_cast<unsigned char>(c)].code);
     return t;
}

bool isalnum(char c) {
     return ctype_table[static_cast<unsigned char>(c)][0];
}

bool isalpha(char c) {
     return ctype_table[static_cast<unsigned char>(c)][1];
}

bool isblank(char c) {
     return ctype_table[static_cast<unsigned char>(c)][2];
}

bool iscntrl(char c) {
     return ctype_table[static_cast<unsigned char>(c)][3];
}

bool isdigit(char c) {
     return ctype_table[static_cast<unsigned char>(c)][4];
}

bool isgraph(char c) {
     return ctype_table[static_cast<unsigned char>(c)][5];
}

bool islower(char c) {
     return ctype_table[static_cast<unsigned char>(c)][6];
}

bool isprint(char c) {
     return ctype_table[static_cast<unsigned char>(c)][7];
}

bool ispunct(char c) {
     return ctype_table[static_cast<unsigned char>(c)][8];
}

bool isspace(char c) {
     return ctype_table[static_cast<unsigned char>(c)][9];
}

bool isupper(char c) {
     return ctype_table[static_cast<unsigned char>(c)][10];
}

bool isxdigit(char c) {
     return ctype_table[static_cast<unsigned char>(c)][11];
}

char tolower(char c) {
     return tolower_table[static_cast<unsigned char>(c)];
}

char toupper(char c) {
     return toupper_table[static_cast<unsigned char>(c)];
}

namespace {

/**
 * Order of letters with marks is as follows:
 *
 * 1) ogonek
 * 2) stroke
 * 3) acute
 * 4) dot above
 * 5) caron
 * 6) diaeresis
 *r7) ing above
 * 8) breve
 * 9) grave
 * 10) cedilla
 * 11) circumflex
 * 12) tilde
 * 13) double acute
 *
 * 0 - 64     null - @      0 - 64
 * 65 - 90    A - Z
 * 91 - 96    [ - `        65 - 70
 * 97 - 122   a - z
 * 123 - 135  { - >>       71 - 83
 * 136 - 244  letters
 * 245 - 255  punct. marks 84 - 94
 *
 * There are 161 letters with codes 95 - 255.
 *
 * \todo Check above facts.
 */
unsigned char ordtab[256] = {
     0,    // 0x00, NULL
     1,    // 0x01, START OF HEADING
     2,    // 0x02, START OF TEXT
     3,    // 0x03, END OF TEXT
     4,    // 0x04, END OF TRANSMISSION
     5,    // 0x05, ENQUIRY
     6,    // 0x06, ACKNOWLEDGE
     7,    // 0x07, BELL
     8,    // 0x08, BACKSPACE
     9,    // 0x09, CHARACTER TABULATION
     10,   // 0x0a, LINE FEED
     11,   // 0x0b, LINE TABULATION
     12,   // 0x0c, FORM FEED
     13,   // 0x0d, CARRIAGE RETURN
     14,   // 0x0e, SHIFT OUT
     15,   // 0x0f, SHIFT IN
     16,   // 0x10, DATA LINK ESCAPE
     17,   // 0x11, DEVICE CONTROL ONE
     18,   // 0x12, DEVICE CONTROL TWO
     19,   // 0x13, DEVICE CONTROL THREE
     20,   // 0x14, DEVICE CONTROL FOUR
     21,   // 0x15, NEGATIVE ACKNOWLEDGE
     22,   // 0x16, SYNCHRONOUS IDLE
     23,   // 0x17, END OF TRANSMISSION BLOCK
     24,   // 0x18, CANCEL
     25,   // 0x19, END OF MEDIUM
     26,   // 0x1a, SUBSTITUTE
     27,   // 0x1b, ESCAPE
     28,   // 0x1c, INFORMATION SEPARATOR FOUR
     29,   // 0x1d, INFORMATION SEPARATOR THREE
     30,   // 0x1e, INFORMATION SEPARATOR TWO
     31,   // 0x1f, INFORMATION SEPARATOR ONE
     32,   // 0x20, SPACE
     33,   // 0x21, EXCLAMATION MARK
     34,   // 0x22, QUOTATION MARK
     35,   // 0x23, NUMBER SIGN
     36,   // 0x24, DOLLAR SIGN
     37,   // 0x25, PERCENT SIGN
     38,   // 0x26, AMPERSAND
     39,   // 0x27, APOSTROPHE
     40,   // 0x28, LEFT PARENTHESIS
     41,   // 0x29, RIGHT PARENTHESIS
     42,   // 0x2a, ASTERISK
     43,   // 0x2b, PLUS SIGN
     44,   // 0x2c, COMMA
     45,   // 0x2d, HYPHEN-MINUS
     46,   // 0x2e, FULL STOP
     47,   // 0x2f, SOLIDUS
     48,   // 0x30, DIGIT ZERO
     49,   // 0x31, DIGIT ONE
     50,   // 0x32, DIGIT TWO
     51,   // 0x33, DIGIT THREE
     52,   // 0x34, DIGIT FOUR
     53,   // 0x35, DIGIT FIVE
     54,   // 0x36, DIGIT SIX
     55,   // 0x37, DIGIT SEVEN
     56,   // 0x38, DIGIT EIGHT
     57,   // 0x39, DIGIT NINE
     58,   // 0x3a, COLON
     59,   // 0x3b, SEMICOLON
     60,   // 0x3c, LESS-THAN SIGN
     61,   // 0x3d, EQUALS SIGN
     62,   // 0x3e, GREATER-THAN SIGN
     63,   // 0x3f, QUESTION MARK
     64,   // 0x40, COMMERCIAL AT
     95,   // 0x41, LATIN CAPITAL LETTER A
     113,  // 0x42, LATIN CAPITAL LETTER B
     115,  // 0x43, LATIN CAPITAL LETTER C
     123,  // 0x44, LATIN CAPITAL LETTER D
     129,  // 0x45, LATIN CAPITAL LETTER E
     143,  // 0x46, LATIN CAPITAL LETTER F
     145,  // 0x47, LATIN CAPITAL LETTER G
     147,  // 0x48, LATIN CAPITAL LETTER H
     149,  // 0x49, LATIN CAPITAL LETTER I
     159,  // 0x4a, LATIN CAPITAL LETTER J
     161,  // 0x4b, LATIN CAPITAL LETTER K
     163,  // 0x4c, LATIN CAPITAL LETTER L
     171,  // 0x4d, LATIN CAPITAL LETTER M
     173,  // 0x4e, LATIN CAPITAL LETTER N
     181,  // 0x4f, LATIN CAPITAL LETTER O
     197,  // 0x50, LATIN CAPITAL LETTER P
     199,  // 0x51, LATIN CAPITAL LETTER Q
     201,  // 0x52, LATIN CAPITAL LETTER R
     207,  // 0x53, LATIN CAPITAL LETTER S
     216,  // 0x54, LATIN CAPITAL LETTER T
     222,  // 0x55, LATIN CAPITAL LETTER U
     236,  // 0x56, LATIN CAPITAL LETTER V
     238,  // 0x57, LATIN CAPITAL LETTER W
     240,  // 0x58, LATIN CAPITAL LETTER X
     242,  // 0x59, LATIN CAPITAL LETTER Y
     248,  // 0x5a, LATIN CAPITAL LETTER Z
     65,   // 0x5b, LEFT SQUARE BRACKET
     66,   // 0x5c, REVERSE SOLIDUS
     67,   // 0x5d, RIGHT SQUARE BRACKET
     68,   // 0x5e, CIRCUMFLEX ACCENT
     69,   // 0x5f, LOW LINE
     70,   // 0x60, GRAVE ACCENT
     104,  // 0x61, LATIN SMALL LETTER A
     114,  // 0x62, LATIN SMALL LETTER B
     119,  // 0x63, LATIN SMALL LETTER C
     126,  // 0x64, LATIN SMALL LETTER D
     136,  // 0x65, LATIN SMALL LETTER E
     144,  // 0x66, LATIN SMALL LETTER F
     146,  // 0x67, LATIN SMALL LETTER G
     148,  // 0x68, LATIN SMALL LETTER H
     154,  // 0x69, LATIN SMALL LETTER I
     160,  // 0x6a, LATIN SMALL LETTER J
     162,  // 0x6b, LATIN SMALL LETTER K
     167,  // 0x6c, LATIN SMALL LETTER L
     172,  // 0x6d, LATIN SMALL LETTER M
     177,  // 0x6e, LATIN SMALL LETTER N
     189,  // 0x6f, LATIN SMALL LETTER O
     198,  // 0x70, LATIN SMALL LETTER P
     200,  // 0x71, LATIN SMALL LETTER Q
     204,  // 0x72, LATIN SMALL LETTER R
     211,  // 0x73, LATIN SMALL LETTER S
     219,  // 0x74, LATIN SMALL LETTER T
     229,  // 0x75, LATIN SMALL LETTER U
     237,  // 0x76, LATIN SMALL LETTER V
     239,  // 0x77, LATIN SMALL LETTER W
     241,  // 0x78, LATIN SMALL LETTER X
     245,  // 0x79, LATIN SMALL LETTER Y
     252,  // 0x7a, LATIN SMALL LETTER Z
     71,   // 0x7b, LEFT CURLY BRACKET
     72,   // 0x7c, VERTICAL LINE
     73,   // 0x7d, RIGHT CURLY BRACKET
     74,   // 0x7e, TILDE
     75,   // 0x7f, DELETE
     76,   // 0x80, NO-BREAK SPACE
     77,   // 0x81, SECTION SIGN
     78,   // 0x82, COPYRIGHT SIGN
     79,   // 0x83, LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
     80,   // 0x84, REGISTERED SIGN
     81,   // 0x85, DEGREE SIGN
     82,   // 0x86, PLUS-MINUS SIGN
     83,   // 0x87, RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
     101,  // 0x88, LATIN CAPITAL LETTER A WITH GRAVE
     97,   // 0x89, LATIN CAPITAL LETTER A WITH ACUTE
     102,  // 0x8a, LATIN CAPITAL LETTER A WITH CIRCUMFLEX
     103,  // 0x8b, LATIN CAPITAL LETTER A WITH TILDE
     98,   // 0x8c, LATIN CAPITAL LETTER A WITH DIAERESIS
     99,   // 0x8d, LATIN CAPITAL LETTER A WITH RING ABOVE
     118,  // 0x8e, LATIN CAPITAL LETTER C WITH CEDILLA
     134,  // 0x8f, LATIN CAPITAL LETTER E WITH GRAVE
     131,  // 0x90, LATIN CAPITAL LETTER E WITH ACUTE
     135,  // 0x91, LATIN CAPITAL LETTER E WITH CIRCUMFLEX
     133,  // 0x92, LATIN CAPITAL LETTER E WITH DIAERESIS
     152,  // 0x93, LATIN CAPITAL LETTER I WITH GRAVE
     150,  // 0x94, LATIN CAPITAL LETTER I WITH ACUTE
     153,  // 0x95, LATIN CAPITAL LETTER I WITH CIRCUMFLEX
     151,  // 0x96, LATIN CAPITAL LETTER I WITH DIAERESIS
     176,  // 0x97, LATIN CAPITAL LETTER N WITH TILDE
     185,  // 0x98, LATIN CAPITAL LETTER O WITH GRAVE
     183,  // 0x99, LATIN CAPITAL LETTER O WITH ACUTE
     186,  // 0x9a, LATIN CAPITAL LETTER O WITH CIRCUMFLEX
     187,  // 0x9b, LATIN CAPITAL LETTER O WITH TILDE
     184,  // 0x9c, LATIN CAPITAL LETTER O WITH DIAERESIS
     182,  // 0x9d, LATIN CAPITAL LETTER O WITH STROKE
     226,  // 0x9e, LATIN CAPITAL LETTER U WITH GRAVE
     223,  // 0x9f, LATIN CAPITAL LETTER U WITH ACUTE
     227,  // 0xa0, LATIN CAPITAL LETTER U WITH CIRCUMFLEX
     224,  // 0xa1, LATIN CAPITAL LETTER U WITH DIAERESIS
     243,  // 0xa2, LATIN CAPITAL LETTER Y WITH ACUTE
     215,  // 0xa3, LATIN SMALL LETTER SHARP S
     110,  // 0xa4, LATIN SMALL LETTER A WITH GRAVE
     106,  // 0xa5, LATIN SMALL LETTER A WITH ACUTE
     111,  // 0xa6, LATIN SMALL LETTER A WITH CIRCUMFLEX
     112,  // 0xa7, LATIN SMALL LETTER A WITH TILDE
     107,  // 0xa8, LATIN SMALL LETTER A WITH DIAERESIS
     108,  // 0xa9, LATIN SMALL LETTER A WITH RING ABOVE
     122,  // 0xaa, LATIN SMALL LETTER C WITH CEDILLA
     141,  // 0xab, LATIN SMALL LETTER E WITH GRAVE
     138,  // 0xac, LATIN SMALL LETTER E WITH ACUTE
     142,  // 0xad, LATIN SMALL LETTER E WITH CIRCUMFLEX
     140,  // 0xae, LATIN SMALL LETTER E WITH DIAERESIS
     157,  // 0xaf, LATIN SMALL LETTER I WITH GRAVE
     155,  // 0xb0, LATIN SMALL LETTER I WITH ACUTE
     158,  // 0xb1, LATIN SMALL LETTER I WITH CIRCUMFLEX
     156,  // 0xb2, LATIN SMALL LETTER I WITH DIAERESIS
     180,  // 0xb3, LATIN SMALL LETTER N WITH TILDE
     193,  // 0xb4, LATIN SMALL LETTER O WITH GRAVE
     191,  // 0xb5, LATIN SMALL LETTER O WITH ACUTE
     194,  // 0xb6, LATIN SMALL LETTER O WITH CIRCUMFLEX
     195,  // 0xb7, LATIN SMALL LETTER O WITH TILDE
     192,  // 0xb8, LATIN SMALL LETTER O WITH DIAERESIS
     190,  // 0xb9, LATIN SMALL LETTER O WITH STROKE
     233,  // 0xba, LATIN SMALL LETTER U WITH GRAVE
     230,  // 0xbb, LATIN SMALL LETTER U WITH ACUTE
     234,  // 0xbc, LATIN SMALL LETTER U WITH CIRCUMFLEX
     231,  // 0xbd, LATIN SMALL LETTER U WITH DIAERESIS
     246,  // 0xbe, LATIN SMALL LETTER Y WITH ACUTE
     247,  // 0xbf, LATIN SMALL LETTER Y WITH DIAERESIS
     100,  // 0xc0, LATIN CAPITAL LETTER A WITH BREVE
     109,  // 0xc1, LATIN SMALL LETTER A WITH BREVE
     96,   // 0xc2, LATIN CAPITAL LETTER A WITH OGONEK
     105,  // 0xc3, LATIN SMALL LETTER A WITH OGONEK
     116,  // 0xc4, LATIN CAPITAL LETTER C WITH ACUTE
     120,  // 0xc5, LATIN SMALL LETTER C WITH ACUTE
     117,  // 0xc6, LATIN CAPITAL LETTER C WITH CARON
     121,  // 0xc7, LATIN SMALL LETTER C WITH CARON
     125,  // 0xc8, LATIN CAPITAL LETTER D WITH CARON
     128,  // 0xc9, LATIN SMALL LETTER D WITH CARON
     124,  // 0xca, LATIN CAPITAL LETTER D WITH STROKE
     127,  // 0xcb, LATIN SMALL LETTER D WITH STROKE
     130,  // 0xcc, LATIN CAPITAL LETTER E WITH OGONEK
     137,  // 0xcd, LATIN SMALL LETTER E WITH OGONEK
     132,  // 0xce, LATIN CAPITAL LETTER E WITH CARON
     139,  // 0xcf, LATIN SMALL LETTER E WITH CARON
     165,  // 0xd0, LATIN CAPITAL LETTER L WITH ACUTE
     169,  // 0xd1, LATIN SMALL LETTER L WITH ACUTE
     166,  // 0xd2, LATIN CAPITAL LETTER L WITH CARON
     170,  // 0xd3, LATIN SMALL LETTER L WITH CARON
     164,  // 0xd4, LATIN CAPITAL LETTER L WITH STROKE
     168,  // 0xd5, LATIN SMALL LETTER L WITH STROKE
     174,  // 0xd6, LATIN CAPITAL LETTER N WITH ACUTE
     178,  // 0xd7, LATIN SMALL LETTER N WITH ACUTE
     175,  // 0xd8, LATIN CAPITAL LETTER N WITH CARON
     179,  // 0xd9, LATIN SMALL LETTER N WITH CARON
     188,  // 0xda, LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
     196,  // 0xdb, LATIN SMALL LETTER O WITH DOUBLE ACUTE
     202,  // 0xdc, LATIN CAPITAL LETTER R WITH ACUTE
     205,  // 0xdd, LATIN SMALL LETTER R WITH ACUTE
     203,  // 0xde, LATIN CAPITAL LETTER R WITH CARON
     206,  // 0xdf, LATIN SMALL LETTER R WITH CARON
     208,  // 0xe0, LATIN CAPITAL LETTER S WITH ACUTE
     212,  // 0xe1, LATIN SMALL LETTER S WITH ACUTE
     210,  // 0xe2, LATIN CAPITAL LETTER S WITH CEDILLA
     214,  // 0xe3, LATIN SMALL LETTER S WITH CEDILLA
     209,  // 0xe4, LATIN CAPITAL LETTER S WITH CARON
     213,  // 0xe5, LATIN SMALL LETTER S WITH CARON
     218,  // 0xe6, LATIN CAPITAL LETTER T WITH CEDILLA
     221,  // 0xe7, LATIN SMALL LETTER T WITH CEDILLA
     217,  // 0xe8, LATIN CAPITAL LETTER T WITH CARON
     220,  // 0xe9, LATIN SMALL LETTER T WITH CARON
     225,  // 0xea, LATIN CAPITAL LETTER U WITH RING ABOVE
     232,  // 0xeb, LATIN SMALL LETTER U WITH RING ABOVE
     228,  // 0xec, LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
     235,  // 0xed, LATIN SMALL LETTER U WITH DOUBLE ACUTE
     244,  // 0xee, LATIN CAPITAL LETTER Y WITH DIAERESIS
     249,  // 0xef, LATIN CAPITAL LETTER Z WITH ACUTE
     253,  // 0xf0, LATIN SMALL LETTER Z WITH ACUTE
     250,  // 0xf1, LATIN CAPITAL LETTER Z WITH DOT ABOVE
     254,  // 0xf2, LATIN SMALL LETTER Z WITH DOT ABOVE
     251,  // 0xf3, LATIN CAPITAL LETTER Z WITH CARON
     255,  // 0xf4, LATIN SMALL LETTER Z WITH CARON
     84,   // 0xf5, HYPHEN
     85,   // 0xf6, EN DASH
     86,   // 0xf7, EM DASH
     87,   // 0xf8, LEFT SINGLE QUOTATION MARK
     88,   // 0xf9, RIGHT SINGLE QUOTATION MARK
     89,   // 0xfa, SINGLE HIGH-REVERSED-9 QUOTATION MARK
     90,   // 0xfb, RIGHT DOUBLE QUOTATION MARK
     91,   // 0xfc, DOUBLE LOW-9 QUOTATION MARK
     92,   // 0xfd, HORIZONTAL ELLIPSIS
     93,   // 0xfe, MINUS SIGN
     94,   // 0xff, REPLACEMENT CHARACTER
};

}  // anonymous namespace

int chrcmp(char lhs, char rhs) {
     return ordtab[static_cast<unsigned char>(lhs)] -
            ordtab[static_cast<unsigned char>(rhs)];
}

int alpha_strcmp(char const* lhs, char const* rhs) {
     while (*lhs && *lhs == *rhs) {
          lhs++;
          rhs++;
     }
     return chrcmp(*lhs, *rhs);
}

bool is_capit(plstring const& s) {
     auto const it = s.cbegin();
     if (it == s.cend())
          return true;
     if (!isupper(*it))
          return false;
     return std::all_of(it + 1, s.cend(), islower);
}

void print_character_table(std::ostream& f) {
     f << "Dec   Hex  Oct   Code     Char Description\n";
     for (std::size_t i = 0; i < std::size(character_table); i++) {
          auto const& uc = character_table[i];
          f << std::dec << std::setw(3) << std::setfill(' ') << i
            << "   " << std::hex << std::setw(2) << std::setfill('0')
            << i << "   " << std::oct << std::setw(3)
            << std::setfill('0') << i << "   "
            << "U+" << std::hex << std::setw(4) << uc.code << "   ";
          char const c = i;
          if (isprint(c))
               f << SHG::Encoding::utf32_to_utf8(uc.code);
          else
               f << ' ';
          f << "    " << uc.name;
          f << '\n';
     }
}

unsigned char const* get_ordtab() {
     return ordtab;
}

}  // namespace Charset

}  // namespace SHG::PLP
