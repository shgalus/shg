/* utils.h: miscellaneous utilities */

/**
 * \file include/shg/utils.h
 * Miscellaneous utilities.
 * \date Created on 3 June 2009.
 */

#ifndef SHG_UTILS_H
#define SHG_UTILS_H

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <valarray>
#include <vector>
#include "shg/vector.h"

namespace SHG {

/**
 * \defgroup miscellaneous_utilities Miscellaneous utilities
 *
 * Miscellaneous utilities.
 *
 * \{
 */

/**
 * Returns square of the argument.
 */
template<class T>
inline T sqr(T a) {return a * a;}

/**
 * Returns maximum of the two arguments.
 */
template<class T>
inline const T& max(const T& a, const T& b) {return a < b ? b : a;}

/**
 * Returns minimum of the two arguments.
 */
template<class T>
inline const T& min(const T& a, const T& b) {return a < b ? a : b;}

/**
 * Returns signum of the argument. \f[ \mathrm{sgn}(x) = \left\{
 * \begin{array}{rl} 1 & \mbox{if $x > 0$,} \\ 0 & \mbox{if $x = 0$,}
 * \\ -1 & \mbox{if $x < 0$.} \end{array} \right. \f]
 */
template<class T>
inline int sgn(T x) {return x > 0 ? 1 : (x < 0 ? -1 : 0);}

/**
 * Integer version of floor.
 */
template <class I = int, class F>
I ifloor(F x) {
     static_assert(std::is_integral<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_signed<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_arithmetic<F>::value,
                   "F must be an arithmetic type.");
     return static_cast<I>(std::floor(x));
}

/**
 * Integer version of ceil.
 */
template <class I = int, class F>
I iceil(F x) {
     static_assert(std::is_integral<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_signed<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_arithmetic<F>::value,
                   "F must be an arithmetic type.");
     return static_cast<I>(std::ceil(x));
}

/**
 * Calculates quotient and remainder of two integers.
 *
 * For each pair of integer numbers \f$a\f$ and \f$b\f$, where \f$ b
 * \neq 0\f$, there exists exactly one pair of integer numbers \f$q\f$
 * and \f$r\f$ (quotient and remainder), for which \f[ a = qb + r \;
 * \mbox{and} \; 0 \leq r < |b|. \f] (See \cite mostowski-stark-1975,
 * page 173).
 *
 * \implementation
 *
 * The standard \cite ansi-cpp-2012 states in paragraph 5.6.4:
 *
 * > The binary / operator yields the quotient, and the binary %
 * > operator yields the remainder from the division of the first
 * > expression by the second. If the second operand of / or % is zero
 * > the behavior is undefined. For integral operands the / operator
 * > yields the algebraic quotient with any fractional part discarded
 * > (this is often called truncation towards zero); if the quotient
 * > a/b is representable in the type of the result, (a/b)*b + a\%b is
 * > equal to a.
 *
 * We denote by \f$\oslash\f$ division in C++. It is defined by \f[a
 * \oslash b = \mathrm{trunc}\left(a / b \right) = \left\{
 * \begin{array}{ll} \lfloor a / b \rfloor & \mbox{if $a / b \geq 0$,}
 * \\ \lceil a / b \rceil & \mbox{if $a / b \leq 0$.} \end{array}
 * \right. \f] Using inequalities \f$x - 1 < \lfloor x \rfloor \leq
 * x\f$ and \f$x \leq \lceil x \rceil < x + 1\f$ for \f$x = a / b\f$
 * we get four cases: \f{align*}{ a / b \geq 0, \quad b > 0, \quad 0 &
 * \leq a - b \times (a \oslash b) < b, \\ a / b \geq 0, \quad b < 0,
 * \quad b & < a - b \times (a \oslash b) \leq 0, \\ a / b \leq 0,
 * \quad b > 0, \quad -b & < a - b \times (a \oslash b) \leq 0, \\ a /
 * b \leq 0, \quad b < 0, \quad 0 & \leq a - b \times (a \oslash b) <
 * -b. \f} In the first and the last cases, that is for \f$a \geq
 * 0\f$, \f$a \oslash b\f$ and \f$a \% b\f$ give the correct results.
 * To get the correct results in the remaining cases, that is for \f$a
 * < 0\f$, if \f$a \% b \neq 0\f$, we have to decrease \f$a \% b\f$ by
 * \f$b\f$ and increase \f$a \oslash b\f$ by 1 in the second case and
 * increase \f$a \% b\f$ by \f$b\f$ and decrease \f$a \oslash b\f$ by
 * 1 in the third case. Let us note that for \f$a \geq 0\f$ the
 * results of operators \f$\oslash\f$ and \% are correct. Let us also
 * note that the second and the third case which requires correction
 * may be detected by testing whether \f$ a \% b \f$ is less than 0 or
 * not.
 */
template <class T>
struct Integer_division {
     /**
      * The constructor accepts \f$a\f$ and \f$b\f$ and the created
      * object contains \f$q\f$ and \f$r\f$ described in
      * Integer_division.
      *
      * \exception std::invalid_argument if \f$b = 0\f$
      */
     Integer_division(T a, T b);
     T q;                       /**< quotient */
     T r;                       /**< remainder */
     /**
      * Returns quotient described in Integer_division.
      *
      * \exception std::invalid_argument if \f$b = 0\f$
      */
     static T quotient(T a, T b);
     /**
      * Returns remainder described in Integer_division.
      *
      * \exception std::invalid_argument if \f$b = 0\f$
      */
     static T remainder(T a, T b);
};

template <class T>
Integer_division<T>::Integer_division(T a, T b)
     : q(), r() {
     if (b == 0)
          throw std::invalid_argument(
               "Integer_division::Integer_division");
     auto d = std::div(a, b);
     assert((a < 0 && d.rem != 0) == (d.rem < 0));
     if (d.rem < 0) {
          if (b < 0) {
               q = d.quot + 1;
               r = d.rem - b;
          } else {
               q = d.quot - 1;
               r = d.rem + b;
          }
     } else {
          q = d.quot;
          r = d.rem;
     }
}

template <class T>
T Integer_division<T>::quotient(T a, T b) {
     if (b == 0)
          throw std::invalid_argument("Integer_division::quotient");
     auto d = std::div(a, b);
     return d.rem < 0 ?
          b < 0 ? d.quot + 1 : d.quot - 1
          : d.quot;
}

template <class T>
T Integer_division<T>::remainder(T a, T b) {
     if (b == 0)
          throw std::invalid_argument("Integer_division::remainder");
     const T r = a % b;
     return r < 0 ? r + std::abs(b) : r;
}

/**
 * Writes a variable to a binary stream.
 */
template <class T>
inline void write_binary(const T& a, std::ostream& f) {
     f.write(reinterpret_cast<const char*>(&a), sizeof a);
}

/**
 * Reads a variable from a binary stream.
 */
template <class T>
inline void read_binary(T& a, std::istream& f) {
     f.read(reinterpret_cast<char*>(&a), sizeof a);
}

/**
 * Reads whole file into a character array. The file \a filename is
 * opened for reading in binary mode.
 *
 * \exception std::bad_alloc if there is no memory
 *
 * \exception SHG::File_error if there was an error when reading the
 * file
 */
Vecchar wfread(const char* filename);

/**
 * Skips comment and returns the next non-empty line.
 *
 * The function reads the next line from the stream f into s. Then,
 * eventual comment and leading and trailing whitespace are discarded
 * from s. If s turns out to be empty, the function continues with the
 * next line. The function returns f. It is asserted at the end of the
 * function that s.size() > 0 if and only if (bool)f is true.
 *
 * A comment extends from the first occurence of c to the end of line.
 */
std::istream& skipcomment(std::istream& f,
                          std::string& s,
                          const std::string& c = "#");

/**
 * Constant string containing six whitespace characters.
 */
extern const char* const white_space;

/**
 * Removes characters from the left of a string.
 */
std::string& ltrim(std::string& s,
                   const std::string& trimchars = white_space);

/**
 * Removes characters from the right of a string.
 */
std::string& rtrim(std::string& s,
                   const std::string& trimchars = white_space);

/**
 * Removes characters from both sides of a string.
 */
std::string& trim(std::string& s,
                  const std::string& trimchars = white_space);

/**
 * Cleans a string. Removes characters from both sides of a string and
 * replaces every sequence of characters by a single character.
 */
std::string& clean_string(std::string& s,
                          const std::string& trimchars = white_space,
                          char replace_char = ' ');

/**
 * Creates vector of strings from a string of fields.
 *
 * Creates a vector of fields from s separated by characters from sep.
 * If sep == "", the returned vector has no elements if s == "",
 * otherwise it has exactly one element; if sep != "", the returned
 * vector has no elements if s == "" or s does not contain characters
 * other than those from sep.
 */
std::vector<std::string>
split_string(const std::string& s,
             const std::string& sep = white_space);

/**
 * Indirectly sorts a vector.
 *
 * Given a const vector<T>& w, the function returns such a vector v
 * that w[v[0]] <= w[v[1]] <= ... <= w[v[n - 1]], and v.size() ==
 * w.size() = n. The vector v is a permutation of 0, 1, ..., n - 1.
 */
template <class T>
std::vector<typename std::vector<T>::size_type>
indirect_sort(const std::vector<T>& w) {
     typedef typename std::vector<T>::size_type size_type;
     typedef typename std::vector<size_type>::size_type size_type1;
     const size_type1 n = w.size();
     std::vector<size_type> v(n);
     for (size_type1 i = 0; i < n; i++)
          v[i] = i;
     std::sort(v.begin(), v.end(), [&w](size_type i, size_type j) {
               return w[i] < w[j];
          });
     return v;
}

/**
 * Binary searching in a sorted subvector using relational operators.
 * Searches for a key in sorted v[l], ..., v[u - 1]. Returns true if
 * the key is found, then v[i] == key. If the key is not found, the
 * value of is unchanged. The function works with both std::vector and
 * SHG::Vector.
 *
 * \internal
 *
 * Previous version of this function was like this:
 *
 * template<class T>
 * bool vbsearch(const T& key, const std::vector<T>& v,
 *               typename std::vector<T>::size_type l,
 *               typename std::vector<T>::size_type u,
 *               typename std::vector<T>::size_type& i) {
 *      if (u-- == 0 || u < l)
 *           return false;
 *
 *      for(;;) {
 *           i = (l + u) / 2;
 *           if (key < v[i]) {
 *                if (i <= l)
 *                     return false;
 *                u = i - 1;
 *           } else if (key > v[i]) {
 *                if (i >= u)
 *                     return false;
 *                l = i + 1;
 *           } else
 *                return true;
 *      }
 * }
 */
template<class T, class V, class I>
bool vbsearch(const T& key, const V& v, I l, I u, I& i) {
     if (u < l)
          return false;
     const auto end = v.begin() + u;
     const auto it = std::lower_bound(v.begin() + l, end, key);
     if (it != end && !(key < *it)) {
          i = std::distance(v.begin(), it);
          return true;
     }
     return false;
}

/**
 * Binary searching in a sorted vector using relational operators.
 * Searches for a key in sorted v. Returns true if the key is found,
 * then v[i] == key. If the key is not found, the value of is
 * unchanged. The function works with both std::vector and
 * SHG::Vector.
 */
template<class T, class V, class I>
inline bool vbsearch(const T& key, const V& v, I& i) {
     return vbsearch(key, v, static_cast<I>(0),
                     static_cast<I>(v.size()), i);
}

/**
 * Binary searching in a sorted subvector using comparing function.
 * Searches for a key in sorted v[l], ..., v[u - 1]. Returns true if
 * the key is found, then v[i] == key. If the key is not found, the
 * value of is unchanged. The comparing function must be such that
 * comp(x, y) == true if x < y. The function works with both
 * std::vector and SHG::Vector.
 */
template<class T, class V, class I, class Compare>
bool vbsearch(const T& key, const V& v, I l, I u, I& i, Compare comp) {
     if (u < l)
          return false;
     const auto end = v.begin() + u;
     const auto it = std::lower_bound(v.begin() + l, end, key, comp);
     if (it != end && !comp(key, *it)) {
          i = std::distance(v.begin(), it);
          return true;
     }
     return false;
}

/**
 * Binary searching in a sorted vector using comparing function.
 * Searches for a key in sorted v. Returns true if the key is found,
 * then v[i] == key. If the key is not found, the value of is
 * unchanged. The comparing function must be such that comp(x, y) ==
 * true if x < y. The function works with both std::vector and
 * SHG::Vector.
 */
template<class T, class V, class I, class Compare>
inline bool vbsearch(const T& key, const V& v, I& i, Compare comp) {
     return vbsearch(key, v, static_cast<I>(0),
                     static_cast<I>(v.size()), i, comp);
}

/**
 * Searches for a key in an unsorted vector.
 *
 * Returns true if the key is found, otherwise returns false. If the
 * key is found, then v[i] == key.
 */
template<class T>
bool vsearch(const T& key, const std::vector<T>& v,
             typename std::vector<T>::size_type& i) {
     for (i = 0; i < v.size(); i++)
          if (v[i] == key)
               return true;
     return false;
}

/**
 * Searches for a key in an unsorted vector.
 *
 * Returns index of the key in the vector v. If the key is not found,
 * it is added.
 */
template<class T>
typename std::vector<T>::size_type
vadd(const T& key, std::vector<T>& v) {
     typename std::vector<T>::size_type i;
     for (i = 0; i < v.size(); i++)
          if (v[i] == key)
               return i;
     v.push_back(key);
     return i;
}

/**
 * Removes unneeded white space from a string.
 *
 * Removes white space from both sides of s and replaces every
 * sequence of white space by a single space. Returns the string s.
 */
char* strtrim(char* s);

/**
 * Reentrant version of strtok.
 *
 * See description of standard C library function strtok from cstring.
 * The function strrtok keeps information which is necessary between
 * calls in the variable next which is set at the first call and
 * should not be changed between calls. The implementation follows
 * that of Linux strtok_r(3).
 */
char* strrtok(char* s, const char* delim, char** next);

/**
 * Duplicates a string. It allocates space with new(nothrow). If the
 * returned value is different from 0, it should be released with
 * delete[].
 */
char* strdup(const char* s);

/**
 * Allocates C-style matrix. An \a m by \a n matrix is allocated as
 * one memory block with pointers to rows. The matrix should be
 * deallocated by free_c_matrix(T**).
 *
 * \exception std::bad_alloc if there is no memory
 */
template <class T>
T** alloc_c_matrix(std::size_t m, std::size_t n) {
     T** p = new T*[m];
     try {p[0] = new T[m * n];}
     catch (std::bad_alloc) {delete[] p; throw;}
     for (std::size_t i = 1; i < m; i++)
          p[i] = p[i - 1] + n;
     return p;
}

/**
 * Deallocates C-style matrix. The matrix should be allocated by
 * alloc_c_matrix(std::size_t, std::size_t).
 */
template <class T>
void free_c_matrix(T** p) {
     if (p) {
          delete[] p[0];
          delete p;
     }
}

/**
 * Greatest common divisor. See \cite knuth-2002b.
 */
template <class T>
T gcd(T u, T v) {
     T r;
     if (u < 0)
          u = -u;
     if (v < 0)
          v = -v;
     while (v) {
          r = u % v;
          u = v;
          v = r;
     }
     return u;
}

/**
 * Greatest common divisor for unsigned int type. Specialization
 * without checking if u < 0 or v < 0. \sa SHG::gcd.
 */
template <>
unsigned gcd<unsigned>(unsigned u, unsigned v);

/**
 * Measures time intervals in seconds.
 */
class Timer {
public:
     /**
      * Starts measuring time. Remembers the moment of time at which
      * the constructor was called.
      */
     Timer() : start_time(std::chrono::steady_clock::now()) {}

