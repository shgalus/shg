/* vector.cc: testing vector class and auxiliary functions */

/**
 * \file testing/vector.cc
 * Testing vector class and auxiliary functions.
 * \date Created on 9 November 2013.
 */

#include <sstream>
#include "shg/except.h"
#include "shg/vector.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

using std::initializer_list;
using std::ios_base;
using std::move;
using std::out_of_range;
using std::pair;
using std::stringstream;
using SHG::Vecint;
using SHG::arithmetic_progression;

void test_vector_members() {
     {
          // constructors
          const Vecint v0;
          SHG_ASSERT(v0.size() == 0);
          const Vecint v1(10);
          SHG_ASSERT(v1.size() == 10);
          const Vecint v2(10, 11);
          SHG_ASSERT(v2.size() == 10);
          for (size_t i = 0; i < v2.size(); i++)
               SHG_ASSERT(v2[i] == 11);
          const int a[] {0, 1, 2, 3};
          const Vecint v3(3, a);
          SHG_ASSERT(v3.size() == 3);
          for (size_t i = 0; i < v3.size(); i++)
               SHG_ASSERT(v3[i] == static_cast<int>(i));
          const initializer_list<int> il = {1, 3, 5, 7, 9};
          const Vecint v4(il);
          SHG_ASSERT(v4.size() == il.size());
          initializer_list<int>::const_iterator it = il.begin();
          for (size_t i = 0; i < v4.size(); i++, ++it)
               SHG_ASSERT(v4[i] == *it);
          SHG_ASSERT(it == il.end());
          const Vecint v5 {1, 3, 5, 7, 9};
          SHG_ASSERT(equal(v4, v5));
          SHG_ASSERT(v4 == v5);
          const Vecint v6 {};
          SHG_ASSERT(v6.size() == 0);
          const Vecint v7(v5), v8(v6);
          SHG_ASSERT(equal(v7, v5) && equal(v8, v6));
          SHG_ASSERT(v7 == v5 && v8 == v6);
          Vecint v9(v0), v10(v2), v11(v3);
          Vecint v12(move(v9)), v13(move(v10)), v14(move(v11));
          SHG_ASSERT(equal(v12, v0) && equal(v9, v0));
          SHG_ASSERT(v12 == v0 && v9 == v0);
          SHG_ASSERT(equal(v13, v2) && equal(v10, v0));
          SHG_ASSERT(v13 == v2 && v10 == v0);
          SHG_ASSERT(equal(v14, v3) && equal(v11, v0));
          SHG_ASSERT(v14 == v3 && v11 == v0);
     }
     {
          // assignment
          const Vecint v0, v1(10, 13);
          Vecint v2, v3;
          SHG_ASSERT(equal(v2 = v0, v0));
          SHG_ASSERT(v2 == v0);
          SHG_ASSERT(equal(v3 = v1, v1));
          SHG_ASSERT(v3 == v1);
          SHG_ASSERT(equal(v3 = v0, v0));
          SHG_ASSERT(v3 == v0);
          const Vecint v4 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
          const Vecint v5 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
          v2 = v4;
          v3 = v5;
          v2 = move(v3);
          SHG_ASSERT(equal(v2, v5) && equal(v3, v4));
          SHG_ASSERT(v2 == v5 && v3 == v4);
          v2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
          SHG_ASSERT(equal(v2, v4));
          SHG_ASSERT(v2 == v4);
          v2 = 2;
          SHG_ASSERT(v2.size() == 10);
          for (size_t i = 0; i < v2.size(); i++)
               SHG_ASSERT(v2[i] == 2);
     }
     {
          // element access
          Vecint v {1, 2, 3, 4, 5}, w(v);
          for (size_t i = 0; i < v.size(); i++)
               SHG_ASSERT(v[i] == w[i]);
          for (size_t i = 0; i < w.size(); i++)
               w(i) = v(i);
          SHG_ASSERT(equal(w, v));
          SHG_ASSERT(w == v);
          for (size_t i = 0; i < w.size(); i++)
               w[i] = v(i);
          SHG_ASSERT(equal(w, v));
          SHG_ASSERT(w == v);
          for (size_t i = 0; i < w.size(); i++)
               SHG_ASSERT(v(i) == w(i));
          try {
               v.at(v.size())++;
               SHG_ASSERT(false);
          } catch (const out_of_range&) {}
          try {
               const int i = v.at(v.size());
               SHG_ASSERT(i == v.at(v.size() - 1));
               SHG_ASSERT(false);
          } catch (const out_of_range&) {}
     }
     {
          // resize(), assign(), c_vec(), swap()
          Vecint v;
          v.resize(3);
          SHG_ASSERT(v.size() == 3);
          v.resize(0);
          SHG_ASSERT(v.size() == 0);
          v.assign(3, 2);
          SHG_ASSERT(equal(v, {2, 2, 2}));
          const Vecint cv {0, 2, 4, 6, 8, 10};
          v = cv;
          const int* const pcv = cv.c_vec();
          int* const pv = v.c_vec();
          for (size_t i = 0; i < v.size(); i++)
               SHG_ASSERT(pcv[i] == pv[i]);
          for (size_t i = 0; i < v.size(); i++)
               pv[i]++;
          for (size_t i = 0; i < v.size(); i++)
               SHG_ASSERT(pcv[i] == pv[i] - 1);
          for (size_t i = 0; i < v.size(); i++)
               pv[i]--;
          SHG_ASSERT(equal(cv, v));
          SHG_ASSERT(cv == v);

          const Vecint x0 {0, 3, 6, 9}, y0 {1, 4, 7, 10}, z0;
          Vecint x(x0), y(y0), z(z0);
          x.swap(y);
          SHG_ASSERT(equal(x, y0) && equal(y, x0));
          SHG_ASSERT(x == y0 && y == x0);
          x.swap(y);
          SHG_ASSERT(equal(x, x0) && equal(y, y0));
          x.swap(z);
          SHG_ASSERT(equal(x, z0) && equal(z, x0));
     }
     {
          // iterators
          for (int n = 0; n <= 5; n++) {
               Vecint w = SHG::arithmetic_progression(n, 0, 1);
               const Vecint v(w);
               int k = 0;
               for (Vecint::iterator i = w.begin(); i != w.end(); ++i)
                    SHG_ASSERT(++*i == ++k);
               SHG_ASSERT(k == n);
               for (Vecint::reverse_iterator i = w.rbegin();
                    i != w.rend(); ++i)
                    SHG_ASSERT(--*i == --k);
               SHG_ASSERT(k == 0);
               SHG_ASSERT(equal(v, w));
               for (Vecint::const_iterator i = w.begin();
                    i != w.end(); ++i)
                    SHG_ASSERT(*i == k++);
               SHG_ASSERT(k == n);
               for (Vecint::const_reverse_iterator i = w.rbegin();
                    i != w.rend(); ++i)
                    SHG_ASSERT(*i == --k);
               SHG_ASSERT(k == 0);
               for (Vecint::const_iterator i = w.cbegin();
                    i != w.cend(); ++i)
                    SHG_ASSERT(*i == k++);
               SHG_ASSERT(k == n);
               for (Vecint::const_reverse_iterator i = w.crbegin();
                    i != w.crend(); ++i)
                    SHG_ASSERT(*i == --k);
               SHG_ASSERT(k == 0);
          }
     }
     {
          // test constructors, assignment operators and conversion
          // operators for std::vector, std::basic_string and
          // std::valarray
          using std::equal;
          using std::begin;
          using std::end;
          {
               using std::vector;
               const vector<int> a {1, 3, 5, 7};
               const Vecint v(a);
               SHG_ASSERT(a.size() == v.size());
               SHG_ASSERT(equal(begin(a), end(a), begin(v)));
               Vecint w;
               w = a;
               SHG_ASSERT(a.size() == w.size());
               SHG_ASSERT(equal(begin(a), end(a), begin(w)));
               const vector<int> b(w);
               SHG_ASSERT(b.size() == a.size());
               SHG_ASSERT(equal(begin(b), end(b), begin(a)));
          }
          {
               using std::string;
               using SHG::Vecchar;
               const string a("aceg");
               const Vecchar v(a);
               SHG_ASSERT(a.size() == v.size());
               SHG_ASSERT(equal(begin(a), end(a), begin(v)));
               Vecchar w;
               w = a;
               SHG_ASSERT(a.size() == w.size());
               SHG_ASSERT(equal(begin(a), end(a), begin(w)));
               const string b(w);
               SHG_ASSERT(b.size() == a.size());
               SHG_ASSERT(equal(begin(b), end(b), begin(a)));
          }
          {
               using std::valarray;
               const valarray<int> a {1, 3, 5, 7};
               const Vecint v(a);
               SHG_ASSERT(a.size() == v.size());
               SHG_ASSERT(equal(begin(a), end(a), begin(v)));
               Vecint w;
               w = a;
               SHG_ASSERT(a.size() == w.size());
               SHG_ASSERT(equal(begin(a), end(a), begin(w)));
               const valarray<int> b(w);
               SHG_ASSERT(b.size() == a.size());
               SHG_ASSERT(equal(begin(b), end(b), begin(a)));
          }
     }
}

