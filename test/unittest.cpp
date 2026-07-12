#include "../include/TreeSet.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <numeric>
#include <vector>

using ::testing::ElementsAre;

class TreeSetTest : public ::testing::Test {
protected:
  TreeSet<int> tree;

  auto inorder_vec() -> std::vector<int> {
    std::vector<int> result;
    for (auto it = tree.begin(); it != tree.end(); ++it)
      result.push_back(*it);
    return result;
  }

  auto preorder_vec() -> std::vector<int> {
    std::vector<int> result;
    for (auto it = tree.begin<preorder_t>(); it != tree.end<preorder_t>(); ++it)
      result.push_back(*it);
    return result;
  }

  auto postorder_vec() -> std::vector<int> {
    std::vector<int> result;
    for (auto it = tree.begin<postorder_t>(); it != tree.end<postorder_t>();
         ++it)
      result.push_back(*it);
    return result;
  }

  auto reverse_inorder_vec() -> std::vector<int> {
    std::vector<int> result;
    for (auto it = tree.rbegin(); it != tree.rend(); ++it)
      result.push_back(*it);
    return result;
  }

  auto is_sorted_ascending(const std::vector<int> &v) -> bool {
    return std::is_sorted(v.begin(), v.end());
  }

  auto is_sorted_descending(const std::vector<int> &v) -> bool {
    return std::is_sorted(v.begin(), v.end(), std::greater<int>{});
  }

  auto contains_all(const std::vector<int> &v,
                    std::initializer_list<int> expected) -> bool {
    for (int val : expected) {
      if (std::find(v.begin(), v.end(), val) == v.end())
        return false;
    }
    return true;
  }
};

// ============================================================================
// Group 1: Insert and Remove
// ============================================================================

TEST_F(TreeSetTest, InsertIntoEmptyTree) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_EQ(tree.size(), 1u);
  EXPECT_TRUE(tree.is_element(5));
}

TEST_F(TreeSetTest, InsertDuplicateReturnsFalse) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_FALSE(tree.insert(5));
  EXPECT_EQ(tree.size(), 1u);
}

TEST_F(TreeSetTest, InsertMultiplePreservesSize) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  EXPECT_EQ(tree.size(), 3u);
}

TEST_F(TreeSetTest, RemoveExistingReturnsTrue) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  ASSERT_TRUE(tree.remove(3));
  EXPECT_EQ(tree.size(), 2u);
  EXPECT_FALSE(tree.is_element(3));
}

TEST_F(TreeSetTest, RemoveNonexistentReturnsFalse) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_FALSE(tree.remove(99));
  EXPECT_EQ(tree.size(), 1u);
}

TEST_F(TreeSetTest, RemoveFromEmptyTreeReturnsFalse) {
  EXPECT_FALSE(tree.remove(5));
}

TEST_F(TreeSetTest, RemoveRoot) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  ASSERT_TRUE(tree.remove(5));
  EXPECT_EQ(tree.size(), 2u);
  EXPECT_FALSE(tree.is_element(5));
  EXPECT_TRUE(is_sorted_ascending(inorder_vec()));
}

TEST_F(TreeSetTest, InsertAndRemoveMultiple) {
  for (int i = 1; i <= 10; ++i)
    ASSERT_TRUE(tree.insert(i));
  EXPECT_EQ(tree.size(), 10u);

  ASSERT_TRUE(tree.remove(2));
  ASSERT_TRUE(tree.remove(5));
  ASSERT_TRUE(tree.remove(8));

  EXPECT_EQ(tree.size(), 7u);
  EXPECT_FALSE(tree.is_element(2));
  EXPECT_FALSE(tree.is_element(5));
  EXPECT_FALSE(tree.is_element(8));

  auto in = inorder_vec();
  EXPECT_EQ(in.size(), 7u);
  EXPECT_TRUE(is_sorted_ascending(in));
  EXPECT_THAT(in, ElementsAre(1, 3, 4, 6, 7, 9, 10));
}

// ============================================================================
// Group 2: RBT Properties (observable via rotations)
// ============================================================================

TEST_F(TreeSetTest, InsertLeftRotation) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  ASSERT_TRUE(tree.insert(3));
  EXPECT_EQ(tree.root()->value, 2);
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 2, 3));
}

TEST_F(TreeSetTest, InsertRightRotation) {
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(2));
  ASSERT_TRUE(tree.insert(1));
  EXPECT_EQ(tree.root()->value, 2);
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 2, 3));
}

TEST_F(TreeSetTest, InsertLeftRightRotation) {
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  EXPECT_EQ(tree.root()->value, 2);
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 2, 3));
}

