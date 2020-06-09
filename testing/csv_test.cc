#include "shg/csv.h"
#include <algorithm>
#include <ios>
#include <sstream>
#include "testing.h"

namespace SHG::BTesting {

using std::equal;
using std::ios_base;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::vector;

namespace {

struct Case {
     const vector<vector<string>> raw;
     const string csv;
};

const vector<Case> vc{
     {{{"10", "20", "30"}, {"40", "50", "60"}, {"70", "80", "90"}},
      {"10,20,30\r\n40,50,60\r\n70,80,90\r\n"}},
     {{{"10", ",abc", "30"}, {"40", "50", "60"}, {"70", "80", "90"}},
      {"10,\",abc\",30\r\n40,50,60\r\n70,80,90\r\n"}},
     {{{"10", ",a\r\nbc", "30"},
       {"40", "50", "60"},
       {"70", "80", "90"}},
      {"10,\",a\r\nbc\",30\r\n40,50,60\r\n70,80,90\r\n"}},
     {{{"1\"0", "2\"0", "30"},
       {"40", "50", "60"},
       {"70", "80", "90"}},
      {"\"1\"\"0\",\"2\"\"0\",30\r\n40,50,60\r\n70,80,90\r\n"}},
     {{{"1\"0", "2\"\"0", "30"},
       {"40", "50", "60"},
       {"70", "80", "90"}},
      {"\"1\"\"0\",\"2\"\"\"\"0\",30\r\n40,50,60\r\n70,80,90\r\n"}},
     {{{"10", "20", "30"},
       {"40", "a,b\"c\rd\ne\"\"", "60"},
       {"70", "80", "90"}},
      {"10,20,30\r\n40,\"a,b\"\"c\rd\ne\"\"\"\"\",60\r\n70,80,90"
       "\r\n"}},
     {{{"10", "20", "30"}, {"40", "", ""}, {"70", "80", "90"}},
      {"10,20,30\r\n40,,\r\n70,80,90\r\n"}},
     {{{"{\r\n"
        "  \"title\": \"Films\",\r\n"
        "  [\r\n"
        "    {\r\n"
        "      \"director\": \"Jerzy Antczak\",\r\n"
        "      \"title\": \"Noce i dnie\",\r\n"
        "      \"year\": 1975\r\n"
        "    },\r\n"
        "    {\r\n"
        "      \"director\": \"Andrzej Wajda\",\r\n"
        "      \"title\": \"Korczak\",\r\n"
        "      \"year\": 1990\r\n"
        "    },\r\n"
        "    {\r\n"
        "      \"director\": \"Wojciech Marczewski\",\r\n"
        "      \"title\": \"Zmory\",\r\n"
        "      \"year\": 1978\r\n"
        "    }\r\n"
        "  ]\r\n"
        "}\r\n"}},
      {"\"{\r\n"
       "  \"\"title\"\": \"\"Films\"\",\r\n"
       "  [\r\n"
       "    {\r\n"
       "      \"\"director\"\": \"\"Jerzy Antczak\"\",\r\n"
       "      \"\"title\"\": \"\"Noce i dnie\"\",\r\n"
       "      \"\"year\"\": 1975\r\n"
       "    },\r\n"
       "    {\r\n"
       "      \"\"director\"\": \"\"Andrzej Wajda\"\",\r\n"
       "      \"\"title\"\": \"\"Korczak\"\",\r\n"
       "      \"\"year\"\": 1990\r\n"
       "    },\r\n"
       "    {\r\n"
       "      \"\"director\"\": \"\"Wojciech Marczewski\"\",\r\n"
       "      \"\"title\"\": \"\"Zmory\"\",\r\n"
       "      \"\"year\"\": 1978\r\n"
       "    }\r\n"
       "  ]\r\n"
       "}\r\n"
       "\"\r\n"}},
     {{{"10", "20", "30"}, {"", "", ""}, {"70", "80", "90"}},
      {"10,20,30\r\n,,\r\n70,80,90\r\n"}},
};

}  // anonymous namespace

BOOST_AUTO_TEST_SUITE(csv_test)

BOOST_AUTO_TEST_CASE(csv_writer_test) {
     ostringstream ss(ios_base::out | ios_base::binary);
     CSV_writer csvw(ss);
     for (const auto& td : vc) {
          ss.str("");
          for (const auto& r : td.raw)
               csvw.write_record(r);
          BOOST_CHECK(ss.str() == td.csv);
     }
}

BOOST_AUTO_TEST_CASE(csv_reader_test) {
     for (const auto& c : vc) {
          istringstream ss(c.csv, ios_base::in | ios_base::binary);
          CSV_reader csvr(ss);
          vector<vector<string>>::size_type k = 0;
          vector<string> v;
          for (;;) {
               csvr.getrec(v);
               if (v.size() == 0)
                    break;
               while (k < c.raw.size() && c.raw[k].size() == 0)
                    k++;
               if (k == c.raw.size())
                    break;
               BOOST_CHECK(k < c.raw.size());
               BOOST_CHECK(v.size() == c.raw[k].size());
               BOOST_CHECK(
                    equal(v.cbegin(), v.cend(), c.raw[k].cbegin()));
               k++;
          }
          BOOST_CHECK(k == c.raw.size());
     }
}

BOOST_AUTO_TEST_CASE(empty_input_test) {
     istringstream ss(ios_base::in | ios_base::binary);
     CSV_reader csvr(ss);
     vector<string> v;
     ss.str("");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 0);
}

BOOST_AUTO_TEST_CASE(without_end_of_record_test) {
     istringstream ss(ios_base::in | ios_base::binary);
     CSV_reader csvr(ss);
     vector<string> v;
     ss.str("a");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 1);
     BOOST_CHECK(v[0] == "a");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 0);
}

BOOST_AUTO_TEST_CASE(only_end_of_record_test) {
     istringstream ss(ios_base::in | ios_base::binary);
     CSV_reader csvr(ss);
     vector<string> v;
     ss.str("\r\n");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 1);
     BOOST_CHECK(v[0] == "");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 0);
}

BOOST_AUTO_TEST_CASE(field_separator_followed_by_end_of_record_test) {
     istringstream ss(ios_base::in | ios_base::binary);
     CSV_reader csvr(ss);
     vector<string> v;
     ss.str(",\r\n");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 2);
     BOOST_CHECK(v[0] == "");
     BOOST_CHECK(v[1] == "");
     csvr.getrec(v);
     BOOST_CHECK(v.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
