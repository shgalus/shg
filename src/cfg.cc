/**
 * \file src/cfg.cc
 * Context-free grammars.
 */

#include <shg/cfg.h>
#include <cassert>
#include <cctype>
#include <set>
#include <iostream>
#include <utility>
#include <algorithm>
#include <shg/utils.h>

namespace SHG::PLP {

CFG_error::CFG_error() : std::runtime_error("internal CFG error") {}

CFG_error::CFG_error(char const* message)
     : std::runtime_error(message) {}

namespace {

std::string error_message(char const* file, int line) {
     std::string s = "internal CFG error,  file: ";
     s += file;
     s += " line: " + std::to_string(line);
     return s;
}

}  // anonymous namespace

CFG_error::CFG_error(char const* file, int line)
     : std::runtime_error(error_message(file, line)) {}

Empty_sentence::Empty_sentence()
     : CFG_error("empty sentence received for parsing") {}

Unknown_terminal::Unknown_terminal()
     : CFG_error("unknown terminal received for parsing") {}

Invalid_grammar::Invalid_grammar() : CFG_error("invalid grammar") {}

bool operator==(Production const& a, Production const& b) {
     return a.lhs == b.lhs && a.rhs == b.rhs;
}

bool operator!=(Production const& a, Production const& b) {
     return !(a == b);
}

std::string to_string(Production const& p) {
     std::string s{"{"};
     s += "\"" + p.lhs + "\", {";
     auto const n = p.rhs.size();
     for (std::vector<std::string>::size_type i = 0; i < n; i++) {
          s += "\"" + p.rhs[i] + "\"";
          if (i + 1 < n)
               s += ", ";
     }
     s += "}},\n";
     return s;
}

std::ostream& operator<<(std::ostream& stream, Production const& p) {
     stream << "{\"" << p.lhs << "\", {";
     auto const n = p.rhs.size();
     for (std::vector<std::string>::size_type i = 0; i < n; i++) {
          stream << "\"" << p.rhs[i] << "\"";
          if (i + 1 < n)
               stream << ", ";
     }
     stream << "}}";
     return stream;
}

bool is_valid(Vecprod const& vp) {
     auto const isspace = [](unsigned char c) {
          return std::isspace(c);
     };
     if (vp.size() == 0)
          return false;
     for (Vecprod::size_type i = 0; i < vp.size(); i++) {
          auto const& p = vp[i];
          if (p.lhs == "")
               return false;
          if (std::any_of(p.lhs.begin(), p.lhs.end(), isspace))
               return false;
          for (auto const& v : p.rhs) {
               if (v == "")
                    return false;
               if (std::any_of(v.begin(), v.end(), isspace))
                    return false;
          }
          for (Vecprod::size_type j = i + 1; j < vp.size(); j++)
               if (p.lhs == vp[j].lhs && p.rhs == vp[j].rhs)
                    return false;
     }
     return true;
}

bool CNF_validator::is_valid(Vecprod const& vp) {
     if (vp.size() == 0)
          return false;
     v_.clear();
     for (auto const& p : vp) {
          if (p.lhs.empty())
               return false;
          v_.insert(p.lhs);
     }
     t_.clear();
     for (auto const& p : vp)
          for (auto const& s : p.rhs) {
               if (s.empty())
                    return false;
               if (v_.find(s) == v_.end())
                    t_.insert(s);
          }
     if (t_.size() == 0)
          return false;
     s_ = vp[0].lhs;
     for (auto const& p : vp)
          if (p.rhs.size() == 1) {
               if (t_.find(p.rhs[0]) == t_.end())
                    return false;
          } else if (p.rhs.size() == 2) {
               if (v_.find(p.rhs[0]) == v_.end())
                    return false;
               if (v_.find(p.rhs[1]) == v_.end())
                    return false;
          } else {
               return false;
          }
     return true;
}

bool operator==(Vecprod const& a, Vecprod const& b) {
     if (a.size() != b.size())
          return false;
     for (Vecprod::size_type i = 0; i < a.size(); i++)
          if (a[i] != b[i])
               return false;
     return true;
}

bool operator!=(Vecprod const& a, Vecprod const& b) {
     return !(a == b);
}

std::string to_string(Vecprod const& vp) {
     std::string s;
     for (auto const& p : vp) {
          s += p.lhs + " -->";
          for (auto const& sym : p.rhs)
               s += " " + sym;
          s += '\n';
     }
     return s;
}

std::string to_string2(Vecprod const& vp) {
     std::string s;
     s = "{\n";
     for (auto const& p : vp)
          s += "     " + to_string(p);
     s += "};\n";
     return s;
}

std::string to_string(Sentence const& sent) {
     std::string s{'{'};
     bool first{true};
     for (auto const& t : sent) {
          if (first)
               first = false;
          else
               s += ", ";
          s += "\"" + t + "\"";
     }
     s += "},\n";
     return s;
}

std::string to_string(Vecsent const& vs) {
     std::string s;
     for (auto const& sent : vs)
          s += to_string(sent);
     return s;
}

CYK::Parse_node::Parse_node(std::string node) : node_(node) {}

CYK::Parse_node::Parse_node(std::string node,
                            Vecprod::size_type production)
     : node_(node), production_(production) {}

void CYK::set_grammar(Vecprod const& cfg) {
     CNF_validator v;
     if (!v.is_valid(cfg))
          throw Invalid_grammar();
     // There should be no start symbol on rhs.
     for (auto const& p : cfg)
          if (p.rhs.size() == 2)
               if (p.rhs[0] == v.s() || p.rhs[1] == v.s())
                    throw Invalid_grammar();
     cfg_ = &cfg;
     v_ = v.v();
     t_ = v.t();
}

/**
 * Implementation of \cite hopcroft-ullman-2003, page 163.
 */
bool CYK::recognize(Sentence const& X) {
     int const n = X.size();
     if (n == 0)
          throw Empty_sentence();
     for (auto const& t : X)
          if (t_.find(t) == t_.end())
               Unknown_terminal();
     std::vector<std::vector<std::set<std::string>>> V(n);
     for (int i = 0; i < n; i++) {
          V[i].resize(n - i);
          for (auto const& p : *cfg_)
               if (p.rhs.size() == 1 && p.rhs[0] == X[i])
                    V[i][0].insert(p.lhs);
     }
     for (int j = 1; j < n; j++) {
          for (int i = 0; i < n - j; i++) {
               for (int k = 0; k < j; k++) {
                    auto const& vl = V[i][k];
                    auto const& vr = V[i + k + 1][j - k - 1];
                    for (auto itl = vl.cbegin(); itl != vl.cend();
                         ++itl)
                         for (auto itr = vr.cbegin();
                              itr != vr.cend(); ++itr) {
                              for (auto const& p : *cfg_)
                                   if (p.rhs.size() == 2 &&
                                       p.rhs[0] == *itl &&
                                       p.rhs[1] == *itr)
                                        V[i][j].insert(p.lhs);
                         }
               }
          }
     }
     // Assume that the start symbol is the lhs of the first
     // production.
     return V[0][n - 1].count((*cfg_)[0].lhs) > 0;
}

void CYK::parse(Sentence const& X) {
     int const n = X.size();
     if (n == 0)
          throw Empty_sentence();
     for (auto const& t : X)
          if (t_.find(t) == t_.end())
               Unknown_terminal();
     V_.clear();
     V_.resize(n);
     for (int i = 0; i < n; i++) {
          V_[i].resize(n - i);
          for (Vecprod::size_type k = 0; k < cfg_->size(); k++) {
               Production const& p = (*cfg_)[k];
               if (p.rhs.size() == 1 && p.rhs[0] == X[i])
                    V_[i][0].push_back(Info(k));
          }
     }
     for (int j = 1; j < n; j++)
          for (int i = 0; i < n - j; i++)
               for (int k = 0; k < j; k++)
                    add_production(j, i, k);
}

CYK::Parse_tree const& CYK::gen() {
     tree_.clear();
     int const n = V_[0].size();
     if (n == 0)
          return tree_;
     auto const& v = V_[0][n - 1];
     int const m = static_cast<int>(v.size());
     for (int k = 0; k < m; k++) {
          auto const& q = v[k];
          if ((*cfg_)[q.p].lhs == (*cfg_)[0].lhs)
               add_node(q, tree_);
     }
     return tree_;
}

CYK::Info::Info(Vecprod::size_type p, int il, int jl, int kl, int ir,
                int jr, int kr)
     : p(p), il(il), jl(jl), kl(kl), ir(ir), jr(jr), kr(kr) {}

CYK::Info::Info(Vecprod::size_type p)
     : p(p), il(-1), jl(-1), kl(-1), ir(-1), jr(-1), kr(-1) {}

void CYK::add_production(int j, int i, int k) {
     int const ir = i + k + 1;
     int const jr = j - k - 1;
     auto const& left = V_[i][k];
     auto const& right = V_[ir][jr];
     for (int kl = 0; kl < static_cast<int>(left.size()); kl++) {
          auto const& lsym = (*cfg_)[left[kl].p].lhs;
          for (int kr = 0; kr < static_cast<int>(right.size());
               kr++) {
               auto const& rsym = (*cfg_)[right[kr].p].lhs;
               for (Vecprod::size_type ip = 0; ip < cfg_->size();
                    ip++) {
                    Production const& p = (*cfg_)[ip];
                    if (p.rhs.size() == 2 && p.rhs[0] == lsym &&
                        p.rhs[1] == rsym)
                         V_[i][j].push_back(
                              Info(ip, i, k, kl, ir, jr, kr));
               }
          }
     }
}

void CYK::add_node(Info const& q, Parse_tree& t) {
     t.push_back({(*cfg_)[q.p].lhs, q.p});
     if (q.il < 0) {
          t[t.degree() - 1].push_back(
               Parse_node((*cfg_)[q.p].rhs[0]));
     } else {
          add_node(V_[q.il][q.jl][q.kl], t[t.degree() - 1]);
          add_node(V_[q.ir][q.jr][q.kr], t[t.degree() - 1]);
     }
}

void Conversion_to_CNF::convert(Vecprod const& vp) {
     if (!is_valid(vp))
          throw CFG_error("invalid vector of productions");
     vp_ = vp;
     set_variables();
     set_terminals();
     s_ = vp_[0].lhs;
     for (auto const& p : vp_)
          if (p.lhs == s_ && p.rhs.size() == 0)
               throw CFG_error("grammar generates empty string");
     cfg_.clear();
     nnewv_ = 0;
     start();
     term();
     bin();
     del();
     unit();
     remove_useless();
     order();
}

void Conversion_to_CNF::set_variables() {
     v_.clear();
     for (auto const& p : vp_) {
          if (p.lhs.empty() || p.lhs[0] == reserved_char_)
               throw CFG_error("invalid variable name");
          v_.insert(p.lhs);
     }
}

void Conversion_to_CNF::set_terminals() {
     t_.clear();
     for (auto const& p : vp_)
          for (auto const& s : p.rhs) {
               if (s.empty() || s[0] == reserved_char_)
                    throw CFG_error(
                         "invalid variable or terminal name");
               if (v_.find(s) == v_.end())
                    t_.insert(s);
          }
     if (t_.size() == 0)
          throw CFG_error("no terminals found");
}

std::string Conversion_to_CNF::newname() {
     return "_" + std::to_string(nnewv_++);
}

void Conversion_to_CNF::start() {
     Production p;
     p.rhs.push_back(s_);
     s_ = p.lhs = newname();
     v_.insert(s_);
     cfg_.push_back(p);
     for (auto const& p : vp_)
          cfg_.push_back(p);
}

void Conversion_to_CNF::term() {
     auto const n = cfg_.size();
     Vecprod::size_type i, j;
     for (i = 0; i < n; i++) {
          auto const p = cfg_[i];
          if (p.rhs.size() <= 1)
               continue;
          for (auto const& s : p.rhs) {
               if (t_.find(s) != t_.end()) {
                    Production q;
                    q.lhs = newname();
                    v_.insert(q.lhs);
                    q.rhs.push_back(s);
                    cfg_.push_back(q);
                    for (j = i; j < n; j++) {
                         auto& r = cfg_[j];
                         if (r.rhs.size() <= 1)
                              continue;
                         for (auto& s1 : r.rhs)
                              if (s1 == s)
                                   s1 = q.lhs;
                    }
               }
          }
     }
}

void Conversion_to_CNF::bin() {
     auto const n = cfg_.size();
     std::string newn;
     std::vector<std::string>::size_type j;
     for (Vecprod::size_type i = 0; i < n; i++) {
          auto const p = cfg_[i];
          auto const k = p.rhs.size();
          if (k <= 2)
               continue;
          Production q;
          newn = q.lhs = newname();
          q.rhs.push_back(p.rhs[k - 2]);
          q.rhs.push_back(p.rhs[k - 1]);
          cfg_.push_back(q);
          v_.insert(newn);
          for (j = 1; j < k - 2; j++) {
               q.lhs = newname();
               q.rhs[0] = p.rhs[k - 2 - j];
               q.rhs[1] = newn;
               cfg_.push_back(q);
               v_.insert(q.lhs);
               newn = q.lhs;
          }
          cfg_[i].rhs.clear();
          cfg_[i].rhs.push_back(p.rhs[0]);
          cfg_[i].rhs.push_back(newn);
     }
}

/*
 * Algorithm.
 *
 * 1. If at the beginning exists S --> epsilon - throw.
 *
 * 2. Determine the set of all nonterminals that derive epsilon:
 *
 * If a rule A --> epsilon exists, A is nullable.
 *
 * If a rule A --> X_1 ... X_n exists and every X_i is nullable, A is
 * also nullable.
 *
 * Obtain an intermediate grammar by replacing each rule A --> X_1 ...
 * X_n by all versions with some nullable X_i omitted. If the
 * intermediate grammar has production S --> epsilon - throw. Remove
 * all epsilon rules and duplicates.
 *
 * Example:
 *
 * 1. S --> A b B
 * 2. S --> C
 * 3. B --> A A
 * 4. B --> A C
 * 5. C --> b
 * 6. C --> c
 * 7. A --> a
 * 8. A --> epsilon
 *
 * Nullable nonterminals: A, B.
 *
 * 1. S --> A b B
 *    S --> A b
 *    S -->   b B
 *    S -->   b
 * 2. S --> C
 * 3. B --> A A
 *    B --> A
 *    B -->   A     - this will be removed as duplicate
 *    B --> epsilon - this will be removed as nullable
 * 4. B --> AC
 *    B -->  C
 * 5. C --> b
 * 6. C --> c
 * 7. A --> a
 * 8. A --> epsilon - this will be removed as nullable
 */
void Conversion_to_CNF::del() {
     nullable();
     w_.clear();
     for (auto const& p : cfg_)
          expand(p);
     cfg_.clear();
     // Here check for 0_ --> epsilon
     // and remove duplicates.
     // After unit also remove duplicates and productions A --> A.
     for (auto const& p : w_)
          if (p.rhs.size() != 0)
               cfg_.push_back(p);
     remove_duplicates(cfg_);
}

void Conversion_to_CNF::unit() {
     Vecprod::size_type i;
     Production p0;
     for (;;) {
          bool found = false;
          for (auto it = cfg_.begin(); it != cfg_.end(); ++it) {
               if (it->rhs.size() != 1)
                    continue;
               if (v_.find(it->rhs[0]) != v_.end()) {
                    found = true;
                    p0 = *it;
                    cfg_.erase(it);
                    break;
               }
          }
          if (!found)
               break;
          auto const n = cfg_.size();
          for (i = 0; i < n; i++) {
               auto& p = cfg_[i];
               if (p.lhs == p0.rhs[0]) {
                    Production p1 = p;
                    p1.lhs = p0.lhs;
                    cfg_.push_back(p1);
               }
          }
     }
     remove_duplicates(cfg_);
}

void Conversion_to_CNF::order() {
     Vecprod::size_type i, k = 0;
     // Productions _0 --> first.
     do {
          for (i = k; i < cfg_.size() && cfg_[i].lhs != s_; i++)
               ;
          if (i < cfg_.size())
               std::swap(cfg_[i], cfg_[k++]);
     } while (i < cfg_.size());
     // Right-hand sides with two nonterminals.
     do {
          for (i = k; i < cfg_.size() && cfg_[i].rhs.size() != 2; i++)
               ;
          if (i < cfg_.size())
               std::swap(cfg_[i], cfg_[k++]);
     } while (i < cfg_.size());
     // Right-hand sides with one terminal.
     do {
          for (i = k; i < cfg_.size() && cfg_[i].rhs.size() != 1; i++)
               ;
          if (i < cfg_.size())
               std::swap(cfg_[i], cfg_[k++]);
     } while (i < cfg_.size());
}

void Conversion_to_CNF::nullable() {
     null_.clear();
     for (auto const& p : cfg_)
          if (p.rhs.size() == 0)
               null_.insert(p.lhs);
     if (null_.empty())
          return;
     bool found;
     do {
          found = false;
          for (auto const& p : cfg_) {
               if (std::all_of(p.rhs.begin(), p.rhs.end(),
                               [this](std::string const& s) {
                                    return null_.find(s) !=
                                           null_.end();
                               }))
                    if (null_.find(p.lhs) == null_.end()) {
                         null_.insert(p.lhs);
                         found = true;
                    }
          }
     } while (found);
}

void Conversion_to_CNF::expand(Production const& p) {
     size_t nnullable = 0;  // number of nullable variables on rhs
     std::vector<bool> v(p.rhs.size(), false);
     for (std::vector<std::string>::size_type i = 0; i < p.rhs.size();
          i++)
          if (null_.find(p.rhs[i]) != null_.end()) {
               nnullable++;
               v[i] = true;
          }
     if (nnullable == 0) {
          w_.push_back(p);
          return;
     }
     std::vector<bool> bv(nnullable, false);
     for (;;) {
          Production p1;
          p1.lhs = p.lhs;
          size_t j = 0;
          for (size_t i = 0; i < v.size(); i++) {
               if (v[i]) {
                    if (bv[j++])
                         ;
                    else
                         p1.rhs.push_back(p.rhs[i]);
               } else
                    p1.rhs.push_back(p.rhs[i]);
          }
          assert(j == nnullable);
          w_.push_back(p1);
          size_t i = 0;
          while (i < bv.size() && bv[i])
               bv[i++] = false;
          if (i == bv.size())
               break;
          bv[i] = true;
     }
}

/**
 * Variable is useless if it is different from the start symbol and it
 * appears on the right hand side of no production. Production is
 * useless if its left hand side variable is useless.
 */
void Conversion_to_CNF::remove_useless() {
     for (auto it = v_.begin(); it != v_.end();) {
          if (*it == s_) {
               ++it;
               continue;
          }
          bool found = false;
          for (auto const& p : cfg_) {
               auto const& r = p.rhs;
               if (std::find(r.begin(), r.end(), *it) != r.end()) {
                    found = true;
                    break;
               }
          }
          if (found) {
               ++it;
               continue;
          }
          // Remove all productions with lhs == *it.
          auto end = std::remove_if(
               cfg_.begin(), cfg_.end(),
               [it](Production const& p) { return p.lhs == *it; });
          cfg_.erase(end, cfg_.end());
          // Remove *it from v_.
          it = v_.erase(it);
     }
}

Vecprod full_cnf(unsigned n, unsigned m) {
     if (n == 0 || m == 0)
          throw CFG_error("invalid number of symbols");
     std::vector<std::string> v(n);
     std::vector<std::string> t(m);
     for (unsigned i = 0; i < n; i++)
          v[i] = 'v' + std::to_string(i);
     for (unsigned i = 0; i < m; i++)
          t[i] = 't' + std::to_string(i);
     Vecprod vp;
     for (unsigned i = 0; i < n; i++) {
          for (unsigned j = 0; j < n; j++)
               for (unsigned k = 0; k < n; k++)
                    vp.push_back({v[i], {v[j], v[k]}});
          for (unsigned j = 0; j < m; j++)
               vp.push_back({v[i], {t[j]}});
     }
     return vp;
}

}  // namespace SHG::PLP
