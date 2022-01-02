#include <shg/charset.h>
#include <cstring>
#include <algorithm>
#include <ios>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(charset_test)

namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_CASE(unicode_test) {
     using namespace PLP::Charset;
     for (unsigned i = 0; i < 0x80; i++)
          BOOST_CHECK(unicode(i) == i);
     BOOST_CHECK(unicode('\x80') == 0x00a0);
     BOOST_CHECK(unicode('\xf2') == 0x017c);
     BOOST_CHECK(unicode('\xff') == 0xfffd);
     BOOST_CHECK(std::strcmp(name('\x80'), "NO-BREAK SPACE") == 0);
     BOOST_CHECK(std::strcmp(name('\xf2'),
                             "LATIN SMALL LETTER Z WITH DOT ABOVE") ==
                 0);
     BOOST_CHECK(std::strcmp(name('\xff'), "REPLACEMENT CHARACTER") ==
                 0);
}

BOOST_AUTO_TEST_CASE(unicode_to_char_test) {
     using namespace PLP::Charset;
     for (int i = 0; i < 0x80; i++)
          BOOST_CHECK(unicode_to_char(i) == i);
     BOOST_CHECK(unicode_to_char(0x00a0) == '\x80');
     BOOST_CHECK(unicode_to_char(0x017c) == '\xf2');
     BOOST_CHECK(unicode_to_char(0xfffd) == '\xff');
     BOOST_CHECK_THROW(unicode_to_char(0x00a1),
                       Invalid_character_error);
}

BOOST_AUTO_TEST_CASE(conversion_test) {
     using namespace PLP::Charset;
     std::string s;
     for (int i = 0; i < 256; i++)
          s += i;
     std::string const t = charset_to_utf8(s);
     std::string const u = utf8_to_charset(t);
     BOOST_CHECK(s == u);
}

BOOST_AUTO_TEST_CASE(ctype_test) {
     using namespace PLP::Charset;
     BOOST_CHECK(tolower('\xf1') == '\xf2');
     BOOST_CHECK(toupper('\xf2') == '\xf1');
}

// clang-format off

