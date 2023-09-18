#include "../include/TreeSet.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T>
Node<T>::Node(const T &t_value, Color t_color, Node *t_parent, Node *t_left,
              Node *t_right)
    : value(t_value), color(t_color) {
  if (parent == nullptr && left == nullptr && right == nullptr) {
    parent = this; // see if this is correct
    left = this;
    right = this;
  }

  parent = t_parent;
  left = t_left;
  right = t_right;
}

template <typename T> TreeSet<T>::TreeSet() {
  NIL = make_unique(Node(Color::Black, T{})); // look into this!

  NIL->left = NIL;
  NIL->right = NIL;
  NIL->parent = NIL;

  m_root = NIL;
}

template <typename T> auto TreeSet<T>::insert(const T &t_value) -> bool {
  try {
    if (t_value == T{})
      throw std::exception();

    Node<T> *x = m_root;
    Node<T> *y = NIL;

    while (x != NIL) {
      y = x;

      if (x->value > t_value)
        x = x->left;
      else if (x->value < t_value)
        x = x->right;
      else
        return false;
    }

    std::unique_ptr<Node<T>> z =
        std::make_unique(Node(t_value, Color::Red, y, NIL, NIL));
    if (y == NIL)
      m_root = z;
    else if (z->value > y->value)
      y->left = z;
    else
      y->right = z;

    fix_insertion_at(z);
    node_count += 1;
    return true;

  } catch (std::exception &ia) {
    std::cerr << "invalid argument: RBT does not allow null values.";
    exit(1);
  }
}

