#include <cassert>
#include <iostream>
#include <utility>  // std::move, std::swap
#include <shg/tree.h>

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

/// \cite knuth-2002a, pages 362, 367.
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

void destructor_test() {
     Ttree t = knuth1();
}

void copy_constructor_test() {
     Ttree t = knuth1();
     Ttree t1(t);
     assert(t1 == t);
}

void move_constructor_test() {
     Ttree t = knuth1();
     Ttree const u = knuth1();
     assert(u == t);
     Ttree const v(std::move(t));
     assert(v == u);
     assert(!(v != u));
     assert(t.data().txt == "");
     assert(t.degree() == 0);
}

void assignment_test() {
     Ttree const t = knuth1();
     Ttree u;
     u = t;
     assert(u == t);
}

void move_assignment_test() {
     Ttree t;
     t = knuth1();
     Ttree const u = knuth1();
     Ttree v;
     v = std::move(t);
     assert(v == u);
     assert(t.degree() == 0);
     assert(t.data().txt == "");
}

void move_test() {
     Ttree t0 = knuth1();
     Ttree u0{{"ROOT", 0}};
     u0.push_back({"1", 1});
     Ttree t(t0);
     Ttree u(u0);
     assert(t == t0 && u == u0);
     t = std::move(u);
     assert(t == u0 && u == t0);
}

void swap_test() {
     Ttree t0 = knuth1();
     Ttree u0{{"ROOT", 0}};
     u0.push_back({"1", 1});
     Ttree t(t0);
     Ttree u(u0);
     assert(t == t0 && u == u0);
     std::swap(t, u);
     assert(t == u0 && u == t0);
}

void push_back_test() {
     char const* const r =
          "(A(B((A(B, C(K)), D(E(H), F(J), G))), C(K)), D(E(H), "
          "F(J), G))";
     Ttree t = knuth1();
     t[0][0].push_back(t);
     assert(t.to_string() == r);
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

void tree_insert_test() {
     Ttree t1, t2;

     simple_trees(t1, t2);
     t1.insert(0, t2);
     assert(t1.to_string() == "A(E(F, G), B(C), D)");
     simple_trees(t1, t2);
     t1.insert(1, t2);
     assert(t1.to_string() == "A(B(C), E(F, G), D)");
     simple_trees(t1, t2);
     t1.insert(2, t2);
     assert(t1.to_string() == "A(B(C), D, E(F, G))");
     simple_trees(t1, t2);
     try {
          t1.insert(3, t2);
          assert(0);
     } catch (std::out_of_range const&) {
     }

     simple_trees(t1, t2);
     t1[0].insert(0, t2);
     assert(t1.to_string() == "A(B(E(F, G), C), D)");
     simple_trees(t1, t2);
     t1[0].insert(1, t2);
     assert(t1.to_string() == "A(B(C, E(F, G)), D)");
     simple_trees(t1, t2);
     try {
          t1[0].insert(2, t2);
          assert(0);
     } catch (std::out_of_range const&) {
     }

     simple_trees(t1, t2);
     t1[1].insert(0, t2);
     assert(t1.to_string() == "A(B(C), D(E(F, G)))");
     simple_trees(t1, t2);
     try {
          t1[1].insert(1, t2);
          assert(0);
     } catch (std::out_of_range const&) {
     }
}

void clear_test() {
     Ttree t1, t2, t3;
     simple_trees(t1, t2);
     t1.clear();
     t2.clear();
     assert(t1 == t3);
     assert(t2 == t3);
}

void push_front_test() {
     Ttree t1, t2;
     simple_trees(t1, t2);
     t1.push_front(t2);
     assert(t1.to_string() == "A(E(F, G), B(C), D)");
     simple_trees(t1, t2);
     Data data;
     data.txt = 'H';
     t1.push_front(data);
     assert(t1.to_string() == "A(H, B(C), D)");
}

void iterator_test() {
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
     assert(s == r0);

     s.clear();
     for (auto it = t1.begin(); it != t1.end(); it++)
          s += '\n' + it->to_string();
     assert(s == r0);

     s.clear();
     for (auto const& n : t1)
          s += '\n' + n.to_string();
     assert(s == r0);

     Data data;
     data.txt = 'A';
     Ttree t3(data);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); ++it)
          s += '\n' + it->to_string();
     assert(s == r1);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); it++)
          s += '\n' + it->to_string();
     assert(s == r1);

     s.clear();
     for (auto const& n : t3)
          s += '\n' + n.to_string();
     assert(s == r1);

     Ttree::iterator it1, it2;
     it1 = t3.begin();
     it2 = t3.begin();
     for (;;) {
          assert(it1 == it2);
          if (it1 == t3.end())
               break;
          it1++;
          it2++;
     }
}