constexpr char const* const print_character_table_result =
     "Dec   Hex  Oct   Code     Char Description\n"
     "  0   00   000   U+0000        NULL\n"
     "  1   01   001   U+0001        START OF HEADING\n"
     "  2   02   002   U+0002        START OF TEXT\n"
     "  3   03   003   U+0003        END OF TEXT\n"
     "  4   04   004   U+0004        END OF TRANSMISSION\n"
     "  5   05   005   U+0005        ENQUIRY\n"
     "  6   06   006   U+0006        ACKNOWLEDGE\n"
     "  7   07   007   U+0007        BELL\n"
     "  8   08   010   U+0008        BACKSPACE\n"
     "  9   09   011   U+0009        CHARACTER TABULATION\n"
     " 10   0a   012   U+000a        LINE FEED\n"
     " 11   0b   013   U+000b        LINE TABULATION\n"
     " 12   0c   014   U+000c        FORM FEED\n"
     " 13   0d   015   U+000d        CARRIAGE RETURN\n"
     " 14   0e   016   U+000e        SHIFT OUT\n"
     " 15   0f   017   U+000f        SHIFT IN\n"
     " 16   10   020   U+0010        DATA LINK ESCAPE\n"
     " 17   11   021   U+0011        DEVICE CONTROL ONE\n"
     " 18   12   022   U+0012        DEVICE CONTROL TWO\n"
     " 19   13   023   U+0013        DEVICE CONTROL THREE\n"
     " 20   14   024   U+0014        DEVICE CONTROL FOUR\n"
     " 21   15   025   U+0015        NEGATIVE ACKNOWLEDGE\n"
     " 22   16   026   U+0016        SYNCHRONOUS IDLE\n"
     " 23   17   027   U+0017        END OF TRANSMISSION BLOCK\n"
     " 24   18   030   U+0018        CANCEL\n"
     " 25   19   031   U+0019        END OF MEDIUM\n"
     " 26   1a   032   U+001a        SUBSTITUTE\n"
     " 27   1b   033   U+001b        ESCAPE\n"
     " 28   1c   034   U+001c        INFORMATION SEPARATOR FOUR\n"
     " 29   1d   035   U+001d        INFORMATION SEPARATOR THREE\n"
     " 30   1e   036   U+001e        INFORMATION SEPARATOR TWO\n"
     " 31   1f   037   U+001f        INFORMATION SEPARATOR ONE\n"
     " 32   20   040   U+0020        SPACE\n"
     " 33   21   041   U+0021   !    EXCLAMATION MARK\n"
     " 34   22   042   U+0022   \"    QUOTATION MARK\n"
     " 35   23   043   U+0023   #    NUMBER SIGN\n"
     " 36   24   044   U+0024   $    DOLLAR SIGN\n"
     " 37   25   045   U+0025   %    PERCENT SIGN\n"
     " 38   26   046   U+0026   &    AMPERSAND\n"
     " 39   27   047   U+0027   '    APOSTROPHE\n"
     " 40   28   050   U+0028   (    LEFT PARENTHESIS\n"
     " 41   29   051   U+0029   )    RIGHT PARENTHESIS\n"
     " 42   2a   052   U+002a   *    ASTERISK\n"
     " 43   2b   053   U+002b   +    PLUS SIGN\n"
     " 44   2c   054   U+002c   ,    COMMA\n"
     " 45   2d   055   U+002d   -    HYPHEN-MINUS\n"
     " 46   2e   056   U+002e   .    FULL STOP\n"
     " 47   2f   057   U+002f   /    SOLIDUS\n"
     " 48   30   060   U+0030   0    DIGIT ZERO\n"
     " 49   31   061   U+0031   1    DIGIT ONE\n"
     " 50   32   062   U+0032   2    DIGIT TWO\n"
     " 51   33   063   U+0033   3    DIGIT THREE\n"
     " 52   34   064   U+0034   4    DIGIT FOUR\n"
     " 53   35   065   U+0035   5    DIGIT FIVE\n"
     " 54   36   066   U+0036   6    DIGIT SIX\n"
     " 55   37   067   U+0037   7    DIGIT SEVEN\n"
     " 56   38   070   U+0038   8    DIGIT EIGHT\n"
     " 57   39   071   U+0039   9    DIGIT NINE\n"
     " 58   3a   072   U+003a   :    COLON\n"
     " 59   3b   073   U+003b   ;    SEMICOLON\n"
     " 60   3c   074   U+003c   <    LESS-THAN SIGN\n"
     " 61   3d   075   U+003d   =    EQUALS SIGN\n"
     " 62   3e   076   U+003e   >    GREATER-THAN SIGN\n"
     " 63   3f   077   U+003f   ?    QUESTION MARK\n"
     " 64   40   100   U+0040   @    COMMERCIAL AT\n"
     " 65   41   101   U+0041   A    LATIN CAPITAL LETTER A\n"
     " 66   42   102   U+0042   B    LATIN CAPITAL LETTER B\n"
     " 67   43   103   U+0043   C    LATIN CAPITAL LETTER C\n"
     " 68   44   104   U+0044   D    LATIN CAPITAL LETTER D\n"
     " 69   45   105   U+0045   E    LATIN CAPITAL LETTER E\n"
     " 70   46   106   U+0046   F    LATIN CAPITAL LETTER F\n"
     " 71   47   107   U+0047   G    LATIN CAPITAL LETTER G\n"
     " 72   48   110   U+0048   H    LATIN CAPITAL LETTER H\n"
     " 73   49   111   U+0049   I    LATIN CAPITAL LETTER I\n"
     " 74   4a   112   U+004a   J    LATIN CAPITAL LETTER J\n"
     " 75   4b   113   U+004b   K    LATIN CAPITAL LETTER K\n"
     " 76   4c   114   U+004c   L    LATIN CAPITAL LETTER L\n"
     " 77   4d   115   U+004d   M    LATIN CAPITAL LETTER M\n"
     " 78   4e   116   U+004e   N    LATIN CAPITAL LETTER N\n"
     " 79   4f   117   U+004f   O    LATIN CAPITAL LETTER O\n"
     " 80   50   120   U+0050   P    LATIN CAPITAL LETTER P\n"
     " 81   51   121   U+0051   Q    LATIN CAPITAL LETTER Q\n"
     " 82   52   122   U+0052   R    LATIN CAPITAL LETTER R\n"
     " 83   53   123   U+0053   S    LATIN CAPITAL LETTER S\n"
     " 84   54   124   U+0054   T    LATIN CAPITAL LETTER T\n"
     " 85   55   125   U+0055   U    LATIN CAPITAL LETTER U\n"
     " 86   56   126   U+0056   V    LATIN CAPITAL LETTER V\n"
     " 87   57   127   U+0057   W    LATIN CAPITAL LETTER W\n"
     " 88   58   130   U+0058   X    LATIN CAPITAL LETTER X\n"
     " 89   59   131   U+0059   Y    LATIN CAPITAL LETTER Y\n"
     " 90   5a   132   U+005a   Z    LATIN CAPITAL LETTER Z\n"
     " 91   5b   133   U+005b   [    LEFT SQUARE BRACKET\n"
     " 92   5c   134   U+005c   \\    REVERSE SOLIDUS\n"
     " 93   5d   135   U+005d   ]    RIGHT SQUARE BRACKET\n"
     " 94   5e   136   U+005e   ^    CIRCUMFLEX ACCENT\n"
     " 95   5f   137   U+005f   _    LOW LINE\n"
     " 96   60   140   U+0060   `    GRAVE ACCENT\n"
     " 97   61   141   U+0061   a    LATIN SMALL LETTER A\n"
     " 98   62   142   U+0062   b    LATIN SMALL LETTER B\n"
     " 99   63   143   U+0063   c    LATIN SMALL LETTER C\n"
     "100   64   144   U+0064   d    LATIN SMALL LETTER D\n"
     "101   65   145   U+0065   e    LATIN SMALL LETTER E\n"
     "102   66   146   U+0066   f    LATIN SMALL LETTER F\n"
     "103   67   147   U+0067   g    LATIN SMALL LETTER G\n"
     "104   68   150   U+0068   h    LATIN SMALL LETTER H\n"
     "105   69   151   U+0069   i    LATIN SMALL LETTER I\n"
     "106   6a   152   U+006a   j    LATIN SMALL LETTER J\n"
     "107   6b   153   U+006b   k    LATIN SMALL LETTER K\n"
     "108   6c   154   U+006c   l    LATIN SMALL LETTER L\n"
     "109   6d   155   U+006d   m    LATIN SMALL LETTER M\n"
     "110   6e   156   U+006e   n    LATIN SMALL LETTER N\n"
     "111   6f   157   U+006f   o    LATIN SMALL LETTER O\n"
     "112   70   160   U+0070   p    LATIN SMALL LETTER P\n"
     "113   71   161   U+0071   q    LATIN SMALL LETTER Q\n"
     "114   72   162   U+0072   r    LATIN SMALL LETTER R\n"
     "115   73   163   U+0073   s    LATIN SMALL LETTER S\n"
     "116   74   164   U+0074   t    LATIN SMALL LETTER T\n"
     "117   75   165   U+0075   u    LATIN SMALL LETTER U\n"
     "118   76   166   U+0076   v    LATIN SMALL LETTER V\n"
     "119   77   167   U+0077   w    LATIN SMALL LETTER W\n"
     "120   78   170   U+0078   x    LATIN SMALL LETTER X\n"
     "121   79   171   U+0079   y    LATIN SMALL LETTER Y\n"
     "122   7a   172   U+007a   z    LATIN SMALL LETTER Z\n"
     "123   7b   173   U+007b   {    LEFT CURLY BRACKET\n"
     "124   7c   174   U+007c   |    VERTICAL LINE\n"
     "125   7d   175   U+007d   }    RIGHT CURLY BRACKET\n"
     "126   7e   176   U+007e   ~    TILDE\n"
     "127   7f   177   U+007f        DELETE\n"
     "128   80   200   U+00a0   \302\240    NO-BREAK SPACE\n"
     "129   81   201   U+00a7   \302\247    SECTION SIGN\n"
     "130   82   202   U+00a9   \302\251    COPYRIGHT SIGN\n"
     "131   83   203   U+00ab   \302\253    LEFT-POINTING DOUBLE ANGLE QUOTATION MARK\n"
     "132   84   204   U+00ae   \302\256    REGISTERED SIGN\n"
     "133   85   205   U+00b0   \302\260    DEGREE SIGN\n"
     "134   86   206   U+00b1   \302\261    PLUS-MINUS SIGN\n"
     "135   87   207   U+00bb   \302\273    RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK\n"
     "136   88   210   U+00c0   \303\200    LATIN CAPITAL LETTER A WITH GRAVE\n"
     "137   89   211   U+00c1   \303\201    LATIN CAPITAL LETTER A WITH ACUTE\n"
     "138   8a   212   U+00c2   \303\202    LATIN CAPITAL LETTER A WITH CIRCUMFLEX\n"
     "139   8b   213   U+00c3   \303\203    LATIN CAPITAL LETTER A WITH TILDE\n"
     "140   8c   214   U+00c4   \303\204    LATIN CAPITAL LETTER A WITH DIAERESIS\n"
     "141   8d   215   U+00c5   \303\205    LATIN CAPITAL LETTER A WITH RING ABOVE\n"
     "142   8e   216   U+00c7   \303\207    LATIN CAPITAL LETTER C WITH CEDILLA\n"
     "143   8f   217   U+00c8   \303\210    LATIN CAPITAL LETTER E WITH GRAVE\n"
     "144   90   220   U+00c9   \303\211    LATIN CAPITAL LETTER E WITH ACUTE\n"
     "145   91   221   U+00ca   \303\212    LATIN CAPITAL LETTER E WITH CIRCUMFLEX\n"
     "146   92   222   U+00cb   \303\213    LATIN CAPITAL LETTER E WITH DIAERESIS\n"
     "147   93   223   U+00cc   \303\214    LATIN CAPITAL LETTER I WITH GRAVE\n"
     "148   94   224   U+00cd   \303\215    LATIN CAPITAL LETTER I WITH ACUTE\n"
     "149   95   225   U+00ce   \303\216    LATIN CAPITAL LETTER I WITH CIRCUMFLEX\n"
     "150   96   226   U+00cf   \303\217    LATIN CAPITAL LETTER I WITH DIAERESIS\n"
     "151   97   227   U+00d1   \303\221    LATIN CAPITAL LETTER N WITH TILDE\n"
     "152   98   230   U+00d2   \303\222    LATIN CAPITAL LETTER O WITH GRAVE\n"
     "153   99   231   U+00d3   \303\223    LATIN CAPITAL LETTER O WITH ACUTE\n"
     "154   9a   232   U+00d4   \303\224    LATIN CAPITAL LETTER O WITH CIRCUMFLEX\n"
     "155   9b   233   U+00d5   \303\225    LATIN CAPITAL LETTER O WITH TILDE\n"
     "156   9c   234   U+00d6   \303\226    LATIN CAPITAL LETTER O WITH DIAERESIS\n"
     "157   9d   235   U+00d8   \303\230    LATIN CAPITAL LETTER O WITH STROKE\n"
     "158   9e   236   U+00d9   \303\231    LATIN CAPITAL LETTER U WITH GRAVE\n"
     "159   9f   237   U+00da   \303\232    LATIN CAPITAL LETTER U WITH ACUTE\n"
     "160   a0   240   U+00db   \303\233    LATIN CAPITAL LETTER U WITH CIRCUMFLEX\n"
     "161   a1   241   U+00dc   \303\234    LATIN CAPITAL LETTER U WITH DIAERESIS\n"
     "162   a2   242   U+00dd   \303\235    LATIN CAPITAL LETTER Y WITH ACUTE\n"
     "163   a3   243   U+00df   \303\237    LATIN SMALL LETTER SHARP S\n"
     "164   a4   244   U+00e0   \303\240    LATIN SMALL LETTER A WITH GRAVE\n"
     "165   a5   245   U+00e1   \303\241    LATIN SMALL LETTER A WITH ACUTE\n"
     "166   a6   246   U+00e2   \303\242    LATIN SMALL LETTER A WITH CIRCUMFLEX\n"
     "167   a7   247   U+00e3   \303\243    LATIN SMALL LETTER A WITH TILDE\n"
     "168   a8   250   U+00e4   \303\244    LATIN SMALL LETTER A WITH DIAERESIS\n"
     "169   a9   251   U+00e5   \303\245    LATIN SMALL LETTER A WITH RING ABOVE\n"
     "170   aa   252   U+00e7   \303\247    LATIN SMALL LETTER C WITH CEDILLA\n"
     "171   ab   253   U+00e8   \303\250    LATIN SMALL LETTER E WITH GRAVE\n"
     "172   ac   254   U+00e9   \303\251    LATIN SMALL LETTER E WITH ACUTE\n"
     "173   ad   255   U+00ea   \303\252    LATIN SMALL LETTER E WITH CIRCUMFLEX\n"
     "174   ae   256   U+00eb   \303\253    LATIN SMALL LETTER E WITH DIAERESIS\n"
     "175   af   257   U+00ec   \303\254    LATIN SMALL LETTER I WITH GRAVE\n"
     "176   b0   260   U+00ed   \303\255    LATIN SMALL LETTER I WITH ACUTE\n"
     "177   b1   261   U+00ee   \303\256    LATIN SMALL LETTER I WITH CIRCUMFLEX\n"
     "178   b2   262   U+00ef   \303\257    LATIN SMALL LETTER I WITH DIAERESIS\n"
     "179   b3   263   U+00f1   \303\261    LATIN SMALL LETTER N WITH TILDE\n"
     "180   b4   264   U+00f2   \303\262    LATIN SMALL LETTER O WITH GRAVE\n"
     "181   b5   265   U+00f3   \303\263    LATIN SMALL LETTER O WITH ACUTE\n"
     "182   b6   266   U+00f4   \303\264    LATIN SMALL LETTER O WITH CIRCUMFLEX\n"
     "183   b7   267   U+00f5   \303\265    LATIN SMALL LETTER O WITH TILDE\n"
     "184   b8   270   U+00f6   \303\266    LATIN SMALL LETTER O WITH DIAERESIS\n"
     "185   b9   271   U+00f8   \303\270    LATIN SMALL LETTER O WITH STROKE\n"
     "186   ba   272   U+00f9   \303\271    LATIN SMALL LETTER U WITH GRAVE\n"
     "187   bb   273   U+00fa   \303\272    LATIN SMALL LETTER U WITH ACUTE\n"
     "188   bc   274   U+00fb   \303\273    LATIN SMALL LETTER U WITH CIRCUMFLEX\n"
     "189   bd   275   U+00fc   \303\274    LATIN SMALL LETTER U WITH DIAERESIS\n"
     "190   be   276   U+00fd   \303\275    LATIN SMALL LETTER Y WITH ACUTE\n"
     "191   bf   277   U+00ff   \303\277    LATIN SMALL LETTER Y WITH DIAERESIS\n"
     "192   c0   300   U+0102   \304\202    LATIN CAPITAL LETTER A WITH BREVE\n"
     "193   c1   301   U+0103   \304\203    LATIN SMALL LETTER A WITH BREVE\n"
     "194   c2   302   U+0104   \304\204    LATIN CAPITAL LETTER A WITH OGONEK\n"
     "195   c3   303   U+0105   \304\205    LATIN SMALL LETTER A WITH OGONEK\n"
     "196   c4   304   U+0106   \304\206    LATIN CAPITAL LETTER C WITH ACUTE\n"
     "197   c5   305   U+0107   \304\207    LATIN SMALL LETTER C WITH ACUTE\n"
     "198   c6   306   U+010c   \304\214    LATIN CAPITAL LETTER C WITH CARON\n"
     "199   c7   307   U+010d   \304\215    LATIN SMALL LETTER C WITH CARON\n"
     "200   c8   310   U+010e   \304\216    LATIN CAPITAL LETTER D WITH CARON\n"
     "201   c9   311   U+010f   \304\217    LATIN SMALL LETTER D WITH CARON\n"
     "202   ca   312   U+0110   \304\220    LATIN CAPITAL LETTER D WITH STROKE\n"
     "203   cb   313   U+0111   \304\221    LATIN SMALL LETTER D WITH STROKE\n"
     "204   cc   314   U+0118   \304\230    LATIN CAPITAL LETTER E WITH OGONEK\n"
     "205   cd   315   U+0119   \304\231    LATIN SMALL LETTER E WITH OGONEK\n"
     "206   ce   316   U+011a   \304\232    LATIN CAPITAL LETTER E WITH CARON\n"
     "207   cf   317   U+011b   \304\233    LATIN SMALL LETTER E WITH CARON\n"
     "208   d0   320   U+0139   \304\271    LATIN CAPITAL LETTER L WITH ACUTE\n"
     "209   d1   321   U+013a   \304\272    LATIN SMALL LETTER L WITH ACUTE\n"
     "210   d2   322   U+013d   \304\275    LATIN CAPITAL LETTER L WITH CARON\n"
     "211   d3   323   U+013e   \304\276    LATIN SMALL LETTER L WITH CARON\n"
     "212   d4   324   U+0141   \305\201    LATIN CAPITAL LETTER L WITH STROKE\n"
     "213   d5   325   U+0142   \305\202    LATIN SMALL LETTER L WITH STROKE\n"
     "214   d6   326   U+0143   \305\203    LATIN CAPITAL LETTER N WITH ACUTE\n"
     "215   d7   327   U+0144   \305\204    LATIN SMALL LETTER N WITH ACUTE\n"
     "216   d8   330   U+0147   \305\207    LATIN CAPITAL LETTER N WITH CARON\n"
     "217   d9   331   U+0148   \305\210    LATIN SMALL LETTER N WITH CARON\n"
     "218   da   332   U+0150   \305\220    LATIN CAPITAL LETTER O WITH DOUBLE ACUTE\n"
     "219   db   333   U+0151   \305\221    LATIN SMALL LETTER O WITH DOUBLE ACUTE\n"
     "220   dc   334   U+0154   \305\224    LATIN CAPITAL LETTER R WITH ACUTE\n"
     "221   dd   335   U+0155   \305\225    LATIN SMALL LETTER R WITH ACUTE\n"
     "222   de   336   U+0158   \305\230    LATIN CAPITAL LETTER R WITH CARON\n"
     "223   df   337   U+0159   \305\231    LATIN SMALL LETTER R WITH CARON\n"
     "224   e0   340   U+015a   \305\232    LATIN CAPITAL LETTER S WITH ACUTE\n"
     "225   e1   341   U+015b   \305\233    LATIN SMALL LETTER S WITH ACUTE\n"
     "226   e2   342   U+015e   \305\236    LATIN CAPITAL LETTER S WITH CEDILLA\n"
     "227   e3   343   U+015f   \305\237    LATIN SMALL LETTER S WITH CEDILLA\n"
     "228   e4   344   U+0160   \305\240    LATIN CAPITAL LETTER S WITH CARON\n"
     "229   e5   345   U+0161   \305\241    LATIN SMALL LETTER S WITH CARON\n"
     "230   e6   346   U+0162   \305\242    LATIN CAPITAL LETTER T WITH CEDILLA\n"
     "231   e7   347   U+0163   \305\243    LATIN SMALL LETTER T WITH CEDILLA\n"
     "232   e8   350   U+0164   \305\244    LATIN CAPITAL LETTER T WITH CARON\n"
     "233   e9   351   U+0165   \305\245    LATIN SMALL LETTER T WITH CARON\n"
     "234   ea   352   U+016e   \305\256    LATIN CAPITAL LETTER U WITH RING ABOVE\n"
     "235   eb   353   U+016f   \305\257    LATIN SMALL LETTER U WITH RING ABOVE\n"
     "236   ec   354   U+0170   \305\260    LATIN CAPITAL LETTER U WITH DOUBLE ACUTE\n"
     "237   ed   355   U+0171   \305\261    LATIN SMALL LETTER U WITH DOUBLE ACUTE\n"
     "238   ee   356   U+0178   \305\270    LATIN CAPITAL LETTER Y WITH DIAERESIS\n"
     "239   ef   357   U+0179   \305\271    LATIN CAPITAL LETTER Z WITH ACUTE\n"
     "240   f0   360   U+017a   \305\272    LATIN SMALL LETTER Z WITH ACUTE\n"
     "241   f1   361   U+017b   \305\273    LATIN CAPITAL LETTER Z WITH DOT ABOVE\n"
     "242   f2   362   U+017c   \305\274    LATIN SMALL LETTER Z WITH DOT ABOVE\n"
     "243   f3   363   U+017d   \305\275    LATIN CAPITAL LETTER Z WITH CARON\n"
     "244   f4   364   U+017e   \305\276    LATIN SMALL LETTER Z WITH CARON\n"
     "245   f5   365   U+2010   \342\200\220    HYPHEN\n"
     "246   f6   366   U+2013   \342\200\223    EN DASH\n"
     "247   f7   367   U+2014   \342\200\224    EM DASH\n"
     "248   f8   370   U+2018   \342\200\230    LEFT SINGLE QUOTATION MARK\n"
     "249   f9   371   U+2019   \342\200\231    RIGHT SINGLE QUOTATION MARK\n"
     "250   fa   372   U+201b   \342\200\233    SINGLE HIGH-REVERSED-9 QUOTATION MARK\n"
     "251   fb   373   U+201d   \342\200\235    RIGHT DOUBLE QUOTATION MARK\n"
     "252   fc   374   U+201e   \342\200\236    DOUBLE LOW-9 QUOTATION MARK\n"
     "253   fd   375   U+2026   \342\200\246    HORIZONTAL ELLIPSIS\n"
     "254   fe   376   U+2212   \342\210\222    MINUS SIGN\n"
     "255   ff   377   U+fffd   \357\277\275    REPLACEMENT CHARACTER\n";

