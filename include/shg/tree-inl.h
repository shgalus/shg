/**
 * \file include/shg/tree-inl.h
 * Implementation of inline functions and templates from tree.h.
 */

#ifndef SHG_TREE_INL_H
#define SHG_TREE_INL_H

#include <stdexcept>

namespace SHG {

template <typename Data>
template <typename Value_type>
typename Tree<Data>::template Iterator<Value_type>&
Tree<Data>::Iterator<Value_type>::operator++() {
     if (p_->children_.size() > 0) {
          p_ = p_->children_[0];
          return *this;
     }
     for (;;) {
          if (p_->parent_ == nullptr) {
               p_ = nullptr;
               return *this;
          }
          if (p_->pos_ + 1 < p_->parent_->children_.size()) {
               p_ = p_->parent_->children_[p_->pos_ + 1];
               return *this;
          }
          p_ = p_->parent_;
     }
}

template <typename Data>
template <typename Value_type>
typename Tree<Data>::template Iterator<Value_type>
Tree<Data>::Iterator<Value_type>::operator++(int) {
     Iterator tmp = *this;
     ++*this;
     return tmp;
}

template <typename Data>
Tree<Data>::Tree(Data const& data) : data_(data) {}

template <typename Data>
Tree<Data>::Tree(Tree const& other) {
     delete_children();
     data_ = other.data_;
     parent_ = other.parent_;
     pos_ = other.pos_;
     for (Index i = 0; i < other.children_.size(); i++)
          children_.push_back(copy(this, i, other.children_[i]));
}

template <typename Data>
Tree<Data>::Tree(Tree&& other) noexcept
     : data_(other.data_),
       parent_(other.parent_),
       pos_(other.pos_),
       children_(other.children_) {
     other.parent_ = nullptr;
     other.pos_ = 0;
     other.children_.clear();
}

/**
 * \relates SHG::Tree::~Tree
 *
 * \todo "\relates SHG::Tree::~Tree" is only to prevent the following
 * Doxygen warning: no matching class member found for template
 * < Data > SHG::Tree::Tree::~Tree()
 */
template <typename Data>
Tree<Data>::Tree::~Tree() {
     delete_children();
}

template <typename Data>
Tree<Data>& Tree<Data>::operator=(Tree const& other) {
     if (this != &other) {
          delete_children();
          data_ = other.data_;
          parent_ = other.parent_;
          pos_ = other.pos_;
          for (Index i = 0; i < other.children_.size(); i++)
               children_.push_back(copy(this, i, other.children_[i]));
     }
     return *this;
}

template <typename Data>
Tree<Data>& Tree<Data>::operator=(Tree&& other) noexcept {
     if (this != &other) {
          std::swap(data_, other.data_);
          std::swap(parent_, other.parent_);
          std::swap(pos_, other.pos_);
          std::swap(children_, other.children_);
     }
     return *this;
}

template <typename Data>
void Tree<Data>::clear() {
     delete_children();
     children_.clear();
     data_ = Data();
     parent_ = nullptr;
     pos_ = 0;
}

template <typename Data>
inline Data const& Tree<Data>::data() const {
     return data_;
}

template <typename Data>
inline Data& Tree<Data>::data() {
     return data_;
}

template <typename Data>
inline typename Tree<Data>::Index Tree<Data>::degree() const {
     return children_.size();
}

template <typename Data>
typename Tree<Data>::Index Tree<Data>::level() const {
     Index lvl = 0;
     for (Tree const* p = this; p->parent_ != nullptr; p = p->parent_)
          lvl++;
     return lvl;
}

template <typename Data>
inline bool Tree<Data>::is_leaf() const {
     return children_.size() == 0;
}

template <typename Data>
std::string Tree<Data>::to_string() const {
     std::string s{data_.to_string()};
     if (children_.size() > 0) {
          s += "(";
          for (Index i = 0; i < children_.size(); i++) {
               s += children_[i]->to_string();
               if (i + 1 < children_.size())
                    s += ", ";
          }
          s += ")";
     }
     return s;
}

template <typename Data>
inline Tree<Data> const& Tree<Data>::operator[](Index i) const {
     return *children_.at(i);
}

template <typename Data>
inline Tree<Data>& Tree<Data>::operator[](Index i) {
     return *children_.at(i);
}

template <typename Data>
inline void Tree<Data>::push_back(Data const& data) {
     push_back(Tree(data));
}

template <typename Data>
void Tree<Data>::push_back(Tree const& t) {
     Tree* u = new Tree(t);
     u->parent_ = this;
     u->pos_ = children_.size();
     children_.push_back(u);
}

template <typename Data>
inline void Tree<Data>::push_front(Data const& data) {
     push_front(Tree(data));
}

template <typename Data>
inline void Tree<Data>::push_front(Tree const& t) {
     insert(0, t);
}

template <typename Data>
inline void Tree<Data>::insert(Index pos, Data const& data) {
     insert(pos, Tree(data));
}

template <typename Data>
void Tree<Data>::insert(Index pos, Tree const& t) {
     auto const sz = children_.size();
     if (pos > sz)
          throw std::out_of_range("invalid position in insert()");
     children_.resize(sz + 1);
     for (auto i = sz; i > pos; i--) {
          children_[i - 1]->pos_++;
          children_[i] = children_[i - 1];
     }
     Tree* u = new Tree(t);
     u->parent_ = this;
     u->pos_ = pos;
     children_[pos] = u;
}

template <typename Data>
inline typename Tree<Data>::iterator Tree<Data>::begin() {
     return iterator(this);
}

template <typename Data>
inline typename Tree<Data>::iterator Tree<Data>::end() {
     return iterator();
}

template <typename Data>
inline typename Tree<Data>::const_iterator Tree<Data>::cbegin()
     const {
     return const_iterator(this);
}

template <typename Data>
inline typename Tree<Data>::const_iterator Tree<Data>::cend() const {
     return const_iterator();
}

template <typename Data>
inline typename Tree<Data>::const_iterator Tree<Data>::begin() const {
     return const_iterator(this);
}

template <typename Data>
inline typename Tree<Data>::const_iterator Tree<Data>::end() const {
     return const_iterator();
}

template <typename Data>
void Tree<Data>::delete_children() {
     for (Index i = 0; i < children_.size(); i++)
          delete children_[i];
}

template <typename Data>
Tree<Data>* Tree<Data>::copy(Tree* parent, Index pos,
                             Tree const* tree) {
     Tree* t = new Tree;
     t->data_ = tree->data_;
     t->parent_ = parent;
     t->pos_ = pos;
     for (Index i = 0; i < tree->children_.size(); i++)
          t->children_.push_back(copy(t, i, tree->children_[i]));
     return t;
}

template <typename Data>
bool operator==(Tree<Data> const& t1, Tree<Data> const& t2) {
     if (t1.data_ != t2.data_)
          return false;
     if (t1.children_.size() != t2.children_.size())
          return false;
     for (typename Tree<Data>::Index i = 0; i < t1.children_.size();
          i++)
          if (!(*t1.children_[i] == *t2.children_[i]))
               return false;
     return true;
}

template <typename Data>
inline bool operator!=(Tree<Data> const& t1, Tree<Data> const& t2) {
     return !(t1 == t2);
}

}  // namespace SHG

#endif
