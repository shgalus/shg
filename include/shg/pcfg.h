/**
 * \file include/shg/pcfg.h
 * Probabilistic context free grammar.
 */

#ifndef SHG_PCFG_H
#define SHG_PCFG_H

#include <map>
#include <limits>
#include <tuple>
#include <shg/cfg.h>
#include <shg/tree.h>
#include <shg/rng.h>

namespace SHG::PLP {

struct S_production {
     Production prod{};
     double prob{};
};

std::ostream& operator<<(std::ostream& stream,
                         S_production const& sp);

using Vecsprod = std::vector<S_production>;

/**
 * Splits Vecsprod into Vecprod and vector of probabilities.
 */
std::pair<Vecprod, std::vector<double>> split(Vecsprod const& vsp);

std::ostream& operator<<(std::ostream& stream, Vecsprod const& vsp);

/**
 * Probabilistic context free grammar.
 *
 * Written according to \cite manning-schutze-2003.
 *
 * \implementation
 *
 * There are \f$n\f$ nonterminals numbered from \f$0\f$ to \f$n - 1\f$
 * and \f$V\f$ terminals numbered from \f$0\f$ to \f$V - 1\f$.
 * Original names of terminals are stored in #nonterminal_names_ and
 * #terminal_names_. #nmap_ and #tmap_ map names to numbers. Binary
 * and unary productions are stored in #bp_ and #up_.
 *
 */
class PCFG {
public:
     using Index = std::size_t;
     struct Parse_node {
          std::string symbol{};  ///< Nonterminal or terminal name.
          double prob{};         ///< Probability for nonterminals.
          bool is_terminal{false};
          std::string to_string() const;
     };
     using Parse_tree = Tree<Parse_node>;

     PCFG() = default;
     PCFG(PCFG const& other);
     PCFG(PCFG&& other) noexcept;
     PCFG& operator=(PCFG const& other);
     PCFG& operator=(PCFG&& other);

     /**
      * Sets the grammar. \c vp must be in Chomsky normal form. \c
      * prob must contain probabilities such that the production \c
      * vp[i] has probability \c prob[i].
      */
     void set(Vecprod const& vp, std::vector<double> const& prob,
              double tol = 100.0 *
                           std::numeric_limits<double>::epsilon());

     void clear();
     bool is_valid() const;
     Vecsprod vecsprod() const;

     /**
      * Calculates probability of the string \c w using inside
      * probabilities according to formulas
      * \cite manning-schutze-2003, (11.16), (11.17) on page 392.
      */
     double prob(Sentence const& w);

     /**
      * Calculates logarithm of probability of all the strings in \c
      * vs as if they were independent. The vector \c vs must not be
      * empty.
      */
     double log_prob(Vecsent const& vs);

     /**
      * Calculates probability of the string \c w using outside
      * probabilities according to formulas
      * \cite manning-schutze-2003, (11.18), (11.19) on page 394.
      * This function is added for easy testing alpha().
      */
     double prob_outside(Sentence const& w, Index k);

     /**
      * Finds the most probable parse for the sentence \f$w\f$. See
      * \cite manning-schutze-2003, section 11.3.3 on pages 396 - 398.
      */
     void cyk(Sentence const& w, double& prob, Parse_tree& t);

     int inside_outside(Vecsent const& vs, double accuracy,
                        double tol, int max_iter, int& iter,
                        double& logp);

     /**
      * Removes productions with probabilities less than or equal to
      * \f$\varepsilon \in [0, 1)\f$.
      *
      * \todo This function does not remove useless variables. Adopt
      * algorithm from \cite hopcroft-ullman-2003, section 4.4, pages
      * 106-109.
      */
     void remove_productions(double eps);

private:
     using Binary = std::map<std::pair<Index, Index>, double>;
     using Unary = std::map<Index, double>;
     using Prob_table = std::vector<std::vector<std::vector<double>>>;
     struct Backtrace {
          Index j;
          Index k;
          Index r;
     };
     using Backtrace_table =
          std::vector<std::vector<std::vector<Backtrace>>>;
     using F_table = std::vector<std::vector<std::vector<
          std::vector<std::vector<std::vector<double>>>>>>;
     using GH_table =
          std::vector<std::vector<std::vector<std::vector<double>>>>;

     Index n_{};  ///< Number of nonterminals \f$n\f$.
     Index V_{};  ///< Number of terminals \f$V\f$.

     /**
      * %Vector of binary productions. \f$\var{bp\_}[j]\f$ contains
      * pairs \f$((r, s), \var{prob})\f$ such that
      * \f$\Pr(N^j \rightarrow N^r N^s) = \var{prob}\f$.
      */
     std::vector<Binary> bp_{};
     /**
      * %Vector of unary productions. \f$\var{up\_}[j]\f$ contains
      * pairs \f$(k, \var{prob})\f$ such that \f$\Pr(N^j \rightarrow
      * w_k) = \var{prob}\f$.
      */
     std::vector<Unary> up_{};

