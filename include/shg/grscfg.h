/**
 * \file include/shg/grscfg.h
 * Generating random strings from a context free grammar.
 */

#ifndef SHG_GRSCFG_H
#define SHG_GRSCFG_H

#include <map>
#include <utility>
#include <tuple>
#include <boost/multiprecision/cpp_int.hpp>
#include <shg/cfg.h>
#include <shg/mzt.h>

namespace SHG::PLP {

/**
 * Generates random sentences from a context free grammar.
 * Documentation is in \cite mckenzie-1997 (<a href =
 * "mckenzie.pdf">local copy</a>).
 */
class McKenzie {
public:
     /**
      * Sets a grammar to be used. The grammar does not have to be in
      * Chomsky normal form. The class has not been tested for
      * grammars with unit productions (ie. variable --> variable).
      */
     void set_grammar(Vecprod const& vp);
     /** Generate \c count sentences each of length \c length. */
     Vecsent generate(int count, int length);

private:
     using Int = boost::multiprecision::cpp_int;
     using List = std::vector<Int>;
     using Map1 = std::map<std::pair<int, int>, List>;
     using Map2 = std::map<std::tuple<int, int, int, int>, List>;

     std::vector<std::vector<std::vector<int>>> P_{};
     std::vector<std::string> termnls_{};
     int r_{};               ///< number of nonterminals
     std::vector<int> s_{};  ///< numbers of productions
     Map1 map1{};            ///< memoized values of f_f
     Map2 map2{};            ///< memoized values of f_f_prim

     MZT mzt_{};  ///< random number generator

     List f_f(int i, int n);
     Int sum(List const& list);
     List f_f_prim(int i, int j, int k, int n);

     Sentence f_g(int i, int n);
     int choose(List const& list);
     Sentence f_g_prim(int i, int j, int k, int n);

     bool is_terminal(int k) const;
     int t_(int i, int j) const;
};

inline bool McKenzie::is_terminal(int k) const {
     return k > r_;
}

inline int McKenzie::t_(int i, int j) const {
     return static_cast<int>(P_[i][j].size()) - 1;
}

}  // namespace SHG::PLP

#endif
