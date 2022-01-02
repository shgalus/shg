/**
 * \file include/shg/tree.h
 * The class Tree_node.
 */

#ifndef SHG_TREE_H
#define SHG_TREE_H

#include <string>
#include <vector>
#include <iterator>
#include <cstddef>

namespace SHG {

/**
 * General tree.
 *
 * \todo Conversion of iterators.
 *
 * It is possible for std::vector:
 * \code
 * std::vector<int> v {1, 2, 3};
 * std::vector<int>::iterator it = v.begin();
 * std::vector<int>::const_iterator cit = v.cbegin();
 * BOOST_CHECK(it == cit);
 * cit = it;
 * \endcode
 * but it is impossible for Tree:
 * \code
 * Tree t = knuth1();
 * Tree::iterator it = t.begin();
 * Tree::const_iterator cit = t.cbegin();
 * BOOST_CHECK(it == cit); // error
 * cit = it; // error
 * \endcode
 *
 * \todo On the use of the template keyword in `typename
 * Tree<Data>::%template` in definitions of iterator increment
 * operators, see
 * https://en.cppreference.com/w/cpp/language/dependent_name, section
 * "The template disambiguator for dependent names".
 */
template <typename Data>
class Tree {
public:
     using Index = typename std::vector<Tree*>::size_type;

     template <typename Value_type>
     struct Iterator {
          using iterator_category = std::forward_iterator_tag;
          using difference_type = std::ptrdiff_t;
          using value_type = Value_type;
          using pointer = value_type*;
          using reference = value_type&;

          Iterator() = default;
          Iterator(pointer p) : p_(p) {}
          reference operator*() { return *p_; }
          pointer operator->() { return p_; }

          Iterator& operator++();
          Iterator operator++(int);
          friend bool operator==(Iterator const& a,
                                 Iterator const& b) {
               return a.p_ == b.p_;
          }
          friend bool operator!=(Iterator const& a,
                                 Iterator const& b) {
               return a.p_ != b.p_;
          }

     private:
          pointer p_{nullptr};
     };

     using iterator = Iterator<Tree>;
     using const_iterator = Iterator<Tree const>;

     Tree() = default;
     Tree(Data const& data);
     Tree(Tree const& other);
     Tree(Tree&& other) noexcept;
     virtual ~Tree();

     Tree& operator=(Tree const& other);
     Tree& operator=(Tree&& other) noexcept;

     /** After clear(), the tree is as default constructed. */
     void clear();

     /** Data of this node. */
     Data const& data() const;
     Data& data();
     /** The number of children of this node. */
     Index degree() const;
     /** The number of edges between this node and the root. */
     Index level() const;
     /** True if this node has no children. */
     bool is_leaf() const;
     /**
      * Line notation of this tree. See \cite knuth-2002a,
      * section 2.3, picture 20(b), page 324 and section 2.3.3,
      * formula (1), page 362.
      */
     std::string to_string() const;

     /** The i-th child of this node. */
     Tree const& operator[](Index i) const;
     Tree& operator[](Index i);

     /** Adds a leaf after the last child. */
     void push_back(Data const& data);
     /** Adds a subtree after the last child. */
     void push_back(Tree const& t);
     /** Adds a leaf before the first child. */
     void push_front(Data const& data);
     /** Adds a subtree before the first child. */
     void push_front(Tree const& t);
     /** Adds a leaf at position 0 <= pos <= degree(). */
     void insert(Index pos, Data const& data);
     /** Adds a subtree at position 0 <= pos <= degree(). */
     void insert(Index pos, Tree const& t);

     /**
      * \name Iterators.
      * Iterating nodes follows the algorithm similar to preorder
      * order in binary trees \cite knuth-2002a, section 2.3.1, page
      * 331:
      * -# visit the root
      * -# traverse each child in order from left to right
      */
     /** \{ */
     iterator begin();
     iterator end();
     const_iterator cbegin() const;
     const_iterator cend() const;
     const_iterator begin() const;
     const_iterator end() const;
     /** \} */

private:
     Data data_{};
     Tree* parent_{nullptr};
     Index pos_{};  ///< position of this node in parent's children_
     std::vector<Tree*> children_{};

     void delete_children();
     static Tree* copy(Tree* parent, Index pos, Tree const* tree);
     template <typename T>
     friend bool operator==(Tree<T> const& t1, Tree<T> const& t2);
};

template <typename Data>
bool operator==(Tree<Data> const& t1, Tree<Data> const& t2);

template <typename Data>
bool operator!=(Tree<Data> const& t1, Tree<Data> const& t2);

}  // namespace SHG

#include <shg/tree-inl.h>

#endif
