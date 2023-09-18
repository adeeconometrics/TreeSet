#ifndef __TREESET_H__
#define __TREESET_H__

#include <cstddef>
#include <memory>

enum Color : bool {
  Red = true,
  Black = false,
};

template <typename T> struct Node {
  Node *parent{nullptr};
  Node *left{nullptr};
  Node *right{nullptr};

  Color color = Color::Red;
  T value{};

public:
  Node(const T &t_value, Node *t_parent) : value(t_value), parent(t_parent){};
  Node(const T &t_value, Color t_color) : value(t_value), color(t_color){};
  Node(const T &t_value, Color t_color, Node<T> *t_parent, Node<T> *t_left,
       Node<T> *t_right);

};

// RBT implementation
template <typename T> class TreeSet { 
public: 
    TreeSet();

    auto insert(const T &t_value = T{}) -> bool;
    auto remove(const T& t_value) -> bool;

    auto size() const noexcept -> std::size_t;
    auto height() const noexcept -> std::size_t;
    auto is_empty() const noexcept -> bool;
    auto is_element(const T& t_value) const noexcept -> bool;

    auto root() const noexcept -> Node<T>;
    auto min(Node<T>* t_node) const noexcept -> Node<T>;
    auto max(Node<T>* t_node) const noexcept -> Node<T>;

private:
    auto fix_insertion_at(Node<T>* t_node) -> void;
    auto left_rotate(Node<T>* t_node) -> void;
    auto right_rotate(Node<T>* t_node) -> void;
    auto fix_delete(Node<T>* t_node) -> void;

    auto successor(Node<T>* t_node) -> Node<T>*;
    auto search(const T& t_value, Node<T>* t_node) -> Node<T>*;
    auto height(Node<T>* t_node) -> std::size_t;

    auto transplant(Node<T>* u, Node<T>* v) -> void; 
    auto swap_color(Node<T>* a, Node<T>* b) -> void;
    auto update_parent_child_link(
        Node<T>* t_parent, Node<T>* t_old_child, Node<T>* t_new_child 
    ) -> void;

private:
    Node<T>* m_root {nullptr}; // should I change this?
    std::unique_ptr<Node<T>> NIL{nullptr};
    std::size_t node_count {0};

};

#endif // __TREESET_H__