// clang-format on

BOOST_AUTO_TEST_CASE(print_character_table_test) {
     using std::ostringstream;
     using std::ios_base;
     using namespace PLP::Charset;
     ostringstream oss(ios_base::out | ios_base::binary);
     print_character_table(oss);
     BOOST_CHECK(oss.str() == print_character_table_result);
}

BOOST_AUTO_TEST_CASE(ordtab_test) {
     using PLP::Charset::get_ordtab;
     int const n = 256;
     std::vector<bool> b(n);
     auto t = get_ordtab();
     for (int i = 0; i < n; i++) {
          BOOST_CHECK(!b[t[i]]);
          b[t[i]] = true;
     }
}

BOOST_DATA_TEST_CASE(chrcmp_test, bdata::xrange(256), xr) {
     using PLP::Charset::chrcmp;
     using PLP::Charset::isalpha;
     int const n = 256;
     int const i = xr;
     BOOST_CHECK(chrcmp(i, i) == 0);
     if (!isalpha(i))
          for (int j = 0; j < n; j++)
               if (isalpha(j))
                    BOOST_CHECK(chrcmp(i, j) < 0);
}

constexpr char const* const isprint_order_test_result =
     " !\"#$%&'()*+,-./"
     "0123456789:;<=>?@[\\]^_`{|}~"
     "\302\240\302\247\302\251\302\253\302\256\302\260\302\261\302"
     "\273\342\200\220\342\200\223\342\200\224\342\200\230\342\200"
     "\231\342\200\233\342\200\235\342\200\236\342\200\246\342\210"
     "\222\357\277\275A\304\204\303\201\303\204\303\205\304\202\303"
     "\200\303\202\303\203a\304\205\303\241\303\244\303\245\304\203"
     "\303\240\303\242\303\243BbC\304\206\304\214\303\207c\304\207"
     "\304\215\303\247D\304\220\304\216d\304\221\304\217E\304\230\303"
     "\211\304\232\303\213\303\210\303\212e\304\231\303\251\304\233"
     "\303\253\303\250\303\252FfGgHhI\303\215\303\217\303\214\303\216"
     "i\303\255\303\257\303\254\303\256JjKkL\305\201\304\271\304\275l"
     "\305\202\304\272\304\276MmN\305\203\305\207\303\221n\305\204"
     "\305\210\303\261O\303\230\303\223\303\226\303\222\303\224\303"
     "\225\305\220o\303\270\303\263\303\266\303\262\303\264\303\265"
     "\305\221PpQqR\305\224\305\230r\305\225\305\231S\305\232\305\240"
     "\305\236s\305\233\305\241\305\237\303\237T\305\244\305\242t\305"
     "\245\305\243U\303\232\303\234\305\256\303\231\303\233\305\260u"
     "\303\272\303\274\305\257\303\271\303\273\305\261VvWwXxY\303\235"
     "\305\270y\303\275\303\277Z\305\271\305\273\305\275z\305\272\305"
     "\274\305\276";

