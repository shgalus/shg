/* unicode.cc: Unicode encoding */

/**
 * \file src/unicode.cc
 * Unicode encoding.
 * \date Created on 21 September 2010.
 */

#include <cstdlib>
#include <stdexcept>
#include "shg/unicode.h"

namespace SHG {

std::string utf8_encode(std::uint32_t c) {
/*
 * 00000000-0000007F: 00000000 00000000 00000000 0xxxxxxx (7 bits)
 * is translated to
 * 0xxxxxxx
 *
 * 00000080-000007FF: 00000000 00000000 00000xxx xxxxxxxx (11 bits)
 * is translated to
 * 110xxxxx 10xxxxxx
 *
 * 00000800-0000FFFF: 00000000 00000000 xxxxxxxx xxxxxxxx (16 bits)
 * is translated to
 * 1110xxxx 10xxxxxx 10xxxxxx
 *
 * 00010000-001FFFFF: 00000000 000xxxxx xxxxxxxx xxxxxxxx (21 bits)
 * is translated to
 * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * 00200000-03FFFFFF: 000000xx xxxxxxxx xxxxxxxx xxxxxxxx (26 bits)
 * is translated to
 * 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * 04000000-7FFFFFFF: 0xxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (31 bits)
 * is translated to
 * 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
     std::string::size_type nbytes;
     unsigned char w[6];
     if (c < 0x80u) {
          w[0] = static_cast<unsigned char>(c);
          nbytes = 1;
     } else if (c < 0x0800u) {
          w[0] = static_cast<unsigned char>(0xc0u + c / 0x40u);
          w[1] = 0x80u + c % 0x40u;
          nbytes = 2;
     } else if (c < 0x10000u) {
          /* Check for UTF-16 surrogate. */
          if (c >= 0xd800u && c < 0xe000u)
               goto error;
          w[0] = static_cast<unsigned char>(0xe0u + c / 0x1000u);
          c %= 0x1000u;
          w[1] = static_cast<unsigned char>(0x80u + c / 0x40u);
          w[2] = 0x80u + c % 0x40u;
          nbytes = 3;
     } else if (c < 0x200000u) {
          w[0] = static_cast<unsigned char>(0xf0u + c / 0x40000u);
          c %= 0x40000u;
          w[1] = static_cast<unsigned char>(0x80u + c / 0x1000u);
          c %= 0x1000u;
          w[2] = static_cast<unsigned char>(0x80u + c / 0x40u);
          w[3] = 0x80u + c % 0x40u;
          nbytes = 4;
     } else if (c < 0x4000000u) {
          w[0] = 0xf8u + c / 0x1000000u;
          c %= 0x1000000u;
          w[1] = static_cast<unsigned char>(0x80u + c / 0x40000u);
          c %= 0x40000u;
          w[2] = static_cast<unsigned char>(0x80u + c / 0x1000u);
          c %= 0x1000u;
          w[3] = static_cast<unsigned char>(0x80u + c / 0x40u);
          w[4] = 0x80u + c % 0x40u;
          nbytes = 5;
     } else if (c < 0x80000000u) {
          w[0] = 0xfcu + c / 0x40000000u;
          c %= 0x40000000u;
          w[1] = 0x80u + c / 0x1000000u;
          c %= 0x1000000u;
          w[2] = static_cast<unsigned char>(0x80u + c / 0x40000u);
          c %= 0x40000u;
          w[3] = static_cast<unsigned char>(0x80u + c / 0x1000u);
          c %= 0x1000u;
          w[4] = static_cast<unsigned char>(0x80u + c / 0x40u);
          w[5] = 0x80u + c % 0x40u;
          nbytes = 6;
     } else {
          goto error;
     }
     return std::string(reinterpret_cast<char*>(w), nbytes);
error:
     throw std::invalid_argument(__func__);
}

namespace {

/**
 * Decodes UTF-8 sequence contained in s starting from position i. If
 * the sequence is correct, its Unicode code is returned and the
 * position i is set to the beginning of the next sequence. If the
 * sequence is incorrect, std::invalid_argument is thrown and the
 * position in i remains undefinite.
 */
std::uint32_t getc(const std::string& s, std::string::size_type& i) {
     std::uint32_t u;
     int nbytes;
     unsigned char b = s[i];
     if (b < 0x80u) {
          u = b;
          nbytes = 0;
     } else if (b < 0xc0u) {
          goto error;
     } else if (b < 0xe0u) {
          u = b & 0x1fu;
          nbytes = 1;
     } else if (b < 0xf0u) {
          u = b & 0x0fu;
          nbytes = 2;
     } else if (b < 0xf8u) {
          u = b & 0x07u;
          nbytes = 3;
     } else if (b < 0xfcu) {
          u = b & 0x03u;
          nbytes = 4;
     } else if (b < 0xfeu) {
          u = b & 0x01u;
          nbytes = 5;
     } else {
          goto error;
     }
     for (int j = 0; j < nbytes; j++)
          if (++i >= s.size() ||
              ((b = s[i]) & 0xc0u) != 0x80u)
               goto error;
          else
               (u <<= 6) += b & 0x3fu;
     /* Check for overlong sequence. */
     if (u < 0x80u) {
          if (nbytes > 0)
               goto error;
     } else if (u < 0x0800u) {
          if (nbytes > 1)
               goto error;
     } else if (u < 0x10000u) {
          if (nbytes > 2)
               goto error;
     } else if (u < 0x200000u) {
          if (nbytes > 3)
               goto error;
     } else if (u < 0x4000000u) {
          if (nbytes > 4)
               goto error;
     } else if (u < 0x80000000u) {
          if (nbytes > 5)
               goto error;
     }
     /* Check for UTF-16 surrogate. */
     if (u >= 0xd800u && u < 0xe000u)
          goto error;
     return u;
error:
     throw std::invalid_argument(__func__);
}

}       // anonymous namespace

std::vector<std::uint32_t> utf8_decode(const std::string& s) {
     std::vector<std::uint32_t> v;
     for (std::string::size_type i = 0; i < s.size(); i++)
          v.push_back(getc(s, i));
     return v;
}

std::string::size_type utf8_strlen(const std::string& s) {
     std::string::size_type n = 0;
     for (std::string::size_type i = 0; i < s.size(); i++, n++)
          getc(s, i);
     return n;
}

namespace {

/**
 * Unicode codes of ISO_8859-2:1987 character set. Characters 0xa0 to
 * 0xff.
 */
const std::uint32_t iso88592[96] = {
     0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7,
     0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b,
     0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7,
     0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c,
     0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
     0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
     0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
     0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
     0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
     0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
     0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
     0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
};

}       // anonymous namespace

std::uint32_t l2tou(char c) {
     const unsigned char uc = static_cast<unsigned char>(c);
     return uc < 0xa0u ? uc : iso88592[uc - 0xa0];
}

}       // namespace SHG
