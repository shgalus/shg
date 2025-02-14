/**
 * \file include/shg/utils-inl.h
 * Implementation of inline functions and templates from utils.h.
 */

#ifndef SHG_UTILS_INL_H
#define SHG_UTILS_INL_H

namespace SHG {

template <typename... Types>
inline void ignore_unused_variable(Types...) {}

template <typename Target, typename Source>
Target narrow_cast(Source x) {
     auto y = static_cast<Target>(x);
     if (static_cast<Source>(y) != x)
          throw std::runtime_error("narrow_cast<>() failed");
     return y;
}

template <class T>
inline constexpr T sqr(T x) noexcept {
     return x * x;
}

template <class T>
inline constexpr T cube(T x) noexcept {
     return x * x * x;
}

template <class T>
inline constexpr int sgn(T x) {
     return x > T(0) ? 1 : (x < T(0) ? -1 : 0);
}

template <class I, class F>
I ifloor(F x) {
     static_assert(std::is_integral<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_signed<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_floating_point<F>::value,
                   "F must be a floating point type.");
     F const y = std::floor(x);
     if (y >= static_cast<F>(std::numeric_limits<I>::max()) ||
         y <= static_cast<F>(std::numeric_limits<I>::min()))
          throw std::overflow_error("overflow in ifloor");
     return static_cast<I>(y);
}

template <class I, class F>
I iceil(F x) {
     static_assert(std::is_integral<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_signed<I>::value,
                   "I must be a signed integer type.");
     static_assert(std::is_floating_point<F>::value,
                   "F must be a floating point type.");
     F const y = std::ceil(x);
     if (y >= static_cast<F>(std::numeric_limits<I>::max()) ||
         y <= static_cast<F>(std::numeric_limits<I>::min()))
          throw std::overflow_error("overflow in iceil");
     return static_cast<I>(y);
}

template <typename T>
void divide(T const& a, T const& b, T& q, T& r) {
     static_assert(std::numeric_limits<T>::is_integer);
     static_assert(std::numeric_limits<T>::is_signed);

     if (b == 0)
          throw std::invalid_argument("divide: division by zero");
     if constexpr (std::is_same<
                        T, boost::multiprecision::cpp_int>::value) {
          divide_qr(a, b, q, r);
     } else {
          if (a == static_cast<T>(std::numeric_limits<T>::min()) &&
              b == static_cast<T>(-1))
               throw std::out_of_range("divide: result out of range");
          q = a / b;
          r = a % b;
     }
     if ((a < 0 && r != 0) != (r < 0))
          throw std::runtime_error("divide: implementation error");
     if (r < 0) {
          if (b < 0) {
               q++;
               r -= b;
          } else {
               q--;
               r += b;
          }
     }
}

template <typename T>
bool divides(T const& a, T const& b) {
     if (a == 0)
          throw std::invalid_argument("division by zero divides");
     return b % a == 0;
}

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

template <class T>
Extended_gcd<T>::Extended_gcd(T const& u, T const& v) {
     calculate(u, v);
}

// clang-format off

template <class T>
void Extended_gcd<T>::calculate(T const& u, T const& v) {
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

template <typename T>
T ipower(T const& x, int n) {
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

template <class T>
T mod1(T x, T y) {
     static_assert(std::is_floating_point<T>::value,
                   "floating-point type required");
     return y == T(0) ? x : x - y * std::floor(x / y);
}

template <class T>
T round(T x, int n) {
     static_assert(std::is_floating_point<T>::value,
                   "T must be a floating-point type.");
     T const d = std::pow(static_cast<T>(10.0), n);
     return std::round(x * d) / d;
}

template <class T>
inline void write_binary(T const& a, std::ostream& f) {
     f.write(reinterpret_cast<char const*>(&a), sizeof a);
}

template <>
inline void write_binary(std::string const& a, std::ostream& f) {
     write_binary(a.size(), f);
     f.write(a.c_str(), a.size());
}

template <class T>
inline void read_binary(T& a, std::istream& f) {
     f.read(reinterpret_cast<char*>(&a), sizeof a);
}

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

template <typename T>
std::vector<std::basic_string<T>> split_string(
     std::basic_string<T> const& s, std::basic_string<T> const& sep) {
     using S = std::basic_string<T>;
     std::vector<S> v;
     typename S::size_type const len = sep.size();
     if (len == 0) {
          v.push_back(s);
          return v;
     }
     typename S::size_type p1 = 0, p2;
     while ((p2 = s.find(sep, p1)) != S::npos) {
          v.push_back(s.substr(p1, p2 - p1));
          p1 = p2 + len;
     }
     v.push_back(s.substr(p1));
     return v;
}

template <class T>
std::vector<typename std::vector<T>::size_type> indirect_sort(
     std::vector<T> const& w) {
     typedef typename std::vector<T>::size_type size_type;
     typedef typename std::vector<size_type>::size_type size_type1;
     size_type1 const n = w.size();
     std::vector<size_type> v(n);
     for (size_type1 i = 0; i < n; i++)
          v[i] = i;
     std::sort(v.begin(), v.end(), [&w](size_type i, size_type j) {
          return w[i] < w[j];
     });
     return v;
}

template <class T>
T** alloc_c_matrix(std::size_t m, std::size_t n) {
     T** p = new T*[m];
     try {
          p[0] = new T[m * n];
     } catch (std::bad_alloc const&) {
          delete[] p;
          throw;
     }
     for (std::size_t i = 1; i < m; i++)
          p[i] = p[i - 1] + n;
     return p;
}

template <class T>
void free_c_matrix(T** p) {
     if (p) {
          delete[] p[0];
          delete[] p;
     }
}

template <typename T>
std::ostream& operator<<(std::ostream& stream,
                         std::vector<T> const& v) {
     stream.width(0);
     for (typename std::vector<T>::size_type i = 0; i < v.size(); i++)
          stream << i << ' ' << v[i] << '\n';
     return stream;
}

template <typename T>
bool have_equal_content(std::vector<T> const& v,
                        std::vector<T> const& w) {
     if (v.size() != w.size())
          return false;
     return std::is_permutation(v.cbegin(), v.cend(), w.cbegin());
}

template <typename T>
inline bool contains(std::vector<T> const& v, T const& e) {
     return std::find(v.cbegin(), v.cend(), e) != v.cend();
}

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

template <typename T>
void remove_duplicates(std::vector<T>& v) {
     auto end = v.end();
     for (auto it = v.begin(); it != end; ++it)
          end = std::remove(it + 1, end, *it);
     v.erase(end, v.end());
}

constexpr std::size_t length(char const* s) {
     char const* t = s;
     while (*t != '\0')
          t++;
     return t - s;
}

template <typename T>
std::string to_string(T const& x) {
     std::ostringstream oss;
     oss << x;
     if (!oss)
          throw std::runtime_error("to_string() failed");
     return oss.str();
}

template <typename T>
T from_string(std::string const& s) {
     T x;
     std::istringstream iss(s);
     iss >> x;
     if (!iss)
          throw std::runtime_error("from_string() failed");
     return x;
}

}  // namespace SHG

#endif
