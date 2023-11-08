/**
 * \file src/pcfg.cc
 * Probabilistic context free grammar.
 */

// #define PCFG_DEBUG

#include <shg/pcfg.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <shg/utils.h>

namespace SHG::PLP {

std::ostream& operator<<(std::ostream& stream,
                         S_production const& sp) {
     stream << "{" << sp.prod << ", " << sp.prob << "}";
     return stream;
}

std::ostream& operator<<(std::ostream& stream, Vecsprod const& vsp) {
     for (auto const& sp : vsp)
          stream << sp << ",\n";
     return stream;
}

std::pair<Vecprod, std::vector<double>> split(Vecsprod const& vsp) {
     Vecprod vp(vsp.size());
     std::vector<double> prob(vsp.size());
     for (Vecsprod::size_type i = 0; i < vsp.size(); i++) {
          vp[i] = vsp[i].prod;
          prob[i] = vsp[i].prob;
     }
     return {vp, prob};
}

std::string PCFG::Parse_node::to_string() const {
     std::string s{symbol};
     if (!is_terminal) {
          s += ' ';
          s += std::to_string(prob);
     }
     return s;
}

PCFG::PCFG(PCFG const& other)
     : n_(other.n_),
       V_(other.V_),
       bp_(other.bp_),
       up_(other.up_),
       nonterminal_names_(other.nonterminal_names_),
       terminal_names_(other.terminal_names_),
       nmap_(other.nmap_),
       tmap_(other.tmap_),
       w_(other.w_),
       m_(other.m_),
       beta_(other.beta_),
       alpha_(other.alpha_),
       delta_(other.delta_),
       psi_(other.psi_),
       ftab_(other.ftab_),
       gtab_(other.gtab_),
       htab_(other.htab_) {
     fill_names();
}

PCFG::PCFG(PCFG&& other) noexcept
     : n_(std::move(other.n_)),
       V_(std::move(other.V_)),
       bp_(std::move(other.bp_)),
       up_(std::move(other.up_)),
       nonterminal_names_(std::move(other.nonterminal_names_)),
       terminal_names_(std::move(other.terminal_names_)),
       nmap_(std::move(other.nmap_)),
       tmap_(std::move(other.tmap_)),
       w_(std::move(other.w_)),
       m_(std::move(other.m_)),
       beta_(std::move(other.beta_)),
       alpha_(std::move(other.alpha_)),
       delta_(std::move(other.delta_)),
       psi_(std::move(other.psi_)),
       ftab_(std::move(other.ftab_)),
       gtab_(std::move(other.gtab_)),
       htab_(std::move(other.htab_)) {
     other.clear();
     fill_names();
}

PCFG& PCFG::operator=(PCFG const& other) {
     if (this != &other) {
          n_ = other.n_;
          V_ = other.V_;
          bp_ = other.bp_;
          up_ = other.up_;
          nonterminal_names_ = other.nonterminal_names_;
          terminal_names_ = other.terminal_names_;
          nmap_ = other.nmap_;
          tmap_ = other.tmap_;
          w_ = other.w_;
          m_ = other.m_;
          beta_ = other.beta_;
          alpha_ = other.alpha_;
          delta_ = other.delta_;
          psi_ = other.psi_;
          ftab_ = other.ftab_;
          gtab_ = other.gtab_;
          htab_ = other.htab_;
          fill_names();
     }
     return *this;
}

PCFG& PCFG::operator=(PCFG&& other) {
     n_ = std::move(other.n_);
     V_ = std::move(other.V_);
     bp_ = std::move(other.bp_);
     up_ = std::move(other.up_);
     nonterminal_names_ = std::move(other.nonterminal_names_);
     terminal_names_ = std::move(other.terminal_names_);
     nmap_ = std::move(other.nmap_);
     tmap_ = std::move(other.tmap_);
     w_ = std::move(other.w_);
     m_ = std::move(other.m_);
     beta_ = std::move(other.beta_);
     alpha_ = std::move(other.alpha_);
     delta_ = std::move(other.delta_);
     psi_ = std::move(other.psi_);
     ftab_ = std::move(other.ftab_);
     gtab_ = std::move(other.gtab_);
     htab_ = std::move(other.htab_);
     other.clear();
     fill_names();
     return *this;
}

void PCFG::set(Vecprod const& vp, std::vector<double> const& prob,
               double tol) {
     using Pair = std::pair<std::string, Index>;

     clear();
     if (tol < 0.0)
          throw std::invalid_argument("tol must not be negative");
     if (!SHG::PLP::is_valid(vp))
          goto fail;
     for (auto const& p : vp)
          if (auto it = nmap_.find(p.lhs); it == nmap_.end())
               nmap_.insert(Pair(p.lhs, nmap_.size()));
     n_ = nmap_.size();
     if (n_ == 0)
          goto fail;
     for (auto const& p : vp) {
          if (p.rhs.size() == 2) {
               if (auto it = nmap_.find(p.rhs[0]); it == nmap_.end())
                    goto fail;
               if (auto it = nmap_.find(p.rhs[1]); it == nmap_.end())
                    goto fail;
          } else if (p.rhs.size() == 1) {
               if (auto it = nmap_.find(p.rhs[0]); it != nmap_.end())
                    goto fail;
               if (auto it = tmap_.find(p.rhs[0]); it == tmap_.end())
                    tmap_.insert(Pair(p.rhs[0], tmap_.size()));
          } else {
               goto fail;
          }
     }
     V_ = tmap_.size();
     if (V_ == 0)
          goto fail;

     nonterminal_names_.resize(n_);
     terminal_names_.resize(V_);
     fill_names();

     if (vp.size() != prob.size())
          goto fail;

     bp_.resize(n_);
     up_.resize(n_);
     beta_.resize(n_);
     alpha_.resize(n_);
     delta_.resize(n_);
     psi_.resize(n_);

     // Assign probabilities to productions.
     for (Vecprod::size_type i = 0; i < vp.size(); i++) {
          auto const& p = vp[i];
          Index const j = nmap_.find(p.lhs)->second;
          if (p.rhs.size() == 2) {
               Index const r = nmap_.find(p.rhs[0])->second;
               Index const s = nmap_.find(p.rhs[1])->second;
               bp_[j].insert({{r, s}, prob[i]});
          } else {
               Index const k = tmap_.find(p.rhs[0])->second;
               up_[j].insert({k, prob[i]});
          }
     }
     if (!probs_valid(tol))
          goto fail;
     return;
fail:
     clear();
     throw std::invalid_argument("invalid grammar in PCFG::set()");
}

void PCFG::clear() {
     n_ = 0;
     V_ = 0;
     bp_.clear();
     up_.clear();
     nonterminal_names_.clear();
     terminal_names_.clear();
     nmap_.clear();
     tmap_.clear();
     w_.clear();
     m_ = 0;
     psi_.clear();
     beta_.clear();
     alpha_.clear();
     delta_.clear();
     ftab_.clear();
     gtab_.clear();
     htab_.clear();
}

bool PCFG::is_valid() const {
     return n_ > 0;
}

Vecsprod PCFG::vecsprod() const {
     Vecsprod vsp;
     for (Index i = 0; i < bp_.size(); i++)
          for (auto const& p : bp_[i]) {
               S_production sp;
               sp.prod.lhs = *nonterminal_names_[i];
               sp.prod.rhs.resize(2);
               sp.prod.rhs[0] = *nonterminal_names_[p.first.first];
               sp.prod.rhs[1] = *nonterminal_names_[p.first.second];
               sp.prob = p.second;
               vsp.push_back(sp);
          }
     for (Index i = 0; i < up_.size(); i++)
          for (auto const& p : up_[i]) {
               S_production sp;
               sp.prod.lhs = *nonterminal_names_[i];
               sp.prod.rhs.push_back(*terminal_names_[p.first]);
               sp.prob = p.second;
               vsp.push_back(sp);
          }
     return vsp;
}

double PCFG::prob(Sentence const& w) {
     check();
     set(w);
     fill_beta();
     return beta_[0][0][m_ - 1];
}

double PCFG::log_prob(Vecsent const& vs) {
     check();
     if (vs.empty())
          throw std::invalid_argument("empty sentence");
     bool ok;
     double const lp = log_prob(vs, ok);
     if (!ok)
          throw std::runtime_error("non-positive probability");
     return lp;
}

double PCFG::prob_outside(Sentence const& w, Index k) {
     check();
     set(w);
     if (k >= m_)
          throw std::invalid_argument(
               "k must be less than length of sentence");
     fill_beta();
     fill_alpha();
     double sum = 0.0;
     for (Index j = 0; j < n_; j++)
          sum += alpha_[j][k][k] * prob(j, w_[k]);
     return sum;
}

void PCFG::cyk(Sentence const& w, double& prob, Parse_tree& t) {
     check();
     set(w);
     fill_beta();
     fill_delta();
     prob = delta_[0][0][m_ - 1];
     t.clear();
     add_node(0, 0, m_ - 1, t);
}

int PCFG::inside_outside(Vecsent const& vs, double accuracy,
                         double tol, int max_iter, int& iter,
                         double& logp) {
     check();
     if (vs.empty())
          throw std::invalid_argument("empty vector of sentences");
     for (Sentence const& s : vs) {
          if (s.empty())
               throw std::invalid_argument("empty sentence");
          for (auto const& w : s)
               if (tmap_.find(w) == tmap_.end())
                    throw std::invalid_argument(
                         "invalid terminal in sentence");
     }
     if (accuracy <= 0.0)
          throw std::invalid_argument("accuracy must be positive");

     int status = 0;
     bool ok;
     std::vector<Binary> bp_old;
     std::vector<Unary> up_old;
     double logp_old;

     iter = 0;
     init_tables(vs);
     logp = log_prob(vs, ok);
     if (!ok)
          return 1;
     for (;;) {
          if (iter >= max_iter) {
               status = 2;
               break;
          }
          iter++;
          bp_old = bp_;
          up_old = up_;
          logp_old = logp;
          status = one_iteration(vs);
          if (status != 0) {
               if (status == 1)
                    status = 3;
               else if (status == 2)
                    status = 4;
               break;
          }
          if (!probs_valid(tol)) {
               status = 5;
               break;
          }
          logp = log_prob(vs, ok);
          if (!ok) {
               status = 6;
               break;
          }
          if (logp < logp_old) {
               status = 7;
               break;
          }
          if (logp - logp_old < accuracy)
               break;
     }
     if (status > 2) {
          bp_ = bp_old;
          up_ = up_old;
          logp = logp_old;
     }
     return status;
}

void PCFG::remove_productions(double eps) {
     if (eps < 0.0 || eps >= 1.0)
          throw std::logic_error("eps should be in [0, 1)");

     std::vector<Binary> bp(bp_.size());
     std::vector<Unary> up(up_.size());
     for (Index i = 0; i < n_; i++) {
          double sum = 0.0;
          for (auto const& p : bp_[i])
               if (p.second > eps) {
                    bp[i][p.first] = p.second;
                    sum += p.second;
               }
          for (auto const& p : up_[i])
               if (p.second > eps) {
                    up[i][p.first] = p.second;
                    sum += p.second;
               }
          for (auto& p : bp[i])
               p.second /= sum;
          for (auto& p : up[i])
               p.second /= sum;
     }
     bp_ = bp;
     up_ = up;
}

void PCFG::check() const {
     if (!is_valid())
          throw std::logic_error("PCFG is uninitialized");
}

bool PCFG::probs_valid(double tol) const {
     for (Index i = 0; i < n_; i++) {
          double sum = 0.0;
          for (auto const& p : bp_[i]) {
               if (p.second < 0.0)
                    return false;
               sum += p.second;
          }
          for (auto const& p : up_[i]) {
               if (p.second < 0.0)
                    return false;
               sum += p.second;
          }
          if (std::abs(sum - 1.0) > tol)
               return false;
     }
     return true;
}

double PCFG::prob(Index j, Index r, Index s) {
     assert(j < n_);
     assert(r < n_);
     assert(s < n_);
     auto const& b = bp_[j];
     if (auto const it = b.find({r, s}); it != b.end())
          return it->second;
     return 0.0;
}

double PCFG::prob(Index j, Index k) {
     assert(j < n_);
     assert(k < V_);
     auto const& u = up_[j];
     if (auto const it = u.find(k); it != u.end())
          return it->second;
     return 0.0;
}

double PCFG::log_prob(Vecsent const& vs, bool& ok) {
     ok = true;
     double s = 0.0;
     for (auto const& w : vs) {
          double const lp = std::log(prob(w));
          if (!std::isfinite(lp)) {
               ok = false;
               return 0.0;
          }
          s += lp;
     }
     return s;
}

void PCFG::set(Sentence const& w) {
     if (w.empty())
          throw std::invalid_argument("empty sentence");
     m_ = w.size();
     w_.resize(m_);
     for (Sentence::size_type i = 0; i < m_; i++)
          if (auto const it = tmap_.find(w[i]); it != tmap_.end())
               w_[i] = it->second;
          else {
               w_.clear();
               m_ = 0;
               throw std::invalid_argument(
                    "invalid terminal in sentence");
          }
}

void PCFG::add_node(Index i, Index p, Index q, Parse_tree& t) {
     t.data().symbol = *nonterminal_names_.at(i);
     Parse_node nd;
     if (p == q) {
          t.data().prob = prob(i, w_[p]);
          nd.symbol = *terminal_names_.at(w_[p]);
          nd.is_terminal = true;
          t.push_back(nd);
          return;
     }
     auto const& psi = psi_[i][p][q];
     t.data().prob = prob(i, psi.j, psi.k);
     t.push_back(nd);
     add_node(psi.j, p, psi.r, t[0]);
     t.push_back(nd);
     add_node(psi.k, psi.r + 1, q, t[1]);
}

void PCFG::fill_beta() {
     for (Index j = 0; j < n_; j++) {
          beta_[j].clear();
          beta_[j].resize(m_);
          for (Index p = 0; p < m_; p++) {
               beta_[j][p].resize(m_);
               beta_[j][p][p] = prob(j, w_[p]);
          }
     }
     for (Index dg = 1; dg < m_; dg++)
          for (Index p = 0; p < m_ - dg; p++) {
               Index const q = p + dg;
               for (Index j = 0; j < n_; j++) {
                    double sum = 0.0;
                    for (auto const& b : bp_[j]) {
                         for (Index d = p; d < q; d++)
                              sum += b.second *
                                     beta_[b.first.first][p][d] *
                                     beta_[b.first.second][d + 1][q];
                    }
                    beta_[j][p][q] = sum;
               }
          }
}

void PCFG::fill_alpha() {
     for (Index j = 0; j < n_; j++) {
          alpha_[j].clear();
          alpha_[j].resize(m_);
          for (auto& p : alpha_[j])
               p.resize(m_);
     }
     alpha_[0][0][m_ - 1] = 1.0;
     Index dg = m_ - 1;
     while (dg-- > 0) {
          for (Index p = 0; p < m_ - dg; p++) {
               Index const q = p + dg;
               for (Index j = 0; j < n_; j++) {
                    double sum = 0.0;
                    for (Index f = 0; f < bp_.size(); f++) {
                         for (auto const& b : bp_[f]) {
                              if (b.first.first == j) {
                                   Index const g = b.first.second;
                                   for (Index e = q + 1; e < m_;
                                        e++) {
                                        sum += alpha_[f][p][e] *
                                               b.second *
                                               beta_[g][q + 1][e];
                                   }
                              }
                              if (b.first.second == j) {
                                   Index const g = b.first.first;
                                   for (Index e = 0; e < p; e++)
                                        sum += alpha_[f][e][q] *
                                               b.second *
                                               beta_[g][e][p - 1];
                              }
                         }
                    }
                    alpha_[j][p][q] = sum;
               }
          }
     }
}

void PCFG::fill_delta() {
     for (Index i = 0; i < n_; i++) {
          delta_[i].clear();
          delta_[i].resize(m_);
          psi_[i].clear();
          psi_[i].resize(m_);
          for (Index p = 0; p < m_; p++) {
               delta_[i][p].resize(m_);
               delta_[i][p][p] = prob(i, w_[p]);
               psi_[i][p].resize(m_);
          }
     }
     for (Index dg = 1; dg < m_; dg++)
          for (Index p = 0; p < m_ - dg; p++) {
               Index const q = p + dg;
               for (Index i = 0; i < n_; i++) {
                    double max = 0.0;
                    for (Index j = 0; j < n_; j++)
                         for (Index k = 0; k < n_; k++)
                              for (Index r = p; r < q; r++) {
                                   double const d =
                                        prob(i, j, k) *
                                        delta_[j][p][r] *
                                        delta_[k][r + 1][q];
                                   if (d > max) {
                                        max = d;
                                        psi_[i][p][q] = {j, k, r};
                                   }
                              }
                    delta_[i][p][q] = max;
               }
          }
}

void PCFG::fill_names() {
     for (auto const& s : nmap_)
          nonterminal_names_[s.second] = &s.first;
     for (auto const& s : tmap_)
          terminal_names_[s.second] = &s.first;
}

void PCFG::init_tables(Vecsent const& vs) {
     Index const omega = vs.size();
     Index const maxm =  // length of the longest sentence
          std::max_element(vs.begin(), vs.end(),
                           [](auto const& s1, auto const& s2) {
                                return s1.size() < s2.size();
                           })
               ->size();
     ftab_.clear();
     gtab_.clear();
     htab_.clear();
     ftab_.resize(omega);
     gtab_.resize(omega);
     htab_.resize(omega);
     for (Index i = 0; i < omega; i++) {
          ftab_[i].resize(maxm);
          gtab_[i].resize(maxm);
          htab_[i].resize(maxm);
          for (Index p = 0; p < maxm; p++) {
               ftab_[i][p].resize(maxm);
               htab_[i][p].resize(maxm);
               for (Index q = 0; q < maxm; q++) {
                    ftab_[i][p][q].resize(n_);
                    htab_[i][p][q].resize(n_);
                    for (Index j = 0; j < n_; j++) {
                         ftab_[i][p][q][j].resize(n_);
                         for (Index r = 0; r < n_; r++)
                              ftab_[i][p][q][j][r].resize(n_);
                    }
               }
          }
          for (Index h = 0; h < maxm; h++) {
               gtab_[i][h].resize(n_);
               for (Index j = 0; j < n_; j++)
                    gtab_[i][h][j].resize(V_);
          }
     }
}

int PCFG::one_iteration(Vecsent const& vs) {
     Index const omega = vs.size();
     // fill ftab_, gtab_, htab_ as in (11.28)
     for (Index i = 0; i < omega; i++) {
          m_ = vs[i].size();
          w_.resize(m_);
          for (Sentence::size_type k = 0; k < m_; k++)
               w_[k] = tmap_.find(vs[i][k])->second;
          fill_beta();
          fill_alpha();
          double const denominator = beta_[0][0][m_ - 1];
          if (denominator <= 1e-50)
               return 1;
          for (Index p = 0; p < m_; p++)
               for (Index q = p + 1; q < m_; q++)
                    for (Index j = 0; j < n_; j++)
                         for (Index r = 0; r < n_; r++)
                              for (Index s = 0; s < n_; s++) {
                                   double sum = 0.0;
                                   for (Index d = p; d < q; d++)
                                        sum += alpha_[j][p][q] *
                                               prob(j, r, s) *
                                               beta_[r][p][d] *
                                               beta_[s][d + 1][q];
                                   ftab_[i][p][q][j][r][s] =
                                        sum / denominator;
                              }
          for (Index h = 0; h < m_; h++)
               for (Index j = 0; j < n_; j++)
                    for (Index k = 0; k < V_; k++)
                         if (w_[h] == k)
                              gtab_[i][h][j][k] = alpha_[j][h][h] *
                                                  beta_[j][h][h] /
                                                  denominator;
                         else
                              gtab_[i][h][j][k] = 0.0;
          for (Index p = 0; p < m_; p++)
               for (Index q = p; q < m_; q++)
                    for (Index j = 0; j < n_; j++)
                         htab_[i][p][q][j] = alpha_[j][p][q] *
                                             beta_[j][p][q] /
                                             denominator;
     }

     for (Index j = 0; j < n_; j++) {
          double den = 0.0;
          for (Index i = 0; i < omega; i++)
               for (Index p = 0; p < vs[i].size(); p++)
                    for (Index q = p; q < vs[i].size(); q++)
                         den += htab_[i][p][q][j];
          if (den <= 1e-50)
               return 2;
          for (auto& p : bp_[j]) {
               double num = 0.0;
               Index const r = p.first.first;
               Index const s = p.first.second;
               for (Index i = 0; i < omega; i++)
                    for (Index p = 0; p < vs[i].size() - 1; p++)
                         for (Index q = p + 1; q < vs[i].size(); q++)
                              num += ftab_[i][p][q][j][r][s];
               p.second = num / den;
          }
          for (auto& p : up_[j]) {
               double num = 0.0;
               Index const k = p.first;
               for (Index i = 0; i < omega; i++)
                    for (Index h = 0; h < vs[i].size(); h++)
                         num += gtab_[i][h][j][k];
               p.second = num / den;
          }
     }
     return 0;
}

bool operator==(PCFG::Backtrace const& a, PCFG::Backtrace const& b) {
     return a.j == b.j && a.k == b.k && a.r == b.r;
}

bool operator!=(PCFG::Backtrace const& a, PCFG::Backtrace const& b) {
     return !(a == b);
}

bool operator==(PCFG const& a, PCFG const& b) {
     if (a.n_ != b.n_)
          return false;
     if (a.V_ != b.V_)
          return false;
     if (a.bp_ != b.bp_)
          return false;
     if (a.up_ != b.up_)
          return false;
     if (a.nonterminal_names_.size() != b.nonterminal_names_.size())
          return false;
     for (std::vector<std::string const*>::size_type i = 0;
          i < a.nonterminal_names_.size(); i++)
          if (*a.nonterminal_names_[i] != *b.nonterminal_names_[i])
               return false;
     if (a.terminal_names_.size() != b.terminal_names_.size())
          return false;
     for (std::vector<std::string const*>::size_type i = 0;
          i < a.terminal_names_.size(); i++)
          if (*a.terminal_names_[i] != *b.terminal_names_[i])
               return false;
     if (a.nmap_ != b.nmap_)
          return false;
     if (a.tmap_ != b.tmap_)
          return false;
     if (a.w_ != b.w_)
          return false;
     if (a.m_ != b.m_)
          return false;
     if (a.beta_ != b.beta_)
          return false;
     if (a.alpha_ != b.alpha_)
          return false;
     if (a.delta_ != b.delta_)
          return false;
     if (a.psi_ != b.psi_)
          return false;
     if (a.ftab_ != b.ftab_)
          return false;
     if (a.gtab_ != b.gtab_)
          return false;
     if (a.htab_ != b.htab_)
          return false;
     return true;
}

bool operator!=(PCFG const& a, PCFG const& b) {
     return !(a == b);
}

std::ostream& operator<<(std::ostream& stream, PCFG const& g) {
     PCFG::Index nb = 0, nu = 0;
     for (PCFG::Index i = 0; i < g.bp_.size(); i++) {
          nb += g.bp_[i].size();
          nu += g.up_[i].size();
     }
     stream << "Number of nonterminals: "
            << g.nonterminal_names_.size() << '\n'
            << "Number of terminals: " << g.terminal_names_.size()
            << '\n'
            << "Number of binary productions: " << nb << '\n'
            << "Number of unary productions: " << nu << '\n';
     stream << "\nNonterminals:\n";
     for (PCFG::Index j = 0; j < g.nonterminal_names_.size(); j++)
          stream << j << ' ' << *g.nonterminal_names_[j] << '\n';
     stream << "\nTerminals:\n";
     for (PCFG::Index k = 0; k < g.terminal_names_.size(); k++)
          stream << k << ' ' << *g.terminal_names_[k] << '\n';
     stream << "\nBinary productions:\n";
     for (PCFG::Index i = 0; i < g.bp_.size(); i++)
          for (auto const& q : g.bp_[i]) {
               stream << q.second << ' ' << *g.nonterminal_names_[i]
                      << " --> "
                      << *g.nonterminal_names_[q.first.first] << ' '
                      << *g.nonterminal_names_[q.first.second]
                      << '\n';
          }
     stream << "\nUnary productions:\n";
     for (PCFG::Index i = 0; i < g.up_.size(); i++)
          for (auto const& q : g.up_[i]) {
               stream << q.second << ' ' << *g.nonterminal_names_[i]
                      << " --> " << *g.terminal_names_[q.first]
                      << '\n';
          }
     return stream;
}

void add_probabilities(Vecprod const& vp, std::vector<double>& prob,
                       RNG& rng) {
     CNF_validator validator;
     if (!validator.is_valid(vp))
          throw std::invalid_argument("grammar not in CNF");
     prob.resize(vp.size());
     for (auto const& nt : validator.v()) {
          Vecprod::size_type const nprods = std::count_if(
               vp.begin(), vp.end(),
               [nt](Production const& p) { return p.lhs == nt; });
          Vecdouble x(nprods);
          rng.simplex_surface(x);
          Vecprod::size_type k = 0;
          for (Vecprod::size_type i = 0; i < vp.size(); i++)
               if (vp[i].lhs == nt)
                    prob[i] = x[k++];
          assert(k == nprods);
     }
}

}  // namespace SHG::PLP