constexpr char const* const isalpha_order_test_result =
     "A\304\204\303\201\303\204\303\205\304\202\303\200\303\202\303"
     "\203a\304\205\303\241\303\244\303\245\304\203\303\240\303\242"
     "\303\243BbC\304\206\304\214\303\207c\304\207\304\215\303\247D"
     "\304\220\304\216d\304\221\304\217E\304\230\303\211\304\232\303"
     "\213\303\210\303\212e\304\231\303\251\304\233\303\253\303\250"
     "\303\252FfGgHhI\303\215\303\217\303\214\303\216i\303\255\303"
     "\257\303\254\303\256JjKkL\305\201\304\271\304\275l\305\202\304"
     "\272\304\276MmN\305\203\305\207\303\221n\305\204\305\210\303"
     "\261O\303\230\303\223\303\226\303\222\303\224\303\225\305\220o"
     "\303\270\303\263\303\266\303\262\303\264\303\265\305\221PpQqR"
     "\305\224\305\230r\305\225\305\231S\305\232\305\240\305\236s\305"
     "\233\305\241\305\237\303\237T\305\244\305\242t\305\245\305\243U"
     "\303\232\303\234\305\256\303\231\303\233\305\260u\303\272\303"
     "\274\305\257\303\271\303\273\305\261VvWwXxY\303\235\305\270y"
     "\303\275\303\277Z\305\271\305\273\305\275z\305\272\305\274\305"
     "\276";

