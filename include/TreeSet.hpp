#ifndef TREESET_HPP
#define TREESET_HPP

#include <cstddef>
#include <iterator>
#include <type_traits>

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

#include "detail/Iterator.tpp"
#include "detail/TreeSet.tpp"

#endif // TREESET_HPP