TEST_F(TreeSetTest, InsertRightLeftRotation) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(2));
  EXPECT_EQ(tree.root()->value, 2);
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 2, 3));
}

TEST_F(TreeSetTest, InsertRecoloringSequence) {
  ASSERT_TRUE(tree.insert(10));
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(15));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  ASSERT_TRUE(tree.insert(12));
  ASSERT_TRUE(tree.insert(20));
  ASSERT_TRUE(tree.insert(1));
  EXPECT_EQ(tree.size(), 8u);
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 3, 5, 7, 10, 12, 15, 20));
}

TEST_F(TreeSetTest, RootIsAlwaysBlack) {
  for (int i = 1; i <= 7; ++i)
    ASSERT_TRUE(tree.insert(i));
  EXPECT_EQ(tree.root()->color, Color::Black);
}

TEST_F(TreeSetTest, RemoveMaintainsSortedOrder) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(4));
  ASSERT_TRUE(tree.insert(6));
  ASSERT_TRUE(tree.insert(8));

  ASSERT_TRUE(tree.remove(3));
  ASSERT_TRUE(tree.remove(5));
  ASSERT_TRUE(tree.remove(7));

  auto in = inorder_vec();
  EXPECT_TRUE(is_sorted_ascending(in));
  EXPECT_THAT(in, ElementsAre(1, 4, 6, 8));
}

// ============================================================================
// Group 3: In-order Traversal
// ============================================================================

TEST_F(TreeSetTest, InorderEmptyTree) {
  EXPECT_TRUE(tree.begin() == tree.end());
  EXPECT_TRUE(inorder_vec().empty());
}

TEST_F(TreeSetTest, InorderSingleNode) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_THAT(inorder_vec(), ElementsAre(5));
}

TEST_F(TreeSetTest, InorderBalancedTree) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 3, 4, 5, 6, 7, 8));
}

TEST_F(TreeSetTest, InorderAfterRemovals) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  ASSERT_TRUE(tree.remove(4));
  ASSERT_TRUE(tree.remove(7));
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 3, 5, 6, 8));
}

// ============================================================================
// Group 4: Pre-order Traversal
// ============================================================================

TEST_F(TreeSetTest, PreorderEmptyTree) {
  EXPECT_TRUE(tree.begin<preorder_t>() == tree.end<preorder_t>());
  EXPECT_TRUE(preorder_vec().empty());
}

TEST_F(TreeSetTest, PreorderSingleNode) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_THAT(preorder_vec(), ElementsAre(5));
}

TEST_F(TreeSetTest, PreorderBalancedTree) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));

  auto pre = preorder_vec();
  auto in = inorder_vec();

  EXPECT_EQ(pre.size(), 7u);
  EXPECT_THAT(in, ElementsAre(1, 3, 4, 5, 6, 7, 8));
  EXPECT_TRUE(contains_all(pre, {1, 3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(pre.front(), tree.root()->value);
}

TEST_F(TreeSetTest, PreorderAfterRemovals) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  ASSERT_TRUE(tree.remove(4));
  ASSERT_TRUE(tree.remove(7));

  auto pre = preorder_vec();
  EXPECT_EQ(pre.size(), 5u);
  EXPECT_TRUE(contains_all(pre, {1, 3, 5, 6, 8}));
  EXPECT_EQ(pre.front(), tree.root()->value);
}

// ============================================================================
// Group 5: Post-order Traversal
// ============================================================================

TEST_F(TreeSetTest, PostorderEmptyTree) {
  EXPECT_TRUE(tree.begin<postorder_t>() == tree.end<postorder_t>());
  EXPECT_TRUE(postorder_vec().empty());
}

TEST_F(TreeSetTest, PostorderSingleNode) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_THAT(postorder_vec(), ElementsAre(5));
}

TEST_F(TreeSetTest, PostorderBalancedTree) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));

  auto post = postorder_vec();
  auto in = inorder_vec();

  EXPECT_EQ(post.size(), 7u);
  EXPECT_THAT(in, ElementsAre(1, 3, 4, 5, 6, 7, 8));
  EXPECT_TRUE(contains_all(post, {1, 3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(post.back(), tree.root()->value);
}

TEST_F(TreeSetTest, PostorderAfterRemovals) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  ASSERT_TRUE(tree.remove(4));
  ASSERT_TRUE(tree.remove(7));

  auto post = postorder_vec();
  EXPECT_EQ(post.size(), 5u);
  EXPECT_TRUE(contains_all(post, {1, 3, 5, 6, 8}));
  EXPECT_EQ(post.back(), tree.root()->value);
}

// ============================================================================
// Group 6: Iterator Movement (in-order forward)
// ============================================================================

TEST_F(TreeSetTest, BeginPointsToSmallest) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  EXPECT_EQ(*tree.begin(), 3);
}

