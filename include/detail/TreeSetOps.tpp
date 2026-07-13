#ifndef TREESET_OPS_TPP
#define TREESET_OPS_TPP

template <typename T, typename Compare>
auto symmetric_diff(const TreeSet<T, Compare> &t_lhs,
                    const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  TreeSet<T, Compare> result;
  auto it_a = t_lhs.cbegin();
  auto it_b = t_rhs.cbegin();
  auto end_a = t_lhs.cend();
  auto end_b = t_rhs.cend();

  while (it_a != end_a && it_b != end_b) {
    if (t_lhs.m_comp(*it_a, *it_b)) {
      result.insert(*it_a);
      ++it_a;
    } else if (t_lhs.m_comp(*it_b, *it_a)) {
      result.insert(*it_b);
      ++it_b;
    } else {
      ++it_a;
      ++it_b;
    }
  }
  while (it_a != end_a) {
    result.insert(*it_a);
    ++it_a;
  }
  while (it_b != end_b) {
    result.insert(*it_b);
    ++it_b;
  }
  return result;
}

template <typename T, typename Compare>
auto set_union(const TreeSet<T, Compare> &t_lhs,
               const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  TreeSet<T, Compare> result;
  auto it_a = t_lhs.cbegin();
  auto it_b = t_rhs.cbegin();
  auto end_a = t_lhs.cend();
  auto end_b = t_rhs.cend();

  while (it_a != end_a && it_b != end_b) {
    if (t_lhs.m_comp(*it_a, *it_b)) {
      result.insert(*it_a);
      ++it_a;
    } else if (t_lhs.m_comp(*it_b, *it_a)) {
      result.insert(*it_b);
      ++it_b;
    } else {
      result.insert(*it_a);
      ++it_a;
      ++it_b;
    }
  }
  while (it_a != end_a) {
    result.insert(*it_a);
    ++it_a;
  }
  while (it_b != end_b) {
    result.insert(*it_b);
    ++it_b;
  }
  return result;
}

template <typename T, typename Compare>
auto intersection(const TreeSet<T, Compare> &t_lhs,
                  const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  TreeSet<T, Compare> result;
  auto it_a = t_lhs.cbegin();
  auto it_b = t_rhs.cbegin();
  auto end_a = t_lhs.cend();
  auto end_b = t_rhs.cend();

  while (it_a != end_a && it_b != end_b) {
    if (t_lhs.m_comp(*it_a, *it_b)) {
      ++it_a;
    } else if (t_lhs.m_comp(*it_b, *it_a)) {
      ++it_b;
    } else {
      result.insert(*it_a);
      ++it_a;
      ++it_b;
    }
  }
  return result;
}

template <typename T, typename Compare>
auto asymmetric_diff(const TreeSet<T, Compare> &t_lhs,
                     const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  TreeSet<T, Compare> result;
  auto it_a = t_lhs.cbegin();
  auto it_b = t_rhs.cbegin();
  auto end_a = t_lhs.cend();
  auto end_b = t_rhs.cend();

  while (it_a != end_a && it_b != end_b) {
    if (t_lhs.m_comp(*it_a, *it_b)) {
      result.insert(*it_a);
      ++it_a;
    } else if (t_lhs.m_comp(*it_b, *it_a)) {
      ++it_b;
    } else {
      ++it_a;
      ++it_b;
    }
  }
  while (it_a != end_a) {
    result.insert(*it_a);
    ++it_a;
  }
  return result;
}

template <typename T, typename Compare>
auto set_or(const TreeSet<T, Compare> &t_lhs,
            const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  return set_union(t_lhs, t_rhs);
}

template <typename T, typename Compare>
auto set_and(const TreeSet<T, Compare> &t_lhs,
             const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  return intersection(t_lhs, t_rhs);
}

template <typename T, typename Compare>
auto product(const TreeSet<T, Compare> &t_lhs,
             const TreeSet<T, Compare> &t_rhs)
    -> TreeSet<std::pair<T, T>, std::less<std::pair<T, T>>> {
  TreeSet<std::pair<T, T>, std::less<std::pair<T, T>>> result;
  for (auto it_a = t_lhs.cbegin(); it_a != t_lhs.cend(); ++it_a) {
    for (auto it_b = t_rhs.cbegin(); it_b != t_rhs.cend(); ++it_b) {
      result.insert(std::make_pair(*it_a, *it_b));
    }
  }
  return result;
}

template <typename T, typename Compare>
auto operator-(const TreeSet<T, Compare> &t_lhs,
               const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  return asymmetric_diff(t_lhs, t_rhs);
}

template <typename T, typename Compare>
auto operator||(const TreeSet<T, Compare> &t_lhs,
                const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  return set_or(t_lhs, t_rhs);
}

template <typename T, typename Compare>
auto operator&&(const TreeSet<T, Compare> &t_lhs,
                const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare> {
  return set_and(t_lhs, t_rhs);
}

template <typename T, typename Compare>
auto operator*(const TreeSet<T, Compare> &t_lhs,
               const TreeSet<T, Compare> &t_rhs)
    -> TreeSet<std::pair<T, T>, std::less<std::pair<T, T>>> {
  return product(t_lhs, t_rhs);
}

#endif // TREESET_OPS_TPP