BOOST_AUTO_TEST_CASE(order_test) {
     using PLP::Charset::chrcmp;
     using PLP::Charset::isalpha;
     using PLP::Charset::isprint;
     using PLP::Charset::charset_to_utf8;

     auto const cmp = [](char lhs, char rhs) {
          return chrcmp(lhs, rhs) < 0;
     };
     std::string s;

     for (int i = 0; i < 256; i++)
          if (isprint(i))
               s.push_back(i);
     std::sort(s.begin(), s.end(), cmp);
     BOOST_CHECK(charset_to_utf8(s) == isprint_order_test_result);

     s.clear();
     for (int i = 0; i < 256; i++)
          if (isalpha(i))
               s.push_back(i);
     std::sort(s.begin(), s.end(), cmp);
     BOOST_CHECK(charset_to_utf8(s) == isalpha_order_test_result);
}

BOOST_AUTO_TEST_CASE(alpha_strcmp_test) {
     using PLP::Charset::alpha_strcmp;
     BOOST_CHECK(alpha_strcmp("", "") == 0);
     BOOST_CHECK(alpha_strcmp("a", "") > 0);
     BOOST_CHECK(alpha_strcmp("", "a") < 0);
     BOOST_CHECK(alpha_strcmp("a", "a") == 0);
     BOOST_CHECK(alpha_strcmp("\302\245", "\245\302") < 0);
}

