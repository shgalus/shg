/**
 * \file src/nkjp.cc
 * National Corpus of Polish processing.
 */

#include <shg/nkjp.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>

namespace SHG::PLP {

void NKJP::transform(Path const& ipath, Path const& opath) {
     ipath_ = ipath;
     opath_ = opath;
     collect_file_names();
     std::cout << file_names_.size() << '\n';
     for (Vecstring::size_type k = 0; k < file_names_.size(); k++)
          transform_one_file(k);
}

std::string const NKJP::srcfn{"ann_morphosyntax.xml"};

NKJP::Path NKJP::ofpath(Vecstring::size_type k) {
     std::string s{std::to_string(k + 1)};
     if (s.size() < 4)
          s.insert(0, 4 - s.size(), '0');
     return opath_ / (s + ".txt");
}

void NKJP::collect_file_names() {
     using std::filesystem::recursive_directory_iterator;
     using boost::ends_with;

     file_names_.clear();
     for (auto const& entry : recursive_directory_iterator{ipath_})
          if (entry.is_regular_file())
               if (ends_with(entry.path().native(), srcfn))
                    file_names_.push_back(entry.path().string());
     std::sort(file_names_.begin(), file_names_.end());
}

void NKJP::transform_one_file(Vecstring::size_type k) {
     using std::ios_base, std::ifstream, std::ofstream;
     using std::runtime_error;
     using namespace pugi;

     std::ifstream f(file_names_[k], ios_base::in | ios_base::binary);
     if (!f)
          throw runtime_error("could not open input file");
     std::ofstream g(ofpath(k), ios_base::out | ios_base::binary);
     if (!g)
          throw runtime_error("could not open ouput file");
     xml_document doc;
     xml_parse_result const result = doc.load(f);
     if (!result)
          throw runtime_error("negative parse result");
     xml_node const body = doc.child("teiCorpus")
                                .child("TEI")
                                .child("text")
                                .child("body");

     for (xml_node paragraph = body.first_child(); paragraph;
          paragraph = paragraph.next_sibling()) {
          for (xml_node sentence = paragraph.first_child(); sentence;
               sentence = sentence.next_sibling()) {
               for (xml_node segment = sentence.first_child();
                    segment; segment = segment.next_sibling()) {
                    transform_one_segment(segment, g);
               }
               g << "EOS\n";
          }
          g << "EOP\n";
     }
}

void NKJP::transform_one_segment(pugi::xml_node const& segment,
                                 std::ofstream& g) {
     using std::strcmp;
     using std::strchr;
     using std::runtime_error;
     using namespace pugi;

     xml_node nd = segment.first_child();

     if (strcmp(nd.attribute("type").value(), "morph") != 0)
          throw runtime_error("<fs type=\"morph\"> expected");
     nd = nd.first_child();
     if (strcmp(nd.attribute("name").value(), "orth") != 0)
          throw runtime_error("<f name=\"orth\"> expected");
     char const* orth = nd.child_value("string");
     if (*orth == '\0')
          throw runtime_error("empty orth string");
     if (strchr(orth, '\t') != nullptr)
          throw runtime_error("orth contains a tab");
     nd = nd.next_sibling();
     int n = 0;
     do {
          if (++n > 2)
               throw runtime_error("<f name=\"disamb\"> expected");
          nd = nd.next_sibling();
     } while (strcmp(nd.attribute("name").value(), "disamb") != 0);
     nd = nd.first_child().first_child().next_sibling();
     if (strcmp(nd.attribute("name").value(), "interpretation") != 0)
          throw runtime_error("<f name=\"interpretation\"> expected");

     char const* interpretation = nd.child_value("string");
     if (*interpretation == '\0')
          throw runtime_error("empty interpretation string");
     // There is a '\t' character in the file
     // 620-3-010001854/ann_morphosyntax.xml.
     if (strcmp(interpretation,
                " \t http://www.fpp.co.uk/books/Goebbels/"
                "Goebbels.pdf:subst:sg:nom:m3") == 0) {
          interpretation += 3;
     }
     if (strchr(interpretation, '\t') != nullptr) {
          // std::cout << "*** \"" << interpretation << "\" ***\n";
          throw runtime_error("interpretation contains a tab");
     }

     g << orth << '\t' << interpretation << '\n';
}

}  // namespace SHG::PLP
