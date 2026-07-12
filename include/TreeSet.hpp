#ifndef TREESET_HPP
#define TREESET_HPP

#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>

enum Color : bool {
  Red = true,
  Black = false,
};

template <typename T> struct Node {
  Node *parent{nullptr};
  Node *left{nullptr};
  Node *right{nullptr};
  Color color{Color::Red};
  T value{};

  Node() = default;
  Node(const T &t_value, Node *t_parent) : parent(t_parent), value(t_value) {}
  Node(const T &t_value, Color t_color) : color(t_color), value(t_value) {}
  Node(const T &t_value, Color t_color, Node *t_parent, Node *t_left,
       Node *t_right)
      : parent(t_parent), left(t_left), right(t_right), color(t_color),
        value(t_value) {}
};

struct inorder_t {};
struct preorder_t {};
struct postorder_t {};
inline constexpr inorder_t inorder{};
inline constexpr preorder_t preorder{};
inline constexpr postorder_t postorder{};

template <typename T> class TreeSet;

template <typename T, typename Traversal = inorder_t, bool IsConst = false>
class TreeSetIterator {
public:
  using value_type = T;
  using reference = std::conditional_t<IsConst, const T, T> &;
  using pointer = std::conditional_t<IsConst, const T, T> *;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  TreeSetIterator() = default;
  TreeSetIterator(Node<T> *t_current, const TreeSet<T> *t_tree, bool t_reverse)
      : m_current(t_current), m_tree(t_tree), m_reverse(t_reverse) {}

  auto operator*() const -> reference { return m_current->value; }
  auto operator->() const -> pointer { return &m_current->value; }

  auto operator++() -> TreeSetIterator & {
    if (m_reverse) {
      advance_backward();
    } else {
      advance_forward();
    }
    return *this;
  }