void is_leaf_test() {
     Ttree t1, t2;
     simple_trees(t1, t2);
     std::string s;
     for (auto const& n : t1)
          if (n.is_leaf())
               s += n.to_string();
     assert(s == "CD");
}

Ttree simple_tree() {
     Ttree t1, t2;
     simple_trees(t1, t2);
     return t1;
}

void const_iterator_test() {
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
     assert(s == r0);

     s.clear();
     for (auto it = t1.cbegin(); it != t1.cend(); it++)
          s += '\n' + it->to_string();
     assert(s == r0);

     s.clear();
     for (auto const& n : t1)
          s += '\n' + n.to_string();
     assert(s == r0);

     Data data;
     data.txt = 'A';
     Ttree t3(data);

     s.clear();
     for (auto it = t3.cbegin(); it != t3.cend(); ++it)
          s += '\n' + it->to_string();
     assert(s == r1);

     s.clear();
     for (auto it = t3.cbegin(); it != t3.cend(); it++)
          s += '\n' + it->to_string();
     assert(s == r1);

     s.clear();
     for (auto const& n : t3)
          s += '\n' + n.to_string();
     assert(s == r1);

     Ttree::const_iterator it1, it2;
     it1 = t3.cbegin();
     it2 = t3.cbegin();
     for (;;) {
          assert(it1 == it2);
          if (it1 == t3.cend())
               break;
          it1++;
          it2++;
     }
}

// with begin() and end() instead of cbegin() and cend().
void const_iterator_test2() {
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
     assert(s == r0);

     s.clear();
     for (auto it = t1.begin(); it != t1.end(); it++)
          s += '\n' + it->to_string();
     assert(s == r0);

     s.clear();
     for (auto const& n : t1)
          s += '\n' + n.to_string();
     assert(s == r0);

     Data data;
     data.txt = 'A';
     Ttree t3(data);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); ++it)
          s += '\n' + it->to_string();
     assert(s == r1);

     s.clear();
     for (auto it = t3.begin(); it != t3.end(); it++)
          s += '\n' + it->to_string();
     assert(s == r1);

     s.clear();
     for (auto const& n : t3)
          s += '\n' + n.to_string();
     assert(s == r1);

     Ttree::const_iterator it1, it2;
     it1 = t3.cbegin();
     it2 = t3.cbegin();
     for (;;) {
          assert(it1 == it2);
          if (it1 == t3.cend())
               break;
          it1++;
          it2++;
     }
}

void level_test() {
     Ttree t;
     assert(t.level() == 0);
     t = knuth1();
     assert(t.level() == 0);
     assert(t[0].level() == 1);
     assert(t[1].level() == 1);
     assert(t[0][0].level() == 2);
     assert(t[0][1].level() == 2);
     assert(t[1][0].level() == 2);
     assert(t[1][1].level() == 2);
     assert(t[1][2].level() == 2);
     assert(t[0][1][0].level() == 3);
     assert(t[1][0][0].level() == 3);
     assert(t[1][1][0].level() == 3);
}

int main() {
     destructor_test();
     copy_constructor_test();
     move_constructor_test();
     assignment_test();
     move_assignment_test();
     move_test();
     swap_test();
     push_back_test();
     tree_insert_test();
     clear_test();
     push_front_test();
     iterator_test();
     is_leaf_test();
     const_iterator_test();
     const_iterator_test2();
     level_test();
}
