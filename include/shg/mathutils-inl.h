/**
 * \file include/shg/mathutils-inl.h
 * Implementation of inline functions and templates from mathutils.h.
 */

#ifndef SHG_MATHUTILS_INL_H
#define SHG_MATHUTILS_INL_H

namespace SHG {

template <typename T>
inline T fibonacci(long long n) {
     static_assert(std::numeric_limits<T>::is_integer);
     static_assert(std::numeric_limits<T>::is_signed);
     if (n < 0)
          throw std::invalid_argument(
               "n must not be negative in fibonacci");
     return boost::math::fibonacci<T>(n);
}

template <typename T>
bool is_fibonacci(T const& n) {
     static_assert(std::numeric_limits<T>::is_integer);
     static_assert(std::numeric_limits<T>::is_signed);
     if (n < 0)
          return false;
     boost::multiprecision::cpp_int k = 5 * n * n;
     boost::multiprecision::cpp_int r;
     sqrt(k + 4, r);
     if (r == 0)
          return true;
     if (k >= 4) {
          sqrt(k - 4, r);
          if (r == 0)
               return true;
     }
     return false;
}

template <typename T>
bool is_square_free(T const& n) {
     static_assert(std::numeric_limits<T>::is_integer);
     static_assert(std::numeric_limits<T>::is_signed);

     using Ifact = Integer_factorization<T>;
     using Factor = Ifact::Factor;

     if (n == 0)
          return false;
     if (n == 1 || n == -1)
          return true;
     Ifact f;
     f.factorize(abs(n));
     std::vector<Factor> const& r = f.repr();
     assert(r.size() > 0);
     auto const it =
          std::find_if(r.cbegin(), r.cend(),
                       [](Factor const& f) { return f.n > 1; });
     return it == r.cend();
}

template <typename T>
inline Lin_Dioph2<T>::Lin_Dioph2(T const& a1, T const& a2,
                                 T const& b) {
     solve(a1, a2, b);
}

template <typename T>
void Lin_Dioph2<T>::solve(T const& a1, T const& a2, T const& b) {
     if (a1 == 0 && a2 == 0)
          throw std::invalid_argument(
               "invalid argument in Lin_Dioph2::solve");
     SHG::Extended_gcd<T> const g{abs(a1), abs(a2)};
     d = g.u3;
     if (b % d != 0) {
          status = Status::no_solution;
          return;
     }
     status = Status::success;
     T const u = b / d;
     x1 = (a1 < 0 ? -g.u1 : g.u1) * u;
     x2 = (a2 < 0 ? -g.u2 : g.u2) * u;
     c1 = a2 / d;
     c2 = -a1 / d;
}

template <typename T>
bool Lin_Dioph<T>::solve(Mat const& A, Vec const& b) {
     Index const m = A.nrows();
     Index const n = A.ncols();
     if (m < 1 || n < 1 || b.size() != m)
          throw std::invalid_argument(
               "invalid argument in Lin_Dioph::solve");
     T d;
     Mat Y;
     Index k;
     if (!solveinteger(m, n, A, b, d, x0, Y, k) || d != 1) {
          x0.resize(0);
          C.resize(0, 0);
          return false;
     }
     C.resize(n, k);
     for (Index i = 0; i < n; i++)
          for (Index j = 0; j < k; j++)
               C(i, j) = Y(j, i);
     return true;
}

template <typename T>
Lin_Dioph<T>::Index Lin_Dioph<T>::intrank(Mat& mat, Index m, Index n,
                                          Index e) {
     Index i, j, k, t, itop, inext;
     T Q, nextel;
     Vec a(m);
     auto const findnext = [&]() {
          nextel = 0;
          for (k = i; k < m; k++)
               if (a(k) > nextel && k != itop) {
                    nextel = a(k);
                    inext = k;
               }
     };
     auto const swaprows = [&]() {
          for (k = j; k < t; k++) {
               Q = -mat(i, k);
               mat(i, k) = mat(itop, k);
               mat(itop, k) = Q;
          }
          a(i) = itop;
     };
     auto const reduce = [&]() {
          Q = mat(itop, j) / mat(inext, j);
          for (k = j; k < t; k++)
               mat(itop, k) -= Q * mat(inext, k);
          a(itop) = mat(itop, j) < 0 ? -mat(itop, j) : mat(itop, j);
     };

     i = j = itop = 0;
     t = n + e;
     bool first1 = true;
     bool first2 = true;
NEXTROW:
     if (itop != i)
          swaprows();
     if (first1)
          first1 = false;
     else
          i++;
     if (i >= m)
          goto OUT;
NEXTCOL:
     if (first2)
          first2 = false;
     else
          j++;
     if (j >= n)
          goto OUT;
     for (k = i; k < m; k++)
          a(k) = mat(k, j) < 0 ? -mat(k, j) : mat(k, j);
     itop = i - 1;
     findnext();
     if (nextel == 0)
          goto NEXTCOL;
CONTINUE:
     itop = inext;
     findnext();
     if (nextel == 0)
          goto NEXTROW;
     reduce();
     goto CONTINUE;
OUT:
     return i - 1;
}

template <typename T>
bool Lin_Dioph<T>::solveinteger(Index m, Index n, Mat const& A,
                                Vec const& b, T& d, Vec& x, Mat& Y,
                                Index& k) {
     Index i, j, rank;
     T s;
     Mat mat(n + 1, m + n + 1);

     for (j = 0; j < m; j++) {
          mat(0, j) = -b(j);
          for (i = 0; i < n; i++)
               mat(i + 1, j) = A(j, i);
     }
     for (j = 0; j < n + 1; j++)
          for (i = 0; i < n + 1; i++)
               mat(i, j + m) = i == j ? 1 : 0;
     rank = intrank(mat, n + 1, m + 1, n);
     d = mat(rank, m);
     if (d == 0)
          return false;
     for (i = rank; i < m; i++)
          if (mat(rank, i) != 0)
               return false;
     s = d < 0 ? -1 : 1;
     d *= s;
     k = n - rank;
     x.resize(n);
     Y.resize(k, n);
     for (i = 0; i < n; i++) {
          x(i) = mat(rank, m + i + 1) * s;
          for (j = 0; j < k; j++)
               Y(j, i) = mat(rank + j + 1, m + i + 1);
     }
     return true;
}

}  // namespace SHG

#endif