BOOST_AUTO_TEST_CASE(boost_starts_ends_test) {
     using boost::starts_with;
     using boost::ends_with;
     std::string const s = "abcd";

     BOOST_CHECK(starts_with("", ""));
     BOOST_CHECK(!starts_with("", "a"));
     BOOST_CHECK(starts_with(s, "a"));
     BOOST_CHECK(starts_with(s, "ab"));
     BOOST_CHECK(starts_with(s, "abc"));
     BOOST_CHECK(starts_with(s, "abcd"));
     BOOST_CHECK(!starts_with(s, "abcde"));
     BOOST_CHECK(!starts_with(s, "x"));

     BOOST_CHECK(ends_with("", ""));
     BOOST_CHECK(!ends_with("", "a"));
     BOOST_CHECK(ends_with(s, "d"));
     BOOST_CHECK(ends_with(s, "cd"));
     BOOST_CHECK(ends_with(s, "bcd"));
     BOOST_CHECK(ends_with(s, "abcd"));
     BOOST_CHECK(!ends_with(s, "abcde"));
     BOOST_CHECK(!ends_with(s, "x"));
}

BOOST_AUTO_TEST_CASE(lowercase_uppercase_capitalize_test) {
     using PLP::Charset::lowercase;
     using PLP::Charset::uppercase;
     using PLP::Charset::capitalize;
     using PLP::Charset::utf8_to_charset;

     std::string s;
     lowercase(s);
     BOOST_CHECK(s == "");
     uppercase(s);
     BOOST_CHECK(s == "");
     capitalize(s);
     BOOST_CHECK(s == "");
     s = "aBcD";
     lowercase(s);
     BOOST_CHECK(s == "abcd");
     s = "aBcD";
     uppercase(s);
     BOOST_CHECK(s == "ABCD");
     s = "aBcD";
     capitalize(s);
     BOOST_CHECK(s == "Abcd");

     s = "1aBcD";
     lowercase(s);
     BOOST_CHECK(s == "1abcd");
     s = "1aBcD";
     uppercase(s);
     BOOST_CHECK(s == "1ABCD");
     s = "1aBcD";
     capitalize(s);
     BOOST_CHECK(s == "1abcd");

     auto const low =  // \k{a}\'{c}\k{e}{\l}\'{n}\'{o}\'{s}\'{z}\.{z}
          "\304\205\304\207\304\231\305\202\305\204\303\263\305\233"
          "\305\272\305\274";
     auto const upp =  // \k{A}\'{C}\k{E}{\L}\'{N}\'{O}\'{S}\'{Z}\.{Z}
          "\304\204\304\206\304\230\305\201\305\203\303\223\305\232"
          "\305\271\305\273";
     auto const cap =  // \k{A}\'{c}\k{e}{\l}\'{n}\'{o}\'{s}\'{z}\.{z}
          "\304\204\304\207\304\231\305\202\305\204\303\263\305\233"
          "\305\272\305\274";
     auto const mix =  // \k{A}\'{c}\k{E}{\l}\'{N}\'{o}\'{S}\'{z}\.{Z}
          "\304\204\304\207\304\230\305\202\305\203\303\263\305\232"
          "\305\272\305\273";

     s = utf8_to_charset(mix);
     lowercase(s);
     BOOST_CHECK(s == utf8_to_charset(low));
     s = utf8_to_charset(mix);
     uppercase(s);
     BOOST_CHECK(s == utf8_to_charset(upp));
     s = utf8_to_charset(mix);
     capitalize(s);
     BOOST_CHECK(s == utf8_to_charset(cap));
}

