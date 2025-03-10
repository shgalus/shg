/**
 * \file include/shg/ipart.h
 * Partitions of integer numbers.
 */

#ifndef SHG_IPART_H
#define SHG_IPART_H

#include <stdexcept>
#include <vector>

namespace SHG {

/**
 * \defgroup partitions_of_integers Partitions of integer numbers
 *
 * Partitions of integer numbers.
 *
 * \{
 */

/**
 * Generates consequent partition of integer number. The function is
 * rewritten from Algol procedure \cite stockmal-1962.
 *
 * Original comment: This procedure operates on a given partition of
 * the positive integer N into parts <= K, to produce a consequent
 * partition if one exists. Each partition is represented by the
 * integers c[1] thru c[K], where c[j] is the number of parts of the
 * partition equal to the integer j. If entry is made with G = false,
 * procedure ignores the input array c, sets G = true, and produces
 * the first partition of N ones. Upon each successive entry with G =
 * true, a consequent partition is stored in c[1] thru c[K]. For N =
 * KX, the final partition is c[K] = X. For N = KX + r, 1 <= r <= K -
 * 1, final partition is c[K] = X, c[r] = 1. When entry is made with
 * array c = final partition, c is left unchanged and G is reset to
 * false.
 *
 * \warning This is a rough translation from Algol, so size of the
 * array c must be at least one more then K.
 *
 * \bug Do not call this function with N = K = 1!
 */
void partgen(int c[], int N, int K, bool& G);

/**
 * Generates all partitions of positive integer. For each generated
 * partition the function f is called. This is an implementation of
 * the algorithm 4.1 AccelAsc(n) \cite kelleher-sullivan-2014,
 * page&nbsp;25. Partitions are generated in lexicographic order and
 * each partition is ordered non-decreasingly. For example, the 11
 * partitions of 6 are:
 *
 * -# 1 1 1 1 1 1
 * -# 1 1 1 1 2
 * -# 1 1 1 3
 * -# 1 1 2 2
 * -# 1 1 4
 * -# 1 2 3
 * -# 1 5
 * -# 2 2 2
 * -# 2 4
 * -# 3 3
 * -# 6
 *
 * In the function f(k, a), the parts are in a[0], a[1], ..., a[k -
 * 1].
 */
template <typename F>
void accel_asc(int n, F& f);

/**
 * Generates all partitions of positive integer. For each generated
 * partition the function f is called. This is an implementation of
 * the algorithm 3.1 RuleAsc(n) \cite kelleher-sullivan-2014,
 * page&nbsp;13. Partitions are generated in lexicographic order and
 * each partition is ordered non-decreasingly. For example, the 11
 * partitions of 6 are:
 *
 * -# 1 1 1 1 1 1
 * -# 1 1 1 1 2
 * -# 1 1 1 3
 * -# 1 1 2 2
 * -# 1 1 4
 * -# 1 2 3
 * -# 1 5
 * -# 2 2 2
 * -# 2 4
 * -# 3 3
 * -# 6
 *
 * In the function f(k, a), the parts are in a[0], a[1], ..., a[k -
 * 1].
 */
template <typename F>
void rule_asc(int n, F& f);

/**
 * Generates all ordered partitions of positive integer. For each
 * generated partition the function f is called. Partitions are
 * generated in lexicographic order. For example, the 8 partitions of
 * 4 are generated in this order:
 *
 * -# 1 1 1 1
 * -# 1 1 2
 * -# 1 2 1
 * -# 1 3
 * -# 2 1 1
 * -# 2 2
 * -# 3 1
 * -# 4
 *
 * In the function f(k, a), the parts are in a[0], a[1], ..., a[k -
 * 1].
 *
 * See \cite lipski-marek-1986, pages 63 and 86.
 */
template <typename F>
class Ordered_partitions_generator {
public:
     Ordered_partitions_generator(int n, F& f);
     void generate();

private:
     void generate(int n, int k);
     std::vector<int> a_{};
     F& f_;
};

/** \} */ /* end of group partitions_of_integers */

}  // namespace SHG

#include <shg/ipart-inl.h>

#endif
