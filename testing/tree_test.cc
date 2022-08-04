#include <shg/tree.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(tree_test)

struct Data {
     std::string txt{};
     int dummy{};
     std::string to_string() const { return txt; }
};

bool operator==(Data const& data1, Data const& data2) {
     return data1.txt == data2.txt;
}
bool operator!=(Data const& data1, Data const& data2) {
     return data1.txt != data2.txt;
}

using Ttree = SHG::Tree<Data>;

/**
 * A tree from \cite knuth-2002a, pages 362, 367. Note missing label
 * of the root.
 * \verbatim
                 +---+
                 |   |
                 +---+
                /     \
               /       \
              /         \
             /           \
            /             \
       +---+               +---+
       | A |               | D |
       +---+               +---+
       /   \              /  |  \
      /     \            /   |   \
     /       \          /    |    \
   +---+   +---+   +---+   +---+   +---+
   | B |   | C |   | E |   | F |   | G |
   +---+   +---+   +---+   +---+   +---+
             |       |       |
             |       |       |
             |       |       |
           +---+   +---+   +---+
           | K |   | H |   | J |
           +---+   +---+   +---+
   \endverbatim
 */
Ttree knuth1() {
     Data d;
     Ttree t;
     d.txt = 'A';
     t.push_back(d);
     d.txt = 'D';
     t.push_back(d);
     d.txt = 'B';
     t[0].push_back(d);
     d.txt = 'C';
     t[0].push_back(d);
     d.txt = 'K';
     t[0][1].push_back(d);
     d.txt = 'E';
     t[1].push_back(d);
     d.txt = 'F';
     t[1].push_back(d);
     d.txt = 'G';
     t[1].push_back(d);
     d.txt = 'H';
     t[1][0].push_back(d);
     d.txt = 'J';
     t[1][1].push_back(d);
     return t;
}

BOOST_AUTO_TEST_CASE(default_constructor_test) {
     Ttree const t;
     BOOST_CHECK(t.degree() == 0);
     BOOST_CHECK(t.is_leaf());
     BOOST_CHECK(t.to_string() == "");
}

BOOST_AUTO_TEST_CASE(destructor_test) {
     Ttree t = knuth1();
     BOOST_CHECK(t.to_string() == "(A(B, C(K)), D(E(H), F(J), G))");
}

BOOST_AUTO_TEST_CASE(copy_constructor_test) {
     Ttree t = knuth1();
     Ttree t1(t);
     BOOST_CHECK(t1 == t);
}

BOOST_AUTO_TEST_CASE(move_constructor_test) {
     Ttree t = knuth1();
     Ttree const u = knuth1();
     BOOST_CHECK(u == t);
     Ttree const v(std::move(t));
     BOOST_CHECK(v == u);
     BOOST_CHECK(!(v != u));
     BOOST_CHECK(t.data().txt == "");
     BOOST_CHECK(t.degree() == 0);
}

BOOST_AUTO_TEST_CASE(assignment_test) {
     Ttree const t = knuth1();
     Ttree u;
     u = t;
     BOOST_CHECK(u == t);
}

BOOST_AUTO_TEST_CASE(move_assignment_test) {
     Ttree t;
     t = knuth1();
     Ttree const u = knuth1();
     Ttree v;
     v = std::move(t);
     BOOST_CHECK(v == u);
     BOOST_CHECK(t.degree() == 0);
     BOOST_CHECK(t.data().txt == "");
}

BOOST_AUTO_TEST_CASE(move_test) {
     Ttree t0 = knuth1();
     Ttree u0{{"ROOT", 0}};
     u0.push_back({"1", 1});
     Ttree t(t0);
     Ttree u(u0);
     BOOST_CHECK(t == t0 && u == u0);
     t = std::move(u);
     BOOST_CHECK(t == u0 && u == t0);
}

BOOST_AUTO_TEST_CASE(swap_test) {
     Ttree t0 = knuth1();
     Ttree u0{{"ROOT", 0}};
     u0.push_back({"1", 1});
     Ttree t(t0);
     Ttree u(u0);
     BOOST_CHECK(t == t0 && u == u0);
     std::swap(t, u);
     BOOST_CHECK(t == u0 && u == t0);
}

BOOST_AUTO_TEST_CASE(push_back_test) {
     char const* const r =
          "(A(B((A(B, C(K)), D(E(H), F(J), G))), C(K)), D(E(H), "
          "F(J), G))";
     Ttree t = knuth1();
     t[0][0].push_back(t);
     BOOST_CHECK(t.to_string() == r);
}