     /** Pointers to names of nonterminals. */
     std::vector<std::string const*> nonterminal_names_{};
     /** Pointers to names of terminals. */
     std::vector<std::string const*> terminal_names_{};
     /// Maps nonterminal names to numbers.
     std::map<std::string, Index> nmap_{};
     /// Maps terminal names to numbers.
     std::map<std::string, Index> tmap_{};

     std::vector<Index> w_{};  ///< Current sentence \f$w\f$.
     Index m_{};  ///< Length \f$m\f$ of current sentence \f$w\f$.

     /**
      * Table of inside probabilities \cite manning-schutze-2003,
      * pages 392-393. \f$\var{beta\_}(j, p, q) = \beta_j(p, q), \quad
      * 0 \leq j < n, \quad 0 \leq p \leq q < m\f$.
      */
     Prob_table beta_{};
     /**
      * Table of outside probabilities \cite manning-schutze-2003,
      * pages 394-396. \f$\var{alpha\_}(j, p, q) = \alpha_j(p, q),
      * \quad 0 \leq j < n, \quad 0 \leq p \leq q < m\f$.
      */
     Prob_table alpha_{};

     /**
      * \name Tables used in %CYK algorithm.
      *  \{
      */
     /**
      * Table of the greatest inside probabilities
      * \cite manning-schutze-2003, pages 396-398. \f$\var{delta\_}(i,
      * p, q) = \delta_i(p, q), \quad 0 \leq i < n, \quad 0 \leq p
      * \leq q < m\f$.
      */
     Prob_table delta_{};
     /**
      * Table of backtrace information for %CYK algorithm
      * \cite manning-schutze-2003, pages 397-398. \f$\var{psi\_}(i,
      * p, q) = \psi_i(p, q), \quad 0 \leq i < n, \quad 0 \leq p \leq
      * q < m\f$.
      */
     Backtrace_table psi_{};
     /** \} */

     /**
      * \name Tables used in inside-outside algorithm.
      *  \{
      */
     /**
      * The table 11.28 for \f$f_i(p, q, j, r, s)\f$,
      * \cite manning-schutze-2003, page 400.
      */
     F_table ftab_{};
     /**
      * The table 11.28 for \f$g_i(h, j, k)\f$,
      * \cite manning-schutze-2003, page 400.
      */
     GH_table gtab_{};
     /**
      * The table 11.28 for \f$h_i(p, q, j)\f$,
      * \cite manning-schutze-2003, page 400.
      */
     GH_table htab_{};
     /** \} */

     void check() const;

     /**
      * Returns true if probabilities are valid.
      */
     bool probs_valid(double tol) const;

     /// Returns \f$\Pr(N^j \rightarrow N^r N^s)\f$.
     double prob(Index j, Index r, Index s);
     /// Returns \f$\Pr(N^j \rightarrow w_k)\f$.
     double prob(Index j, Index k);

     double log_prob(Vecsent const& vs, bool& ok);

     /**
      * Translates terminals in \c w into terminal numbers and puts
      * them in \c w_.
      */
     void set(Sentence const& w);

     void add_node(Index i, Index p, Index q, Parse_tree& t);

     /**
      * Calculates \f$\beta_j(p, q)\f$ according to formulas
      * \cite manning-schutze-2003, pages 392, 393.
      */
     void fill_beta();

     /**
      * Calculates \f$\alpha_j(p, q)\f$ according to formulas
      * \cite manning-schutze-2003, pages 394 - 396.
      *
      * \todo What about rules \f$N^j \rightarrow N^j N^j\f$? Should
      * they be counted twice? Check this on grammar having such a
      * production and compoare with probability counted with beta.
      */
     void fill_alpha();

     /**
      * Calculates \f$\delta_i(p, q)\f$ according to formulas
      * \cite manning-schutze-2003, page 397.
      */
     void fill_delta();

     void init_tables(Vecsent const& vs);
     void fill_names();
     int one_iteration(Vecsent const& vs);

     friend bool operator==(Backtrace const& a, Backtrace const& b);
     friend bool operator!=(Backtrace const& a, Backtrace const& b);
     friend bool operator==(PCFG const& a, PCFG const& b);
     friend std::ostream& operator<<(std::ostream& stream,
                                     PCFG const& g);
};

bool operator==(PCFG const& a, PCFG const& b);
bool operator!=(PCFG const& a, PCFG const& b);

std::ostream& operator<<(std::ostream& stream, PCFG const& g);

/**
 * Given context free grammar in Chomsky normal form, generates random
 * probability for each production so as the grammar and probabilities
 * could be used in PCFG::set(Vecprod const&, std::vector<double>
 * const&). Probabilities are put into \f$\var{prob}\f$.
 * \f$\var{rng}\f$ is used to generate probabilities.
 */
void add_probabilities(Vecprod const& vp, std::vector<double>& prob,
                       RNG& rng);

}  // namespace SHG::PLP

#endif