TEST_F(TreeSetTest, ForwardTraversalCollectsSortedOrder) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  EXPECT_THAT(inorder_vec(), ElementsAre(1, 3, 4, 5, 6, 7, 8));
}

TEST_F(TreeSetTest, PostIncrementReturnsOldValue) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  auto it = tree.begin();
  auto old = it++;
  EXPECT_EQ(*old, 1);
  EXPECT_EQ(*it, 2);
}

TEST_F(TreeSetTest, PreIncrementReturnsNewValue) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  auto it = tree.begin();
  auto &ref = ++it;
  EXPECT_EQ(*ref, 2);
  EXPECT_EQ(*it, 2);
}

TEST_F(TreeSetTest, IncrementPastLastReachesEnd) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  ASSERT_TRUE(tree.insert(3));
  auto it = tree.begin();
  ++it;
  ++it;
  ++it;
  EXPECT_TRUE(it == tree.end());
}

TEST_F(TreeSetTest, IteratorDistanceEqualsSize) {
  for (int v : {5, 3, 7, 1, 4})
    ASSERT_TRUE(tree.insert(v));
  EXPECT_EQ(std::distance(tree.begin(), tree.end()), 5);
}

TEST_F(TreeSetTest, RangeBasedForLoop) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  std::vector<int> collected;
  for (const auto &v : tree)
    collected.push_back(v);
  EXPECT_THAT(collected, ElementsAre(1, 3, 4, 5, 6, 7, 8));
}

// ============================================================================
// Group 7: Reverse Iterator Movement (in-order backward)
// ============================================================================

TEST_F(TreeSetTest, RbeginPointsToLargest) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));
  EXPECT_EQ(*tree.rbegin(), 7);
}

TEST_F(TreeSetTest, ReverseTraversalCollectsReverseSorted) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));
  EXPECT_THAT(reverse_inorder_vec(), ElementsAre(8, 7, 6, 5, 4, 3, 1));
}

TEST_F(TreeSetTest, PostIncrementOnReverseReturnsOldValue) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  ASSERT_TRUE(tree.insert(3));
  auto it = tree.rbegin();
  auto old = it++;
  EXPECT_EQ(*old, 3);
  EXPECT_EQ(*it, 2);
}

TEST_F(TreeSetTest, PreIncrementOnReverseReturnsNewValue) {
  ASSERT_TRUE(tree.insert(1));
  ASSERT_TRUE(tree.insert(2));
  ASSERT_TRUE(tree.insert(3));
  auto it = tree.rbegin();
  auto &ref = ++it;
  EXPECT_EQ(*ref, 2);
  EXPECT_EQ(*it, 2);
}

TEST_F(TreeSetTest, PostDecrementReturnsOldValue) {
  for (int v : {1, 2, 3})
    ASSERT_TRUE(tree.insert(v));
  auto it = tree.end();
  --it;
  auto old = it--;
  EXPECT_EQ(*old, 3);
  EXPECT_EQ(*it, 2);
}

TEST_F(TreeSetTest, PreDecrementReturnsNewValue) {
  for (int v : {1, 2, 3})
    ASSERT_TRUE(tree.insert(v));
  auto it = tree.end();
  --it;
  auto &ref = --it;
  EXPECT_EQ(*ref, 2);
  EXPECT_EQ(*it, 2);
}

// ============================================================================
// Group 8: Const Iterator
// ============================================================================

TEST_F(TreeSetTest, CBeginCEndSameAsBeginEnd) {
  for (int v : {5, 3, 7})
    ASSERT_TRUE(tree.insert(v));

  std::vector<int> from_begin, from_cbegin;
  for (auto it = tree.begin(); it != tree.end(); ++it)
    from_begin.push_back(*it);
  for (auto it = tree.cbegin(); it != tree.cend(); ++it)
    from_cbegin.push_back(*it);

  EXPECT_EQ(from_begin, from_cbegin);
}

TEST_F(TreeSetTest, CRbeginCREndSameAsRbeginRend) {
  for (int v : {5, 3, 7})
    ASSERT_TRUE(tree.insert(v));

  std::vector<int> from_rbegin, from_crbegin;
  for (auto it = tree.rbegin(); it != tree.rend(); ++it)
    from_rbegin.push_back(*it);
  for (auto it = tree.crbegin(); it != tree.crend(); ++it)
    from_crbegin.push_back(*it);

  EXPECT_EQ(from_rbegin, from_crbegin);
}

// ============================================================================
// Group 9: Tag-dispatched Iterators
// ============================================================================