void test_vector_functions() {
     const Vecint wirth {44, 55, 12, 42, 94, 18, 6, 67}, empty;
     const ios_base::openmode mode = ios_base::out | ios_base::in |
          ios_base::binary;

     // sum, min, max, minmax, minloc, maxloc, minmaxloc
     SHG_ASSERT(sum(wirth) == 338);
     SHG_ASSERT(min(wirth) == 6);
     SHG_ASSERT(max(wirth) == 94);
     pair<int, int> mm = minmax(wirth);
     SHG_ASSERT(mm.first == 6 && mm.second == 94);
     SHG_ASSERT(minloc(wirth) == 6);
     SHG_ASSERT(maxloc(wirth) == 4);
     pair<size_t, size_t> mml = minmaxloc(wirth);
     SHG_ASSERT(mml.first == 6 && mml.second == 4);
     SHG_ASSERT(sum(empty) == 0);
     SHG_ASSERT(minloc(empty) == 0);
     SHG_ASSERT(maxloc(empty) == 0);
     mml = minmaxloc(empty);
     SHG_ASSERT(mml.first == 0 && mml.second == 0);

     // clear, swap
     Vecint v(wirth), w(empty);
     clear(v);
     SHG_ASSERT(v.size() == 0);
     clear(w);
     SHG_ASSERT(w.size() == 0);
     v = wirth;
     w = {1, 2, 3, 4};
     swap(v, w);
     SHG_ASSERT(equal(v, {1, 2, 3, 4}) && equal(w, wirth));
     clear(v);
     swap(v, w);
     SHG_ASSERT(equal(v, wirth) && equal(w, empty));

     // sorting
     sort(v = wirth);
     SHG_ASSERT(equal(v, {6, 12, 18, 42, 44, 55, 67, 94}));
     sort(v = wirth, 2, 7);
     SHG_ASSERT(equal(v, {44, 55, 6, 12, 18, 42, 94, 67}));
     reverse_sort(v = wirth);
     SHG_ASSERT(equal(v, {94, 67, 55, 44, 42, 18, 12, 6}));
     reverse_sort(v = wirth, 2, 7);
     SHG_ASSERT(equal(v, {44, 55, 94, 42, 18, 12, 6, 67}));

     // operator<<, operator>>, print, write, read
     clear(v);
     clear(w);
     {
          stringstream ss;
          ss << wirth;
          ss >> v;
          SHG_ASSERT(!ss.fail());
          SHG_ASSERT(equal(v, wirth));
     }
     clear(v);
     clear(w);
     {
          stringstream ss;
          ss << v;
          ss >> w;
          SHG_ASSERT(!ss.fail());
          SHG_ASSERT(equal(w, v));
     }
     v = wirth;
     {
          stringstream ss;
          ss << "a\n1\n2\n3\n";
          ss >> v;
          SHG_ASSERT(ss.fail() && equal(v, wirth));
     }
     {
          stringstream ss;
          ss << "3\na\n2\n3\n";
          ss >> v;
          SHG_ASSERT(ss.fail() && equal(v, wirth));
     }
     {
          stringstream ss;
          ss << "3\n1\na\n3\n";
          ss >> v;
          SHG_ASSERT(ss.fail() && equal(v, wirth));
     }
     {
          stringstream ss;
          ss << "3\n1\n2\na\n";
          ss >> v;
          SHG_ASSERT(ss.fail() && equal(v, wirth));
     }
     {
          stringstream ss;
          print(wirth, ss);
          SHG_ASSERT(ss.str() == "{44, 55, 12, 42, 94, 18, 6, 67}");
     }
     {
          stringstream ss;
          print(empty, ss);
          SHG_ASSERT(ss.str() == "{}");
     }
     v = wirth;
     clear(w);
     {
          stringstream ss(mode);
          write(v, ss);
          SHG_ASSERT(!ss.fail());
          read(w, ss);
          SHG_ASSERT(!ss.fail());
          SHG_ASSERT(equal(w, v));
     }
     {
          stringstream ss(mode);
          SHG::Vecdouble x = arithmetic_progression(100, 0.0, 0.1), y;
          write(x, ss);
          SHG_ASSERT(!ss.fail());
          read(y, ss);
          SHG_ASSERT(!ss.fail());
          SHG_ASSERT(equal(y, x));
     }
     {
          stringstream ss(mode);
          SHG::Vecdouble x, y;
          write(x, ss);
          SHG_ASSERT(!ss.fail());
          read(y, ss);
          SHG_ASSERT(!ss.fail());
          SHG_ASSERT(equal(y, x));
     }

     // maximum_norm_distance
     v = w = wirth;
     SHG_ASSERT(maximum_norm_distance(v, w) == 0);
     w(0)++;
     SHG_ASSERT(maximum_norm_distance(v, w) == 1);

     // arithmetic_progression
     v = arithmetic_progression(7, 2, 3);
     SHG_ASSERT(v.size() == 7);
     for (size_t i = 0; i < v.size(); i++)
          SHG_ASSERT(v[i] == static_cast<int>(2 + 3 * i));
     v = arithmetic_progression(0, 1, 2);
     SHG_ASSERT(v.size() == 0);
}

}       // anonymous namespace

void test_vector() {
     test_vector_members();
     test_vector_functions();
}

}       // namespace Testing
}       // namespace SHG
