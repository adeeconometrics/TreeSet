#ifndef TREESET_ITERATOR_TPP
#define TREESET_ITERATOR_TPP

#include <type_traits>

template <typename T, typename Traversal, bool IsConst, typename Compare>
auto TreeSetIterator<T, Traversal, IsConst, Compare>::advance_forward()
    -> void {
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

template <typename T, typename Traversal, bool IsConst, typename Compare>
auto TreeSetIterator<T, Traversal, IsConst, Compare>::advance_backward()
    -> void {
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

#endif // TREESET_ITERATOR_TPP
