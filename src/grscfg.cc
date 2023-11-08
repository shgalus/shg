/**
 * \file src/grscfg.cc
 * Generating random strings from a context free grammar.
 */

// #define GRSCFG_DEBUG

#include <shg/grscfg.h>
#include <numeric>
#ifdef GRSCFG_DEBUG
#include <iostream>
#endif
#include <shg/utils.h>

#define ERROR() throw SHG::PLP::CFG_error(__func__)

namespace SHG::PLP {

void McKenzie::set_grammar(Vecprod const& vp) {
     using Pair = std::pair<std::string, int>;
     std::map<std::string, int> nmap{};
     std::map<std::string, int> tmap{};
     P_.clear();
     termnls_.clear();
     termnls_.push_back("");
     s_.clear();
     map1.clear();
     map2.clear();

     // Collect nonterminals and terminals.
     for (auto const& p : vp)
          if (auto it = nmap.find(p.lhs); it == nmap.end())
               nmap.insert(Pair(p.lhs, nmap.size() + 1));
     if (nmap.empty())
          ERROR();
     for (auto const& p : vp)
          if (p.rhs.empty())
               ERROR();
          else
               for (auto const& s : p.rhs)
                    if (nmap.find(s) == nmap.end()) {
                         if (auto it = tmap.find(s);
                             it == tmap.end()) {
                              termnls_.push_back(s);
                              tmap.insert(Pair(
                                   s, nmap.size() + tmap.size() + 1));
                         }
                    }
     if (tmap.empty())
          ERROR();

     // Check if productions are different.
     for (Vecprod::size_type i = 0; i < vp.size(); i++) {
          auto const& p = vp[i];
          for (Vecprod::size_type j = i + 1; j < vp.size(); j++)
               if (p.lhs == vp[j].lhs && p.rhs == vp[j].rhs)
                    ERROR();
     }

     // Build the representation of productions in the matrix P_.
     r_ = nmap.size();
     P_.resize(r_ + 1);
     for (int i = 1; i <= r_; i++)
          P_[i].push_back({});
     s_.resize(r_ + 1, 0);
     for (auto const& p : vp) {
          int const i = nmap[p.lhs];
          if (i < 1 || i > r_)
               ERROR();
          std::vector<int> a;
          a.push_back(0);
          for (auto const& s : p.rhs) {
               int x = 0;
               if (auto it = nmap.find(s); it != nmap.end())
                    x = it->second;
               else
                    x = tmap.find(s)->second;
               if (x <= 0)
                    ERROR();
               a.push_back(x);
          }
          P_[i].push_back(a);
          s_[i]++;
     }

#ifdef GRSCFG_DEBUG
     std::cout << "-------------------------\n";
     std::cout << "nonterminals:\n";
     for (auto const& [s, i] : nmap)
          std::cout << s << "   " << i << '\n';
     std::cout << "terminals:\n";
     for (auto const& [s, i] : tmap)
          std::cout << s << "   " << i << '\n';
     std::cout << "productions:\n";
     for (int i = 1; i <= r_; i++) {
          for (int j = 1; j <= s_[i]; j++) {
               std::cout << i << "  -->";
               for (int k = 1; k <= t_(i, j); k++)
                    std::cout << "  " << P_[i][j][k];
               std::cout << '\n';
          }
     }
     std::cout << "-------------------------\n";
#endif
}

Vecsent McKenzie::generate(int count, int length) {
     Vecsent v;
     for (int i = 0; i < count; i++)
          v.push_back(f_g(1, length));
     return v;
}

McKenzie::List McKenzie::f_f(int i, int n) {
     auto const p = std::make_pair(i, n);
     auto const it = map1.find(p);
     if (it != map1.end())
          return it->second;
     List list(s_[i] + 1, 0);
     for (int j = 1; j <= s_[i]; j++)
          list[j] = sum(f_f_prim(i, j, 1, n));
     map1.insert({p, list});
     return list;
}

McKenzie::Int McKenzie::sum(List const& list) {
     return std::accumulate(list.begin(), list.end(), Int(0));
}

McKenzie::List McKenzie::f_f_prim(int i, int j, int k, int n) {
     auto const t = std::make_tuple(i, j, k, n);
     auto const it = map2.find(t);
     if (it != map2.end())
          return it->second;
     List list(1, 0);
     if (n > 0) {
          if (is_terminal(P_[i][j][k])) {
               if (k == t_(i, j))
                    list.push_back(n == 1 ? 1 : 0);
               else
                    list.push_back(sum(f_f_prim(i, j, k + 1, n - 1)));
          } else if (k == t_(i, j)) {
               list.push_back(sum(f_f(P_[i][j][k], n)));
          } else {
               for (int el = 1; el <= n - t_(i, j) + k; el++) {
                    Int const s = sum(f_f(P_[i][j][k], el)) *
                                  sum(f_f_prim(i, j, k + 1, n - el));
                    list.push_back(s);
               }
          }
     }
     map2.insert({t, list});
     return list;
}

Sentence McKenzie::f_g(int i, int n) {
#ifdef GRSCFG_DEBUG
     std::cout << "f_g(" << i << ", " << n << ")\n";
#endif
     List const list = f_f(i, n);
     int const r = choose(list);
     if (r == 0)
          return {};
     return f_g_prim(i, r, 1, n);
}

/**
 * Given the list \f$l = [l_1, ..., l_m]\f$, return an index \f$i\f$
 * between 1 and \f$m\f$ at random with probability
 * \f[\frac{l_i}{\sum_{j = 1}^m l_j}\f].
 */
int McKenzie::choose(List const& list) {
     if (list.size() < 2)
          ERROR();
     if (list[0] != 0)
          ERROR();
     Int const s = std::accumulate(list.begin(), list.end(), Int(0));
     if (s == 0)
          return 0;
     if (s < 0)
          ERROR();
     int const m = list.size() - 1;
     Vecdouble p(m);
     double const sd = s.convert_to<double>();
     for (List::size_type i = 1; i < list.size(); i++)
          p[i - 1] = list[i].convert_to<double>() / sd;
     int r = mzt_.finite(p) + 1;
     if (r < 1 || r > m)
          ERROR();
     return r;
}

Sentence McKenzie::f_g_prim(int i, int j, int k, int n) {
#ifdef GRSCFG_DEBUG
     std::cout << "f_g_prim(" << i << ", " << j << ", " << k << ", "
               << n << ")\n";
#endif
     Sentence s;
     if (is_terminal(P_[i][j][k])) {
          s.push_back(termnls_[P_[i][j][k] - r_]);
          if (k != t_(i, j)) {
               Sentence s2 = f_g_prim(i, j, k + 1, n - 1);
               s.insert(s.end(), s2.begin(), s2.end());
          }
          return s;
     }
     if (k == t_(i, j))
          return f_g(P_[i][j][k], n);
     else {
          int const el = choose(f_f_prim(i, j, k, n));
          if (el == 0)
               ERROR();
          s = f_g(P_[i][j][k], el);
          Sentence s2 = f_g_prim(i, j, k + 1, n - el);
          s.insert(s.end(), s2.begin(), s2.end());
          return s;
     }
}

}  // namespace SHG::PLP