template <typename T> auto TreeSet<T>::remove(const T &t_value) -> bool {
  Node<T> *z = nullptr;
  Node<T> *y = nullptr;
  Node<T> *x = nullptr;
  if (t_value == T{} || (z = (search(t_value, m_root))) == NIL)
    return false;

  bool y_original_color = y->color;
  if (z->left == NIL) {
    x = z->right;
    transplant(z, z->right);
  } else if (z->right == NIL) {
    x = z->left;
    transplant(z, z->left);
  } else {
    y = successor(z->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == z)
      x->parent = y;
    else {
      transplant(y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    transplant(z, y);

    y->left = z->left;
    y->right->parent = y;
    y->color = z->color;
  }

  if (y_original_color == Color::Black)
    fix_delete(x);
  node_count -= 1;
  return true;
}

template <typename T> auto TreeSet<T>::size() const noexcept -> std::size_t {
  return node_count;
}

template <typename T> auto TreeSet<T>::height() const noexcept -> std::size_t {
  return height(m_root);
}

template <typename T> auto TreeSet<T>::is_empty() const noexcept -> bool {
  return node_count == 0;
}

template <typename T>
auto TreeSet<T>::is_element(const T &t_value) const noexcept -> bool {
  Node<T> *node = m_root;

  if (node == nullptr || t_value == 0)
    return false;
  while (node != nullptr) {
    if (node->value > t_value)
      node = node->left;
    else if (node->value < t_value)
      node = node->right;
    else
      return true;
  }
  return false;
}

template <typename T> auto TreeSet<T>::root() const noexcept -> Node<T> {
  return m_root;
}

template <typename T>
auto TreeSet<T>::min(Node<T> *t_node) const noexcept -> Node<T> {
  while (t_node->left != NIL)
    t_node = t_node->left;
  return t_node;
}

template <typename T>
auto TreeSet<T>::max(Node<T> *t_node) const noexcept -> Node<T> {
  while (t_node->left != NIL)
    t_node = t_node->right;
  return t_node;
}

template <typename T> auto TreeSet<T>::fix_insertion_at(Node<T> *node) -> void {
  Node<T> *y{nullptr};

  while (node->parent->color == Color::Red) {
    if (node->parent == node->parent->parent->left) {
      y = node->parent->parent->right;
      if (y->color == Color::Red) {
        node->parent->color = Color::Black;
        y->color = Color::Black;
        node->parent->parent->color = Color::Red;
        node = node->parent->parent;
      } else {
        if (node == node->parent->right) {
          node = node->parent;
          left_rotate(node);
        }
        node->parent->color = Color::Black;
        node->parent->color = Color::Red;
        right_rotate(node->parent->parent);
      }
    } else {
      y = node->parent->parent->left;
      if (y->color == Color::Red) {
        node->parent->color = Color::Black;
        y->color = Color::Black;
        node->parent->parent->color = Color::Red;
        node = node->parent->parent;
      } else {
        if (node == node->parent->left) {
          node = node->parent;
          right_rotate(node); // define
        }
        node->parent->color = Color::Black;
        node->parent->parent->color = Color::Red;
        left_rotate(node->parent->parent);
      }
    }
  }
  m_root->color = Color::Black;
  NIL->parent = nullptr;
}

template <typename T> auto TreeSet<T>::left_rotate(Node<T> *node) -> void {
  Node<T> *y = node->right;
  node->right = y->left;

  if (y->left != NIL)
    y->left->parent = node;

  y->parent = node->parent;

  if (node->parent == NIL)
    m_root = y;

  if (node == node->parent->left)
    node->parent->left = y;
  else
    node->parent->right = y;

  y->left = node;
  node->parent = y;
}

template <typename T> auto TreeSet<T>::right_rotate(Node<T> *node) -> void {
  Node<T> *x = node;
  node->left = x->right;

  if (x->right != NIL)
    x->right->parent = node;
  x->parent = node->parent;

  if (node->parent == NIL)
    m_root = x;
  if (node == node->parent->left)
    node->parent->left = x;
  else
    node->parent->right = x;
  x->right = node;
  node->parent = x;
}

template <typename T> auto TreeSet<T>::fix_delete(Node<T> *x) -> void {
  while (x != m_root && x->color == Color::Black) {
    if (x == x->parent->left) {
      Node<T> *w = x->parent->right;
      if (w->color == Color::Red) {
        w->color = Color::Black;
        x->parent->color = Color::Red;
        left_rotate(x->parent);
        w = x->parent->right;
      }

      if (w->left->color == Color::Black && w->right->color == Color::Black) {
        w->color = Color::Red;
        x = x->parent;
        continue;
      } else if (w->right->color == Color::Black) {
        w->left->color = Color::Red;
        w->color = Color::Red;
        right_rotate(w);
        w = x->parent->right;
      }

      if (w->right->color == Color::Red) {
        w->color = x->parent->color;
        x->parent->color = Color::Black;
        w->right->color = Color::Black;
        left_rotate(x->parent);

        x = m_root;
      }
    } else {
      Node<T> *w = x->parent->left;
      if (w->color == Color::Red) {
        w->color = Color::Black;
        x->parent->color = Color::Red;
        right_rotate(x->parent);
        w = x->parent->left;
      }

      if (w->right->color == Color::Black && w->left->color == Color::Black) {
        w->color = Color::Red;
        x = x->parent;
        continue;
      } else if (w->left->color == Color::Black) {
        w->right->color = Color::Black;
        w->color = Color::Red;
        left_rotate(w);
        w = x->parent->left;
      }

      if (w->left->color == Color::Red) {
        w->color = x->parent->color;
        x->parent->color = Color::Black;
        w->left->color = Color::Black;
        right_rotate(x->parent);
        x = m_root;
      }
    }
  }
  x->color = Color::Black;
}

template <typename T> auto TreeSet<T>::successor(Node<T> *node) -> Node<T> * {
  if (node == NIL || node->left == NIL)
    return node;
  return successor(node->left);
}

template <typename T>
auto TreeSet<T>::search(const T &value, Node<T> *node) -> Node<T> * {
  if (node == NIL)
    return NIL;
  else if (node->value == value)
    return node;
  else if (node->value < value)
    return search(value, node->right); // review this one.

  return search(value, node->left);
}

template <typename T> auto TreeSet<T>::height(Node<T> *node) -> std::size_t {
  if (node == NIL)
    return 0;

  if (node->left == NIL && node->right == NIL)
    return 1;

  return 1 + (height(node->left) > height(node->right)) ? height(node->left)
                                                        : height(node->right);
}

template <typename T>
auto TreeSet<T>::transplant(Node<T> *u, Node<T> *v) -> void {
  if (u->parent == NIL)
    m_root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

template <typename T>
auto TreeSet<T>::swap_color(Node<T> *a, Node<T> *b) -> void {
  std::swap(a->color, b->color);
}

template <typename T>
auto TreeSet<T>::update_parent_child_link(Node<T> *t_parent,
                                          Node<T> *t_old_child,
                                          Node<T> *t_new_child) -> void {
  if (t_parent != NIL) {
    if (t_parent->left == t_old_child)
      t_parent->left = t_new_child;
    else
      t_parent->right = t_new_child;
  }
}