/*
 * Output of simple_trees.
 *
 *     +---+             +---+
 *     | A |             | E |
 *     +---+             +---+
 *     /   \             /  \
 *    /     \           /    \
 *   /       \         /      \
 * +---+   +---+     +---+   +---+
 * | B |   | D |     | F |   | G |
 * +---+   +---+     +---+   +---+
 *   |
 *   |
 *   |
 * +---+
 * | C |
 * +---+
 * t1: A(B(C), D)    t2: E(F, G)
 */
void simple_trees(Ttree& t1, Ttree& t2) {
     Data data;
     data.txt = 'A';
     t1 = Ttree(data);
     data.txt = 'B';
     t1.push_back(data);
     data.txt = 'C';
     t1[0].push_back(data);
     data.txt = 'D';
     t1.push_back(data);
     data.txt = 'E';
     t2 = Ttree(data);
     data.txt = 'F';
     t2.push_back(data);
     data.txt = 'G';
     t2.push_back(data);
}

BOOST_AUTO_TEST_CASE(tree_insert_test) {
     Ttree t1, t2;

     simple_trees(t1, t2);
     t1.insert(0, t2);
     BOOST_CHECK(t1.to_string() == "A(E(F, G), B(C), D)");
     simple_trees(t1, t2);
     t1.insert(1, t2);
     BOOST_CHECK(t1.to_string() == "A(B(C), E(F, G), D)");
     simple_trees(t1, t2);
     t1.insert(2, t2);
     BOOST_CHECK(t1.to_string() == "A(B(C), D, E(F, G))");
     simple_trees(t1, t2);
     BOOST_CHECK_THROW(t1.insert(3, t2), std::out_of_range);

     simple_trees(t1, t2);
     t1[0].insert(0, t2);
     BOOST_CHECK(t1.to_string() == "A(B(E(F, G), C), D)");
     simple_trees(t1, t2);
     t1[0].insert(1, t2);
     BOOST_CHECK(t1.to_string() == "A(B(C, E(F, G)), D)");
     simple_trees(t1, t2);
     BOOST_CHECK_THROW(t1[0].insert(2, t2), std::out_of_range);

     simple_trees(t1, t2);
     t1[1].insert(0, t2);
     BOOST_CHECK(t1.to_string() == "A(B(C), D(E(F, G)))");
     simple_trees(t1, t2);
     BOOST_CHECK_THROW(t1[1].insert(1, t2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(clear_test) {
     Ttree t1, t2, t3;
     simple_trees(t1, t2);
     t1.clear();
     t2.clear();
     BOOST_CHECK(t1 == t3);
     BOOST_CHECK(t2 == t3);
}

BOOST_AUTO_TEST_CASE(push_front_test) {
     Ttree t1, t2;
     simple_trees(t1, t2);
     t1.push_front(t2);
     BOOST_CHECK(t1.to_string() == "A(E(F, G), B(C), D)");
     simple_trees(t1, t2);
     Data data;
     data.txt = 'H';
     t1.push_front(data);
     BOOST_CHECK(t1.to_string() == "A(H, B(C), D)");
}

BOOST_AUTO_TEST_CASE(iterator_test) {
     Ttree t1, t2;
     simple_trees(t1, t2);
     std::string s;
     char const* const r0 = R"(
A(B(C), D)
B(C)
C
D)";
     char const* const r1 = "\nA";

     for (auto it = t1.begin(); it != t1.end(); ++it)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r0);

     s.clear();
     for (auto it = t1.begin(); it != t1.end(); it++)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r0);

     s.clear();
     for (auto const& n : t1)
          s += '\n' + n.to_string();
     BOOST_CHECK(s == r0);

     Data data;
     data.txt = 'A';
     Ttree t3(data);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); ++it)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r1);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); it++)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r1);

     s.clear();
     for (auto const& n : t3)
          s += '\n' + n.to_string();
     BOOST_CHECK(s == r1);

     Ttree::iterator it1, it2;
     it1 = t3.begin();
     it2 = t3.begin();
     for (;;) {
          BOOST_CHECK(it1 == it2);
          if (it1 == t3.end())
               break;
          it1++;
          it2++;
     }
}

BOOST_AUTO_TEST_CASE(is_leaf_test) {
     Ttree t1, t2;
     simple_trees(t1, t2);
     std::string s;
     for (auto const& n : t1)
          if (n.is_leaf())
               s += n.to_string();
     BOOST_CHECK(s == "CD");
}