BOOST_AUTO_TEST_CASE(is_proper_prefix_suffix_test) {
     using PLP::Charset::is_proper_prefix;
     using PLP::Charset::is_proper_suffix;

     BOOST_CHECK(is_proper_prefix("niebrzydki", ""));
     BOOST_CHECK(is_proper_prefix("niebrzydki", "nie"));
     BOOST_CHECK(!is_proper_prefix("niebrzydki", "nia"));
     BOOST_CHECK(!is_proper_prefix("nie", "nie"));

     BOOST_CHECK(is_proper_suffix("niebrzydki", ""));
     BOOST_CHECK(is_proper_suffix("niebrzydki", "dki"));
     BOOST_CHECK(!is_proper_suffix("niebrzydki", "aki"));
     BOOST_CHECK(!is_proper_suffix("nie", "nie"));
}

BOOST_AUTO_TEST_CASE(completeness_test) {
     using namespace PLP;
     for (int i = 0; i < 256; i++) {
          bool const a = Charset::isalpha(i);
          bool const b = Charset::islower(i);
          bool const c = Charset::isupper(i);
          BOOST_CHECK(a == b || c);
     }
}

BOOST_AUTO_TEST_CASE(is_lower_test) {
     using PLP::Charset::is_lower;
     BOOST_CHECK(is_lower(""));
     BOOST_CHECK(is_lower("a"));
     BOOST_CHECK(is_lower("abc"));
     BOOST_CHECK(!is_lower("Abc"));
     BOOST_CHECK(!is_lower("aBc"));
     BOOST_CHECK(!is_lower("abC"));
     BOOST_CHECK(is_lower("\303\305\315"));   // ace
     BOOST_CHECK(!is_lower("\302\305\315"));  // Ace
     BOOST_CHECK(!is_lower("\303\304\315"));  // aCe
     BOOST_CHECK(!is_lower("\303\305\314"));  // acE
}

