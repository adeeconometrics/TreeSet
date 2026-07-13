#ifndef TREESET_TPP
#define TREESET_TPP

#include <utility>

template <typename T, typename Compare>
TreeSet<T, Compare>::TreeSet()
    : m_nil(new Node<T>(T{}, Color::Black, nullptr, nullptr, nullptr)),
      m_root(m_nil) {
  m_nil->left = m_nil->right = m_nil->parent = m_nil;
}

template <typename T, typename Compare>
TreeSet<T, Compare>::TreeSet(std::initializer_list<T> t_init) : TreeSet() {
  for (const auto &val : t_init) {
    insert(val);
  }
}

template <typename T, typename Compare> TreeSet<T, Compare>::~TreeSet() {
  destroy(m_root);
  delete m_nil;
  m_nil = nullptr;
}

template <typename T, typename Compare>
TreeSet<T, Compare>::TreeSet(TreeSet &&t_other) noexcept
    : m_nil(t_other.m_nil), m_root(t_other.m_root),
      node_count(t_other.node_count), m_comp(std::move(t_other.m_comp)) {
  t_other.m_nil = new Node<T>(T{}, Color::Black, nullptr, nullptr, nullptr);
  t_other.m_nil->left = t_other.m_nil->right = t_other.m_nil->parent =
      t_other.m_nil;
  t_other.m_root = t_other.m_nil;
  t_other.node_count = 0;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::operator=(TreeSet &&t_other) noexcept -> TreeSet & {
  if (this != &t_other) {
    destroy(m_root);
    delete m_nil;
    m_nil = t_other.m_nil;
    m_root = t_other.m_root;
    node_count = t_other.node_count;
    m_comp = std::move(t_other.m_comp);
    t_other.m_nil = new Node<T>(T{}, Color::Black, nullptr, nullptr, nullptr);
    t_other.m_nil->left = t_other.m_nil->right = t_other.m_nil->parent =
        t_other.m_nil;
    t_other.m_root = t_other.m_nil;
    t_other.node_count = 0;
  }
  return *this;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::destroy(Node<T> *t_node) -> void {
  if (t_node != m_nil) {
    destroy(t_node->left);
    destroy(t_node->right);
    delete t_node;
    t_node = nullptr;
  }
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::insert(const T &t_value) -> bool {
  auto *z = new Node<T>(t_value, Color::Red, m_nil, m_nil, m_nil); // nosem
  auto *y = m_nil;
  auto *x = m_root;

  while (x != m_nil) {
    y = x;
    if (m_comp(t_value, x->value)) {
      x = x->left;
    } else if (m_comp(x->value, t_value)) {
      x = x->right;
    } else {
      delete z;
      z = nullptr;
      return false;
    }
  }

  z->parent = y;
  if (y == m_nil) {
    m_root = z;
  } else if (m_comp(t_value, y->value)) {
    y->left = z;
  } else {
    y->right = z;
  }

  fix_insertion_at(z);
  ++node_count;
  return true;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::remove(const T &t_value) -> bool {
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
  z = nullptr;
  --node_count;

  if (y_original_color == Color::Black) {
    fix_delete(x);
  }
  return true;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::successor(Node<T> *t_node) -> Node<T> * {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::predecessor(Node<T> *t_node) -> Node<T> * {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::search(const T &t_value,
                                 Node<T> *t_node) const -> Node<T> * {
  if (t_node == m_nil) {
    return m_nil;
  }
  if (!m_comp(t_value, t_node->value) && !m_comp(t_node->value, t_value)) {
    return t_node;
  }
  if (m_comp(t_value, t_node->value)) {
    return search(t_value, t_node->left);
  }
  return search(t_value, t_node->right);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::height(Node<T> *t_node) const -> std::size_t {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::fix_insertion_at(Node<T> *t_node) -> void {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::left_rotate(Node<T> *t_node) -> void {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::right_rotate(Node<T> *t_node) -> void {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::transplant(Node<T> *t_u, Node<T> *t_v) -> void {
  if (t_u->parent == m_nil) {
    m_root = t_v;
  } else if (t_u == t_u->parent->left) {
    t_u->parent->left = t_v;
  } else {
    t_u->parent->right = t_v;
  }
  t_v->parent = t_u->parent;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::swap_color(Node<T> *t_a, Node<T> *t_b) -> void {
  std::swap(t_a->color, t_b->color);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::update_parent_child_link(
    Node<T> *t_parent, Node<T> *t_old_child, Node<T> *t_new_child) -> void {
  if (t_parent != m_nil) {
    if (t_parent->left == t_old_child) {
      t_parent->left = t_new_child;
    } else {
      t_parent->right = t_new_child;
    }
  }
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::fix_delete(Node<T> *t_node) -> void {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::leftmost() const -> Node<T> * {
  if (m_root == m_nil) {
    return m_nil;
  }
  auto *node = m_root;
  while (node->left != m_nil) {
    node = node->left;
  }
  return node;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::rightmost() const -> Node<T> * {
  if (m_root == m_nil) {
    return m_nil;
  }
  auto *node = m_root;
  while (node->right != m_nil) {
    node = node->right;
  }
  return node;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::first_postorder() const -> Node<T> * {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::last_preorder() const -> Node<T> * {
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

template <typename T, typename Compare>
auto TreeSet<T, Compare>::size() const noexcept -> std::size_t {
  return node_count;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::height() const noexcept -> std::size_t {
  return height(m_root);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::is_empty() const noexcept -> bool {
  return node_count == 0;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::is_element(const T &t_value) const noexcept -> bool {
  return search(t_value, m_root) != m_nil;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::comp() const noexcept -> const Compare & {
  return m_comp;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::root() noexcept -> Node<T> * {
  return m_root;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::min(Node<T> *t_node) noexcept -> Node<T> * {
  while (t_node->left != m_nil) {
    t_node = t_node->left;
  }
  return t_node;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::max(Node<T> *t_node) noexcept -> Node<T> * {
  while (t_node->right != m_nil) {
    t_node = t_node->right;
  }
  return t_node;
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::begin() noexcept
    -> TreeSetIterator<T, inorder_t, false, Compare> {
  return TreeSetIterator<T, inorder_t, false, Compare>(leftmost(), this, false);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::end() noexcept
    -> TreeSetIterator<T, inorder_t, false, Compare> {
  return TreeSetIterator<T, inorder_t, false, Compare>(m_nil, this, false);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::rbegin() noexcept
    -> TreeSetIterator<T, inorder_t, false, Compare> {
  return TreeSetIterator<T, inorder_t, false, Compare>(rightmost(), this, true);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::rend() noexcept
    -> TreeSetIterator<T, inorder_t, false, Compare> {
  return TreeSetIterator<T, inorder_t, false, Compare>(m_nil, this, true);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::cbegin() const noexcept
    -> TreeSetIterator<T, inorder_t, true, Compare> {
  return TreeSetIterator<T, inorder_t, true, Compare>(leftmost(), this, false);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::cend() const noexcept
    -> TreeSetIterator<T, inorder_t, true, Compare> {
  return TreeSetIterator<T, inorder_t, true, Compare>(m_nil, this, false);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::crbegin() const noexcept
    -> TreeSetIterator<T, inorder_t, true, Compare> {
  return TreeSetIterator<T, inorder_t, true, Compare>(rightmost(), this, true);
}

template <typename T, typename Compare>
auto TreeSet<T, Compare>::crend() const noexcept
    -> TreeSetIterator<T, inorder_t, true, Compare> {
  return TreeSetIterator<T, inorder_t, true, Compare>(m_nil, this, true);
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::begin() noexcept
    -> TreeSetIterator<T, Traversal, false, Compare> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, false, Compare>(m_root, this, false);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, false, Compare>(first_postorder(),
                                                         this, false);
  } else {
    return TreeSetIterator<T, Traversal, false, Compare>(leftmost(), this,
                                                         false);
  }
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::end() noexcept
    -> TreeSetIterator<T, Traversal, false, Compare> {
  return TreeSetIterator<T, Traversal, false, Compare>(m_nil, this, false);
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::rbegin() noexcept
    -> TreeSetIterator<T, Traversal, false, Compare> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, false, Compare>(last_preorder(), this,
                                                         true);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, false, Compare>(m_root, this, true);
  } else {
    return TreeSetIterator<T, Traversal, false, Compare>(rightmost(), this,
                                                         true);
  }
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::rend() noexcept
    -> TreeSetIterator<T, Traversal, false, Compare> {
  return TreeSetIterator<T, Traversal, false, Compare>(m_nil, this, true);
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::cbegin() const noexcept
    -> TreeSetIterator<T, Traversal, true, Compare> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, true, Compare>(m_root, this, false);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, true, Compare>(first_postorder(), this,
                                                        false);
  } else {
    return TreeSetIterator<T, Traversal, true, Compare>(leftmost(), this,
                                                        false);
  }
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::cend() const noexcept
    -> TreeSetIterator<T, Traversal, true, Compare> {
  return TreeSetIterator<T, Traversal, true, Compare>(m_nil, this, false);
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::crbegin() const noexcept
    -> TreeSetIterator<T, Traversal, true, Compare> {
  if constexpr (std::is_same_v<Traversal, preorder_t>) {
    return TreeSetIterator<T, Traversal, true, Compare>(last_preorder(), this,
                                                        true);
  } else if constexpr (std::is_same_v<Traversal, postorder_t>) {
    return TreeSetIterator<T, Traversal, true, Compare>(m_root, this, true);
  } else {
    return TreeSetIterator<T, Traversal, true, Compare>(rightmost(), this,
                                                        true);
  }
}

template <typename T, typename Compare>
template <typename Traversal>
auto TreeSet<T, Compare>::crend() const noexcept
    -> TreeSetIterator<T, Traversal, true, Compare> {
  return TreeSetIterator<T, Traversal, true, Compare>(m_nil, this, true);
}

#endif // TREESET_TPP
