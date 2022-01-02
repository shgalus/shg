/**
 * \file include/shg/dict_impl.h
 * Polish dictionary implementation.
 */

#ifndef SHG_DICT_IMPL_H
#define SHG_DICT_IMPL_H

#include <cstring>
#include <map>
#include <shg/dict.h>

namespace SHG::PLP {

using Type_index = unsigned char;
using Ending_index = unsigned short;

struct Entry {
     Type_index type{};
     std::string stem{};
     Ending_index ending_index{};
};

struct Entry_type {
     char const* const name{};
     Category const category{};
};

/**
 * The table of types of entries.
 */
extern Entry_type const entry_type[];

/**
 * Number of elements in entry_type[].
 */
constexpr Type_index nenttypes = 51;

struct Cmp {
     bool operator()(char const* lhs, char const* rhs) const {
          return std::strcmp(lhs, rhs) < 0;
     }
};

using Entry_type_map = std::map<char const*, Type_index, Cmp>;

class Invalid_word_file : public Dictionary_error {
public:
     Invalid_word_file();
};

bool operator==(Entry const& a, Entry const& b);
bool cmp(Entry const& a, Entry const& b);
int number_of_forms(Inflexion inflexion);
void complete_category(Category& c, Inflexion infl,
                       unsigned short form);

std::ostream& put(std::string const& s, std::ostream& stream);
std::istream& get(std::string& s, std::istream& stream);
std::ostream& put(unsigned long n, std::ostream& stream);
std::istream& get(unsigned long& n, std::istream& stream);

Entry_type_map init_entry_type_map();
Entry_type_map const& entry_type_map();

/**
 * Returns the longest common prefix of strings in v and removes it
 * from strings in v.
 */
std::string lcp(std::vector<std::string>& v);

/**
 * Returns index of table of endings in Impl::endtab_ for given
 * inflexion.
 */
inline int inflexion_to_index(Inflexion inflexion) {
     return static_cast<int>(inflexion);
}

class Dictionary::Impl {
public:
     void load_source_word_file(std::istream& input);
     void write_source_word_file(std::ostream& output,
                                 bool do_sort) const;
     void load_word_file(std::istream& input);
     void write_word_file(std::ostream& output) const;

     void clear();

     bool has_entry_utf8(char const* s) const;

     void search_utf8(char const* s, Setdesc& sd) const;

     bool has_entry_charset(char const* s) const;
     void search_charset(char const* s, Setdesc& sd) const;

     std::vector<Table_row> report() const;

private:
     using Set_tab = std::vector<std::string>;
     using Inf_tab = std::vector<Set_tab>;
     using End_tab = std::vector<Inf_tab>;
     using Ent_tab = std::vector<Entry>;
     using Inf_tab_szt = Inf_tab::size_type;
     using Set_tab_szt = Set_tab::size_type;
     using End_tab_szt = End_tab::size_type;
     using Ent_tab_szt = Ent_tab::size_type;

     bool find(char const* s, Setdesc* v) const;
     Ending_index insert_ending(int a, Set_tab const& v);
     void sort_entries();
     void remove_duplicate_entries();
     std::istream& get_line(std::string& s);
     bool get_raw_entry();
     std::string main_form(std::string const& stem, int nforms,
                           int index,
                           Ending_index ending_index) const;

     static End_tab::size_type constexpr ninfl =
          static_cast<End_tab::size_type>(Inflexion::conjugation) + 1;
     static Set_tab::size_type constexpr sz[ninfl] = {
          1, 14, 42, 14, 28, 28, 7, 2, 42};

     Ent_tab enttab_{};
     End_tab endtab_{ninfl};
     std::istream* input_{nullptr};
     unsigned long lineno_{};
     Type_index entry_type_{};
     Inflexion inflexion_{};
     std::vector<std::string> forms_{};
};

}  // namespace SHG::PLP

#endif