TEST_F(TreeSetTest, InorderTagGivesSameAsDefault) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));

  std::vector<int> default_order, tag_order;
  for (auto it = tree.begin(); it != tree.end(); ++it)
    default_order.push_back(*it);
  for (auto it = tree.begin<inorder_t>(); it != tree.end<inorder_t>(); ++it)
    tag_order.push_back(*it);

  EXPECT_EQ(default_order, tag_order);
}

TEST_F(TreeSetTest, PreorderTagTraversal) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));

  auto pre = preorder_vec();
  auto in = inorder_vec();

  EXPECT_EQ(pre.size(), 7u);
  EXPECT_THAT(in, ElementsAre(1, 3, 4, 5, 6, 7, 8));
  EXPECT_TRUE(contains_all(pre, {1, 3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(pre.front(), tree.root()->value);
}

TEST_F(TreeSetTest, PostorderTagTraversal) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));

  auto post = postorder_vec();
  auto in = inorder_vec();

  EXPECT_EQ(post.size(), 7u);
  EXPECT_THAT(in, ElementsAre(1, 3, 4, 5, 6, 7, 8));
  EXPECT_TRUE(contains_all(post, {1, 3, 4, 5, 6, 7, 8}));
  EXPECT_EQ(post.back(), tree.root()->value);
}

TEST_F(TreeSetTest, TagRbeginRend) {
  for (int v : {5, 3, 7, 1, 4, 6, 8})
    ASSERT_TRUE(tree.insert(v));

  std::vector<int> rev_pre, rev_post;
  for (auto it = tree.rbegin<preorder_t>(); it != tree.rend<preorder_t>(); ++it)
    rev_pre.push_back(*it);
  for (auto it = tree.rbegin<postorder_t>(); it != tree.rend<postorder_t>();
       ++it)
    rev_post.push_back(*it);

  auto pre = preorder_vec();
  auto post = postorder_vec();

  EXPECT_EQ(rev_pre.size(), 7u);
  EXPECT_EQ(rev_post.size(), 7u);
  EXPECT_TRUE(contains_all(rev_pre, {1, 3, 4, 5, 6, 7, 8}));
  EXPECT_TRUE(contains_all(rev_post, {1, 3, 4, 5, 6, 7, 8}));

  std::vector<int> pre_reversed(pre.rbegin(), pre.rend());
  std::vector<int> post_reversed(post.rbegin(), post.rend());
  EXPECT_EQ(rev_pre, pre_reversed);
  EXPECT_EQ(rev_post, post_reversed);
}

// ============================================================================
// Group 10: Edge Cases
// ============================================================================

TEST_F(TreeSetTest, EmptyTreeBeginEqualsEnd) {
  EXPECT_TRUE(tree.begin() == tree.end());
  EXPECT_TRUE(tree.rbegin() == tree.rend());
  EXPECT_TRUE(tree.cbegin() == tree.cend());
  EXPECT_TRUE(tree.crbegin() == tree.crend());
  EXPECT_TRUE(tree.begin<inorder_t>() == tree.end<inorder_t>());
  EXPECT_TRUE(tree.begin<preorder_t>() == tree.end<preorder_t>());
  EXPECT_TRUE(tree.begin<postorder_t>() == tree.end<postorder_t>());
}

TEST_F(TreeSetTest, SingleElementBeginRbeginSame) {
  ASSERT_TRUE(tree.insert(5));
  EXPECT_EQ(*tree.begin(), 5);
  EXPECT_EQ(*tree.rbegin(), 5);
}

TEST_F(TreeSetTest, LargeTreeTraversal) {
  for (int i = 1; i <= 100; ++i)
    ASSERT_TRUE(tree.insert(i));
  EXPECT_EQ(tree.size(), 100u);

  auto in = inorder_vec();
  EXPECT_EQ(in.size(), 100u);
  EXPECT_TRUE(is_sorted_ascending(in));
  EXPECT_EQ(in.front(), 1);
  EXPECT_EQ(in.back(), 100);
}

TEST_F(TreeSetTest, HeightAfterInsertions) {
  ASSERT_TRUE(tree.insert(1));
  EXPECT_EQ(tree.height(), 1u);

  ASSERT_TRUE(tree.insert(2));
  ASSERT_TRUE(tree.insert(3));
  EXPECT_EQ(tree.height(), 2u);
}

TEST_F(TreeSetTest, RemoveAllElements) {
  ASSERT_TRUE(tree.insert(5));
  ASSERT_TRUE(tree.insert(3));
  ASSERT_TRUE(tree.insert(7));

  ASSERT_TRUE(tree.remove(5));
  ASSERT_TRUE(tree.remove(3));
  ASSERT_TRUE(tree.remove(7));

  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0u);
  EXPECT_TRUE(tree.begin() == tree.end());
}