Ttree simple_tree() {
     Ttree t1, t2;
     simple_trees(t1, t2);
     return t1;
}

BOOST_AUTO_TEST_CASE(const_iterator_test) {
     Ttree const t1 = simple_tree();
     std::string s;
     char const* const r0 = R"(
A(B(C), D)
B(C)
C
D)";
     char const* const r1 = "\nA";

     for (auto it = t1.cbegin(); it != t1.cend(); ++it)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r0);

     s.clear();
     for (auto it = t1.cbegin(); it != t1.cend(); it++)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r0);

     s.clear();
     for (auto const& n : t1)
          s += '\n' + n.to_string();
     BOOST_CHECK(s == r0);

     Data data;
     data.txt = 'A';
     Ttree t3(data);

     s.clear();
     for (auto it = t3.cbegin(); it != t3.cend(); ++it)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r1);

     s.clear();
     for (auto it = t3.cbegin(); it != t3.cend(); it++)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r1);

     s.clear();
     for (auto const& n : t3)
          s += '\n' + n.to_string();
     BOOST_CHECK(s == r1);

     Ttree::const_iterator it1, it2;
     it1 = t3.cbegin();
     it2 = t3.cbegin();
     for (;;) {
          BOOST_CHECK(it1 == it2);
          if (it1 == t3.cend())
               break;
          it1++;
          it2++;
     }
}

/**
 *
 * The same as const_iterator_test but with begin() and end() instead
 * of cbegin() and cend().
 */
BOOST_AUTO_TEST_CASE(const_iterator_test2) {
     Ttree const t1 = simple_tree();
     std::string s;
     char const* const r0 = R"(
A(B(C), D)
B(C)
C
D)";
     char const* const r1 = "\nA";

     for (auto it = t1.begin(); it != t1.end(); ++it)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r0);

     s.clear();
     for (auto it = t1.begin(); it != t1.end(); it++)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r0);

     s.clear();
     for (auto const& n : t1)
          s += '\n' + n.to_string();
     BOOST_CHECK(s == r0);

     Data data;
     data.txt = 'A';
     Ttree t3(data);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); ++it)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r1);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); it++)
          s += '\n' + it->to_string();
     BOOST_CHECK(s == r1);

     s.clear();
     for (auto const& n : t3)
          s += '\n' + n.to_string();
     BOOST_CHECK(s == r1);

     Ttree::const_iterator it1, it2;
     it1 = t3.cbegin();
     it2 = t3.cbegin();
     for (;;) {
          BOOST_CHECK(it1 == it2);
          if (it1 == t3.cend())
               break;
          it1++;
          it2++;
     }
}

BOOST_AUTO_TEST_CASE(level_test) {
     Ttree t;
     BOOST_CHECK(t.level() == 0);
     t = knuth1();
     BOOST_CHECK(t.level() == 0);
     BOOST_CHECK(t[0].level() == 1);
     BOOST_CHECK(t[1].level() == 1);
     BOOST_CHECK(t[0][0].level() == 2);
     BOOST_CHECK(t[0][1].level() == 2);
     BOOST_CHECK(t[1][0].level() == 2);
     BOOST_CHECK(t[1][1].level() == 2);
     BOOST_CHECK(t[1][2].level() == 2);
     BOOST_CHECK(t[0][1][0].level() == 3);
     BOOST_CHECK(t[1][0][0].level() == 3);
     BOOST_CHECK(t[1][1][0].level() == 3);
}

BOOST_AUTO_TEST_CASE(parent_test) {
     Ttree t;
     t = knuth1();
     BOOST_CHECK(t.parent() == nullptr);
     BOOST_CHECK(*t[0].parent() == t);
     BOOST_CHECK(*t[1].parent() == t);
     BOOST_CHECK(*t[0][0].parent() == t[0]);
     BOOST_CHECK(*t[0][1].parent() == t[0]);
     BOOST_CHECK(*t[0][1][0].parent() == t[0][1]);
     BOOST_CHECK(*t[1][0].parent() == t[1]);
     BOOST_CHECK(*t[1][1].parent() == t[1]);
     BOOST_CHECK(*t[1][2].parent() == t[1]);
     BOOST_CHECK(*t[1][0][0].parent() == t[1][0]);
     BOOST_CHECK(*t[1][1][0].parent() == t[1][1]);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