     /**
      * Restarts measuring time. Remembers the moment of time at which
      * this function was called.
      */
     void restart() {start_time = std::chrono::steady_clock::now();}

     /**
      * Returns the difference in seconds between the current time and
      * the time remembered in Timer() or in restart().
      */
     double elapsed() const {
          typedef std::chrono::duration<double> T;
          return std::chrono::duration_cast<T>(
               std::chrono::steady_clock::now() - start_time).count();
     }

private:
     std::chrono::time_point<std::chrono::steady_clock> start_time;
};

/**
 * Wrapper class for a program.
 */
class Program {
public:
     Program();
     Program(int argc, char* argv[]);
     virtual ~Program();
     virtual int main();
     virtual int help();
     int run();
     const char* getname() const;
     void setname(const std::string& name);
protected:
     std::vector<std::string> argv;
     std::string progname;
};

/**
 * Generates all k-combinations of an n-set in lexicographic oreder.
 * There must be 0 < k <= n. The number of combinations is \f[{n
 * \choose k} = \frac{n!}{k!(n - k)!}.\f]
 *
 * \example_program \include comblex.cc
 *
 * \date Written on 17 May 2010.
 *
 * \implementation See \cite reingold-nievergelt-deo-1985, p. 181.
 */
class Comblex {
public:
     /**
      * Initializes generator for the n-element set and k-element
      * combinations.
      *
      * \exception std::invalid_argument if k <= 0 or  n < k
      */
     Comblex(int n, int k);
     /**
      * Advance to the next combination.
      *
      * \returns true if the next combination has been generated,
      * false if there is no next combination
      */
     bool next();
     /**
      * Returns current combination.
      */
     const std::vector<int>& get() const {return a;}

private:
     const int k;
     const int n1;
     const int k1;
     int j;
     std::vector<int> a;
};

/**
 * Returns true if a generalized slice is degenerate. The notion of
 * degeneracy is stronger than that in \cite ansi-cpp-2012,
 * iso.26.6.6.1. The generalized slice \a g is degenerate if
 *
 * - g.size().size() < 1 or
 * - g.stride.size() != g.size().size() or
 * - g.size()[i] < 1 for a certain i or
 * - g.stride()[i] < 1 for a certain i or
 * - an address generated by \a g is repeated.
 *
 * \note A generalized slice \a g generates the set of addresses \f[
 * \left\{s + \sum_{j = 0}^{n - 1} i_jd_j \colon 0 \leq i_j < l_j, j =
 * 0, \ldots, n - 1 \right\}, \f] where \f$s = g.start()\f$, \f$l =
 * g.size()\f$, \f$d = g.stride()\f$, \f$n = l.size() = d.size()\f$.
 */
bool isdegenerate(const std::gslice& g);

/**
 * Returns true if a generalized slice is valid for a valarray. This
 * means that the slice is not degenerate and all addresses it
 * generates are within the range of the valarray of size \a n.
 */
bool isvalid(std::size_t n, const std::gslice& g);

/** \} */ /* end of group miscellaneous_utilities */

}       // namespace SHG

#endif
