#ifndef TREESET_OPS_HPP
#define TREESET_OPS_HPP

#include "TreeSet.hpp"
#include <utility>

template <typename T, typename Compare>
auto symmetric_diff(const TreeSet<T, Compare> &t_lhs,
                    const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto set_union(const TreeSet<T, Compare> &t_lhs,
               const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto intersection(const TreeSet<T, Compare> &t_lhs,
                  const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto asymmetric_diff(const TreeSet<T, Compare> &t_lhs,
                     const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto set_or(const TreeSet<T, Compare> &t_lhs,
            const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto set_and(const TreeSet<T, Compare> &t_lhs,
             const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto product(const TreeSet<T, Compare> &t_lhs,
             const TreeSet<T, Compare> &t_rhs)
    -> TreeSet<std::pair<T, T>, std::less<std::pair<T, T>>>;

template <typename T, typename Compare>
auto operator-(const TreeSet<T, Compare> &t_lhs,
               const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto operator||(const TreeSet<T, Compare> &t_lhs,
                const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto operator&&(const TreeSet<T, Compare> &t_lhs,
                const TreeSet<T, Compare> &t_rhs) -> TreeSet<T, Compare>;

template <typename T, typename Compare>
auto operator*(const TreeSet<T, Compare> &t_lhs,
               const TreeSet<T, Compare> &t_rhs)
    -> TreeSet<std::pair<T, T>, std::less<std::pair<T, T>>>;

#include "detail/TreeSetOps.tpp"

#endif // TREESET_OPS_HPP
