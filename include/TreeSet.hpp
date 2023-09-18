#ifndef __TREESET_H__
#define __TREESET_H__

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
  Node(const T &t_value, bool t_color) : value(t_value), color(t_color){};
  Node(const T &t_value, bool t_color, 
        Node *t_parent, Node *t_left,Node *t_right)
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
};

template <typename T> class RBT { public: };

#endif // __TREESET_H__