BOOST_AUTO_TEST_CASE(is_upper_test) {
     using PLP::Charset::is_upper;
     BOOST_CHECK(is_upper(""));
     BOOST_CHECK(is_upper("A"));
     BOOST_CHECK(is_upper("ABC"));
     BOOST_CHECK(!is_upper("aBC"));
     BOOST_CHECK(!is_upper("AbC"));
     BOOST_CHECK(!is_upper("ABc"));
     BOOST_CHECK(is_upper("\302\304\314"));   // ACE
     BOOST_CHECK(!is_upper("\303\304\314"));  // aCE
     BOOST_CHECK(!is_upper("\302\305\314"));  // AcE
     BOOST_CHECK(!is_upper("\302\304\315"));  // ACe
}

BOOST_AUTO_TEST_CASE(is_capit_test) {
     using PLP::Charset::is_capit;
     BOOST_CHECK(is_capit(""));
     BOOST_CHECK(is_capit("A"));
     BOOST_CHECK(is_capit("Abc"));
     BOOST_CHECK(!is_capit("ABc"));
     BOOST_CHECK(!is_capit("AbC"));
     BOOST_CHECK(!is_capit("ABC"));
     BOOST_CHECK(is_capit("\302"));           // A
     BOOST_CHECK(is_capit("\302\305\315"));   // Ace
     BOOST_CHECK(!is_capit("\302\304\315"));  // ACe
     BOOST_CHECK(!is_capit("\302\305\314"));  // AcE
     BOOST_CHECK(!is_capit("\302\304\314"));  // ACE
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