  auto operator++(int) -> TreeSetIterator {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  auto operator--() -> TreeSetIterator & {
    if (m_reverse) {
      advance_forward();
    } else {
      advance_backward();
    }
    return *this;
  }

  auto operator--(int) -> TreeSetIterator {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  auto operator==(const TreeSetIterator &t_other) const -> bool {
    return m_current == t_other.m_current;
  }

  auto operator!=(const TreeSetIterator &t_other) const -> bool {
    return m_current != t_other.m_current;
  }

private:
  Node<T> *m_current{nullptr};
  const TreeSet<T> *m_tree{nullptr};
  bool m_reverse{false};

  template <typename U> friend class TreeSet;

  auto advance_forward() -> void;
  auto advance_backward() -> void;
};

template <typename T, typename Traversal, bool IsConst>
auto TreeSetIterator<T, Traversal, IsConst>::advance_forward() -> void {
  if constexpr (std::is_same_v<Traversal, inorder_t>) {
    if (m_current->right != m_tree->m_nil) {
      m_current = m_current->right;
      while (m_current->left != m_tree->m_nil) {
        m_current = m_current->left;
      }
    } else {
      auto parent = m_current->parent;
      while (parent != m_tree->m_nil && m_current == parent->right) {
        m_current = parent;
        parent = parent->parent;
      }
      m_current = parent;
    }
  } else if constexpr (std::is_same_v<Traversal, preorder_t>) {
    if (m_current->left != m_tree->m_nil) {
      m_current = m_current->left;
    } else if (m_current->right != m_tree->m_nil) {
      m_current = m_current->right;
    } else {
      auto parent = m_current->parent;
      while (parent != m_tree->m_nil) {
        if (m_current == parent->left && parent->right != m_tree->m_nil) {
          m_current = parent->right;
          return;
        }
        m_current = parent;
        parent = parent->parent;
      }
      m_current = m_tree->m_nil;
    }
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    auto parent = m_current->parent;
    if (parent == m_tree->m_nil) {
      m_current = m_tree->m_nil;
      return;
    }
    if (m_current == parent->left && parent->right != m_tree->m_nil) {
      m_current = parent->right;
      while (true) {
        if (m_current->left != m_tree->m_nil) {
          m_current = m_current->left;
        } else if (m_current->right != m_tree->m_nil) {
          m_current = m_current->right;
        } else {
          break;
        }
      }
    } else if (m_current == parent->left) {
      m_current = parent;
    } else {
      m_current = parent;
    }
  }
}

template <typename T, typename Traversal, bool IsConst>
auto TreeSetIterator<T, Traversal, IsConst>::advance_backward() -> void {
  if constexpr (std::is_same_v<Traversal, inorder_t>) {
    if (m_current == m_tree->m_nil) {
      m_current = m_tree->m_root;
      if (m_current != m_tree->m_nil) {
        while (m_current->right != m_tree->m_nil) {
          m_current = m_current->right;
        }
      }
    } else if (m_current->left != m_tree->m_nil) {
      m_current = m_current->left;
      while (m_current->right != m_tree->m_nil) {
        m_current = m_current->right;
      }
    } else {
      auto parent = m_current->parent;
      while (parent != m_tree->m_nil && m_current == parent->left) {
        m_current = parent;
        parent = parent->parent;
      }
      m_current = parent;
    }
  } else if constexpr (std::is_same_v<Traversal, preorder_t>) {
    if (m_current == m_tree->m_nil) {
      m_current = m_tree->m_root;
      if (m_current == m_tree->m_nil) {
        return;
      }
      while (true) {
        if (m_current->right != m_tree->m_nil) {
          m_current = m_current->right;
        } else if (m_current->left != m_tree->m_nil) {
          m_current = m_current->left;
        } else {
          break;
        }
      }
    } else {
      auto parent = m_current->parent;
      if (parent == m_tree->m_nil) {
        m_current = m_tree->m_nil;
        return;
      }
      if (m_current == parent->left) {
        m_current = parent;
      } else {
        if (parent->left != m_tree->m_nil) {
          m_current = parent->left;
          while (true) {
            if (m_current->right != m_tree->m_nil) {
              m_current = m_current->right;
            } else if (m_current->left != m_tree->m_nil) {
              m_current = m_current->left;
            } else {
              break;
            }
          }
        } else {
          m_current = parent;
        }
      }
    }
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    if (m_current == m_tree->m_nil) {
      m_current = m_tree->m_root;
      return;
    }
    if (m_current->right != m_tree->m_nil) {
      m_current = m_current->right;
      return;
    }
    if (m_current->left != m_tree->m_nil) {
      m_current = m_current->left;
      return;
    }
    while (true) {
      auto parent = m_current->parent;
      if (parent == m_tree->m_nil) {
        m_current = m_tree->m_nil;
        return;
      }
      if (m_current == parent->right && parent->left != m_tree->m_nil) {
        m_current = parent->left;
        return;
      }
      m_current = parent;
    }
  }
}

template <typename T> class TreeSet {
public:
  TreeSet();
  ~TreeSet();

  TreeSet(const TreeSet &) = delete;
  auto operator=(const TreeSet &) -> TreeSet & = delete;

  auto insert(const T &t_value) -> bool;
  auto remove(const T &t_value) -> bool;

  auto size() const noexcept -> std::size_t;
  auto height() const noexcept -> std::size_t;
  auto is_empty() const noexcept -> bool;
  auto is_element(const T &t_value) const noexcept -> bool;

  auto root() noexcept -> Node<T> *;
  auto min(Node<T> *t_node) noexcept -> Node<T> *;
  auto max(Node<T> *t_node) noexcept -> Node<T> *;

  auto begin() noexcept -> TreeSetIterator<T, inorder_t, false>;
  auto end() noexcept -> TreeSetIterator<T, inorder_t, false>;
  auto rbegin() noexcept -> TreeSetIterator<T, inorder_t, false>;
  auto rend() noexcept -> TreeSetIterator<T, inorder_t, false>;

  auto cbegin() const noexcept -> TreeSetIterator<T, inorder_t, true>;
  auto cend() const noexcept -> TreeSetIterator<T, inorder_t, true>;
  auto crbegin() const noexcept -> TreeSetIterator<T, inorder_t, true>;
  auto crend() const noexcept -> TreeSetIterator<T, inorder_t, true>;

  template <typename Traversal>
  auto begin() noexcept -> TreeSetIterator<T, Traversal, false>;
  template <typename Traversal>
  auto end() noexcept -> TreeSetIterator<T, Traversal, false>;
  template <typename Traversal>
  auto rbegin() noexcept -> TreeSetIterator<T, Traversal, false>;
  template <typename Traversal>
  auto rend() noexcept -> TreeSetIterator<T, Traversal, false>;

  template <typename Traversal>
  auto cbegin() const noexcept -> TreeSetIterator<T, Traversal, true>;
  template <typename Traversal>
  auto cend() const noexcept -> TreeSetIterator<T, Traversal, true>;
  template <typename Traversal>
  auto crbegin() const noexcept -> TreeSetIterator<T, Traversal, true>;
  template <typename Traversal>
  auto crend() const noexcept -> TreeSetIterator<T, Traversal, true>;

private:
  auto fix_insertion_at(Node<T> *t_node) -> void;
  auto left_rotate(Node<T> *t_node) -> void;
  auto right_rotate(Node<T> *t_node) -> void;
  auto fix_delete(Node<T> *t_node) -> void;

  auto successor(Node<T> *t_node) -> Node<T> *;
  auto predecessor(Node<T> *t_node) -> Node<T> *;
  auto search(const T &t_value, Node<T> *t_node) const -> Node<T> *;
  auto height(Node<T> *t_node) const -> std::size_t;

  auto transplant(Node<T> *t_u, Node<T> *t_v) -> void;
  auto swap_color(Node<T> *t_a, Node<T> *t_b) -> void;
  auto update_parent_child_link(Node<T> *t_parent, Node<T> *t_old_child,
                                Node<T> *t_new_child) -> void;
  auto destroy(Node<T> *t_node) -> void;
  auto leftmost() const -> Node<T> *;
  auto rightmost() const -> Node<T> *;
  auto first_postorder() const -> Node<T> *;
  auto last_preorder() const -> Node<T> *;

private:
  Node<T> *m_nil{nullptr};
  Node<T> *m_root{nullptr};
  std::size_t node_count{0};

  template <typename U, typename Traversal, bool IsConst>
  friend class TreeSetIterator;
};

template <typename T>
TreeSet<T>::TreeSet()
    : m_nil(new Node<T>(T{}, Color::Black, nullptr, nullptr, nullptr)),
      m_root(m_nil) {
  m_nil->left = m_nil->right = m_nil->parent = m_nil;
}

template <typename T> TreeSet<T>::~TreeSet() {
  destroy(m_root);
  delete m_nil;
}

template <typename T> auto TreeSet<T>::destroy(Node<T> *t_node) -> void {
  if (t_node != m_nil) {
    destroy(t_node->left);
    destroy(t_node->right);
    delete t_node;
  }
}

template <typename T> auto TreeSet<T>::insert(const T &t_value) -> bool {
  auto *z = new Node<T>(t_value, Color::Red, m_nil, m_nil, m_nil);
  auto *y = m_nil;
  auto *x = m_root;

  while (x != m_nil) {
    y = x;
    if (t_value < x->value) {
      x = x->left;
    } else if (x->value < t_value) {
      x = x->right;
    } else {
      delete z;
      return false;
    }
  }

  z->parent = y;
  if (y == m_nil) {
    m_root = z;
  } else if (t_value < y->value) {
    y->left = z;
  } else {
    y->right = z;
  }

  fix_insertion_at(z);
  ++node_count;
  return true;
}

template <typename T> auto TreeSet<T>::remove(const T &t_value) -> bool {
  auto *z = search(t_value, m_root);
  if (z == m_nil) {
    return false;
  }

  auto *y = z;
  auto y_original_color = y->color;
  Node<T> *x = nullptr;

  if (z->left == m_nil) {
    x = z->right;
    transplant(z, z->right);
  } else if (z->right == m_nil) {
    x = z->left;
    transplant(z, z->left);
  } else {
    y = z->right;
    while (y->left != m_nil) {
      y = y->left;
    }
    y_original_color = y->color;
    x = y->right;
    if (y->parent == z) {
      x->parent = y;
    } else {
      transplant(y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    transplant(z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  delete z;
  --node_count;

  if (y_original_color == Color::Black) {
    fix_delete(x);
  }
  return true;
}

template <typename T> auto TreeSet<T>::successor(Node<T> *t_node) -> Node<T> * {
  if (t_node->right != m_nil) {
    auto *node = t_node->right;
    while (node->left != m_nil) {
      node = node->left;
    }
    return node;
  }
  auto *parent = t_node->parent;
  while (parent != m_nil && t_node == parent->right) {
    t_node = parent;
    parent = parent->parent;
  }
  return parent;
}

template <typename T>
auto TreeSet<T>::predecessor(Node<T> *t_node) -> Node<T> * {
  if (t_node->left != m_nil) {
    auto *node = t_node->left;
    while (node->right != m_nil) {
      node = node->right;
    }
    return node;
  }
  auto *parent = t_node->parent;
  while (parent != m_nil && t_node == parent->left) {
    t_node = parent;
    parent = parent->parent;
  }
  return parent;
}

template <typename T>
auto TreeSet<T>::search(const T &t_value, Node<T> *t_node) const -> Node<T> * {
  if (t_node == m_nil) {
    return m_nil;
  }
  if (t_value == t_node->value) {
    return t_node;
  }
  if (t_value < t_node->value) {
    return search(t_value, t_node->left);
  }
  return search(t_value, t_node->right);
}

template <typename T>
auto TreeSet<T>::height(Node<T> *t_node) const -> std::size_t {
  if (t_node == m_nil) {
    return 0;
  }
  if (t_node->left == m_nil && t_node->right == m_nil) {
    return 1;
  }
  auto left_h = height(t_node->left);
  auto right_h = height(t_node->right);
  return (left_h > right_h ? left_h : right_h) + 1;
}

template <typename T>
auto TreeSet<T>::fix_insertion_at(Node<T> *t_node) -> void {
  while (t_node->parent->color == Color::Red) {
    if (t_node->parent == t_node->parent->parent->left) {
      auto *uncle = t_node->parent->parent->right;
      if (uncle->color == Color::Red) {
        t_node->parent->color = Color::Black;
        uncle->color = Color::Black;
        t_node->parent->parent->color = Color::Red;
        t_node = t_node->parent->parent;
      } else {
        if (t_node == t_node->parent->right) {
          t_node = t_node->parent;
          left_rotate(t_node);
        }
        t_node->parent->color = Color::Black;
        t_node->parent->parent->color = Color::Red;
        right_rotate(t_node->parent->parent);
      }
    } else {
      auto *uncle = t_node->parent->parent->left;
      if (uncle->color == Color::Red) {
        t_node->parent->color = Color::Black;
        uncle->color = Color::Black;
        t_node->parent->parent->color = Color::Red;
        t_node = t_node->parent->parent;
      } else {
        if (t_node == t_node->parent->left) {
          t_node = t_node->parent;
          right_rotate(t_node);
        }
        t_node->parent->color = Color::Black;
        t_node->parent->parent->color = Color::Red;
        left_rotate(t_node->parent->parent);
      }
    }
  }
  m_root->color = Color::Black;
}

template <typename T> auto TreeSet<T>::left_rotate(Node<T> *t_node) -> void {
  auto *y = t_node->right;
  t_node->right = y->left;
  if (y->left != m_nil) {
    y->left->parent = t_node;
  }
  y->parent = t_node->parent;
  if (t_node->parent == m_nil) {
    m_root = y;
  } else if (t_node == t_node->parent->left) {
    t_node->parent->left = y;
  } else {
    t_node->parent->right = y;
  }
  y->left = t_node;
  t_node->parent = y;
}

template <typename T> auto TreeSet<T>::right_rotate(Node<T> *t_node) -> void {
  auto *y = t_node->left;
  t_node->left = y->right;
  if (y->right != m_nil) {
    y->right->parent = t_node;
  }
  y->parent = t_node->parent;
  if (t_node->parent == m_nil) {
    m_root = y;
  } else if (t_node == t_node->parent->right) {
    t_node->parent->right = y;
  } else {
    t_node->parent->left = y;
  }
  y->right = t_node;
  t_node->parent = y;
}

template <typename T>
auto TreeSet<T>::transplant(Node<T> *t_u, Node<T> *t_v) -> void {
  if (t_u->parent == m_nil) {
    m_root = t_v;
  } else if (t_u == t_u->parent->left) {
    t_u->parent->left = t_v;
  } else {
    t_u->parent->right = t_v;
  }
  t_v->parent = t_u->parent;
}

template <typename T>
auto TreeSet<T>::swap_color(Node<T> *t_a, Node<T> *t_b) -> void {
  std::swap(t_a->color, t_b->color);
}

template <typename T>
auto TreeSet<T>::update_parent_child_link(Node<T> *t_parent,
                                          Node<T> *t_old_child,
                                          Node<T> *t_new_child) -> void {
  if (t_parent != m_nil) {
    if (t_parent->left == t_old_child) {
      t_parent->left = t_new_child;
    } else {
      t_parent->right = t_new_child;
    }
  }
}

template <typename T> auto TreeSet<T>::fix_delete(Node<T> *t_node) -> void {
  while (t_node != m_root && t_node->color == Color::Black) {
    if (t_node == t_node->parent->left) {
      auto *w = t_node->parent->right;
      if (w->color == Color::Red) {
        w->color = Color::Black;
        t_node->parent->color = Color::Red;
        left_rotate(t_node->parent);
        w = t_node->parent->right;
      }
      if (w->left->color == Color::Black && w->right->color == Color::Black) {
        w->color = Color::Red;
        t_node = t_node->parent;
      } else {
        if (w->right->color == Color::Black) {
          w->left->color = Color::Black;
          w->color = Color::Red;
          right_rotate(w);
          w = t_node->parent->right;
        }
        w->color = t_node->parent->color;
        t_node->parent->color = Color::Black;
        w->right->color = Color::Black;
        left_rotate(t_node->parent);
        t_node = m_root;
      }
    } else {
      auto *w = t_node->parent->left;
      if (w->color == Color::Red) {
        w->color = Color::Black;
        t_node->parent->color = Color::Red;
        right_rotate(t_node->parent);
        w = t_node->parent->left;
      }
      if (w->right->color == Color::Black && w->left->color == Color::Black) {
        w->color = Color::Red;
        t_node = t_node->parent;
      } else {
        if (w->left->color == Color::Black) {
          w->right->color = Color::Black;
          w->color = Color::Red;
          left_rotate(w);
          w = t_node->parent->left;
        }
        w->color = t_node->parent->color;
        t_node->parent->color = Color::Black;
        w->left->color = Color::Black;
        right_rotate(t_node->parent);
        t_node = m_root;
      }
    }
  }
  t_node->color = Color::Black;
}

template <typename T> auto TreeSet<T>::leftmost() const -> Node<T> * {
  if (m_root == m_nil) {
    return m_nil;
  }
  auto *node = m_root;
  while (node->left != m_nil) {
    node = node->left;
  }
  return node;
}

template <typename T> auto TreeSet<T>::rightmost() const -> Node<T> * {
  if (m_root == m_nil) {
    return m_nil;
  }
  auto *node = m_root;
  while (node->right != m_nil) {
    node = node->right;
  }
  return node;
}

template <typename T> auto TreeSet<T>::first_postorder() const -> Node<T> * {
  if (m_root == m_nil) {
    return m_nil;
  }
  auto *node = m_root;
  while (true) {
    if (node->left != m_nil) {
      node = node->left;
    } else if (node->right != m_nil) {
      node = node->right;
    } else {
      break;
    }
  }
  return node;
}

template <typename T> auto TreeSet<T>::last_preorder() const -> Node<T> * {
  if (m_root == m_nil) {
    return m_nil;
  }
  auto *node = m_root;
  while (true) {
    if (node->right != m_nil) {
      node = node->right;
    } else if (node->left != m_nil) {
      node = node->left;
    } else {
      break;
    }
  }
  return node;
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
  return search(t_value, m_root) != m_nil;
}

template <typename T> auto TreeSet<T>::root() noexcept -> Node<T> * {
  return m_root;
}

template <typename T>
auto TreeSet<T>::min(Node<T> *t_node) noexcept -> Node<T> * {
  while (t_node->left != m_nil) {
    t_node = t_node->left;
  }
  return t_node;
}

template <typename T>
auto TreeSet<T>::max(Node<T> *t_node) noexcept -> Node<T> * {
  while (t_node->right != m_nil) {
    t_node = t_node->right;
  }
  return t_node;
}

template <typename T>
auto TreeSet<T>::begin() noexcept -> TreeSetIterator<T, inorder_t, false> {
  return TreeSetIterator<T, inorder_t, false>(leftmost(), this, false);
}

template <typename T>
auto TreeSet<T>::end() noexcept -> TreeSetIterator<T, inorder_t, false> {
  return TreeSetIterator<T, inorder_t, false>(m_nil, this, false);
}

template <typename T>
auto TreeSet<T>::rbegin() noexcept -> TreeSetIterator<T, inorder_t, false> {
  return TreeSetIterator<T, inorder_t, false>(rightmost(), this, true);
}

template <typename T>
auto TreeSet<T>::rend() noexcept -> TreeSetIterator<T, inorder_t, false> {
  return TreeSetIterator<T, inorder_t, false>(m_nil, this, true);
}

template <typename T>
auto TreeSet<T>::cbegin() const noexcept
    -> TreeSetIterator<T, inorder_t, true> {
  return TreeSetIterator<T, inorder_t, true>(leftmost(), this, false);
}

template <typename T>
auto TreeSet<T>::cend() const noexcept -> TreeSetIterator<T, inorder_t, true> {
  return TreeSetIterator<T, inorder_t, true>(m_nil, this, false);
}

template <typename T>
auto TreeSet<T>::crbegin() const noexcept
    -> TreeSetIterator<T, inorder_t, true> {
  return TreeSetIterator<T, inorder_t, true>(rightmost(), this, true);
}

template <typename T>
auto TreeSet<T>::crend() const noexcept -> TreeSetIterator<T, inorder_t, true> {
  return TreeSetIterator<T, inorder_t, true>(m_nil, this, true);
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::begin() noexcept -> TreeSetIterator<T, Traversal, false> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, false>(m_root, this, false);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, false>(first_postorder(), this, false);
  } else {
    return TreeSetIterator<T, Traversal, false>(leftmost(), this, false);
  }
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::end() noexcept -> TreeSetIterator<T, Traversal, false> {
  return TreeSetIterator<T, Traversal, false>(m_nil, this, false);
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::rbegin() noexcept -> TreeSetIterator<T, Traversal, false> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, false>(last_preorder(), this, true);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, false>(m_root, this, true);
  } else {
    return TreeSetIterator<T, Traversal, false>(rightmost(), this, true);
  }
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::rend() noexcept -> TreeSetIterator<T, Traversal, false> {
  return TreeSetIterator<T, Traversal, false>(m_nil, this, true);
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::cbegin() const noexcept
    -> TreeSetIterator<T, Traversal, true> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, true>(m_root, this, false);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, true>(first_postorder(), this, false);
  } else {
    return TreeSetIterator<T, Traversal, true>(leftmost(), this, false);
  }
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::cend() const noexcept -> TreeSetIterator<T, Traversal, true> {
  return TreeSetIterator<T, Traversal, true>(m_nil, this, false);
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::crbegin() const noexcept
    -> TreeSetIterator<T, Traversal, true> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, true>(last_preorder(), this, true);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, true>(m_root, this, true);
  } else {
    return TreeSetIterator<T, Traversal, true>(rightmost(), this, true);
  }
}

template <typename T>
template <typename Traversal>
auto TreeSet<T>::crend() const noexcept -> TreeSetIterator<T, Traversal, true> {
  return TreeSetIterator<T, Traversal, true>(m_nil, this, true);
}

#endif // TREESET_HPP
