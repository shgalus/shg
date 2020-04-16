#include <algorithm>
#include <sstream>
#include "shg/csv.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

struct Test_data {
     std::vector<std::vector<std::string>> raw;
     std::string csv;
};

const std::vector<Test_data> test_data {
     {
          {
               {"10", "20", "30"},
               {"40", "50", "60"},
               {"70", "80", "90"}
          },
          {"10,20,30\r\n40,50,60\r\n70,80,90\r\n"}
     },
     {
          {
               {"10", ",abc", "30"},
               {"40", "50", "60"},
               {"70", "80", "90"}
          },
          {"10,\",abc\",30\r\n40,50,60\r\n70,80,90\r\n"}
     },
     {
          {
               {"10", ",a\r\nbc", "30"},
               {"40", "50", "60"},
               {"70", "80", "90"}
          },
          {"10,\",a\r\nbc\",30\r\n40,50,60\r\n70,80,90\r\n"}
     },
     {
          {
               {"1\"0", "2\"0", "30"},
               {"40", "50", "60"},
               {"70", "80", "90"}
          },
          {"\"1\"\"0\",\"2\"\"0\",30\r\n40,50,60\r\n70,80,90\r\n"}
     },
     {
          {
               {"1\"0", "2\"\"0", "30"},
               {"40", "50", "60"},
               {"70", "80", "90"}
          },
          {"\"1\"\"0\",\"2\"\"\"\"0\",30\r\n40,50,60\r\n70,80,90\r\n"}
     },
     {
          {
               {"10", "20", "30"},
               {"40", "a,b\"c\rd\ne\"\"", "60"},
               {"70", "80", "90"}
          },
          {"10,20,30\r\n40,\"a,b\"\"c\rd\ne\"\"\"\"\",60\r\n70,80,90"
           "\r\n"}
     },
     {
          {
               {"10", "20", "30"},
               {"40", "", ""},
               {"70", "80", "90"}
          },
          {"10,20,30\r\n40,,\r\n70,80,90\r\n"}
     },
     {
          {
               {"{\r\n"
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
                "}\r\n"}
          },
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
           "\"\r\n"}
     },
     {
          {
               {"10", "20", "30"},
               {"", "", ""},
               {"70", "80", "90"}
          },
          {"10,20,30\r\n,,\r\n70,80,90\r\n"}
     },
     {
          {
               {"10", "20", "30"},
               {},
               {"70", "80", "90"}
          },
          {"10,20,30\r\n70,80,90\r\n"}
     },
};

void test_csv_writer() {
     std::ostringstream ss(std::ios_base::out | std::ios_base::binary);
     CSV_writer csvw(ss);
     for (const auto& td : test_data) {
          ss.str("");
          for (const auto& r : td.raw)
               csvw.write_record(r);
          SHG_ASSERT(ss.str() == td.csv);
     }
}

void test_csv_reader() {
     using std::equal;
     for (const auto& td : test_data) {
          std::istringstream ss(
               td.csv,
               std::ios_base::in | std::ios_base::binary);
          CSV_reader csvr(ss);
          std::vector<std::vector<std::string>>::size_type k = 0;
          std::vector<std::string> v;
          for (;;) {
               csvr.getrec(v);
               if (v.size() == 0)
                    break;
               while (k < td.raw.size() && td.raw[k].size() == 0)
                    k++;
               if (k == td.raw.size())
                    break;
               SHG_ASSERT(k < td.raw.size());
               SHG_ASSERT(v.size() == td.raw[k].size());
               SHG_ASSERT(equal(v.cbegin(), v.cend(),
                                td.raw[k].cbegin()));
               k++;
          }
          SHG_ASSERT(k == td.raw.size());
     }
}

void test_csv_reader_special() {
     std::istringstream ss(std::ios_base::in | std::ios_base::binary);
     CSV_reader csvr(ss);
     std::vector<std::string> v;
     ss.str("");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 0);

     ss.clear();
     ss.str("a");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 1);
     SHG_ASSERT(v[0] == "a");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 0);

     ss.clear();
     ss.str("\r\n");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 1);
     SHG_ASSERT(v[0] == "");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 0);

     ss.clear();
     ss.str(",\r\n");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 2);
     SHG_ASSERT(v[0] == "");
     SHG_ASSERT(v[1] == "");
     csvr.getrec(v);
     SHG_ASSERT(v.size() == 0);
}

}       // anonymous namespace

void test_csv() {
     test_csv_writer();
     test_csv_reader();
     test_csv_reader_special();
}

}       // namespace Testing
}       // namespace SHG
