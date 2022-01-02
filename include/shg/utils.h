/**
 * \file include/shg/utils.h
 * Miscellaneous utilities.
 * \date Created on 3 June 2009.
 */

#ifndef SHG_UTILS_H
#define SHG_UTILS_H

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <valarray>
#include <vector>
#include <shg/vector.h>

namespace SHG {

/**
 * \defgroup miscellaneous_utilities Miscellaneous utilities
 *
 * Miscellaneous utilities.
 *
 * \{
 */

constexpr double tolerance{100.0 *
                           std::numeric_limits<double>::epsilon()};

/**
 * Eliminates warnings for unused variables. For the previous
 * implementation, see \cite misfeldt-bumgardner-gray-2004, page 124.
 */
template <typename... Types>
void ignore_unused_variable(Types...) {}

/**
 * Explicit conversion function for conversion between two scalar
 * numeric types. See \cite stroustrup-2013, section 11.5, page 299.
 */
template <typename Target, typename Source>
Target narrow_cast(Source x) {
     auto y = static_cast<Target>(x);
     if (static_cast<Source>(y) != x)
          throw std::runtime_error("narrow_cast<>() failed");
     return y;
}

/**
 * Returns true if and only if \c n is a prime number.
 *
 * \implementation Each integer number can be expressed as \f$6k +
 * i\f$ for certain integer \f$k\f$ and \f$i \in \{-1, 0, 1, 2, 3, 4\}
 * \f$. \f$6k + 0, 6k + 2, 6k + 3, 6k + 4\f$ are not prime and \f$6k -
 * 1, 6k + 1\f$ may be prime. The algorith checks if \f$n\f$ is
 * divisible by the numbers of the form \f$6k - 1, 6k + 1\f$.
 */
bool is_prime(int n);

/**
 * Returns square of the argument.
 */
template <class T>
constexpr inline T sqr(T x) noexcept {
     return x * x;
}

/**
 * Returns cube of the argument.
 */
template <class T>
T cube(T x) noexcept {
     return x * x * x;
}

/**
 * Returns signum of the argument. \f[ \mathrm{sgn}(x) = \left\{
 * \begin{array}{rl} 1 & \mbox{if $x > 0$,} \\ 0 & \mbox{if $x = 0$,}
 * \\ -1 & \mbox{if $x < 0$.} \end{array} \right. \f]
 */
template <class T>
inline int sgn(T x) {
     return x > T(0) ? 1 : (x < T(0) ? -1 : 0);
}

/**
 * Integer version of floor.
 */
template <class I = int, class F>
I ifloor(F x) {
     static_assert(std::is_integral<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_signed<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_floating_point<F>::value,
                   "F must be a floating point type.");
     const F y = std::floor(x);
     if (y >= static_cast<F>(std::numeric_limits<I>::max()) ||
         y <= static_cast<F>(std::numeric_limits<I>::min()))
          throw std::overflow_error("overflow in ifloor");
     return static_cast<I>(y);
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
     static_assert(std::is_floating_point<F>::value,
                   "F must be a floating point type.");
     const F y = std::ceil(x);
     if (y >= static_cast<F>(std::numeric_limits<I>::max()) ||
         y <= static_cast<F>(std::numeric_limits<I>::min()))
          throw std::overflow_error("overflow in iceil");
     return static_cast<I>(y);
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
     T q; /**< quotient */
     T r; /**< remainder */
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
Integer_division<T>::Integer_division(T a, T b) : q(), r() {
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
     return d.rem < 0 ? b < 0 ? d.quot + 1 : d.quot - 1 : d.quot;
}

template <class T>
T Integer_division<T>::remainder(T a, T b) {
     if (b == 0)
          throw std::invalid_argument("Integer_division::remainder");
     const T r = a % b;
     return r < 0 ? r + std::abs(b) : r;
}

/**
 * Extended Euclidean algorithm. For two given nonnegative integer
 * numbers \f$u\f$ and \f$v\f$, the algorithm determines three numbers
 * \f$u_1, u_2, u_3\f$ such that \f$uu_1 + vu_2 = u_3 =
 * \mathrm{gcd}(u, v)\f$.
 *
 * \implementation See \cite knuth-2002b, section 4.5.2, page 366-367.
 */
template <class T>
class Extended_gcd {
     static_assert(std::is_integral<T>::value &&
                        std::is_signed<T>::value,
                   "T must be a signed integer type.");

public:
     Extended_gcd() = default;
     Extended_gcd(const T& u, const T& v);
     void calculate(const T& u, const T& v);
     T u1{}, u2{}, u3{};

private:
     T v1{}, v2{}, v3{};
     T t{};
     T q{};
};

template <class T>
Extended_gcd<T>::Extended_gcd(const T& u, const T& v) {
     calculate(u, v);
}

// clang-format off

template <class T>
void Extended_gcd<T>::calculate(const T& u, const T& v) {
     if (u < 0 || v < 0)
          throw std::invalid_argument(__func__);
     u1 = 1; u2 = 0; u3 = u;
     v1 = 0; v2 = 1; v3 = v;
     while (v3 != 0) {
          q = u3 / v3;
          t = u1 - v1 * q; u1 = v1; v1 = t;
          t = u2 - v2 * q; u2 = v2; v2 = t;
          t = u3 - v3 * q; u3 = v3; v3 = t;
     }
}

// clang-format on

/**
 * Calculates \f$x^n\f$ (right-to-left binary method for
 * exponentiation).
 * \throws std::invalid_argument if \f$n < 0\f$.
 * \implementation See \cite knuth-2002b, section 4.6.3, page 497.
 */
template <typename T>
T pow(const T& x, int n);

template <typename T>
T pow(const T& x, int n) {
     if (n < 0)
          throw std::invalid_argument(__func__);
     T y = T(1);
     T z = x;
     std::div_t dv;
     for (;;) {
          dv = std::div(n, 2);
          if (dv.rem != 0)
               y *= z;
          n = dv.quot;
          if (n == 0)
               return y;
          z *= z;
     }
}

/**
 * Returns floating-point modulo of \f$x\f$ and \f$y\f$:
 * \f[
 * \var{mod1}(x, y) = \begin{cases}
 *  x - y * \lfloor x / y \rfloor & \text{if $y \neq 0$}, \\
 *  x                    & \text{if $y = 0$}.
 * \end{cases}
 * \f]
 * \f$T\f$ must be a floating-point type.
 */
template <class T>
T mod1(T x, T y) {
     static_assert(std::is_floating_point<T>::value,
                   "floating-point type required");
     return y == T(0) ? x : x - y * std::floor(x / y);
}

/**
 * Returns a float rounded to n decimal digits.
 *
 * Examples:
 *
 * \li round(112.495, 2) = 112.5
 * \li round(-112.495, 2) = -112.5
 * \li round(112.495, 0) = 112.0
 * \li round(-112.495, 0) = -112.0
 * \li round(112.495, -2) = 100.0
 * \li round(-112.495, -2) = -100.0
 */
template <class T>
T round(T x, int n) {
     static_assert(std::is_floating_point<T>::value,
                   "T must be a floating-point type.");
     const T d = std::pow(static_cast<T>(10.0), n);
     return std::round(x * d) / d;
}

/**
 * Writes a variable to a binary stream.
 */
template <class T>
inline void write_binary(const T& a, std::ostream& f) {
     f.write(reinterpret_cast<const char*>(&a), sizeof a);
}

/**
 * Writes a string to a binary stream.
 */
template <>
inline void write_binary(const std::string& a, std::ostream& f) {
     write_binary(a.size(), f);
     f.write(a.c_str(), a.size());
}

/**
 * Reads a variable from a binary stream.
 */
template <class T>
inline void read_binary(T& a, std::istream& f) {
     f.read(reinterpret_cast<char*>(&a), sizeof a);
}

/**
 * Reads a string from a binary stream.
 */
template <>
inline void read_binary(std::string& a, std::istream& f) {
     std::string::size_type size;
     read_binary(size, f);
     char* t = new (std::nothrow) char[size];
     if (t == nullptr) {
          f.setstate(std::ios::failbit);
     } else {
          f.read(t, size);
          a.assign(t, size);
          delete[] t;
     }
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
 * Removes unneeded white space from a string.
 *
 * Removes white space from both sides of s and replaces every
 * sequence of white space by a single space. Returns the string s.
 */
char* strtrim(char* s);

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
template <typename T>
std::vector<std::basic_string<T>> split(
     std::basic_string<T> const& s, std::basic_string<T> const& sep) {
     using S = std::basic_string<T>;
     std::vector<S> v;
     typename S::size_type p1, p2 = 0;
     while ((p1 = s.find_first_not_of(sep, p2)) != S::npos) {
          p2 = s.find_first_of(sep, p1);
          if (p2 == S::npos)
               p2 = s.size();
          v.push_back(s.substr(p1, p2 - p1));
     }
     return v;
}

/**
 * Indirectly sorts a vector.
 *
 * Given a const vector<T>& w, the function returns such a vector v
 * that w[v[0]] <= w[v[1]] <= ... <= w[v[n - 1]], and v.size() ==
 * w.size() = n. The vector v is a permutation of 0, 1, ..., n - 1.
 */
template <class T>
std::vector<typename std::vector<T>::size_type> indirect_sort(
     const std::vector<T>& w) {
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
     try {
          p[0] = new T[m * n];
     } catch (const std::bad_alloc&) {
          delete[] p;
          throw;
     }
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
     void restart() { start_time = std::chrono::steady_clock::now(); }

     /**
      * Returns the difference in seconds between the current time and
      * the time remembered in Timer() or in restart().
      */
     double elapsed() const {
          typedef std::chrono::duration<double> T;
          return std::chrono::duration_cast<T>(
                      std::chrono::steady_clock::now() - start_time)
               .count();
     }

private:
     std::chrono::time_point<std::chrono::steady_clock> start_time;
};

/**
 * Generates all k-combinations of an n-set in lexicographic oreder.
 * There must be 0 < k <= n. The number of combinations is \f[{n
 * \choose k} = \frac{n!}{k!(n - k)!}.\f]
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
     const std::vector<int>& get() const { return a; }

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

/**
 * Output operator for vectors.
 */
template <typename T>
std::ostream& operator<<(std::ostream& stream,
                         const std::vector<T>& v) {
     stream.width(0);
     for (typename std::vector<T>::size_type i = 0; i < v.size(); i++)
          stream << i << ' ' << v[i] << '\n';
     return stream;
}

/**
 * Returns true if and only if the two vectors have the same content.
 * \implementation std::is_permutation is used. Another possibility is
 * to sort both vectors and then compare them directly, but this
 * method requires that T is ordered with less than relation.
 */
template <typename T>
bool have_equal_content(const std::vector<T>& v,
                        const std::vector<T>& w) {
     if (v.size() != w.size())
          return false;
     return std::is_permutation(v.cbegin(), v.cend(), w.cbegin());
}

/**
 * Returns true if and only if the vector \c v contains the element \c
 * e.
 */
template <typename T>
inline bool contains(const std::vector<T>& v, const T& e) {
     return std::find(v.cbegin(), v.cend(), e) != v.cend();
}

/**
 * Inserts \c value into \c v at position \c pos.
 * \throws std::out_of_range if pos > v.size()
 */
template <typename T>
void insert(std::vector<T>& v, typename std::vector<T>::size_type pos,
            T const& value) {
     auto const sz = v.size();
     if (pos > sz)
          throw std::out_of_range("invalid position in insert()");
     v.resize(sz + 1);
     for (auto i = sz; i > pos; i--)
          v[i] = v[i - 1];
     v[pos] = value;
}

/**
 * Removes duplicates from \c v. Requires only operator==().
 */
template <typename T>
void remove_duplicates(std::vector<T>& v) {
     auto end = v.end();
     for (auto it = v.begin(); it != end; ++it)
          end = std::remove(it + 1, end, *it);
     v.erase(end, v.end());
}

/**
 * Returns \c s with each byte greater than 127 replaced by its octal
 * code.
 */
std::string to_octal(std::string const& s);

constexpr std::size_t length(char const* s) {
     char const* t = s;
     while (*t != '\0')
          t++;
     return t - s;
}

/** \} */ /* end of group miscellaneous_utilities */

}  // namespace SHG

#endif
