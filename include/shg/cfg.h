/**
 * \file include/shg/cfg.h
 * Context-free grammars.
 */

#ifndef SHG_CFG_H
#define SHG_CFG_H

#include <stdexcept>
#include <string>
#include <set>
#include <vector>
#include <shg/tree.h>

namespace SHG::PLP {

/** \addtogroup polish_language_processing */
/** \{ */

/** \defgroup context_free_grammars Context-free grammars
 *
 * Functions related to context-free grammars.
 *
 * \{
 */

class CFG_error : public std::runtime_error {
public:
     CFG_error();
     explicit CFG_error(char const* message);
     CFG_error(char const* file, int line);
};

class Empty_sentence : public CFG_error {
public:
     Empty_sentence();
};

class Unknown_terminal : public CFG_error {
public:
     Unknown_terminal();
};

class Invalid_grammar : public CFG_error {
public:
     Invalid_grammar();
};

struct Production {
     std::string lhs{};
     std::vector<std::string> rhs{};
};

bool operator==(Production const& a, Production const& b);
bool operator!=(Production const& a, Production const& b);
std::string to_string(Production const& p);
std::ostream& operator<<(std::ostream& stream, Production const& p);

using Vecprod = std::vector<Production>;

/**
 * Vecprod is valid if it is not empty, each symbol has non-empty name
 * and productions are different.
 */
bool is_valid(Vecprod const& vp);

/**
 * Checks if Vecprod is a valid context-free grammar in Chomsky normal
 * form without S --> epsilon.
 */
class CNF_validator {
public:
     bool is_valid(Vecprod const& vp);
     /** Returns the set of variables. */
     std::set<std::string> const& v() const;
     /** Returns the set of terminals. */
     std::set<std::string> const& t() const;
     /** Returns the start symbol. */
     std::string const& s() const;

private:
     std::set<std::string> v_{};  // variables
     std::set<std::string> t_{};  // terminals
     std::string s_{};            // start symbol
};

bool operator==(Vecprod const& a, Vecprod const& b);
bool operator!=(Vecprod const& a, Vecprod const& b);

std::string to_string(Vecprod const& vp);
std::string to_string2(Vecprod const& vp);

using Sentence = std::vector<std::string>;
using Vecsent = std::vector<Sentence>;

std::string to_string(Sentence const& sent);
std::string to_string(Vecsent const& vs);

class CYK {
public:
     class Parse_node {
     public:
          Parse_node() = default;
          explicit Parse_node(std::string node);
          Parse_node(std::string node, Vecprod::size_type production);
          std::string node() const;
          Vecprod::size_type production() const;
          std::string to_string() const;

     private:
          std::string node_{};
          Vecprod::size_type production_{};
     };
     using Parse_tree = Tree<Parse_node>;

     void set_grammar(Vecprod const& cfg);
     bool recognize(Sentence const& X);
     void parse(Sentence const& X);
     Parse_tree const& gen();

private:
     struct Info {
          Info(Vecprod::size_type p, int il, int jl, int kl, int ir,
               int jr, int kr);
          explicit Info(Vecprod::size_type p);
          Vecprod::size_type p;  // number of production
          int il, jl, kl;        // left nonterminal position in V_
          int ir, jr, kr;        // right nonterminal position in V_
     };
     void add_production(int j, int i, int k);
     void add_node(Info const& q, Parse_tree& t);

     Vecprod const* cfg_{nullptr};
     std::vector<std::vector<std::vector<Info>>> V_{};
     Parse_tree tree_{};
     std::set<std::string> v_{};  // variables
     std::set<std::string> t_{};  // terminals
     std::string s_{};            // start symbol
};

/**
 * The class to convert Vecprod to Chomsky Normal Form.
 * The implementation follows \cite chomsky-normal-form-2021
 * (<a href = "cnf.pdf">local copy</a>).
 */
class Conversion_to_CNF {
public:
     void convert(Vecprod const& vp);
     Vecprod const& vp() const;
     Vecprod const& cfg() const;

private:
     static char const reserved_char_ = '_';
     Vecprod vp_{};                  // old Vecprod
     Vecprod cfg_{};                 // new Vecprod
     Vecprod w_{};                   // work Vecprod
     std::set<std::string> v_{};     // variables
     std::set<std::string> t_{};     // terminals
     std::string s_{};               // start symbol
     std::set<std::string> null_{};  // nullable nonterminals
     int nnewv_{};  // number of newly added variables

     void set_variables();
     void set_terminals();
     std::string newname();
     // Eliminates the start symbol from right-hand sides.
     void start();
     // Eliminates rules with nonsolitary terminals.
     void term();
     // Eliminates right-hand sides with more than 2 nonterminals.
     void bin();
     // Eliminates epsilon-rules.
     void del();
     // Eliminates unit rules.
     void unit();
     // Orders productions.
     void order();
     // Determine nullable nonterminals.
     void nullable();
     // Expand production for del().
     void expand(Production const& p);
     /**
      * Remove useless variables and productions.
      *
      * \todo This function does not remove useless variables. Adopt
      * algorithm from \cite hopcroft-ullman-2003, section 4.4, pages
      * 106-109.
      */
     void remove_useless();
};

/**
 * Returns CFG in CNF with \c n nonterminals and \c m terminals
 * having all possible productions A --> B C and A --> w.
 */
Vecprod full_cnf(unsigned n, unsigned m);

inline std::set<std::string> const& CNF_validator::v() const {
     return v_;
}

inline std::set<std::string> const& CNF_validator::t() const {
     return t_;
}

inline std::string const& CNF_validator::s() const {
     return s_;
}

inline std::string CYK::Parse_node::node() const {
     return node_;
}

inline Vecprod::size_type CYK::Parse_node::production() const {
     return production_;
}

inline std::string CYK::Parse_node::to_string() const {
     return node_;
}

inline Vecprod const& Conversion_to_CNF::vp() const {
     return vp_;
}

inline Vecprod const& Conversion_to_CNF::cfg() const {
     return cfg_;
}

/** \} */
/** \} */

}  // namespace SHG::PLP

#endif
