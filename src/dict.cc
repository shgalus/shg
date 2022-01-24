/**
 * \file src/dict.cc
 * Polish dictionary implementation.
 */

#include <shg/dict_impl.h>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <algorithm>
#include <limits>
#include <shg/utils.h>
#include <shg/charset.h>
#include <shg/encoding.h>

namespace SHG::PLP {

Dictionary_error::Dictionary_error()
     : std::runtime_error("internal error") {}

Dictionary_error::Dictionary_error(char const* message)
     : std::runtime_error(message) {}

Dictionary_error::Dictionary_error(char const* message,
                                   unsigned long line)
     : std::runtime_error(std::string(message) + ", line " +
                          std::to_string(line)) {}

int catcmp(Category const& c1, Category const& c2) {
     if (c1.part_of_speech < c2.part_of_speech)
          return -1;
     if (c1.part_of_speech > c2.part_of_speech)
          return 1;
     if (c1.inflexion < c2.inflexion)
          return -1;
     if (c1.inflexion > c2.inflexion)
          return 1;
     if (c1.declension_case < c2.declension_case)
          return -1;
     if (c1.declension_case > c2.declension_case)
          return 1;
     if (c1.number < c2.number)
          return -1;
     if (c1.number > c2.number)
          return 1;
     if (c1.gender < c2.gender)
          return -1;
     if (c1.gender > c2.gender)
          return 1;
     if (c1.degree < c2.degree)
          return -1;
     if (c1.degree > c2.degree)
          return 1;
     if (c1.aspect < c2.aspect)
          return -1;
     if (c1.aspect > c2.aspect)
          return 1;
     if (c1.mood < c2.mood)
          return -1;
     if (c1.mood > c2.mood)
          return 1;
     if (c1.tense < c2.tense)
          return -1;
     if (c1.tense > c2.tense)
          return 1;
     if (c1.person < c2.person)
          return -1;
     if (c1.person > c2.person)
          return 1;
     if (c1.form_of_verb < c2.form_of_verb)
          return -1;
     if (c1.form_of_verb > c2.form_of_verb)
          return 1;
     if (c1.type_of_pronoun < c2.type_of_pronoun)
          return -1;
     if (c1.type_of_pronoun > c2.type_of_pronoun)
          return 1;
     if (c1.type_of_numeral < c2.type_of_numeral)
          return -1;
     if (c1.type_of_numeral > c2.type_of_numeral)
          return 1;
     return 0;
}

bool operator==(Description const& d1, Description const& d2) {
     return d1.main_form == d2.main_form &&
            d1.category == d2.category;
}

Dictionary::Dictionary() : pimpl_(new Impl) {}

Dictionary::~Dictionary() = default;

Dictionary::Dictionary(Dictionary&& other) : Dictionary() {
     pimpl_.swap(other.pimpl_);
}

Dictionary& Dictionary::operator=(Dictionary&& other) {
     pimpl_.swap(other.pimpl_);
     return *this;
}

void Dictionary::load_source_word_file(std::istream& input) {
     try {
          pimpl_->load_source_word_file(input);
     } catch (Dictionary_error const&) {
          pimpl_->clear();
          throw;
     }
}

void Dictionary::write_source_word_file(std::ostream& output,
                                        bool do_sort) const {
     pimpl_->write_source_word_file(output, do_sort);
}

void Dictionary::add_source_word_file(std::istream& input) {
     pimpl_->add_source_word_file(input);
}

void Dictionary::load_word_file(std::istream& input) {
     try {
          pimpl_->load_word_file(input);
     } catch (Dictionary_error const&) {
          pimpl_->clear();
          throw;
     }
}

void Dictionary::write_word_file(std::ostream& output) const {
     pimpl_->write_word_file(output);
}

bool Dictionary::has_entry_utf8(char const* s) const {
     return pimpl_->has_entry_utf8(s);
}

void Dictionary::search_utf8(char const* s, Setdesc& sd) const {
     pimpl_->search_utf8(s, sd);
}

bool Dictionary::has_entry_charset(char const* s) const {
     return pimpl_->has_entry_charset(s);
}

void Dictionary::search_charset(char const* s, Setdesc& sd) const {
     pimpl_->search_charset(s, sd);
}

std::vector<Dictionary::Table_row> Dictionary::report() const {
     return pimpl_->report();
}

Invalid_word_file::Invalid_word_file()
     : Dictionary_error("invalid word file") {}

bool operator==(Entry const& a, Entry const& b) {
     return a.type == b.type && a.stem == b.stem &&
            a.ending_index == b.ending_index;
}

bool cmp(Entry const& a, Entry const& b) {
     int const r = std::strcmp(a.stem.c_str(), b.stem.c_str());
     if (r > 0)
          return true;
     if (r < 0)
          return false;
     if (a.type < b.type)
          return true;
     if (a.type > b.type)
          return false;
     if (a.ending_index < b.ending_index)
          return true;
     return false;
}

int number_of_forms(Inflexion inflexion) {
     switch (inflexion) {
     case Inflexion::uninflected:
          return 1;
     case Inflexion::substantival:
     case Inflexion::numeral:
          return 14;
     case Inflexion::adjectival:
     case Inflexion::conjugation:
          return 42;
     case Inflexion::numeral_1:
     case Inflexion::numeral_2:
          return 28;
     case Inflexion::by_case:
          return 7;
     case Inflexion::by_gender:
          return 2;
     default:
          throw Dictionary_error();
     }
}

std::ostream& put(std::string const& s, std::ostream& stream) {
     return stream << s << '\n';
}

std::istream& get(std::string& s, std::istream& stream) {
     return std::getline(stream, s);
}

std::ostream& put(unsigned long n, std::ostream& stream) {
     return stream << n << '\n';
}

std::istream& get(unsigned long& n, std::istream& stream) {
     std::string s;
     char* str_end;
     std::getline(stream, s);
     if (s.empty())
          throw Invalid_word_file();
     for (auto c : s)
          if (!std::isdigit(static_cast<unsigned char>(c)))
               throw Invalid_word_file();
     if (s.size() > 1 && s[0] == '0')
          throw Invalid_word_file();
     errno = 0;
     n = std::strtoul(s.c_str(), &str_end, 10);
     if (*str_end != '\0' || errno == ERANGE)
          throw Invalid_word_file();
     return stream;
}

Entry_type_map init_entry_type_map() {
     Entry_type_map m;
     for (Type_index i = 0; i < nenttypes; i++) {
          auto const [k, status] = m.insert({entry_type[i].name, i});
          if (!status)
               throw Dictionary_error();
     }
     return m;
}

Entry_type_map const& entry_type_map() {
     static auto const m = init_entry_type_map();
     return m;
}

std::string lcp(std::vector<std::string>& v) {
     std::string prefix;
     std::vector<std::string>::size_type i, j;
     for (i = 0; i < v.size(); i++)
          if (!v[i].empty())
               break;
     assert(i < v.size());  // guaranteed by get_raw_entry()
     std::string::size_type len = v[i].size();
     std::string::size_type k, m;
     for (j = i + 1; j < v.size(); j++)
          if (!v[j].empty()) {
               m = std::max(len, v[j].size());
               for (k = 0; k < m && v[i][k] == v[j][k]; k++)
                    ;
               if (k < len)
                    len = k;
          }
     prefix = v[i].substr(0, len);
     for (i = 0; i < v.size(); i++)
          if (v[i].empty())
               v[i] = "-";
          else
               v[i].erase(0, len);
     return prefix;
}

void Dictionary::Impl::load_source_word_file(std::istream& input) {
     clear();
     add_words(input);
}

void Dictionary::Impl::write_source_word_file(std::ostream& output,
                                              bool do_sort) const {
     std::vector<Ent_tab_szt> srt(enttab_.size());
     for (Ent_tab_szt i = 0; i < enttab_.size(); i++)
          srt[i] = i;
     Entry_type const* const et = entry_type;

     // Comparing function for sorting enttab_ indirectly.
     auto const cmp = [et, this](Ent_tab_szt a, Ent_tab_szt b) {
          using Charset::alpha_strcmp;
          int r;
          Ending_index const aei = enttab_[a].ending_index;
          Ending_index const bei = enttab_[b].ending_index;
          auto const& as = enttab_[a].stem;
          auto const& bs = enttab_[b].stem;
          Inflexion const ai = et[enttab_[a].type].category.inflexion;
          Inflexion const bi = et[enttab_[b].type].category.inflexion;
          int const an = number_of_forms(ai);
          int const bn = number_of_forms(bi);
          int const aii = inflexion_to_index(ai);
          int const bii = inflexion_to_index(bi);
          std::string const mf_a = main_form(as, an, aii, aei);
          std::string const mf_b = main_form(bs, bn, bii, bei);
          r = alpha_strcmp(mf_a.c_str(), mf_b.c_str());
          if (r != 0)
               return r < 0;
          if (enttab_[a].type != enttab_[b].type)
               return enttab_[a].type < enttab_[b].type;
          assert(ai == bi);
          assert(an == bn);
          Set_tab const& ast = endtab_[aii][aei];
          Set_tab const& bst = endtab_[bii][bei];
          for (int i = 0; i < an; i++) {
               auto const af = as + ast[i];
               auto const bf = bs + bst[i];
               r = alpha_strcmp(af.c_str(), bf.c_str());
               if (r != 0)
                    return r < 0;
          }
          throw Dictionary_error();
     };
     if (do_sort)
          std::sort(srt.begin(), srt.end(), cmp);
     std::string s;
     for (Ent_tab_szt i = 0; i < enttab_.size(); i++) {
          auto const& e = enttab_[srt[i]];
          s = entry_type[e.type].name;
          put(s, output);
          Inflexion const infl =
               entry_type[e.type].category.inflexion;
          int const nof = number_of_forms(infl);
          for (int i = 0; i < nof; i++) {
               auto const& ee = endtab_[inflexion_to_index(infl)]
                                       [e.ending_index][i];
               if (ee == "-")
                    s = "-";
               else
                    s = e.stem + ee;
               put(Charset::charset_to_utf8(s), output);
          }
          put("", output);
     }
}

void Dictionary::Impl::add_source_word_file(std::istream& input) {
     add_words(input);
}

void Dictionary::Impl::load_word_file(std::istream& input) {
     clear();
     if (!input.good())
          throw Invalid_word_file();
     unsigned long nent, n;
     get(nent, input);
     for (Ent_tab_szt i = 0; i < nent; i++) {
          Entry e;
          get(n, input);
          if (n >= nenttypes)
               throw Invalid_word_file();
          e.type = n;
          get(e.stem, input);
          get(n, input);
          if (n > std::numeric_limits<Ending_index>::max())
               throw Invalid_word_file();
          e.ending_index = n;
          enttab_.push_back(e);
     }
     if (!std::is_sorted(enttab_.begin(), enttab_.end(), cmp))
          throw Invalid_word_file();
     for (End_tab_szt i = 0; i < endtab_.size(); i++) {
          Inf_tab& a = endtab_[i];
          Set_tab_szt const nn = sz[i];
          get(nent, input);  // TODO check
          a.resize(nent);
          for (Inf_tab_szt j = 0; j < a.size(); j++) {
               for (Set_tab_szt k = 0; k < nn; k++) {
                    std::string s;
                    get(s, input);
                    endtab_[i][j].push_back(s);
               }
          }
     }
     for (auto const& e : enttab_) {
          Inflexion const infl =
               entry_type[e.type].category.inflexion;
          int const ind = inflexion_to_index(infl);
          if (e.ending_index >= endtab_[ind].size())
               throw Invalid_word_file();
     }
}

void Dictionary::Impl::write_word_file(std::ostream& output) const {
     put(enttab_.size(), output);
     for (auto const& e : enttab_) {
          put(e.type, output);  // TODO: unify all index values
                                // as unsigned long
          put(e.stem, output);
          put(e.ending_index, output);
     }
     for (End_tab_szt i = 0; i < endtab_.size(); i++) {
          Inf_tab const& a = endtab_[i];
          put(a.size(), output);
          for (auto const& b : a)
               for (auto const& c : b)
                    put(c, output);
     }
}

void Dictionary::Impl::clear() {
     enttab_.clear();
     for (auto& v : endtab_)
          v.clear();
}

bool Dictionary::Impl::has_entry_utf8(char const* s) const {
     std::string t;
     try {
          t = Charset::utf8_to_charset(s);
     } catch (Encoding::Conversion_error const&) {
          return false;
     } catch (Charset::Invalid_character_error const&) {
          return false;
     }
     return find(t.c_str(), static_cast<Setdesc*>(nullptr));
}

void Dictionary::Impl::search_utf8(char const* s, Setdesc& sd) const {
     std::string t;
     try {
          t = Charset::utf8_to_charset(s);
          find(t.c_str(), &sd);
     } catch (Encoding::Conversion_error const&) {
     } catch (Charset::Invalid_character_error const&) {
     }
}

bool Dictionary::Impl::has_entry_charset(char const* s) const {
     return find(s, static_cast<Setdesc*>(nullptr));
}

void Dictionary::Impl::search_charset(char const* s,
                                      Setdesc& sd) const {
     find(s, &sd);
}

std::vector<Dictionary::Table_row> Dictionary::Impl::report() const {
     std::vector<Table_row> v(nenttypes);
     for (Type_index i = 0; i < nenttypes; i++)
          v[i].entry_type = entry_type[i].name;
     for (auto const& e : enttab_) {
          assert(e.type < nenttypes);
          v[e.type].nentries++;
     }
     return v;
}

void Dictionary::Impl::add_words(std::istream& input) {
     Entry ent;
     input_ = &input;
     lineno_ = 0;

     while (get_raw_entry()) {
          if (enttab_.size() ==
              std::numeric_limits<unsigned long>::max())
               throw Dictionary_error("too many entries", lineno_);
          ent.type = entry_type_;
          ent.stem = lcp(forms_);
          int const p = inflexion_to_index(inflexion_);
          ent.ending_index = insert_ending(p, forms_);
          enttab_.push_back(std::move(ent));
     }
     sort_entries();
     remove_duplicate_entries();
}

bool Dictionary::Impl::find(char const* s, Setdesc* sd) const {
     std::string const u{s};
     Inflexion infl;
     int inflind;
     int nof;
     Description desc;
     std::string pref, suff;
     Entry e;

     auto const cmp = [](Entry const& e1, Entry const& e2) {
          return std::strcmp(e1.stem.c_str(), e2.stem.c_str()) > 0;
     };

     for (std::string::size_type i = 0; i <= u.size(); i++) {
          pref = u.substr(0, i);
          suff = u.substr(i);
          e.stem = pref;
          auto it = std::lower_bound(enttab_.cbegin(), enttab_.cend(),
                                     e, cmp);
          while (it != enttab_.cend() && it->stem == pref) {
               infl = entry_type[it->type].category.inflexion;
               nof = number_of_forms(infl);
               inflind = inflexion_to_index(infl);
               for (int f = 0; f < nof; f++) {
                    std::string const& ee =
                         endtab_[inflind][it->ending_index][f];
                    if (ee != "-" && it->stem + ee == u) {
                         if (sd == nullptr)
                              return true;
                         desc.category =
                              entry_type[it->type].category;
                         complete_category(desc.category, infl, f);
                         desc.main_form = Charset::charset_to_utf8(
                              main_form(pref, nof, inflind,
                                        it->ending_index));
                         sd->insert(std::move(desc));
                    }
               }
               it = std::lower_bound(++it, enttab_.cend(), e, cmp);
          }
     }
     return false;
}

Ending_index Dictionary::Impl::insert_ending(int a,
                                             Set_tab const& v) {
     Inf_tab& e = endtab_[a];
     Ending_index k;
     for (k = 0; k < e.size(); k++)
          if (e[k] == v)
               break;
     if (k == e.size()) {
          if (k == std::numeric_limits<Ending_index>::max())
               throw std::runtime_error("too many endings");
          e.push_back(v);
     }
     return k;
}

void Dictionary::Impl::sort_entries() {
     std::sort(enttab_.begin(), enttab_.end(), cmp);
}

void Dictionary::Impl::remove_duplicate_entries() {
     auto it = enttab_.begin();
     if (it != enttab_.end()) {
          auto prev = it++;
          while (it != enttab_.end())
               if (*it == *prev)
                    it = enttab_.erase(it);
               else
                    prev = it++;
     }
     enttab_.shrink_to_fit();
}

std::istream& Dictionary::Impl::get_line(std::string& s) {
     s.clear();
     if (!input_->good())
          return *input_;
     do {
          std::getline(*input_, s);
          lineno_++;
          if (input_->fail()) {
               s.clear();
               return *input_;
          }
          auto const pos = s.find('#');
          if (pos != std::string::npos)
               s.erase(pos);
          trim(s);
     } while (s.empty());
     return *input_;
}

bool Dictionary::Impl::get_raw_entry() {
     std::string s;
     if (get_line(s).fail())
          return false;
     auto const it = entry_type_map().find(s.c_str());
     if (it == entry_type_map().end())
          throw Dictionary_error("invalid entry name", lineno_);
     entry_type_ = it->second;
     inflexion_ = entry_type[entry_type_].category.inflexion;
     int const n = number_of_forms(inflexion_);
     forms_.resize(n);
     bool ok = false;
     for (int i = 0; i < n; i++) {
          if (get_line(s).fail())
               throw Dictionary_error("not enough forms", lineno_);
          if (s == "-")
               forms_[i].clear();
          else {
               try {
                    forms_[i] = Charset::utf8_to_charset(s);
               } catch (Encoding::Conversion_error const&) {
                    throw Dictionary_error(
                         "invalid Unicode character", lineno_);
               } catch (Charset::Invalid_character_error const&) {
                    throw Dictionary_error(
                         "character not represented in dictionary",
                         lineno_);
               }
               ok = true;
          }
     }
     if (!ok)
          throw Dictionary_error("all forms empty", lineno_);
     return true;
}

std::string Dictionary::Impl::main_form(
     std::string const& stem, int nforms, int index,
     Ending_index ending_index) const {
     for (int i = 0; i < nforms; i++) {
          auto const& ee = endtab_[index][ending_index][i];
          if (ee != "-")
               return stem + ee;
     }
     throw Dictionary_error();
}

}  // namespace SHG::PLP
