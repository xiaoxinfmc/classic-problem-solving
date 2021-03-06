#include <map>
#include <list>
#include <deque>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace array_util {
  template <class type>
  static void print_all_elem(const vector<type> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }

  template <class t1, class t2>
  static void print_all_pairs(const vector<pair<t1, t2>> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr.first << "|" << arr.second << " "; }
    cout << "]" << endl;
  }

  template <class type>
  static void print_all_elem_vec(const vector<vector<type>> & input) {
    cout << "[" << endl;
    for (auto & arr : input){ cout << "  "; print_all_elem<type>(arr); }
    cout << "]" << endl;
  }

  /**
   * 2.1.15 Trapping Rain Water
   * Given n non-negative integers representing an elevation map where the
   * width of each bar is 1, compute how much water it is able to trap after
   * raining. For example, Given [0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1], return 6.
   *                |
   *        |       | |   |
   *    |   | |   | | | | | |
   * ------------------------------------
   *  0 1 0 2 1 0 1 3 2 1 2 1
   *  0 1 2 3 4 5 6 7 8 9 10 11
   * Analysis:
   * - use a stack to keep track of walls, water is only trapped between walls
   *   and dominated by the short side.
   * - 4 0 0 0 5 0 0 0 7
   * - 4 0 0 0 3 0 0 0 7
   * - a wall can be ditched once it is paired with another no shorter piece with a gap.
   * - only add up amount of water when we see 2 walls appear between gaps.
   * - stack: [ 0 ] [ 0 1 ] -> [ 1 ] [ 1 0 ] [ 1 0 2 ] -> [ 2 ] [ 2 1 ] [ 2 1 0 ] [ 2 1 0 1 ] -> [ 2 ] [ 2 3 ] [ 3 ]
   *            |     |-|                      |---|  loop  ^     ^       ^         ^ |---|              |-|
   * - idx:   [ 0 ] [ 0 1 ] -> [ 1 ] [ 1 2 ] [ 1 2 3 ]    [ 3 ] [ 3 4 ] [ 3 4 5 ] [ 3 4 5 6 ]    [ 3 ] [ 3 7 ] [ 7 ]
   * - delta:                                  1+1+2        0(rm all wall)              3          3      3      0
   * - water:          1         1               2          2      2        2           3          3      6      6
   *
   * - stack: [ 3 ] [ 3 2 1 2 ] -> [ 3 ] [ 3 1 ]
   *                    |---|
   * - idx:   [ 7 ] [ 7 8 9 10]    [ 7 ] [ 7 11]
   * - delta:   0         6          6      6 > (11 - 7 - 1) * 1 => 3 * 1 -> 3
   * - water:   6         7          7      7
   * - [ 4 3 2 1 0 3 7 ] gap => 1
   *     0 1 2 3 4 5 6
   *   [ 4 3 2 1   3 ]
   *             1 (delta -> 2)
   *   [ 4 3 2     3 ]
   *         2 * 2 - 2->2 (delta 2+2+2 -> 6)
   *   [ 4 3       3 ]
   *       3 * 3 - 6 -> 3 (delta 6+3+3 -> 12)
   *       |-------|
   *   [ 4 3 ] 7
   *     0 5
   *   [ 4 3 7 ]
   *     0 5 6
   *     delta -> 12 +3 -> 15

   *   [ 4 3 ]      (30)          4
   *   [ 
   * - [ 4 3 2 1 3 ] gap => 0
   *       |-----|
   * - [ 4         7 ]
   *     |---------|
   */
  int calc_max_trapping_water(vector<int> elevations) {
    int total_water = 0, curr_water = 0, delta = 0, wall_gap = 0;
    vector<pair<int, int>> elevation_stack;
    for (int i = 0; i < elevations.size(); i++) {
      if (0 == elevations[i]) { continue; }
      while ((false == elevation_stack.empty()) &&
             (elevation_stack.back().first <= elevations[i] ||
              i - elevation_stack.back().second - 1 > 0)) {
        wall_gap = i - elevation_stack.back().second - 1;
        curr_water = (
          min(elevation_stack.back().first, elevations[i]) * wall_gap - delta
        );
        if (curr_water < 0) {
          delta += elevation_stack.back().first;
          elevation_stack.pop_back(); continue;
        }
        if (elevation_stack.back().first > elevations[i]) { break; }
        delta += (elevation_stack.back().first + curr_water);
        total_water += curr_water;
        elevation_stack.pop_back();
      }
      if (true == elevation_stack.empty()) { delta = 0; }
      elevation_stack.push_back(pair<int, int>(elevations[i], i));
    }
    return total_water;
  }

  /**     v                              v |
   * ==>> 1 2 7  9 11 15             x x x x x x x (x will come before s -> skip xxx)
   * ==>> 3 4 8 10 14 17 19          s s s s s s s (x will cannot be median if x < s)
   *                                     ^ |
   * - 1st -> all 5 from set x, => x x x x x s x s x s s s s s -> x[k - 1] < s[0]
   *                                       v
   * - 2nd -> all 5 from set s, => s s s s s x s x s x x x x x -> s[k - 1] < x[0]
   *                                   | | v                       5/2-1->1    4/2-1->1
   * - 3rd -> some from set sx, => s x s x s x s s s x x x s x -> x[k / 2 - 1] > s[k / 2 - 1]
   *   x[k / 2-1] > s[k / 2-1], k/2-1 elems from s will be ahead of kth elem, vice versa.
   *                            (s0 s1)
   *   so it becomes x[0..k - 1] && s[k / 2..k - 1] else
   *                 x[k / 2..k - 1] && s[0..k - 1] & needs to merge k/2 + 1 elems
   * - always assume left-arr has more elems left.
   */
  static int find_kth_elem(vector<int>::const_iterator l_arr_itr, int l_elem_left,
                           vector<int>::const_iterator r_arr_itr, int r_elem_left,
                           int elems_to_merge) {
    if (r_elem_left > l_elem_left) {
      return find_kth_elem(r_arr_itr, r_elem_left, l_arr_itr,
                           l_elem_left, elems_to_merge);
    }
    if (0 >= l_elem_left) { return INT_MIN; }
    if (0 == r_elem_left) { return * (l_arr_itr + elems_to_merge - 1); }
    if (1 == elems_to_merge) { return min(* l_arr_itr, * r_arr_itr); }

    int elems_to_skip = min(elems_to_merge / 2, r_elem_left);
    if (* (l_arr_itr + elems_to_skip - 1) > * (r_arr_itr + elems_to_skip - 1)) {
      return find_kth_elem(
        l_arr_itr, l_elem_left, r_arr_itr + elems_to_skip,
        r_elem_left - elems_to_skip, elems_to_merge - elems_to_skip
      );
    } else {
      return find_kth_elem(
        l_arr_itr + elems_to_skip, l_elem_left - elems_to_skip,
        r_arr_itr, r_elem_left, elems_to_merge - elems_to_skip
      );
    }
  }

  /**
   * 2.1.5 Median of Two Sorted Arrays
   * There are two sorted arrays A and B of size m and n respectively. Find
   * the median of the two sorted arrays. The overall run time complexity
   * should be O(log(m + n)).           v
   * [ 1, 2, 7, 8, 9 ] => [ 1, 2, 3, 4, 5, 6, 7, 8, 9 ] median is 5 (id => 4)
   * [ 3, 4, 5, 6 ]       5 # in total, first half [ 1, 2 ], from A or B
   * A => [ 1, 2 ], B => [ 3, 4 ] => B covers more as B [1] > A[1]
   * All half of A will be included before B[1] merged => not including 5.
   * => A[k / 2 - 1] == B[k / 2 - 1] => either one is good
   *    A[k / 2 - 1] <  B[k / 2 - 1] => A[start_pos ... k / 2 - 1] => can skip
   *    A[k / 2 - 1] >  B[k / 2 - 1] => B[start_pos ... k / 2 - 1] => can skip
   * ==>> x x x x x x x total: 7, if we need 5th elem from merged sets, then
   * ==>> s s s s s s s total: 7  5 elem needs to be merged before we can pick
   * - upper searching scope is k elems from both set s & set x, then
   *                                       v
   * - 1st -> all 5 from set x, => x x x x x s x s x s s s s s -> x[k - 1] < s[0]
   *                                       v
   * - 2nd -> all 5 from set s, => s s s s s x s x s x x x x x -> s[k - 1] < x[0]
   *                                   | | v                       5/2-1->1    4/2-1->1
   * - 3rd -> some from set sx, => s x s x s x s s s x x x s x -> x[k / 2 - 1] > s[k / 2 - 1]
   *   x[k / 2-1] > s[k / 2-1], k/2-1 elems from s will be ahead of kth elem, vice versa.
   *                            (s0 s1)
   *   so it becomes x[0..k - 1] && s[k / 2..k - 1] else
   *                 x[k / 2..k - 1] && s[0..k - 1]
   */
  static double find_median_elem(vector<int> l_sorted_arr,
                                 vector<int> r_sorted_arr) {
    if (0 == ((l_sorted_arr.size() + r_sorted_arr.size()) % 2)) {
      return (find_kth_elem(l_sorted_arr.begin(), l_sorted_arr.size(),
                            r_sorted_arr.begin(), r_sorted_arr.size(),
                            (l_sorted_arr.size() + r_sorted_arr.size()) / 2) +
              find_kth_elem(l_sorted_arr.begin(), l_sorted_arr.size(),
                            r_sorted_arr.begin(), r_sorted_arr.size(),
                            (l_sorted_arr.size() + r_sorted_arr.size()) / 2 + 1)
      ) / 2.0;
    } else {
      return find_kth_elem(l_sorted_arr.begin(), l_sorted_arr.size(),
                           r_sorted_arr.begin(), r_sorted_arr.size(),
                           (l_sorted_arr.size() + r_sorted_arr.size()) / 2 + 1);
    }
  }

  /**
   * 2.1.6 Longest Consecutive Sequence
   * Given an unsorted array of integers, find the length of the longest
   * consecutive elements sequence.
   * For example, Given [100, 4, 200, 3, 1, 2], The longest consecutive
   * elements sequence is [1, 2, 3, 4]. Return its length: 4.
   * Your algorithm should run in O(n) complexity.
   * - consecutive elements means each elem differs by just 1, longest
   *   sequence will have a min value and a max.
   * - brutal force would be sort than scan => nlogn
   * - use a hashtable, for every entry we keep expanding & logout cnt.
   * - in the meantime, extract the elem from the map(IMPORTANT)
   */
  static int calc_longest_seq_len(vector<int> input) {
    unordered_map<int, int> seq_buf;
    int max_seq_len = 1, start_val = 0;
    for(auto & value : input) { seq_buf[value] = 1; }
    while(false == seq_buf.empty()) {
      start_val = seq_buf.begin()->first;
      for (int i = start_val + 1; seq_buf.end() != seq_buf.find(i); i++) {
        seq_buf[start_val] += seq_buf[i]; seq_buf.erase(i);
      }
      for (int i = start_val - 1; seq_buf.end() != seq_buf.find(i); i--) {
        seq_buf[start_val] += seq_buf[i]; seq_buf.erase(i);
      }
      max_seq_len = max(max_seq_len, seq_buf[start_val]);
      seq_buf.erase(start_val);
    }
    return max_seq_len;
  }

  /**
   * 2.1.3 Search in Rotated Sorted Array
   * Suppose a sorted array is rotated at some pivot unknown to you beforehand.
   * (i.e., 0 1 2 4 5 6 7 might become 4 5 6 7 0 1 2).
   * You are given a target value to search. If found in the array return its
   * index, otherwise return -1. You may assume no duplicate exists in the array.
   * Analysis:
   * - 5 6 7 8 0 1 2
   * - 5 6 7 8 0 1 2 7 => x if right elem > left elem, whole arr has to be no rotation
   * - 4 5 6 7 0 1 2 => 1, either > > _ < < < < or > _ < < < < or < < < _
   *   |     ^     |
   * - arr[min_idx] < arr[target_idx] && arr[target_idx] > arr[max_idx]
   *   search 1, which is smaller than arr[min_idx] && arr[target_idx]
   *   then we search for arr[target_idx + 1] <=> arr[max_idx]
   */
  static int find_in_rotated_sorted_arr(const vector<int> input, int target) {
    int target_idx = 0, target_ret = -1;
    for (int min_idx = 0, max_idx = input.size() - 1; min_idx <= max_idx;) {
      target_idx = (min_idx + max_idx) / 2;
      if (input[target_idx] == target) { target_ret = target_idx; break; }
      if (input[max_idx] >= input[min_idx]) {
        if (target < input[target_idx]) {
          max_idx = target_idx - 1;
        } else {
          min_idx = target_idx + 1;
        }
      } else {
        if ((target < input[target_idx] && target <= input[min_idx]) ||
            (target > input[target_idx] && target >= input[max_idx])) {
          min_idx = target_idx + 1;
        } else {
          max_idx = target_idx - 1;
        }
      }
    }
    return target_ret;
  }

  /**
   * 2.1.12 Next Permutation
   * Implement next permutation, which rearranges numbers into lexicographically
   * next greater permutation of numbers.
   * If such arrangement is not possible, it must rearrange it as the lowest
   * possible order (ie, sorted in ascend-ing order).
   * The replacement must be in-place, do not allocate extra memory.
   * Here are some examples. Inputs are in the left-hand column and its
   * corresponding outputs are in the right-hand column.
   * 1,2,3 -> 1,3,2
   * 3,2,1 -> 1,2,3
   * 1,1,5 -> 1,5,1
   * Analysis:
   * - assume each number are digits, -> 0 - 9
   * - 6 8 7 4 3 2 -> 7 2 3 4 6 8
   *    > < < < <
   * - 6 8 7 4 3 2 9 0 ->
   *    > < < < < < >
   *   6 8 7 4 3 9 2 0
   *
   * - 6 8 7 3 4 2 9 0 ->
   *   6 8 7 3 4 9 2 0
   * - to get the next ascending permutation, we need to find the changes on
   *   least amount of delta (right side portion with larger value, lsb).
   * - it really has nothing to do with the digit value, but for how small the
   *   change it is, the smaller right side portion involved, the better.
   *   xxxxxxxx d0 d1 d2 (d1 < d2) => xxxxxxxx d0 d2 d1
   *   xxxxxxxx d0 d1 d2 (d1 > d2, d0 < d1) => xxxxxxxx d1 d0 d2 && d1 > d0, d0 < d2
   *
   * - 6 8 1 3 7 1 4 0 ->
   *   6 8 1 3 7 4 1 0
   *   6 8 1 3 7 4 0 1
   *
   * - 6 8 1 3 7 5 8 6 1 0 ->
   *             ^   ^
   *   6 8 1 3 7 6 8 5 1 0
   *   6 8 1 3 7 6 8 0 1 5
   *              lsb
   * - 6 8 1 3 7 3 4 2 1 0 ->
   *              < > > >
   *   6 8 1 3 7 4 3 2 1 0 -> 1st step should find the msb digit to swap, -> right most digit to swap for larger value => digits[i] > digits[i - 1]
   *   6 8 1 3 7 4 0 1 2 3 -> last step to invert the whole remaining digits on far right.
   */
  static vector<int> get_next_permutation_asc(const vector<int> digits) {
    vector<int> next_perm(digits.begin(), digits.end());
    int lsb_digit_id = digits.size() - 1, tmp_value = 0;
    for (; lsb_digit_id > 0; lsb_digit_id--) {
      if (next_perm[lsb_digit_id] > next_perm[lsb_digit_id - 1]) {
        break;
      }
    }
    if (lsb_digit_id != 0) {
      int digit_id_to_swap = lsb_digit_id + 1;
      for (; digit_id_to_swap < digits.size(); digit_id_to_swap++) {
        if (next_perm[lsb_digit_id - 1] > next_perm[digit_id_to_swap]) { break; }
      }
      tmp_value = next_perm[lsb_digit_id - 1];
      next_perm[lsb_digit_id - 1] = next_perm[digit_id_to_swap - 1];
      next_perm[digit_id_to_swap - 1] = tmp_value;
    }
    /** - 6 8 1 3 7 3 4(lsb) 2 1 0 -> 6 8 1 3 7 4 3(lsb) 2 1 0
     *                     4 -> 2, 3 -> 1, 1 -> 0 i          size - 1 */
    int swap_digits_cnt = next_perm.size() - lsb_digit_id;
    int swap_operat_cnt = swap_digits_cnt / 2;
    for (int i = 0; i < swap_operat_cnt; i++) { // swap-cnt (-> 3, 0 <-> 2), (-> 4, 0 <-> 3)
      tmp_value = next_perm[lsb_digit_id + i];
      next_perm[lsb_digit_id + i] = next_perm[lsb_digit_id + swap_digits_cnt - i - 1];
      next_perm[lsb_digit_id + swap_digits_cnt - i - 1] = tmp_value;
    }
    return next_perm;
  }

  /**
   * 2.1.7 Two Sum
   * Given an array of integers, find two numbers such that they add up to a
   * specific target number.
   * The function twoSum should return indices of the two numbers such that
   * they add up to the target, where index1 must be less than index2. Please
   * note that your returned answers (both index1 and index2) are not zero-based.
   * You may assume that each input would have exactly one solution.
   * Input: numbers={2, 7, 11, 15}, target=9
   * Output: index1=1, index2=2
   * Observation:
   * - use a map to store the value of a number & its corresponding id (start_from 1)
   */
  vector<int> calc_two_sum(vector<int> & input, int target) {
    vector<int> pair = { -1, -1 };
    unordered_map<int, int> value_to_idx_map;
    for (int i = 0; i < input.size(); i++) { value_to_idx_map[input[i]] = i; }
    int remaining_part = 0;
    for (int i = 0; i < input.size(); i++) {
      remaining_part = target - input[i];
      if (value_to_idx_map.end() == value_to_idx_map.find(remaining_part)) { continue; }
      if (i == value_to_idx_map[remaining_part]) { continue; }
      pair = { i + 1, value_to_idx_map[remaining_part] + 1 };
      break;
    }
    return pair;
  }

  static void test_calc_two_sum() {
    vector<vector<int>> test_output = { { 2, 3 }, { 1, 2 } };
    vector<int> test_input_1 = { 6, 9 };
    vector<vector<int>> test_input_0 = { { 3, 2, 4 }, { 2, 7, 11, 15 } };
    cout << "5. calc_two_sum" << endl;
    for (int i = 0; i < test_output.size(); i++) {
      print_all_elem<int>(test_output[i]);
      cout << "<=>" << endl;
      print_all_elem<int>(calc_two_sum(test_input_0[i], test_input_1[i]));
    }
  }

  /**
   * 167. Two Sum II - Input array is sorted
   * Input: numbers = [2,7,11,15], target = 9
   * Output: [1,2]
   * Explanation: The sum of 2 and 7 is 9. Therefore index1 = 1, index2 = 2.
   */
  static vector<int> calc_two_sum_ii(vector<int> & input, int target) {
    vector<int> pair = { -1, -1 };
    if (input.size() < 2) { return pair; }
    int curr_sum = 0;
    for (int i = 0, j = input.size() - 1; i < j;) {
      curr_sum = input[i] + input[j];
      if (curr_sum == target) { pair = { i + 1, j + 1 }; break; }
      if (curr_sum < target) { i++; continue; }
      if (curr_sum > target) { j--; continue; }
    }
    return pair;
  }

  static void test_calc_two_sum_ii() {
    vector<vector<int>> test_output = { { 1, 3 }, { 1, 2 } };
    vector<int> test_input_1 = { 6, 9 };
    vector<vector<int>> test_input_0 = { { 2, 3, 4 }, { 2, 7, 11, 15 } };
    cout << "5. calc_two_sum_ii" << endl;
    for (int i = 0; i < test_output.size(); i++) {
      print_all_elem<int>(test_output[i]);
      cout << "<=>" << endl;
      print_all_elem<int>(calc_two_sum_ii(test_input_0[i], test_input_1[i]));
    }
  }

  /**
   * 653. Two Sum IV - Input is a BST
   * - Given a Binary Search Tree and a target number, return true if there
   *   exist two elements in BST such that their sum is equal to given target.
   * Example 1:
   *       Input:
   *           5
   *          / \
   *         3   6
   *        / \   \
   *       2   4   7
   * Target = 9 -> True
  typedef TreeNode binary_tree_node;
  static void get_all_values(binary_tree_node * root, vector<int> & num_buf) {
    if (NULL == root) { return; }
    get_all_values(root->left, num_buf);
    num_buf.push_back(root->value);
    get_all_values(root->right, num_buf);
  }
  static bool is_two_sum_existed(binary_tree_node * root, int target) {
    vector<int> num_buf;
    get_all_values(root, num_buf);
    for (int i = 0, j = num_buf.size() - 1; i < j; ) {
      int curr_sum = num_buf[i] + num_buf[j];
      if (target == curr_sum) { return true; }
      if (curr_sum > target) { j--; continue; }
      if (curr_sum < target) { i++; continue; }
    }
    return false;
  }
   */

  /**
   * 2.1.8 3Sum
   * Given an array S of n integers, are there elements a,b,c in S such that
   * a + b + c = 0? Find all unique triplets in array which gives sum of zero.
   * Note:
   * Elements in a triplet (a, b, c) must be ascending order. (ie, a ≤ b ≤ c)
   * The solution set must not contain duplicate triplets.
   * For example, given array S = {-1 0 1 2 -1 -4}.
   * A solution set is: (-1, 0, 1) (-1, -1, 2)
   * - Reduce to 2 sum, as target == elem + sum(elem pair)
   * - use a map to store all possible sum of 2 elem, with sorted values & idx as val.
   * - Or sort the input in place 1st, then use 3 pointers to iterate all comb.
   * - -1 0 1 2 -1 -4
   *    v  *-->  <--*
   * - -4 -1 -1 0 1 2
   * - each time we pick a different number as start point (1st val), then pick
   *   then rest 2 values by moving pointer around from both ends.
   */
  vector<vector<int>> calc_triplets_by_sum(vector<int> input, int target = 0) {
    vector<vector<int>> triplets;
    sort(input.begin(), input.end());
    int curr_sum = 0;
    for (int i = 0; i < input.size(); i++) {
      for (int j = i + 1, k = input.size() - 1; j < k; ) {
        curr_sum = input[i] + input[j] + input[k];
        if (curr_sum == target) {
          triplets.push_back({ input[i], input[j], input[k] });
          while (j + 1 < input.size() && input[j] == input[j + 1]) { j++; }
          while (k - 1 >= 0 && input[k] == input[k - 1]) { k--; }
          j++; k--; continue;
        }
        if (curr_sum < target) { j++; } else { k--; }
      }
      while (i + 1 < input.size() && input[i] == input[i + 1]) { i++; };
    }
    return triplets;
  }

  static void test_calc_triplets_by_sum() {
    cout << "6. calc_triplets_by_sum" << endl;
    vector<int> test_input_1 = { 0, 24, 0, 10 };
    vector<vector<int>> test_input_0 = { { -1, 0, 1, 2, -1, -4 },
                                         { 12, 3, 6, 1, 6, 9 },
                                         { -2, 0, 1, 1, 2 },
                                         { -2, 0, 1, 1, 2 } };
    vector<vector<vector<int>>> test_output = { { { -1, 0, 1 }, { -1, -1, 2 } },
                                                { { 3, 9, 12 }, { 6, 6, 12 } },
                                                { { -2, 0, 2 }, { -2, 1, 1 } }, {} };
    for (int i = 0; i < test_output.size(); i++) {
      print_all_elem_vec<int>(test_output[i]);
      cout << "<=>" << endl;
      print_all_elem_vec<int>(calc_triplets_by_sum(test_input_0[i], test_input_1[i]));
    }
  }

  /**
   * 2.1.10 4Sum
   * Given an array S of n integers, are there elements a, b, c, and d in S such
   * that a + b + c + d = target? Find all unique quadruplets in the array which
   * gives the sum of target.
   * Note:
   * - Elements in a quadruplet (a, b, c, d) must be in non-descending order.
   *   (ie, a ≤ b ≤ c ≤ d)
   * - The solution set must not contain duplicate quadruplets.
   *   For example, given array S = {1 0 -1 0 -2 2}, and target = 0.
   *   A solution set is:
   *   (-1,  0, 0, 1)
   *   (-2, -1, 1, 2)
   *   (-2,  0, 0, 2)
   */
  vector<vector<int>> calc_quadruplets_by_sum(vector<int> input, int target = 0) {
    vector<vector<int>> quadruplets;
    sort(input.begin(), input.end());
    int curr_sum = 0;
    /* <i, j, k, l> */
    for (int i = 0; i < input.size(); i++) {
      for (int j = i + 1; j < input.size(); j++) {
        for (int k = j + 1, l = input.size() - 1; k < l;) {
          curr_sum = input[i] + input[j] + input[k] + input[l];
          if (curr_sum == target) {
            quadruplets.push_back({ input[i], input[j], input[k], input[l] });
            while (k + 1 < input.size() && input[k] == input[k + 1]) { k++; }
            while (l - 1 >= 0 && input[l] == input[l - 1]) { l--; }
            k++; l--; continue;
          }
          if (curr_sum < target) { k++; }
          if (curr_sum > target) { l--; }
        }
        while (j < input.size() - 1 && input[j] == input[j + 1]) { j++; }
      }
      while (i < input.size() - 1 && input[i] == input[i + 1]) { i++; }
    }
    return quadruplets;
  }

  static void test_calc_quadruplets_by_sum() {
    cout << "7. calc_quadruplets_by_sum" << endl;
    vector<int> test_input_1 = { 0, 10 };
    vector<vector<int>> test_input_0 = { { 1, 0, -1, 0, -2, 2 },
                                         { -2, 0, 1, 1, 2 } };
    vector<vector<vector<int>>> test_output = { { { -1, 0, 0, 1 }, { -2, -1, 1, 2 }, { -2, 0, 0, 2 } }, {} };
    for (int i = 0; i < test_output.size(); i++) {
      print_all_elem_vec<int>(test_output[i]);
      cout << "<=>" << endl;
      print_all_elem_vec<int>(calc_quadruplets_by_sum(test_input_0[i], test_input_1[i]));
    }
  }

  /**
   * 75. Sort Colors
   * Given an array with n objects colored red, white or blue, sort them inplace
   * so that objects of the same color are adjacent, with the colors in the order
   * red, white and blue.
   * Here, we will use the integers 0, 1, and 2 to represent the color red,
   * white, and blue respectively.
   * Note: You are not suppose to use the library's sort function for this problem.
   * Example:                 i,j             k        i  j        k
   * Input: [2,0,2,1,1,0] -> { 2, 0, 2, 1, 1, 0 } -> { 0, 0, 2, 1, 1, 2 }
   *                                                      i  j     k
   *                                                 { 0, 0, 2, 1, 1, 2 }
   *                                                      i    j,k
   *                                                 { 0, 0, 1, 1, 2, 2 }
   * Output: [0,0,1,1,2,2]
   * Follow up:
   * A rather straight forward solution is 2-pass algorithm using counting sort.
   * First, iterate the array counting number of 0's, 1's, and 2's, then
   * overwrite array with total number of 0's, then 1's and followed by 2's.
   * Could you come up with a one-pass algorithm using only constant space?
   */
  static vector<int> sort_colors(vector<int> nums) {
    int cnt[3] = { 0, 0, 0 };
    for (int i = 0; i < nums.size(); i++) { cnt[nums[i]]++; }
    int curr_idx = 0;
    for (int i = 0; i < sizeof(cnt)/sizeof(cnt[0]); i++) {
      for (int j = 0; j < cnt[i]; j++) { nums[curr_idx] = i; curr_idx++; }
    }
    return nums;
  }

  static vector<int> fast_sort_colors(vector<int> nums) {
    for (int i = 0, j = 0, k = nums.size() - 1; j <= k;) { 
      switch(nums[j]) {
        case 0: { swap(nums[i], nums[j]); j++; i++; break; }
        case 1: { j++; break; }
        case 2: { swap(nums[j], nums[k]); k--; break; }
      }
    }
    return nums;
  }

  /**
   * 36. Valid Sudoku
   * Determine if a 9x9 Sudoku board is valid. Only the filled cells need to be
   * validated according to the following rules:
   * - Each row must contain the digits 1-9 without repetition.
   * - Each column must contain the digits 1-9 without repetition.
   * - Each of the 9 3x3 boxes of the grid must contain digits 1-9 without rep.
   * Observation:
   * - use loop to validate rows & columns, can be combined
   * - when use top-left point of a box as id of for loop.
   *   (0, 0), (0, 3), (0, 6)
   *   (3, 0), ...
   *    ...
   * - to solve a actual sudoku, could be straight foward backtracking, as we
   *   will fill up numbers from first row to the end, each time we start with
   *   all numbers available for that specific row, and remove the elem & recur.
   * - each time we verify column and box (due to the fact that we only try val
   *   available.
   */
  static bool is_char_valid(vector<vector<char>> & board, int i, int j) {
    return (board[i][j] == '.' || (board[i][j] >= '1' && board[i][j] <= '9'));
  }
  static bool is_char_number(vector<vector<char>> & board, int i, int j) {
    return (board[i][j] >= '1' && board[i][j] <= '9');
  }
  static void reset_check_buffer(vector<bool> & filled_num_lookup) {
    for (int i = 0; i < filled_num_lookup.size(); i++) { filled_num_lookup[i] = false; }
  }
  static int get_number_val(vector<vector<char>> & board, int i, int j) {
    return (int)board[i][j] - (int)'0';
  }

  static bool is_sudoku_valid(vector<vector<char>> board) {
    int board_size = board.size(), curr_num = -1;
    if (board_size != 9) { return false; }

    vector<bool> row_filled_num_lookup(board_size + 1, false);
    vector<bool> col_filled_num_lookup(board_size + 1, false);
    vector<bool> box_filled_num_lookup(board_size + 1, false);

    for (int i = 0; i < board_size; i++) {
      for (int j = 0; j < board_size; j++) {
        /* validate curr box */
        if (0 == i % 3 && 0 == j % 3) {
          for (int box_row = i; box_row < i + 3; box_row++) {
            for (int box_col = j; box_col < j + 3; box_col++) {
              if (false == is_char_valid(board, box_row, box_col)) { return false; }
              if (board[box_row][box_col] != '.') {
                curr_num = (int)board[box_row][box_col] - (int)'0';
                if (true == box_filled_num_lookup[curr_num]) { return false; }
                box_filled_num_lookup[curr_num] = true;
              }
            }
          }
          reset_check_buffer(box_filled_num_lookup);
        }

        /* validate curr row */
        if (false == is_char_valid(board, i, j)) { return false; }
        if (board[i][j] != '.') {
          curr_num = (int)board[i][j] - (int)'0';
          if (true == row_filled_num_lookup[curr_num]) { return false; }
          row_filled_num_lookup[curr_num] = true;
        }

        /* validate curr col */
        if (false == is_char_valid(board, j, i)) { return false; }
        if (board[j][i] != '.') {
          curr_num = (int)board[j][i] - (int)'0';
          if (true == col_filled_num_lookup[curr_num]) { return false; }
          col_filled_num_lookup[curr_num] = true;
        }
      }
      reset_check_buffer(row_filled_num_lookup);
      reset_check_buffer(col_filled_num_lookup);
    }
    return true;
  }

  static void test_is_sudoku_valid() {
    cout << "11. test_is_sudoku_valid" << endl;
    bool result = false;
    vector<vector<vector<char>>> test_input = {
      { {'5','3','.','.','7','.','.','.','.'}, {'6','.','.','1','9','5','.','.','.'}, {'.','9','8','.','.','.','.','6','.'},
        {'8','.','.','.','6','.','.','.','3'}, {'4','.','.','8','.','3','.','.','1'}, {'7','.','.','.','2','.','.','.','6'},
        {'.','6','.','.','.','.','2','8','.'}, {'.','.','.','4','1','9','.','.','5'}, {'.','.','.','.','8','.','.','7','9'} },
      { {'8','3','.','.','7','.','.','.','.'}, {'6','.','.','1','9','5','.','.','.'}, {'.','9','8','.','.','.','.','6','.'},
        {'8','.','.','.','6','.','.','.','3'}, {'4','.','.','8','.','3','.','.','1'}, {'7','.','.','.','2','.','.','.','6'},
        {'.','6','.','.','.','.','2','8','.'}, {'.','.','.','4','1','9','.','.','5'}, {'.','.','.','.','8','.','.','7','9'} },
      { {'3','.','6','5','.','8','4','.','.'}, {'5','2','.','.','.','.','.','.','.'}, {'.','8','7','.','.','.','.','3','1'},
        {'.','.','3','.','1','.','.','8','.'}, {'9','.','.','8','6','3','.','.','5'}, {'.','5','.','.','9','.','6','.','.'},
        {'1','3','.','.','.','.','2','5','.'}, {'.','.','.','.','.','.','.','7','4'}, {'.','.','5','2','.','6','3','.','.'} },
      { {'3','.','6','5','.','8','4','.','9'}, {'5','2','.','.','.','.','.','.','.'}, {'.','8','7','.','.','.','.','3','1'},
        {'.','.','3','.','1','.','.','8','.'}, {'9','.','.','8','6','3','.','.','5'}, {'.','5','.','.','9','.','6','.','.'},
        {'1','3','.','.','.','.','2','5','.'}, {'.','.','.','.','.','.','.','7','4'}, {'.','.','5','2','.','6','3','.','9'} }
    };
    vector<bool> test_output  = { true, false, true, false };
    for (int i = 0; i < test_input.size(); i++) {
      result = is_sudoku_valid(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * ending term: all blanks been filled already
   * each round fill exact 1 number if curr cell not been filled yet, and if we
   * actually filled a value, then we run validation, if true contiue return
   * false otherwise.
   */
  static bool is_current_fill_valid(vector<vector<char>> & board, int row, int col) {
    /* 0 0 -> box top-left, */
    for (int i = 0; i < board.size(); i++) {
      /* verify corresponding row & col to see any conflicts */
      if ((board[row][col] == board[row][i] && col != i) ||
          (board[row][col] == board[i][col] && row != i)) { return false; }
    }
    /* verify corresponding box to see any conflicts */
    int box_start_row = (row / 3) * 3, box_start_col = (col / 3) * 3;
    for (int i = box_start_row; i < box_start_row + 3; i++) {
      for (int j = box_start_col; j < box_start_col + 3; j++) {
        if (i == row && j == col) { continue; }
        if (board[row][col] == board[i][j]) { return false; }
      }
    }
    return true;
  }

  static vector<char> get_avaialbe_values(vector<vector<char>> & board, int row, int col) {
    vector<char> avail_nums;
    vector<bool> lookup(9, false);
    for (int i = 0; i < board.size(); i++) {
      if ('.' != board[row][i]) { lookup[board[row][i] - '1'] = true; }
      if ('.' != board[i][col]) { lookup[board[i][col] - '1'] = true; }
    }
    /* verify corresponding box to see any conflicts */
    int box_start_row = (row / 3) * 3, box_start_col = (col / 3) * 3;
    for (int i = box_start_row; i < box_start_row + 3; i++) {
      for (int j = box_start_col; j < box_start_col + 3; j++) {
        if ('.' != board[i][j]) { lookup[board[i][j] - '1'] = true; }
      }
    }
    for (int i = 0; i < lookup.size(); i++) {
      if (!lookup[i]) { avail_nums.push_back(i + (int)'1'); }
    }
    return avail_nums;
  }

  static bool solve_sudoku_recur(vector<vector<char>> & board,
                                 vector<pair<int, int>> & cells_to_fill) {
    if (true == cells_to_fill.empty()){ return true; }

    pair<int, int> curr_cell = cells_to_fill.back();
    vector<char> num_arr = get_avaialbe_values(board, curr_cell.first, curr_cell.second);
    for (char num_to_fill : num_arr) {
      board[curr_cell.first][curr_cell.second] = num_to_fill;
      cells_to_fill.pop_back();
      if (is_current_fill_valid(board, curr_cell.first, curr_cell.second)) {
        if (solve_sudoku_recur(board, cells_to_fill)) { break; }
        /* if rest of path does not go well, then recover */
        board[curr_cell.first][curr_cell.second] = '.';
        cells_to_fill.push_back(curr_cell);
      } else {
        board[curr_cell.first][curr_cell.second] = '.';
        cells_to_fill.push_back(curr_cell);
      }
    }
    return cells_to_fill.empty();
  }

  static bool solve_sudoku(vector<vector<char>> & board) {
    vector<pair<int, int>> cells_to_fill;
    for (int i = 0; i < board.size(); i++) {
      for (int j = 0; j < board[i].size(); j++) {
        if ('.' != board[i][j]) { continue; }
        cells_to_fill.push_back(pair<int, int>(i, j));
      }
    }
    return solve_sudoku_recur(board, cells_to_fill);
  }

  static void test_solve_sudoku() {
    cout << "12. test_solve_sudoku" << endl;
    bool result = false;
    vector<vector<vector<char>>> test_input = {
      { {'5','3','.','.','7','.','.','.','.'}, {'6','.','.','1','9','5','.','.','.'}, {'.','9','8','.','.','.','.','6','.'},
        {'8','.','.','.','6','.','.','.','3'}, {'4','.','.','8','.','3','.','.','1'}, {'7','.','.','.','2','.','.','.','6'},
        {'.','6','.','.','.','.','2','8','.'}, {'.','.','.','4','1','9','.','.','5'}, {'.','.','.','.','8','.','.','7','9'} },
      { {'8','3','.','.','7','.','.','.','.'}, {'6','.','.','1','9','5','.','.','.'}, {'.','9','8','.','.','.','.','6','.'},
        {'8','.','.','.','6','.','.','.','3'}, {'4','.','.','8','.','3','.','.','1'}, {'7','.','.','.','2','.','.','.','6'},
        {'.','6','.','.','.','.','2','8','.'}, {'.','.','.','4','1','9','.','.','5'}, {'.','.','.','.','8','.','.','7','9'} },
      { {'3','.','6','5','.','8','4','.','.'}, {'5','2','.','.','.','.','.','.','.'}, {'.','8','7','.','.','.','.','3','1'},
        {'.','.','3','.','1','.','.','8','.'}, {'9','.','.','8','6','3','.','.','5'}, {'.','5','.','.','9','.','6','.','.'},
        {'1','3','.','.','.','.','2','5','.'}, {'.','.','.','.','.','.','.','7','4'}, {'.','.','5','2','.','6','3','.','.'} }
    };
    vector<bool> test_output  = { true, false, true };
    for (int i = 0; i < test_input.size(); i++) {
      result = solve_sudoku(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
      if (true == result) { assert(true == is_sudoku_valid(test_input[i])); }
    }
  }

  /**
   * 287. Find the Duplicate Number
   * Given an array nums containing n + 1 integers where each integer is
   * between 1 and n (inclusive), prove that at least one duplicate number must
   * exist. Assume there is only one duplicate number, find the duplicate one.
   * Example 1:
   * - Input: [1,3,4,2,2]
   * - Output: 2
   * Example 2:
   * - Input: [3,1,3,4,2]
   * - Output: 3
   * Note:
   * - You must not modify the array (assume the array is read only).
   * - You must use only constant, O(1) extra space.
   * - Your runtime complexity should be less than O(n2).
   * - There is only one duplicate number in the array, but it could be repeated
   *   more than once.
   * Observation:
   * - an array of size N + 1 of integers, ranging from 1 ... N.
   * - already know that there exists an number appears more than once.
   * - input array is immutable -> no sorting, no swapping
   * - only constant space allowed, -> no hashing/counting.
   * - only one number has replication, -> constant space/counter needed
   * - T(n) <= O(n2), O(n) not viable(scan without caching doesnt give anything)
   * Divide & Conquer -> O(nlogn) ?
   * - Some tech similar to merge-sort? find the duplicates when doing merge?
   * - diff is that we do not need huge space to store the sorting, but the dup?
   * - given that array[0..n/2] no dup & array[n/2 + 1..n] no dup, how to find
   *   dup number between array-0 & array-1? (at most 1 number)
   * -           [ 1, 2,        3, 4, 2 ] ?
   *               v  |         v
   * -           [ 1, 2 ] <=> [ 3, 4, 2 ] ?
   * - sum:          3             9       ==>> total-sum 12
   *     12 - 2 - 9 => 1
   *
   *               n1 n2 = y    n0 n3 n2 = x
   *
   *        v (uniq-set-0) => 3   v (uniq-set-1) => 9
   *            all-diff              all-diff
   *
   *       2v + ((uniq-set-0) + (uniq-set-1)) => 12 sum <= n2 => logn^2 ?
   *            ((uniq-set-0) + (uniq-set-1)) <= n^2
   *       n^2 => 25 & x => 10 ?
   *       13? | (1 <-> 13) <=> 6 | (7 <=> 13) <=> 10
   *
   * - iterate all values from array-left & array-right
   *   calc. total-sum - 2 * value => (uniq-set-0 + uniq-set-1) -> 10 ?  (3 - 1 => 2 | 9 - 1 => 8)
   *
   *
   *            (uniq-set-0) + (uniq-set-1) => 12
   *
   *          (uniq-set-0) => 3     (uniq-set-1) => 9
   *            all-diff              all-diff
   *
   *             x - y ==> n0 + n3 + n2 - n1 - n2 -> 6 == (n0 - n1 + n3)
   *
   *             6 == n0 - n1 + n3
   *             3 == n1 + n2
   *             9 == n0 + n2 + n3
   *
   *             (n0 - n1 + n3) - (n1 + n2) => 3 == n0 - 2n1 + n3 - n2
   *                                           3 == n1 + n2
   *
   *             n2 = (x + y - n0 - n1 - n3) / 2
   *
   *          n0 + n1 + 2 * n2 + n3 = x + y
   *
   *     if n2 exists, than can be divided without a remaninng
   * - when dups are 1st found, each of them came from diff. subset of an array
   *   { a0, a1, ... am } <<==>> { b0, b1, ... bn }
   *
   * - algorithm stops when we 1st find a dup -> when 2 n2 1st appears.
   *
   * - findi-dup(n) => { 2 * find-dup(n / 2) + O(n) }
   * - base case:
   * - 1 elem -> no
   * - 2 elem -> if array[0] == array[1]
   * - n elem -> array[n/2](no dup) & array[n/2](no dup), one number be shared?
   * - [ 1, 2, 3, 4, 2 ]
   *     ^  ^
   *     ^
   *     f    dist-of-slow-in-cycle
   *     |           |
   * x + y == 2 (x + z) => x = y - 2z
   *                           |
   *                 dist-of-fast-in-cycle
   *
   * fast ptr loop { outside cycle, within cycle }
   *                     2x |
   * slow ptr loop { outside cycle, within cycle }
   * (curr_pos | fast => 2 * pos of slow, & live in cycle)
   *                                              *               *       |-------|<----------------------|
   *      k ----------------- h { fast -> x - f - y - z - h - k - y - z - h - k - y - z - h - k - y - z - h
   *      |                   | { slow ->                                 x - f - y - z - h - k - y - z - h
   * x -- y ----------------- z                                           ^       *               *       ^
   * ^(reset slow)                                                        |-------|<----------------------|
   */
  static int find_duplicate(vector<int> nums) {
    int fast_idx = 0, slow_idx = 0, max_size = nums.size();
    if (1 >= max_size) { return -1; }
    do {
      slow_idx = nums[slow_idx] % max_size;
      fast_idx = nums[nums[fast_idx] % max_size] % max_size;
    } while (fast_idx != slow_idx);
    slow_idx = 0;
    do {
      slow_idx = nums[slow_idx] % max_size;
      fast_idx = nums[fast_idx] % max_size;
    } while (fast_idx != slow_idx);
    return slow_idx;
  }

  /**
   * 289. Game of Life
   * According to the Wikipedia article: "The Game of Life, also known simply
   * as Life, is a cellular automaton devised by the British mathematician John
   * Horton Conway in 1970."
   * Given a board with m by n cells, each cell has an initial state live (1)
   * or dead (0). Each cell interacts with its eight neighbors (horizontal,
   * vertical, diagonal) using the following four rules (taken from the above
   * Wikipedia article):
   *
   * - Any live cell with < 2 live neighbors dies, as if by under-population.
   * - Any live cell with 2|3 live neighbors lives on to next generation.
   * - Any live cell with > 3 live neighbors dies, as if by over-population.
   * - Any dead cell with 3 live neighbors is live, as if by reproduction.
   *
   * Write a function to compute the next state (after one update) of the
   * board given its current state. The next state is created by applying
   * the above rules simultaneously to every cell in the current state, where
   * births and deaths occur simultaneously.
   *
   * Example:
   * - Input:
   * [ [0,1,0],
   *   [0,0,1],
   *   [1,1,1],
   *   [0,0,0] ]
   * - Output:
   * [ [0,0,0],
   *   [1,0,1],
   *   [0,1,1],
   *   [0,1,0] ]
   *
   * Follow up:
   * Could you solve it in-place? Remember that the board needs to be updated at
   * the same time: You cannot update some cells first and then use their
   * updated values to update other cells.
   * In this question, we represent the board using a 2D array. In principle,
   * the board is infinite, which would cause problems when the active area
   * encroaches the border of the array. How would you address these problems?
   * Observation:
   * - to update the board in place requires us not to erode any info from input.
   * - but the status of any cell is just 2, either 0 or 1, if no change update.
   * - 0 -> 1 then we can say 2, if 1 -> 0, then we can use 3.
   */
  static void check_and_update_board(vector<vector<int>> & board) {
    enum { DEAD = 0, LIVE = 1, DEAD_TO_LIVE = 2, LIVE_TO_DEAD = 3 };

    for (int i = 0; i < board.size(); i++) {
      for (int j = 0; j < board[i].size(); j++) {

        int live_neighbor_cnt = 0;
        for (int x = i - 1; x <= i + 1; x++) {
          for (int y = j - 1; y <= j + 1; y++) {
            if (x == i && y == j) { continue; }
            if (x >= 0 && x < board.size() && y >= 0 && y < board[x].size()) {
              if (LIVE == board[x][y] || LIVE_TO_DEAD == board[x][y]) { live_neighbor_cnt++; }
            }
          }
        }

        switch(live_neighbor_cnt) {
          case 0:
          case 1: { if (LIVE == board[i][j]) { board[i][j] = LIVE_TO_DEAD; } break; }
          case 2:
          case 3: { if (3 == live_neighbor_cnt && DEAD == board[i][j]) { board[i][j] = DEAD_TO_LIVE; } break; }
          default: { if (LIVE == board[i][j]) { board[i][j] = LIVE_TO_DEAD; } break; }
        }
      }
    }

    for (int i = 0; i < board.size(); i++) {
      for (int j = 0; j < board[i].size(); j++) {
        if (DEAD_TO_LIVE == board[i][j]) { board[i][j] = LIVE; }
        else if (LIVE_TO_DEAD == board[i][j]) { board[i][j] = DEAD; }
      }
    }
  }

  /**
   * 729. My Calendar I
   * Implement a MyCalendar class to store your events. A new event can be added
   * if adding the event will not cause a double booking.
   *
   * Your class will have the method, book(int start, int end). Formally, this
   * represents a booking on the half open interval [start, end), the range of
   * real numbers x such that start <= x < end.
   *
   * A double booking happens when two events have some non-empty intersection
   * (ie., there is some time that is common to both events.)
   *
   * For each call to the method MyCalendar.book, return true if the event can
   * be added to the calendar successfully without causing a double booking.
   * Otherwise, return false and do not add the event to the calendar.
   *
   * Your class will be called like this:
   * - MyCalendar cal = new MyCalendar(); MyCalendar.book(start, end)
   *
   * Example 1:
   * MyCalendar();
   * MyCalendar.book(10, 20); // returns true
   * MyCalendar.book(15, 25); // returns false
   * MyCalendar.book(20, 30); // returns true
   *
   * Explanation:
   * The first event can be booked. The second can't because time 15 is already booked by another event.
   * The third event can be booked, as the first event takes every time less than 20, but not including 20.
   *
   * Note:
   * The number of calls to MyCalendar.book per test case will be at most 1000.
   * In calls to MyCalendar.book(start, end), start and end are integers in the range [0, 10^9].
   */
  class MyCalendar {
  public:
    MyCalendar() {}
    ~MyCalendar() {}
    bool book(int start, int end) {
      if (ledger.end() != ledger.find(end)) { return false; }
      ledger[end] = start;
      map<int, int>::iterator curr_itr = ledger.find(end);
      map<int, int>::iterator temp_itr = curr_itr;
      temp_itr++;
      if (ledger.end() != temp_itr && end > temp_itr->second) {
        ledger.erase(curr_itr); return false;
      }
      if (ledger.begin() == curr_itr) { return true; }
      temp_itr--; temp_itr--;
      if (temp_itr->first > start) {
        ledger.erase(curr_itr); return false;
      }
      return true;
    }
  private:
    /* <finish time, start time> */
    map<int, int> ledger;
  };

  static void test_calendar() {
    MyCalendar calendar;
    int test_input[] = { 10, 20, 15, 25, 20, 30, 30, 40 };
    bool test_output[] = { true, false, true, true }, result = false;
    cout << "15. test_calendar" << endl;
    for (int i = 0; i < sizeof(test_output)/sizeof(test_output[0]); i++) {
      result = calendar.book(test_input[i * 2], test_input[i * 2 + 1]);
      cout << test_output[i] << " <=> " << result << endl;
    }
  }

  /**
   * 287. Find the Duplicate Number
   * - Given an array nums containing n + 1 integers where each integer is
   *   between 1 and n (inclusive), prove that at least one duplicate number
   *   must exist. Assume that there is only one duplicate number, find the
   *   duplicate one.
   * Example 1:
   * - Input: [1,3,4,2,2]
   *   Output: 2
   * Example 2:
   * - Input: [3,1,3,4,2]
   *   Output: 3
   * Note:
   * - You must not modify the array (assume the array is read only).
   * - You must use only constant, O(1) extra space.
   * - Your runtime complexity should be less than O(n2).
   * - There is only one duplicate number in the array, but it could be
   *   repeated more than once.
   * Intuition:
   * - input arr -> size n + 1, & only one # is dupped, & value is in 1 & n
   * - one value dupped (from 1 ~ n), can appear 1 ~ n + 1 times;
   * - no change to raw input, -> no sorting, swap.
   * - O(1) space -> no buffer copy, input is un-ordered.
   * - < O(n2), O(n) not likely, nlogn for divide-conquer?
   * - there must be diff. between the sum, & abs(diff)
   * - sum(arr[0..n]) -> sum(1..n + 1) -> if no dups
   *   min -> 1, max -> 4 -> 5.. been replaced
   * - 1, 2, 2, 2, 3, 4 -> 14 -> 2 x 2 (diff -7) -> some n, n + 1 replaced
   *   1, 2, 3, 4, 5, 6 -> 21
   *   1, 3, 6, 4, 5, 2 -> 21
   *   1, 2, 3, 6, 6, 6 -> 24 -> 2 x 6 (diff +3)
   *   1, 2, 2, 2, 2, 6 -> 15 -> 3 x 2 (diff -6)
   *
   *   x 1, 3, 6, 4, 5, 2 -> 21
   *
   *   min -> 1, max -> 6 -> arr[i..j] been replaced
   *   say value of dup is n, & dupped m times
   *                         (not-replaced)
   * - sum(arr[0..n]) = n * m + valid-sum + n
   *   target-sum     = sum(arr[i]) + n + valid-sum
   *                (m replaced elem)   (not-replaced)
   *   target-sum     = (n + 1)(n + 2) / 2
   *
   * - sum(arr[0..n]) - target-sum = n * m - sum(arr[r])
   *   sum(arr[0..n]) + target-sum = n * m + 2(n + valid-sum) + sum(arr[r])
   *   elem not continous, not likely to be solved.
   */
  static int find_dup_number(const vector<int> & input) {
    int slow_idx = 0, fast_idx = 0;
    do {
      slow_idx = input[slow_idx];
      fast_idx = input[input[fast_idx]];
    } while (slow_idx != fast_idx);
    slow_idx = 0;
    do {
      slow_idx = input[slow_idx];
      fast_idx = input[input[fast_idx]];
    } while (slow_idx != fast_idx);
    return input[slow_idx];
  }

  static void test_find_dup_number() {
    vector<vector<int>> test_input = {{ 1, 3, 4, 2, 2, 2 }, { 3, 1, 3, 4, 2 }};
    vector<int> test_output = { 2, 3 };
    int dup_val = -1;
    cout << "16. test_find_dup_number" << endl;
    for (int i = 0; i < test_output.size(); i++) {
      dup_val = find_dup_number(test_input[i]);
      cout << test_output[i] << " <=> " << dup_val << endl;
    }
  }

  /**
   * 268. Missing Number
   * Given an array containing n distinct numbers taken from 0, 1, 2, ..., n, find the one that is missing from the array.
   * Example 1:
   * - Input: [3,0,1]
   * - Output: 2
   * Example 2:
   * - Input: [9,6,4,2,3,5,7,0,1]
   * - Output: 8
   * Intuition:
   * - input arr size of n -> contains # from { 0..n } - miss
   *   expected value should be (n + 1) * n / 2, with n + 1 number
   *   as we start from 0, the diff should be the miss
   */
  static int find_missing_num(const vector<int> & input) {
    if (input.size() < 1) { return -1; }
    /* arr size is n, then exp. cnt is n + 1, with min 0 & max of n */
    int target_sum = (input.size() + 1) * input.size() / 2;
    int curr_sum = 0;
    for (auto & val : input) { curr_sum += val; }
    return target_sum - curr_sum;
  }

  static void test_find_missing_num() {
    vector<vector<int>> test_input = {{}, { 1 }, { 0 }, { 3, 0, 1 }, { 9, 6, 4, 2, 3, 5, 7, 0, 1 }};
    vector<int> test_output = { -1, 0, 1, 2, 8 };
    int result = -1;
    cout << "17. test_find_missing_num" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = find_missing_num(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 169. Majority Element
   * - Given an array of size n, find the majority element. The majority element
   *   is the element that appears more than n/2 times.
   * - You may assume that array is non-empty and the majority element always
   *   exist in the array.
   * Example 1:
   * - Input: [3,2,3]
   * - Output: 3
   * Example 2:
   * - Input: [2,2,1,1,1,2,2]
   * - Output: 2
   */
  static int find_major_num(const vector<int> & input) {
    int major_num = input[0], major_cnt = 0;
    for (int i = 0; i < input.size(); i++) {
      if (major_num == input[i]) { major_cnt++; } else { major_cnt--; }
      if (0 == major_cnt) { major_num = input[i]; major_cnt++; continue; }
    }
    return major_num;
  }

  static void test_find_major_num() {
    vector<vector<int>> test_input = { { 3, 2, 3 }, { 2, 2, 1, 1, 1, 2, 2 } };
    vector<int> test_output = { 3, 2 };
    int result = -1;
    cout << "18. test_find_major_num" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = find_major_num(test_input[i]);
      cout << test_output[i] << " <=> " << result << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 229. Majority Element II
   * - Given an integer array of size n, find all elements that appear more
   *   than n/3 times.
   * Note: The algorithm should run in linear time and in O(1) space.
   * Example 1:
   * - Input: [3,2,3]
   *   Output: [3]
   * Example 2:
   * - Input: [1,1,1,3,3,2,2,2]
   *   Output: [1,2]
   */
  static vector<int> find_major_numbers_ii(const vector<int> & input, int major_weight = 3) {

    vector<int> result;
    unordered_map<int, int> cnt_lookup, cnt_buff;
    unordered_map<int, int>::iterator curr_itr = cnt_lookup.end(), itr_to_erase;
    int max_size = major_weight - 1, min_cnt = input.size() / major_weight + 1;

    for (int i = 0; i < input.size(); i++) {
      if (cnt_lookup.size() < max_size || cnt_lookup.end() != cnt_lookup.find(input[i])) {
        /* either buffer is not full or already in cnt */
        if (cnt_lookup.end() == cnt_lookup.find(input[i])) {
          cnt_lookup[input[i]] = 1;
        } else {
          cnt_lookup[input[i]] += 1;
        }
      } else {
        /* buffer is full && not in cnt */
        if (curr_itr == cnt_lookup.end()) { curr_itr = cnt_lookup.begin(); }
        curr_itr->second -= 1;
        if (curr_itr->second == 0) {
          itr_to_erase = curr_itr; curr_itr++;
          cnt_lookup.erase(itr_to_erase);
          cnt_lookup[input[i]] = 1;
        } else { curr_itr++; }
      }
    }

    for (auto & val : input) {
      if (cnt_lookup.end() == cnt_lookup.find(val)) { continue; }
      if (cnt_buff.end() == cnt_buff.find(val)) { cnt_buff[val] = 0; }
      cnt_buff[val] += 1;
    }

    for (auto & pair : cnt_buff) {
      if (pair.second >= min_cnt) { result.push_back(pair.first); }
    }

    return result;
  }

  static void test_find_major_numbers_ii() {
    vector<vector<int>> test_output = {{3}, {2,1}, {1}};
    vector<vector<int>> test_input = {{ 3,2,3 }, { 1,1,1,3,3,2,2,2 }, {1,1,1,2,2,1,3,4,5,6}};
    cout << "19. test_find_major_numbers_ii" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      vector<int> result = find_major_numbers_ii(test_input[i]);
      print_all_elem<int>(test_output[i]);
      print_all_elem<int>(result);
      assert(test_output[i].size() == result.size());
      for (int j = 0; j < result.size(); j++) {
        assert(test_output[i][j] == result[j]);
      }
    }
  }

  /**
   * 209. Minimum Size Subarray Sum
   * - Given an array of n positive integers and a positive integer s, find the
   *   minimal length of a contiguous subarray of which the sum ≥ s. If there
   *   isn't one, return 0 instead.
   * Example: 
   * - Input: s = 7, nums = [2,3,1,2,4,3]
   *   Output: 2
   *   Explanation: the subarray [4,3] has the minimal length under constraint.
   * Follow up:
   * - If you have figured out the O(n) solution, try coding another solution
   *   of which the time complexity is O(n log n).
   * Intuition:
   * - sliding window to get current sum of sub-arr, i, j & curr_sum
   * - all num are positive, as curr_sum < target, expand j, incr i otherwise.
   * - also log the min len of window meet the target sum.
   * - basically j will always be one step ahead ready to add in next round.
   */
  static int find_min_window_exceed_sum(int target, const vector<int> input) {
    int min_window_size = INT_MAX, curr_window_sum = 0;
    if (target <= 0 || input.empty()) { return 0; }
    for (int i = 0, j = 0; j <= input.size(); ) {
      if (i > j) { j = i; curr_window_sum = 0; }
      if (curr_window_sum < target) {
        if (j < input.size()) { curr_window_sum += input[j]; j++; } else { j++; }
      } else {
        /* here window sum >= target, & we won't add curr j */
        min_window_size = min(min_window_size, j - i);
        curr_window_sum -= input[i]; i++;
      }
    }
    if (INT_MAX == min_window_size) { min_window_size = 0; }
    return min_window_size;
  }

  static void test_find_min_window_exceed_sum() {
    vector<int> test_input_target = { 11, 4, 2, 2, 7, 7, 0, 4, 303 };
    vector<vector<int>> test_input_arr = { {1,2,3,4,5}, {2,2,2,2}, {2,2,2,2}, {2}, { 2,3,1,2,4,3 }, { 2,3,1,2,4,3,7 }, {}, {9, 99}, {101, 102} };
    vector<int> test_output = { 3, 2, 1, 1, 2, 1, 0, 1, 0 };
    int result = -1;
    cout << "20. test_find_min_window_exceed_sum" << endl;
    for (int i = 0; i < test_input_target.size(); i++) {
      result = find_min_window_exceed_sum(test_input_target[i], test_input_arr[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 152. Maximum Product Subarray
   * - Given an integer array nums, find the contiguous subarray within an array
   *   (containing at least one number) which has the largest product.
   * Example 1:
   * - Input: [2,3,-2,4]
   *   Output: 6
   *   Explanation: [2,3] has the largest product 6.
   * Example 2:
   * - Input: [-2,0,-1]
   *   Output: 0
   *   Explanation: The result cannot be 2, because [-2,-1] is not a subarray.
   * Intuition:
   * - only max prod of sub-arr is needed, -> contigous -> sliding window.
   * - use i, j to mark beginning & end of curr. window, curr_prod -> prod of window[i, j - 1]
   * - when curr window prod > 0, we keep the max one.
   * - seems quite hard to identify the boundary, as we neg * neg -> pos
   * - pre-calc prod of input[j..n] ? such that we know what the pos of closest neg # ?
   * - 2 3 -2 4 3 1 3 1 -> 2 3 -2 4 3 1 3 1
   *   i    j                  ij
   *   2 3 -2 4 -3 -1 3 1 -> 2 3 -2 4 -3 -1 3 1
   *   i            j             i       j
   *   -2, -3, 2, 3, -4, 1, 3, 0, -5 -> -2, -3, 2, 3, -4, 1, 3, 0, -5
   *    i             j                      i         j
   * - if curr sum is 0, then move i to j + 1;
   */
  static int find_max_product(const vector<int> & input) {
    int max_product = INT_MIN, curr_prod = 1;
    if (input.empty()) { return 0; }
    vector<int> next_non_pos_value_pos(input.size(), 0);

    for (int i = 0, j = 0; i < input.size(); ) {
      j = i + 1;
      while (j < input.size() && input[j] > 0) { j++; }
      while (i < j) { next_non_pos_value_pos[i] = j; i++; }
      i = j;
    }
    /* curr_prod will always be kept postive if possible */
    for (int i = 0, j = 0; j < input.size(); ) {
      if (i > j) { j = i; curr_prod = 1; }
      if (j < input.size() && input[j] * curr_prod > 0) {
        curr_prod *= input[j]; j++;
        max_product = max(max_product, curr_prod);
        continue;
      }
      if (j < input.size() && input[j] == 0) {
        i = j + 1; curr_prod = 1;
        max_product = max(max_product, 0);
        continue;
      }
      if (j < input.size() && input[j] * curr_prod < 0) {
        int next_non_pos = next_non_pos_value_pos[j];
        if (next_non_pos < input.size() && input[next_non_pos] < 0) {
          while (j <= next_non_pos) { curr_prod *= input[j]; j++; };
        } else {
          while (i < next_non_pos_value_pos[i] && i < j) {
            curr_prod /= input[i]; i++; if (input[i - 1] < 0) { break; }
          }
          curr_prod *= input[j]; j++;
        }
        max_product = max(max_product, curr_prod);
      }
    }
    if (INT_MIN == max_product) { max_product = 0; }
    return max_product;
  }

  static void test_find_max_product() {
    vector<vector<int>> test_input = { { 1, 0, -1, 2, 3, -5, -2 },  { 3, -1, 4 }, { 1, 2, 3, -1, 99 }, { 1, 0, 1 }, { 1 }, { 2, 3, -2, 4 }, { -2, 0, -1 }, { -2, -3, -4, -5 }, { -2, -3, -4, 0, -5 }, { -2, -3, 2, 3, -4, 1, 3, 0, -5 } };
    vector<int> test_output = { 60, 4, 99, 1, 1, 6, 0, 120, 12, 216 };
    int result = 0;
    cout << "21. test_find_max_product" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = find_max_product(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * Let min[i] denote the min prod of sub-arr for arr[k..i]
   *     max[i] denote the max prod of sub-arr for arr[k..i]
   * min[i] = min(arr[i] * min[i - 1], arr[i], arr[i] * max[i - 1])
   * max[i] = max(arr[i] * max[i - 1], arr[i], arr[i] * min[i - 1])
   */
  static int find_max_product_dp(const vector<int> & input) {
    if (input.empty()) { return 0; }
    int curr_min_prod = input[0], curr_max_prod = input[0],
        prev_min_prod = input[0], max_product = input[0];
    for (int i = 1; i < input.size(); i++) {
      prev_min_prod = curr_min_prod;
      curr_min_prod = min(min(input[i] * curr_min_prod, input[i]), input[i] * curr_max_prod);
      curr_max_prod = max(max(input[i] * curr_max_prod, input[i]), input[i] * prev_min_prod);
      max_product = max(max_product, curr_max_prod);
    }
    return max_product;
  }

  static void test_find_max_product_dp() {
    vector<vector<int>> test_input = { { 1, 0, -1, 2, 3, -5, -2 },  { 3, -1, 4 }, { 1, 2, 3, -1, 99 }, { 1, 0, 1 }, { 1 }, { 2, 3, -2, 4 }, { -2, 0, -1 }, { -2, -3, -4, -5 }, { -2, -3, -4, 0, -5 }, { -2, -3, 2, 3, -4, 1, 3, 0, -5 } };
    vector<int> test_output = { 60, 4, 99, 1, 1, 6, 0, 120, 12, 216 };
    int result = 0;
    cout << "22. test_find_max_product_dp" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = find_max_product_dp(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 128. Longest Consecutive Sequence
   * - Given an unsorted array of integers, find the length of the longest
   *   consecutive elements sequence.
   * - Your algorithm should run in O(n) complexity.
   * Example:
   * - Input: [ 100, 4, 200, 1, 3, 2 ]
   * - Output: 4
   * Explanation:
   * - The longest consecutive elements sequence is [1, 2, 3, 4]. Therefore
   *   its length is 4.
   * Intuition:
   * - O(n) -> no sorting, has to use hash
   * - dump all vals to set, & iterate the original input.
   * - assume all numbers are uniq, for each num from arr:
   *   if arr[i] found in set then
   *     expand both ways until the end & keep max size
   *     delete each elements checked
   *   else
   *     skip, as the element already been covered before.
   *   end
   */
  static int find_lces(const vector<int> & input) {
    int lces = 0, left_num = 0, right_num = 0;
    unordered_set<int> lces_lookup(input.begin(), input.end());
    for (auto & curr_num : input) {
      if (lces_lookup.end() == lces_lookup.find(curr_num)) { continue; }
      left_num = curr_num - 1; right_num = curr_num + 1;
      while (lces_lookup.end() != lces_lookup.find(left_num)) {
        lces_lookup.erase(left_num); left_num--;
      }
      while (lces_lookup.end() != lces_lookup.find(right_num)) {
        lces_lookup.erase(right_num); right_num++;
      }
      lces = max(lces, right_num - left_num - 1);
    }
    return lces;
  }

  static void test_find_lces() {
    vector<vector<int>> test_input = { {}, { 1 }, { 1, 5 }, { 1, 2, 3, 4 }, { 100, 4, 200, 1, 3, 2 } };
    vector<int> test_output = { 0, 1, 1, 4, 4 };
    int result = 0;
    cout << "23. test_find_lces" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = find_lces(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(test_output[i] == result);
    }
  }

  /**
   * 121. Best Time to Buy and Sell Stock
   * - Say you have an array for which the ith element is the price of a given
   *   stock on day i. If you were only permitted to complete at most one
   *   transaction (i.e., buy one and sell one share of the stock), design an
   *   algorithm to find the maximum profit.
   * - Note that you cannot sell a stock before you buy one.
   * Example 1:
   * - Input: [7,1,5,3,6,4]
   *   Output: 5
   *   Explanation: Buy on day 2 (price = 1) and sell on day 5 (price = 6),
   *                profit = 6-1 = 5. Not 7-1 = 6, as selling price needs to be
   *                larger than buying price.
   * Example 2:
   * - Input: [7,6,4,3,1]
   *   Output: 0
   *   Explanation: In this case, no transaction is done, i.e. max profit = 0.
   * Intuition:
   * - find 2 with max ending price - starting price, return 0 if negative.
   */
  static int calc_max_profit(const vector<int> & input) {
    int max_profit = 0;
    if (input.size() < 2) { return max_profit; }
    int max_price = input.back();
    vector<int> max_value_after_pos(input.size(), INT_MIN);
    for (int i = input.size() - 2; i >= 0; i--) {
      max_price = max(max_price, input[i + 1]);
      max_value_after_pos[i] = max_price;
    }

    for (int i = 0; i < input.size() - 1; i++) {
      max_profit = max(max_profit, max_value_after_pos[i] - input[i]);
    }

    return max_profit;
  }

  static void test_calc_max_profit() {
    vector<vector<int>> test_input = { {}, { 1 }, { 1,5,3,6,4,9 }, { 7,1,5,3,6,4,9 }, { 7,1,5,3,6,4 }, { 7,6,4,3,1 } };
    vector<int> test_output = { 0, 0, 8, 8, 5, 0 };
    int profit = -1;
    cout << "23. test_calc_max_profit" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      profit = calc_max_profit(test_input[i]);
      cout << profit << " <=> " << test_output[i] << endl;
      assert(profit == test_output[i]);
    }
  }

  /**
   * 123. Best Time to Buy and Sell Stock III
   * Say you have an array for which the ith element is the price of a given stock on day i.
   * Design an algorithm to find the maximum profit. You may complete at most two transactions.
   * Note: You may not engage in multiple transactions at the same time (i.e., you must sell the stock before you buy again).
   * Example 1:
   * Input: [3,3,5,0,0,3,1,4]
   * Output: 6
   * Explanation: Buy on day 4 (price = 0) and sell on day 6 (price = 3), profit = 3-0 = 3.
   *              Then buy on day 7 (price = 1) and sell on day 8 (price = 4), profit = 4-1 = 3.
   * Example 2:
   * Input: [1,2,3,4,5]
   * Output: 4
   * Explanation: Buy on day 1 (price = 1) and sell on day 5 (price = 5), profit = 5-1 = 4.
   *              Note that you cannot buy on day 1, buy on day 2 and sell them later, as you are
   *              engaging multiple transactions at the same time. You must sell before buying again.
   * Example 3:
   * Input: [7,6,4,3,1]
   * Output: 0
   * Explanation: In this case, no transaction is done, i.e. max profit = 0.
   * Intuition:
   * - Needs to consider 0, 1, 2 trxns that making most of the profits.
   * - Cannot simply max out the trade as we could miss the swing in the middle.
   * - Greedy not an option, then DP?
   * - Let max-profit(i, j) denotes max-profit when sell stock in price j at postion i on trade k
   *   max-profit(i, j, k) = {
   *     input[j] - input[i], if k == 0,
   *     (input[j] - input[i]) + max{ 0 <= y <= i - 1, x < y, max-profit(x, y, k - 1) }
   *   }
   *   return the max value.
   */
  static int calc_max_profit_iii(const vector<int> & input) {
    int max_profit = 0;
    if (input.size() < 2) { return max_profit; }
    vector<vector<vector<int>>> mplookup(2,
      vector<vector<int>>(input.size(), vector<int>(input.size(), 0))
    );
    /* max-profits if selling at pos 0..j for prev-trxn */
    vector<int> prev_mplookup(input.size(), 0);
    vector<int> curr_mplookup(input.size(), 0);
    vector<int> & prev_mplookup_ref = prev_mplookup,
                & curr_mplookup_ref = curr_mplookup,
                & temp_mplookup_ref = prev_mplookup;
    for (int k = 0; k < mplookup.size(); k++) {
      for (int i = 0; i < mplookup[k].size(); i++) {
        for (int j = i + 1; j < mplookup[k][i].size(); j++) {
          mplookup[k][i][j] = input[j] - input[i];
          if (k > 0 && i > 1) { mplookup[k][i][j] += prev_mplookup_ref[i - 1]; }
          max_profit = max(max_profit, mplookup[k][i][j]);
          curr_mplookup_ref[j] = max(curr_mplookup_ref[j - 1], max(curr_mplookup_ref[j], mplookup[k][i][j]));
        }
      }
      temp_mplookup_ref = prev_mplookup_ref;
      prev_mplookup_ref = curr_mplookup_ref;
      curr_mplookup_ref = temp_mplookup_ref;
    }
    return max_profit;
  }

  static void test_calc_max_profit_iii() {
    vector<vector<int>> test_input = { {3,2,6,5,0,3}, {2,1,4,5,2,9,7}, {3,3,5,0,0,3,1,4}, {1,2,3,4,5}, {7,6,4,3,1} };
    vector<int> test_output = { 7, 11, 6, 4, 0 };
    int profit = -1;
    cout << "23. test_calc_max_profit" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      profit = calc_max_profit_iii(test_input[i]);
      cout << profit << " <=> " << test_output[i] << endl;
      assert(profit == test_output[i]);
    }
  }

  static int lean_calc_max_profit_iii(const vector<int> & input) {
    int max_profit = 0;
    if (input.size() < 2) { return max_profit; }
    vector<vector<int>> mplookup(2, vector<int>(input.size(), 0));
    for (int k = 0; k < mplookup.size(); k++) {
      if (0 == k) {
        /* mplookup[k][i] -> max profit possible for input[0..i], either sell not do nothing */
        int cost_basis = input[0];
        for (int i = 1; i < mplookup[k].size(); i++) {
          mplookup[k][i] = max(mplookup[k][i - 1], max(mplookup[k][i], input[i] - cost_basis));
          cost_basis = min(cost_basis, input[i]);
        }
      } else {
        /* mplookup[k][i] -> max profit possible for input[0..i], either do nothing, buy + prev trxn */
        int peak_future_price = input.back();
        for (int i = mplookup[k].size() - 2; i >= 0; i--) {
          mplookup[k][i] = peak_future_price - input[i];
          if (i > 1) { mplookup[k][i] += mplookup[k - 1][i - 1]; }
          mplookup[k][i] = max(mplookup[k - 1][i], mplookup[k][i]);

          max_profit = max(max_profit, mplookup[k][i]);
          peak_future_price = max(peak_future_price, input[i]);
        }
      }
    }
    return max_profit;
  }

  static void test_lean_calc_max_profit_iii() {
    vector<vector<int>> test_input = { {3,2,6,5,0,3}, {2,1,4,5,2,9,7}, {3,3,5,0,0,3,1,4}, {1,2,3,4,5}, {7,6,4,3,1} };
    vector<int> test_output = { 7, 11, 6, 4, 0 };
    int profit = -1;
    cout << "24. test_lean_calc_max_profit" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      profit = lean_calc_max_profit_iii(test_input[i]);
      cout << profit << " <=> " << test_output[i] << endl;
      assert(profit == test_output[i]);
    }
  }

  /**
   * 166. Fraction to Recurring Decimal
   * - Given two integers representing the numerator and denominator of a
   *   fraction, return the fraction in string format.
   * - If fractional part is repeating, enclose repeating part in parentheses.
   * Example 1:
   * - Input: numerator = 1, denominator = 2
   * - Output: "0.5"
   * Example 2:
   * - Input: numerator = 2, denominator = 1
   * - Output: "2"
   * Example 3:
   * - Input: numerator = 2, denominator = 3
   * - Output: "0.(6)"
   * Intuition:
   * - Assume the input is rational (as it should be)
   * - for int part, simply divide
   * - for fraction part, we need to implement divide & find the repeatition.
   * - there will be non-repeat part(any length) & repeat part(any length)
   * - need to judge repeatable part when it occurs, say:
   * - sth similar to kmp failure array?
   *
   * - when a repititon occur, we have a substr [i..j] such that
   *   1. j - i + 1 is even
   *             8 5 7 7 9 8 5 7 7 9
   *
   *   0.0 1 4 2 8 5 7 7 9 8 5 7 7 9
   *     |-----| |-------|
   * - the only way to detect a repition is via looking back the div stack
   *      0 1 4 2 8 5 7 1
   *     ----------------
   *   7  1 0 (remainder -> 1)
   *        7
   *     ----------------
   *        3 0
   *        2 8
   *     ----------------
   *        ...
   *     ----------------
   *                 5 0
   *                 4 9
   *     ----------------
   *                   1 (remainder hit)
   */
  static string convert_fraction_to_decimal(int num, int denom) {
    string decimal_str;
    long numerator = num, denominator = denom;
    long int_part = numerator / denominator;
    bool is_negative = ((numerator > 0 && denominator < 0) || (numerator < 0 && denominator > 0));
    decimal_str = to_string(int_part);
    if (is_negative && '-' != decimal_str[0]) { decimal_str = "-" + decimal_str; }
    if (int_part * denominator == numerator) { return decimal_str; }
    long remainder = abs(numerator - int_part * denominator);
    denominator = abs(denominator);

    vector<int> non_repeating_fraction_digits, repeating_fraction_digits, fraction_digits;
    unordered_map<long, int> remainder_to_index_map({ { remainder, 0 } });
    bool is_repitition_found = false;
    for (; remainder != 0 && false == is_repitition_found; ) {
      int_part = remainder * 10 / denominator;
      remainder = (remainder * 10 - int_part * denominator);
      fraction_digits.push_back(int_part);
      is_repitition_found = (remainder_to_index_map.end() !=
                             remainder_to_index_map.find(remainder));
      if (false == is_repitition_found) {
        remainder_to_index_map[remainder] = fraction_digits.size();
      }
    }
    if (true == is_repitition_found) {
      for (int i = 0; i < remainder_to_index_map[remainder]; i++) {
        non_repeating_fraction_digits.push_back(fraction_digits[i]);
      }
      for (int i = remainder_to_index_map[remainder]; i < fraction_digits.size(); i++) {
        repeating_fraction_digits.push_back(fraction_digits[i]);
      }
    } else {
      non_repeating_fraction_digits = fraction_digits;
    }
    decimal_str.push_back('.');
    for (auto & digit : non_repeating_fraction_digits) {
      decimal_str.push_back((char)('0' + digit));
    }
    if (!repeating_fraction_digits.empty()) {
      decimal_str.push_back('(');
      for (auto & digit : repeating_fraction_digits) {
        decimal_str.push_back((char)('0' + digit));
      }
      decimal_str.push_back(')');
    }
    return decimal_str;
  }

  static void test_convert_fraction_to_decimal() {
    vector<vector<int>> test_input = { { -50, 8 }, { -1, 6 }, { 1, -6 }, { 1, 6 }, { 1, 2 }, { 2, 1 }, { 2, 3 }, { 3227, 555 }, { 2, 7 }, { 22, 7 }, { -2147483648, -1 }, { -1, -2147483648 } };
    vector<string> test_output = { "-6.25", "-0.1(6)", "-0.1(6)", "0.1(6)", "0.5", "2", "0.(6)", "5.8(144)", "0.(285714)", "3.(142857)", "2147483648", "0.0000000004656612873077392578125" };
    string result;
    cout << "25. test_convert_fraction_to_decimal" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = convert_fraction_to_decimal(test_input[i][0], test_input[i][1]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 547. Friend Circles
   * - There are N students in a class. Some of them are friends, while some
   *   are not. Their friendship is transitive in nature. For example, if A is
   *   a direct friend of B, and B is a direct friend of C, then A is an
   *   indirect friend of C. And we defined a friend circle is a group of
   *   students who are direct or indirect friends.
   * - Given a N*N matrix M representing the friend relationship between
   *   students in the class. If M[i][j] = 1, then the ith and jth students are
   *   direct friends with each other, otherwise not. And you have to output the
   *   total number of friend circles among all the students.
   * Example 1:
   * - Input: {{1,1,0}, {1,1,0}, {0,0,1}}
   *   Output: 2
   * Explanation:
   * - The 0th and 1st students are direct friends, so they are in friend circle
   *   The 2nd student himself is in a friend circle. So return 2.
   * Example 2:
   * - Input: {{1,1,0}, {1,1,1}, {0,1,1}}
   *   Output: 1
   * Explanation:
   * - The 0th and 1st students are direct friends, the 1st and 2nd students
   *   are direct friends,
   * - so the 0th and 2nd students are indirect friends.
   * - All of them are in the same friend circle, so return 1.
   * Note:
   * - N is in range [1,200].
   * - M[i][i] = 1 for all students.
   * - If M[i][j] = 1, then M[j][i] = 1.
   */
  class disjoint_set {
  public:
    /* used for union 2 diff components together, return new root */
    int union_set(int comp_idx_l, int comp_idx_r) {
      int root_idx_l = union_find(comp_idx_l),
          root_idx_r = union_find(comp_idx_r),
          root_idx   = root_idx_l;
      if (root_idx_l == root_idx_r) { return root_idx; }
      if (components[root_idx_l] < components[root_idx_r]) {
        components[root_idx_r] += components[root_idx_l];
        components[root_idx_l] = root_idx_r;
        root_idx = root_idx_r;
      } else {
        components[root_idx_l] += components[root_idx_r];
        components[root_idx_r] = root_idx_l;
        root_idx = root_idx_l;
      }
      components_cnt--;
      return root_idx;
    }

    /* return # of dis-connected components */
    int get_components_cnt() { return components_cnt; }

    disjoint_set(int size) {
      components = vector<int>(size, -1); components_cnt = size;
    }
    virtual ~disjoint_set() {}

  private:

    /* used for find the root of a certain components with path compression */
    int union_find(int comp_idx) {
      int root_idx = comp_idx;
      while (components[root_idx] >= 0) { root_idx = components[root_idx]; }
      while (components[comp_idx] >= 0) { components[comp_idx] = root_idx;
                                          comp_idx = components[comp_idx]; }
      return root_idx;
    }

    /* id mark diff. components, value mark its parent, negative value in root for size */
    vector<int> components;
    int components_cnt;
  };

  static int calc_friend_circle_cnt(const vector<vector<int>> & matrix) {
    disjoint_set circles(matrix.size());
    for (int i = 0; i < matrix.size(); i++) {
      for (int j = i + 1; j < matrix[i].size(); j++) {
        if (1 == matrix[i][j]) { circles.union_set(i, j); }
      }
    }
    return circles.get_components_cnt();
  }

  static void test_calc_friend_circle_cnt() {
    int cnt = 0;
    vector<int> test_output = { 1, 2, 1 };
    vector<vector<vector<int>>> test_input = { {{1,1,1},{1,1,1},{1,1,1}}, {{1,1,0}, {1,1,0}, {0,0,1}}, {{1,1,0}, {1,1,1}, {0,1,1}} };
    cout << "26. test_calc_friend_circle_cnt" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      cnt = calc_friend_circle_cnt(test_input[i]);
      cout << cnt << " <=> " << test_output[i] << endl;
      assert(cnt == test_output[i]);
    }
  }

  /**
   * 684. Redundant Connection
   * - In this problem, a tree is an undirected graph that is connected and
   *   has no cycles.
   * - The given input is a graph that started as a tree with N nodes (with
   *   distinct values 1, 2, ..., N), with one additional edge added. The
   *   added edge has two different vertices chosen from 1 to N, and was not
   *   an edge that already existed.
   * - The resulting graph is given as a 2D-array of edges. Each element of
   *   edges is a pair [u, v] with u < v, that represents an undirected edge
   *   connecting nodes u and v.
   * - Return an edge that can be removed so that the resulting graph is a
   *   tree of N nodes. If there are multiple answers, return the answer that
   *   occurs last in the given 2D-array. The answer edge [u, v] should be in
   *   the same format, with u < v.
   * Example 1:
   * - Input: {{1,2}, {1,3}, {2,3}}
   *   Output: {2,3}
   *   Explanation: The given undirected graph will be like this:
   *     1
   *    / \
   *   2 - 3
   * Example 2:
   * - Input: {{1,2}, {2,3}, {3,4}, {1,4}, {1,5}}
   *   Output: {1,4}
   *   Explanation: The given undirected graph will be like this:
   *   5 - 1 - 2
   *       |   |
   *       4 - 3
   * Note:
   * - The size of the input 2D-array will be between 3 and 1000.
   * - Every integer represented in the 2D-array will be between 1 and N,
   *   where N is the size of the input array.
   * Intuition:
   * - a necessary edge means it will connect parts when it is been added.
   * - otherwise, it is redundant.
   */
  static vector<int> get_redundant_edges(const vector<vector<int>> & udag) {
    vector<int> redundant_edge;
    int prev_comp_cnt = 0;
    disjoint_set edge_set(udag.size());
    for (auto & pair : udag) {
      edge_set.union_set(pair.front() - 1, pair.back() - 1);
      if (prev_comp_cnt == edge_set.get_components_cnt()) {
        redundant_edge = pair; break;
      } else {
        prev_comp_cnt = edge_set.get_components_cnt();
      }
    }
    return redundant_edge;
  }

  static void test_get_redundant_edges() {
    vector<vector<int>> test_output = { {2,3}, {1,4} };
    vector<vector<vector<int>>> test_input = { {{1,2}, {1,3}, {2,3}}, {{1,2}, {2,3}, {3,4}, {1,4}, {1,5}} };
    cout << "27. test_get_redundant_edges" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      print_all_elem<int>(test_output[i]);
      cout << "<=>" << endl;
      print_all_elem<int>(get_redundant_edges(test_input[i]));
    }
  }

  /**
   * 74. Search a 2D Matrix
   * - Write an efficient algorithm that searches for a value in an mxn matrix.
   *   This matrix has the following properties:
   * - Integers in each row are sorted from left to right.
   * - The 1st integer of each row is greater than last integer of previous row
   * Example 1:
   * Input:
   * matrix = { {1,   3,  5,  7}, {10, 11, 16, 20}, {23, 30, 34, 50} }
   * target = 3
   * Output: true
   * Example 2:
   * Input:
   * matrix = { {1,   3,  5,  7}, {10, 11, 16, 20}, {23, 30, 34, 50} }
   * target = 13
   * Output: false
   */
  static bool is_value_existed(const vector<vector<int>> & matrix, int target) {
    for (int i = matrix.size() - 1; i >= 0; i--) {
      if (matrix[i].empty() || matrix[i][0] > target) { continue; }
      for (int j = 0; j < matrix[i].size(); j++) {
        if (matrix[i][j] == target) { return true; }
      }
      break;
    }
    return false;
  }

  static void test_is_value_existed() {
    bool result = false;
    vector<bool> test_output = { true, true, false };
    vector<int> test_input_1 = { 3, 3, 13 };
    vector<vector<vector<int>>> test_input_0 = { {{1, 3}}, { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 50} },
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 50} } };
    cout << "28. test_is_value_existed" << endl;
    for (int i = 0; i < test_input_0.size(); i++) {
      result = is_value_existed(test_input_0[i], test_input_1[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  static int binary_search_for_row_less_or_equal_than(const vector<vector<int>> & matrix,
                                                      int min, int max, int target) {
    int mid = (min + max) / 2;
    if (matrix.empty() || mid < 0 || mid >= matrix.size() ||
        matrix[mid].empty()) { return -1; }
    while (min < max) {
      if (matrix[mid].empty()) { return -1; }
      if (matrix[mid][0] == target) { return mid; }
      else if (target < matrix[mid][0]) { max = mid - 1; }
      else { min = mid + 1; }
      mid = (min + max) / 2;
    }
    if (matrix[mid][0] > target) { return mid - 1; }
    return mid;
  }

  /* 0 1 -> (mid 0 min 0 max -1 | min 1 max 1) 0 2 -> (mid 1 min 0 max 0 | min 2, max 2) */
  static int binary_search_from_sorted_vector(const vector<int> & vector,
                                              int min, int max, int target) {
    if (vector.empty()) { return -1; }
    int mid = (min + max) / 2;
    while (min <= max) {
      if (target == vector[mid]) { return mid; }
      else if (target < vector[mid]) { max = mid - 1; }
      else { min = mid + 1; }
      mid = (min + max) / 2;
    }
    return -1;
  }

  static bool fast_is_value_existed(const vector<vector<int>> & matrix, int target) {
    int row = binary_search_for_row_less_or_equal_than(matrix, 0, matrix.size() - 1, target);
    if (-1 == row) { return false; }
    int col = binary_search_from_sorted_vector(matrix[row], 0, matrix[row].size() - 1, target);
    if (-1 == col) { return false; }
    return true;
  }

  static void test_fast_is_value_existed() {
    bool result = false;
    vector<bool> test_output = { false, false, true, true, false, false, true, true, false };
    vector<int> test_input_1 = { 1, 1, 98, 1, 99, -1, 3, 3, 13 };
    vector<vector<vector<int>>> test_input_0 = { {}, {{}},
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 98} },
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 50} },
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 98} },
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 50} },
                                                 { {1, 3} },
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 50} },
                                                 { {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 50} } };
    cout << "29. test_fast_is_value_existed" << endl;
    for (int i = 0; i < test_input_0.size(); i++) {
      result = fast_is_value_existed(test_input_0[i], test_input_1[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 78. Subsets
   * Given a set of distinct integers, nums, return all possible subsets (the power set).
   * Note: The solution set must not contain duplicate subsets.
   * Example:
   * Input: nums = {1,2,3}
   * Output:
   * { {3}, {1}, {2}, {1,2,3}, {1,3}, {2,3}, {1,2}, {} }
   * - goal is to return sum(c(i, n))
   * - dfs search for all uniq-path with diff. nodes.
   * - then path + curr-node + reamining nodes
   * - {} | 3 | { 1, 2 }
   *   { 3 } | 2 | { 1 }
   *   { 3 } | 1 | {}
   * - {} | 2 | { 1 }
   * - {} | 1 | { }
   * - the core of recur is to add all subset ends with max value of an elem (say after sort)
   */
  static void gen_subsets_recur(vector<int> & curr_input, /* remaining elem */
                                vector<int> & curr_subset, /* existing sets */
                                vector<vector<int>> & subsets) {
    if (curr_input.empty()) { return; }
    vector<int> buffer = curr_input;
    while (!curr_input.empty()) {
      curr_subset.push_back(curr_input.back());
      subsets.push_back(curr_subset);
      curr_input.pop_back();
      gen_subsets_recur(curr_input, curr_subset, subsets);
      curr_subset.pop_back();
    }
    curr_input = buffer;
  }

  static vector<vector<int>> gen_subsets(vector<int> & input) {
    vector<int> curr_path;
    vector<vector<int>> subsets = {{}};
    gen_subsets_recur(input, curr_path, subsets);
    return subsets;
  }

  static void test_gen_subsets() {
    vector<vector<int>> result;
    vector<vector<int>> test_input = { {}, {1,2,3} };
    vector<vector<vector<int>>> test_output = { {{}}, { {3}, {1}, {2}, {1,2,3}, {1,3}, {2,3}, {1,2}, {} } };
    cout << "30. test_gen_subsets" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = gen_subsets(test_input[i]);
      print_all_elem_vec<int>(result); cout << "<=>" << endl;
      print_all_elem_vec<int>(test_output[i]);
    }
  }

  static void gen_uniq_subsets_recur(vector<int> & input, vector<int> & path,
                                     vector<vector<int>> & subsets) {
    if (input.empty()) { return; }
    vector<int> buffer = input;
    int curr_elem = 0;
    while (!input.empty()) {
      curr_elem = input.back();
      path.push_back(curr_elem);
      input.pop_back();
      subsets.push_back(path);
      gen_uniq_subsets_recur(input, path, subsets);
      path.pop_back();
      while(!input.empty() && input.back() == curr_elem) {
        input.pop_back();
      }
    }
    input = buffer;
  }

  static vector<vector<int>> gen_uniq_subsets(vector<int> & input) {
    vector<int> path;
    vector<vector<int>> subsets = {{}};
    sort(input.begin(), input.end());
    gen_uniq_subsets_recur(input, path, subsets);
    return subsets;
  }

  static void test_gen_uniq_subsets() {
    vector<vector<int>> result;
    vector<vector<int>> test_input = { {}, {1,2,2} };
    vector<vector<vector<int>>> test_output = { {{}}, {{1},{2},{1,2,2},{1,2},{2,2},{}} };
    cout << "31. test_gen_uniq_subsets" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = gen_uniq_subsets(test_input[i]);
      print_all_elem_vec<int>(result); cout << "<=>" << endl;
      print_all_elem_vec<int>(test_output[i]);
    }
  }

  /**
   * 73. Set Matrix Zeroes
   * - Given a m x n matrix, if an element is 0, set its entire row and column
   *   to 0. Do it in-place.
   * Example 1:
   * Input: { {1,1,1}, {1,0,1}, {1,1,1} }
   * Output: { {1,0,1}, {0,0,0}, {1,0,1} }
   * Example 2:
   * Input: { {0,1,2,0}, {3,4,5,2}, {1,3,1,5} }
   * Output: { {0,0,0,0}, {0,4,5,0}, {0,3,1,0} }
   * - A straight forward solution using O(mn) space is probably a bad idea.
   *   A simple improvement uses O(m + n) space, but not the best solution.
   *   Could you devise a constant space solution?
   */
  static void set_zeroes_recur(vector<vector<int>> & matrix,
                               int start_row, int end_row,
                               int start_col, int end_col) {
    if (start_row > end_row || start_col > end_col) { return; }
    int row = (start_row + end_row) / 2,
        col = (start_col + end_col) / 2;
    if (matrix[row][col] == 0) {
      for (int i = 0; i < matrix.size(); i++) {
        if (matrix[i][col] != 0) { matrix[i][col] = INT_MIN; }
      }
      for (int i = 0; i < matrix[row].size(); i++) {
        if (matrix[row][i] != 0) { matrix[row][i] = INT_MIN; }
      }
    }
    set_zeroes_recur(matrix, start_row, row - 1, start_col, col - 1);
    set_zeroes_recur(matrix, start_row, row - 1, col + 1, end_col);
    set_zeroes_recur(matrix, row + 1, end_row, start_col, col - 1);
    set_zeroes_recur(matrix, row + 1, end_row, col + 1, end_col);
    if (matrix[row][col] != 0) {
      set_zeroes_recur(matrix, row, row, start_col, col - 1);
      set_zeroes_recur(matrix, row, row, col + 1, end_col);
      set_zeroes_recur(matrix, start_row, row - 1, col, col);
      set_zeroes_recur(matrix, row + 1, end_row, col, col);
    }
  }

  static void set_zeroes(vector<vector<int>> & matrix) {
    if (matrix.empty()) { return; }
    set_zeroes_recur(matrix, 0, matrix.size() - 1, 0, matrix.front().size() - 1);
    for (int i = 0; i < matrix.size(); i++) {
      for (int j = 0; j < matrix[i].size(); j++) {
        if (matrix[i][j] == INT_MIN) { matrix[i][j] = 0; }
      }
    }
  }

  static void test_set_zeroes() {
    vector<vector<vector<int>>> test_output = { {}, {{}}, {{0},{0},{0}}, {{0,0,0}},
                                                { {1,0,1}, {0,0,0}, {1,0,1} },
                                                { {0,0,0,0}, {0,4,5,0}, {0,3,1,0} } };
    vector<vector<vector<int>>> test_input = { {}, {{}}, {{1},{0},{1}}, {{1,0,1}},
                                               { {1,1,1}, {1,0,1}, {1,1,1} },
                                               { {0,1,2,0}, {3,4,5,2}, {1,3,1,5} } };
    cout << "32. test_set_zeroes" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      print_all_elem_vec<int>(test_output[i]); cout << "<=>" << endl;
      set_zeroes(test_input[i]);
      print_all_elem_vec<int>(test_input[i]);
    }
  }

  static void lean_set_zeroes(vector<vector<int>> & matrix) {
    if (matrix.empty() || matrix.front().empty()) { return; }
    bool is_first_row_contains_zero = false;
    bool is_first_col_contains_zero = false;

    for (int i = 0; i < matrix.size(); i++) {
      if (matrix[i][0] == 0){ is_first_col_contains_zero = true; break; }
    }
    for (int j = 0; j < matrix.front().size(); j++) {
      if (matrix[0][j] == 0){ is_first_row_contains_zero = true; break; }
    }
    for (int i = 1; i < matrix.size(); i++) {
      for (int j = 1; j < matrix[i].size(); j++) {
        if (0 == matrix[i][j]) { matrix[0][j] = 0; matrix[i][0] = 0; }
      }
    }
    for (int i = 1; i < matrix.size(); i++) {
      if (0 == matrix[i][0]) {
        for (int j = 0; j < matrix[i].size(); j++){ matrix[i][j] = 0; }
      }
    }

    for (int j = 1; j < matrix.front().size(); j++) {
      if (0 == matrix[0][j]) {
        for (int i = 0; i < matrix.size(); i++){ matrix[i][j] = 0; }
      }
    }

    if (true == is_first_row_contains_zero) {
      for (int j = 0; j < matrix.front().size(); j++) { matrix[0][j] = 0; }
    }
    if (true == is_first_col_contains_zero) {
      for (int i = 0; i < matrix.size(); i++) { matrix[i][0] = 0; }
    }
  }

  static void test_lean_set_zeroes() {
    vector<vector<vector<int>>> test_output = { {}, {{}}, {{0},{0},{0}}, {{0,0,0}},
                                                { {1,0,1}, {0,0,0}, {1,0,1} },
                                                { {0,0,0,0}, {0,4,5,0}, {0,3,1,0} } };
    vector<vector<vector<int>>> test_input = { {}, {{}}, {{1},{0},{1}}, {{1,0,1}},
                                               { {1,1,1}, {1,0,1}, {1,1,1} },
                                               { {0,1,2,0}, {3,4,5,2}, {1,3,1,5} } };
    cout << "33. test_lean_set_zeroes" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      print_all_elem_vec<int>(test_output[i]); cout << "<=>" << endl;
      lean_set_zeroes(test_input[i]);
      print_all_elem_vec<int>(test_input[i]);
    }
  }

  /**
   * 64. Minimum Path Sum
   * - Given a m x n grid filled with non-negative numbers, find a path from
   *   top left to bottom right which minimizes sum of all numbers along path.
   * Note: You can only move either down or right at any point in time.
   * Example:
   * Input: {{1,3,1},{1,5,1},{4,2,1}}
   * Output: 7
   * Explanation: Because the path 1->3->1->1->1 minimizes the sum.
   * Intuition:
   * - let max-sum(i, j) max sum path from m(0, 0) ends at m(i, j)
   * - max-sum(i, j) = max{ max-sum(i - 1, j) + m[i, j],
   *                        max-sum(i, j - 1) + m[i, j] }
   * - only prev. row needed as we calc from left -> right.
   * - goal is to calc & return the last elem.
   */
  static int calc_min_sum_path(const vector<vector<int>> & matrix) {
    if (matrix.empty() || matrix.front().empty()) { return -1; }
    vector<int> mslookup = matrix.front();
    for (int i = 1; i < mslookup.size(); i++) {
      mslookup[i] += mslookup[i - 1];
    }
    for (int i = 1; i < matrix.size(); i++) {
      for (int j = 0; j < matrix[i].size(); j++) {
        mslookup[j] = matrix[i][j] + mslookup[j];
        if (j > 0) {
          mslookup[j] = min(mslookup[j], matrix[i][j] + mslookup[j - 1]);
        }
      }
    }
    return mslookup.back();
  }
  static void test_calc_min_sum_path() {
    int result = 0;
    vector<int> test_output = { 7 };
    vector<vector<vector<int>>> test_input = { {{1,3,1},{1,5,1},{4,2,1}} };
    cout << "34. test_calc_max_sum_path" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_min_sum_path(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 62. Unique Paths
   * - A robot is located at the top-left corner of a m x n grid (marked Start
   *   in the diagram below).
   * - The robot can only move either down or right at any point in time. The
   *   robot is trying to reach the bottom-right corner of the grid (marked
   *   Finish in the diagram below).
   * - How many possible unique paths are there?
   * Example 1:
   * Input: m = 3, n = 2
   * Output: 3
   * Explanation:
   * Example 2:
   * Input: m = 7, n = 3
   * Output: 28
   * Intuition:
   * - let cplookup(i, j) denotes # of uniq-paths to m(i, j);
   * - cplookup(i, j) = cplookup(i - 1, j) + cplookup(i, j - 1);
   * - only previous row needed if we calc from left -> right.
   */
  static int cnt_uniq_paths(int row_cnt, int col_cnt) {
    if (row_cnt <= 0 || col_cnt <= 0) { return 0; }
    vector<int> cplookup(col_cnt, 1);
    for (int i = 1; i < row_cnt; i++) {
      for (int j = 0; j < col_cnt; j++) {
        cplookup[j] = (0 == j) ? (cplookup[j]) : (cplookup[j] + cplookup[j - 1]);
      }
    }
    return cplookup.back();
  }

  static void test_cnt_uniq_paths() {
    int result = 0;
    vector<int> test_output = { 3, 28 };
    vector<vector<int>> test_input = { { 3, 2 }, { 7, 3 } };
    cout << "35. test_cnt_uniq_paths" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = cnt_uniq_paths(test_input[i].front(), test_input[i].back());
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 63. Unique Paths II
   * - 1 means no-pass, Input: {{0,0,0},{0,1,0},{0,0,0}} Output: 2
   */
  static int cnt_uniq_paths_ii(const vector<vector<int>> & matrix) {
    if (matrix.empty() || matrix.front().empty()) { return 0; }
    vector<int> cplookup(matrix.front().size(), 1);
    if (matrix[0][0] == 1) { return 0; }
    for (int i = 1; i < cplookup.size(); i++) {
      if (matrix[0][i] == 1) { cplookup[i] = 0; } else { cplookup[i] = cplookup[i - 1]; }
    }
    for (int i = 1; i < matrix.size(); i++) {
      for (int j = 0; j < matrix[i].size(); j++) {
        if (matrix[i][j] == 1) { cplookup[j] = 0; continue; }
        cplookup[j] = (0 == j) ? (cplookup[j]) : (cplookup[j] + cplookup[j - 1]);
      }
    }
    return cplookup.back();
  }

  static void test_cnt_uniq_paths_ii() {
    int result = 0;
    vector<int> test_output = { 0, 2 };
    vector<vector<vector<int>>> test_input = {{{1,0,0},{0,1,0},{0,0,0}},{{0,0,0},{0,1,0},{0,0,0}}};
    cout << "36. test_cnt_uniq_paths_ii" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = cnt_uniq_paths_ii(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 57. Insert Interval
   * - Given a set of non-overlapping intervals, insert a new interval into the
   *   intervals (merge if necessary).
   * - You may assume that the intervals were initially sorted by start times.
   * Example 1:
   * - Input: intervals = {{1,3},{6,9}}, newInterval = {2,5}
   *   Output: {{1,5},{6,9}}
   * Example 2:
   * - Input: intervals = {{1,2},{3,5},{6,7},{8,10},{12,16}}, newInterval = {4,8}
   *   Output: {{1,2},{3,10},{12,16}}
   * - Explanation: new interval {4,8} overlaps with {3,5},{6,7},{8,10}
   * - struct Interval {
   *     int start;
   *     int end;
   *     Interval() : start(0), end(0) {}
   *     Interval(int s, int e) : start(s), end(e) {}
   *   };
   * Intuition:
   * - for all input intervals, there is no overlapping, -> disjoint from each other.
   * - use binary search to identify the zone to insert our new range.
   * - use a list to keep track of all the iterator from input for ease of insertion.
   * - merge interval if necessary.
   * - case I: new-intv [ ... ] or [ ... ] new-intv
   * - case II: [ ... ] new-intv [ ... ]
   * - case III: [ old-intv-0 & new-intv ] or [ new-intv & old-intv-1 ] or
   *             [ old-intv-0 & new-intv & old-intv-1 ]
   *   start                      end
   *     |-------------------------|
   * - |---|   |---|   |---|    |---|    |---|
   *     ^                         ^
   */
  class Interval {
  public:
    Interval(int s = 0, int e = 0) : start(s), end(e) {}
    virtual ~Interval() {}
    int start, end;
    friend ostream & operator<<(ostream & os, const Interval & intv) {
      os << "[ " << intv.start << ", " << intv.end << " ]"; return os;
    }
  };
/*
  static bool is_intv_within_range(interval & intv, interval & intv_chk) {
    return (intv.start <= intv_chk.start && intv_chk.end <= intv.end);
  }
*/
  typedef Interval interval;

  static bool is_intv_preceed_range(interval & intv, interval & intv_chk) {
    return (intv_chk.end < intv.start);
  }

  static bool is_intv_after_range(interval & intv, interval & intv_chk) {
    return (intv_chk.start > intv.end);
  }

  static bool is_intervl_valid(interval & intv) {
    return (intv.start >= 0 && intv.end >= 0 && intv.start <= intv.end);
  }

  static vector<interval> insert_new_interval(vector<interval>& intervals,
                                              interval new_interval) {
    if (!is_intervl_valid(new_interval)) { return intervals; }
    vector<interval> new_interval_vec;

    /* - case I: new-intv or new-intv [ ... ] or [ ... ] new-intv */
    if (true == intervals.empty()) {
      new_interval_vec.push_back(new_interval); return new_interval_vec;
    }
    if (true == is_intv_preceed_range(intervals.front(), new_interval)) {
      new_interval_vec.push_back(new_interval);
      for (auto & intv : intervals) { new_interval_vec.push_back(intv); }
      return new_interval_vec;
    }
    if (true == is_intv_after_range(intervals.back(), new_interval)) {
      for (auto & intv : intervals) { new_interval_vec.push_back(intv); }
      new_interval_vec.push_back(new_interval);
      return new_interval_vec;
    }
    /*
     *   start                     end
     *     |------------------------|
     * - |---|   |---|   |---|    |---|    |---|
     *     ^                        ^
     *   start                end
     *     |-------------------|
     * - |---|   |---|   |---|    |---|    |---|
     *     ^               ^
     *       start                 end
     *         |--------------------|
     * - |---|   |---|   |---|    |---|    |---|
     *             ^                ^
     *       start             end
     *         |----------------|
     * - |---|   |---|   |---|    |---|    |---|
     *             ^       ^
     *       start    end
     *         |-------|
     * - |---|   |---|   |---|    |---|    |---|
     *            ^ ^
     *       start             end
     *         |----------------|
     * - |---|                    |---|
     *     ^                        ^ (cross)
     */
    /* start_itr -> 1st elem, end_itr -> last elem */
    vector<interval>::iterator start_itr = intervals.begin();
    vector<interval>::iterator end_itr = intervals.end(); end_itr--;

    /* as we short circuit before, so start_itr is guaranteed not to be end() */
    while (true == is_intv_after_range(* start_itr, new_interval)) {
      new_interval_vec.push_back(* start_itr); start_itr++;
    }
    /* as we short circuit before, so start_itr is guaranteed not to exceed begin(), but could cross with start_itr by 1 */
    while (true == is_intv_preceed_range(* end_itr, new_interval)) { end_itr--; }

    /* check if start & end cross, actually does not matter, as we always check the time */
    int new_start_time = 0, new_end_time = 0;
    new_start_time = min(start_itr->start, new_interval.start);
    new_end_time = max(end_itr->end, new_interval.end);
    new_interval_vec.push_back(interval(new_start_time, new_end_time));

    for (end_itr++; end_itr != intervals.end(); end_itr++) { new_interval_vec.push_back(* end_itr); }

    return new_interval_vec;
  }

  static void test_insert_new_interval() {
    vector<interval> test_input_1 = { interval(4, 5), interval(1,11), interval(1,11), interval(10,11), interval(10,11), interval(0,1), interval(0,1), interval(2,5), interval(4,8) };
    vector<vector<interval>> test_input_0 = { { interval(2,3),interval(6,9) }, { interval(2,3) }, { interval(2,3),interval(6,9) }, { }, { interval(2,3),interval(6,9) }, { interval(2,3),interval(6,9) }, { interval(1,3),interval(6,9) }, { interval(1,3),interval(6,9) }, { interval(1,2),interval(3,5),interval(6,7),interval(8,10),interval(12,16) } };
    vector<vector<interval>> test_output = { { interval(2,3),interval(4,5),interval(6,9) }, { interval(1,11) }, { interval(1,11) }, { interval(10,11) }, { interval(2,3),interval(6,9),interval(10,11) }, { interval(0,1), interval(2,3),interval(6,9) }, { interval(0,3),interval(6,9) }, { interval(1,5),interval(6,9) }, { interval(1,2),interval(3,10),interval(12,16) } };
    cout << "37. test_insert_new_interval" << endl;
    for (int i = 0; i < test_input_0.size(); i++) {
			print_all_elem<interval>(test_output[i]); cout << "<=>" << endl;
			print_all_elem<interval>(insert_new_interval(test_input_0[i], test_input_1[i]));
    }
  }

  /**
   * 56. Merge Intervals
   * Given a collection of intervals, merge all overlapping intervals.
   * Example 1:
   * Input: [[1,3],[2,6],[8,10],[15,18]]
   * Output: [[1,6],[8,10],[15,18]]
   * Explanation: Since intervals [1,3] and [2,6] overlaps, merge them into [1,6].
   * Example 2:
   * Input: [[1,4],[4,5]]
   * Output: [[1,5]]
   * Explanation: Intervals [1,4] and [4,5] are considerred overlapping.
   */
  /* sort by start time and finishing time asc */
  static bool interval_compare_func(interval & intv_l, interval & intv_r) {
    if (intv_l.start < intv_r.start) { return true; }
    if (intv_l.start == intv_r.start && intv_l.end < intv_r.end ) { return true; }
    return false;
  }

  static vector<interval> merge_intervals(vector<interval> & intervals) {
    if (intervals.size() < 2) { return intervals; }

    vector<interval> new_interval_vec;
    sort(intervals.begin(), intervals.end(), interval_compare_func);
    vector<interval>::iterator start_itr = intervals.begin();
    vector<interval>::iterator end_itr = intervals.begin();
    /* after sort: [ 1, 4 ], [ 2, 4 ], [ 3, 4 ], [ 2, 5 ], [ 3, 5 ] ...  */
    for (; start_itr != intervals.end(); ) {
      int new_start_time = start_itr->start;
      int new_end_time = start_itr->end;
      while(end_itr != intervals.end() && end_itr->start <= new_end_time) {
        new_start_time = min(new_start_time, end_itr->start);
        new_end_time = max(new_end_time, end_itr->end);
        start_itr = end_itr;
        end_itr++;
      }
      new_interval_vec.push_back(interval(new_start_time, new_end_time));
      start_itr = end_itr;
    }
    return new_interval_vec;
  }

  static void test_merge_intervals() {
    vector<vector<interval>> test_input = { { interval(4, 5),interval(2,3),interval(6,9) }, { interval(1,11),interval(2,3) }, { interval(1,11),interval(2,3),interval(6,9) }, {interval(10,11)}, { interval(10,11),interval(2,3),interval(6,9) }, { interval(0,1),interval(2,3),interval(6,9) }, { interval(0,1),interval(1,3),interval(6,9) }, { interval(2,5),interval(1,3),interval(6,9) }, { interval(1,2),interval(3,5),interval(6,7),interval(8,10),interval(12,16),interval(4,8) } };
    vector<vector<interval>> test_output = { { interval(2,3),interval(4,5),interval(6,9) }, { interval(1,11) }, { interval(1,11) }, { interval(10,11) }, { interval(2,3),interval(6,9),interval(10,11) }, { interval(0,1), interval(2,3),interval(6,9) }, { interval(0,3),interval(6,9) }, { interval(1,5),interval(6,9) }, { interval(1,2),interval(3,10),interval(12,16) } };
    cout << "38. test_merge_intervals" << endl;
    for (int i = 0; i < test_input.size(); i++) {
			print_all_elem<interval>(test_output[i]); cout << "<=>" << endl;
			print_all_elem<interval>(merge_intervals(test_input[i]));
    }
  }

  /**
   * 55. Jump Game
   * - Given an array of non-negative integers, you are initially positioned at
   *   the first index of the array.
   * - Each element in the array represents your maximum jump length at position
   * Determine if you are able to reach the last index.
   * Example 1:
   * Input: {2,3,1,1,4}
   * Output: true
   * Explanation: Jump 1 step from index 0 to 1, then 3 steps to the last index.
   * Example 2:
   * Input: {3,2,1,0,4}
   * Output: false
   * Explanation: You will always arrive at index 3 no matter what. Its maximum
   *              jump length is 0, which makes it impossible to reach the last index.
   * Intuition:
   * - no matter where we start to jump, once jumped, then previous credit goes away.
   *   potentially no DP needed.
   * - to be able to reach i, must be a hop point j satisfies dist(i - j) <= arr[j]
   *   where 0 <= j < i, then start from the end, go back to nearest possible point.
   *   it also says if we can jump to i, we should be able to jump to i - 1
   * - we should always be able get our way back to origin point.
   */
  static bool is_last_pos_reachable(const vector<int> & jump_vec) {
    bool is_reachable = false;
    if (jump_vec.size() < 2) { return true; } /* no need to jump at all */
    int end_pos = jump_vec.size() - 1, hop_pos = end_pos - 1;
    while (hop_pos >= 0) {
      while (hop_pos >= 0 && jump_vec[hop_pos] < (end_pos - hop_pos)) { hop_pos--; }
      if (hop_pos >= 0) { end_pos = hop_pos; }; hop_pos--;
    }
    is_reachable = (end_pos == 0);
    return is_reachable;
  }

  static void test_is_last_pos_reachable() {
    bool result = false;
    vector<bool> test_output = { true, false, true, true, true, false };
    vector<vector<int>> test_input = { {4,0,0,0,4}, {0,3,0,0,4},{2,3,0,0,4}, {2,3,1,0,4}, {2,3,1,1,4}, {3,2,1,0,4} };
    cout << "39. test_is_last_pos_reachable" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = is_last_pos_reachable(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 45. Jump Game II
   * - Given an array of non-negative integers, you are initially positioned
   *   at the first index of the array.
   * - Each element in the array represents your maximum jump length at pos.
   * - Your goal is to reach the last index in the minimum number of jumps.
   * Example:
   * - Input: [2,3,1,1,4]
   *   Output: 2
   * Explanation: The minimum number of jumps to reach the last index is 2.
   *              Jump 1 step from index 0 to 1, then 3 steps to the last index
   * Note:
   * - You can assume that you can always reach the last index.
   * Intuition:
   * - usually greedy / dp is good for optimization problem.
   * - assume it takes min k jumps to reach point i & last hop pos is j
   *   then it took k - 1 jumps to reach pos(j) & jump[j] >= i - j
   * - is k - 1 also the min jumps from 0?
   * - assume it took k - 2 jumps to reach j, then we only need k - 1 jumps for i
   *   then we got contradict, so we have optimal subproblem, which is
   * - min(jump from 0 to i) = min(jump from 0 to j) + 1, where j is last hop pos.
   * - problem remains how we find j? given both i & j is reachable?
   *   Guess would be j is the furtherest node reachable to i:
   *   (we need to proof j is reachable from 0)
   *
   * - if j is not reachable, while i is still reachable, then we know there exist
   *   a node to left of j can reach i (contradict with assumption), or we have a
   *   node to the right of j can reach i & it is reachable -> j is also reachable
   *
   * - each time to get j could be tricky, pre-calc for max pos reachable?
   *     1  3  5  4  5
   *   { 1, 2, 3, 1, 1, 4 } vector of things?
   *     0  1  2  3  4  5
   */
  static int get_min_jump(const vector<int> & jump_vec) {
    int jump_cnt = 0;
    if (jump_vec.size() <= 1) { return jump_cnt; }
    vector<int> leftmost_pos_reachable(jump_vec.size(), INT_MAX);
    int curr_span = 0;
    for (int i = 0; i < jump_vec.size() - 1; i++) {
      int target_pos = min(int(jump_vec.size() - 1), int(i + jump_vec[i]));
      for (int j = curr_span + 1; j <= target_pos; j++) { leftmost_pos_reachable[j] = i; }
      curr_span = max(target_pos, curr_span);
    }
    for (int end_pos = jump_vec.size() - 1, hop_pos = end_pos - 1; hop_pos >= 0; ) {
      if (leftmost_pos_reachable[end_pos] != INT_MAX) {
        hop_pos = leftmost_pos_reachable[end_pos];
      } else {
        /* until hop_pos is 1 node ahead to the left of target pos */
        while (hop_pos >= 0 && jump_vec[hop_pos] >= (end_pos - hop_pos)){ hop_pos--; }
        hop_pos++;
      }
      jump_cnt++; end_pos = hop_pos; hop_pos--;
    }
    return jump_cnt;
  }

  static void test_get_min_jump() {
    int result = false;
    vector<int> test_output = { 2, 0, 0, 2, 1, 2, 2, 2, 3 };
    vector<vector<int>> test_input = { {4,1,1,3,1,1,1}, {1}, {}, {2,3,1,1,4}, {4,0,0,0,4}, {2,3,0,0,4}, {2,3,1,0,4}, {2,3,1,1,4}, {1,2,3,1,1,4} };
    cout << "40. test_get_min_jump" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = get_min_jump(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 658. Find K Closest Elements
   * - Given a sorted array, two integers k and x, find the k closest elements
   *   to x in the array. The result should also be sorted in ascending order.
   *   If there is a tie, the smaller elements are always preferred.
   * Example 1:
   * - Input: {1,2,3,4,5}, k=4, x=3
   *   Output: {1,2,3,4}
   * Example 2:
   * - Input: {1,2,3,4,5}, k=4, x=-1
   *   Output: {1,2,3,4}
   * Note:
   * - The value k is positive and will always be smaller than the length of the
   *   sorted array.
   * - Length of the given array is positive and will not exceed 10^4
   * - Absolute value of elements in the array and x will not exceed 10^4
   */
  int bi_search_for_elem_equal_or_less_than(vector<int> & input, int target) {
    int low = 0, high = input.size() - 1, mid = (low + high) / 2;
    while (low < high) {
      if (input[mid] == target) { return mid; }
      if (input[mid] < target) { low = mid + 1; } else { high = mid - 1; }
      mid = (low + high) / 2;
    }
    if (input[high] <= target) { return high; }
    return mid;
  }

  vector<int> find_k_closest_elems(vector<int> & input, int k, int origin) {
    deque<int> kc_set;
    int pivot_id = bi_search_for_elem_equal_or_less_than(input, origin);
    int backward_idx = pivot_id, forward_idx = pivot_id + 1;

    while (kc_set.size() < k) {
      if (backward_idx >= 0 && forward_idx < input.size()) {
        if (abs(input[backward_idx] - origin) <= abs(input[forward_idx] - origin)) {
          kc_set.push_front(input[backward_idx--]);
        } else {
          kc_set.push_back(input[forward_idx++]);
        }
        continue;
      } else if (backward_idx >= 0) {
        kc_set.push_front(input[backward_idx--]); continue;
      } else {
        kc_set.push_back(input[forward_idx++]); continue;
      }
    }
    return vector<int>(kc_set.begin(), kc_set.end());
  }

  static void test_find_k_closest_elems() {
    vector<vector<int>> test_input_0 = { {-9,1,1,5,6,7,8,8}, {0,0,0,1,1,5,6,7,8,8},{0,0,0,1,1,3,3,5,6,7,8,8},{0,0,0,1,1,3,5,6,7,8,8}, {0,0,0,1,3,5,6,7,8,8}, {1,2,3,4,5}, {1,2,3,4,5} };
    vector<int> test_input_1 = { 3, 2, 2, 2, 2, 4, 4 };
    vector<int> test_input_2 = { 2, 2, 2, 2, 2, 3, -1 };
    vector<vector<int>> test_output = { {1,1,5}, {1,1}, {1,1}, {1,1}, {1,3}, {1,2,3,4}, {1,2,3,4} };
    cout << "41. test_find_k_closest_elems" << endl;
    for (int i = 0; i < test_output.size(); i++) {
      print_all_elem<int>(test_output[i]);
      cout << "<=>" << endl;
      print_all_elem<int>(find_k_closest_elems(test_input_0[i], test_input_1[i], test_input_2[i]));
    }
  }
};

int main(void) {
  using array_util::test_calc_two_sum;
  using array_util::test_calc_two_sum_ii;
  using array_util::test_calc_triplets_by_sum;
  using array_util::test_calc_quadruplets_by_sum;

  using array_util::print_all_elem;
  using array_util::print_all_elem_vec;
  using array_util::get_next_permutation_asc;
  using array_util::find_in_rotated_sorted_arr;
  using array_util::calc_longest_seq_len;
  using array_util::find_median_elem;
  using array_util::calc_max_trapping_water;
  using array_util::sort_colors;
  using array_util::fast_sort_colors;
  using array_util::test_is_sudoku_valid;
  using array_util::test_solve_sudoku;
  using array_util::find_duplicate;
  using array_util::check_and_update_board;
  using array_util::test_calendar;
  using array_util::test_find_dup_number;
  using array_util::test_find_missing_num;
  using array_util::test_find_major_num;
  using array_util::test_find_major_numbers_ii;
  using array_util::test_find_min_window_exceed_sum;
  using array_util::test_find_max_product;
  using array_util::test_find_max_product_dp;
  using array_util::test_find_lces;
  using array_util::test_calc_max_profit;
  using array_util::test_calc_max_profit_iii;
  using array_util::test_lean_calc_max_profit_iii;
  using array_util::test_convert_fraction_to_decimal;
  using array_util::test_calc_friend_circle_cnt;
  using array_util::test_get_redundant_edges;
  using array_util::test_is_value_existed;
  using array_util::test_fast_is_value_existed;
  using array_util::test_gen_subsets;
  using array_util::test_gen_uniq_subsets;
  using array_util::test_set_zeroes;
  using array_util::test_lean_set_zeroes;
  using array_util::test_calc_min_sum_path;
  using array_util::test_cnt_uniq_paths;
  using array_util::test_cnt_uniq_paths_ii;
  using array_util::test_insert_new_interval;
  using array_util::test_merge_intervals;
  using array_util::test_is_last_pos_reachable;
  using array_util::test_get_min_jump;
  using array_util::test_find_k_closest_elems;

  cout << "1. get_next_permutation_asc" << endl;
  cout << "[ 6 8 1 3 7 4 0 1 2 3 ] <=> " << endl;
  print_all_elem<int>(get_next_permutation_asc(vector<int>({ 6, 8, 1, 3, 7, 3, 4, 2, 1, 0 })));
  cout << "[ 1 3 2 ] <=> " << endl;
  print_all_elem<int>(get_next_permutation_asc(vector<int>({ 1, 2, 3 })));
  cout << "[ 1 2 3 ] <=> " << endl;
  print_all_elem<int>(get_next_permutation_asc(vector<int>({ 3, 2, 1 })));
  cout << "[ 1 5 1 ] <=> " << endl;
  print_all_elem<int>(get_next_permutation_asc(vector<int>({ 1, 1, 5 })));
  cout << "[ 2 5 1 6 7 8 ] <=> " << endl;
  print_all_elem<int>(get_next_permutation_asc(vector<int>({ 2, 1, 8, 7, 6, 5 })));
  cout << "[ 5 3 6 4 7 9 ] <=> " << endl;
  print_all_elem<int>(get_next_permutation_asc(vector<int>({ 5, 3, 4, 9, 7, 6 })));

  cout << "2. find_in_rotated_sorted_arr" << endl;
  cout << "8 <=> " << find_in_rotated_sorted_arr(vector<int>({ 5, 6, 7, 8, 9, 10, 1, 2, 3 }), 3) << endl;
  cout << "-1 <=> " << find_in_rotated_sorted_arr(vector<int>({ 5, 6, 7, 8, 9, 10, 1, 2, 3 }), 30) << endl;
  cout << "3 <=> " << find_in_rotated_sorted_arr(vector<int>({ 30, 40, 50, 10, 20 }), 10) << endl;
  cout << "2 <=> " << find_in_rotated_sorted_arr(vector<int>({ 4, 5, 6, 7, 8, 9, 1, 2, 3 }), 6) << endl;

  cout << "3. calc_longest_seq_len" << endl;
  cout << "4 <=> " << calc_longest_seq_len(vector<int>({ 100, 4, 200, 3, 1, 2 })) << endl;
  cout << "8 <=> " << calc_longest_seq_len(vector<int>({ 100, 4, 200, 3, 1, 2, 8, 6, 7, 5 })) << endl;

  cout << "4. find_median_elem" << endl;
  cout << "{2, 3, 6, 7, 9} & {1, 4, 8, 10} <=> 6 <=> "
       << find_median_elem(vector<int>({ 2, 3, 6, 7, 9 }),
                           vector<int>({ 1, 4, 8, 10 })) << endl;
  cout << "{1, 4, 8, 10} & {2, 3, 6, 7, 9} <=> 6 <=> "
       << find_median_elem(vector<int>({ 1, 4, 8, 10 }),
                           vector<int>({ 2, 3, 6, 7, 9 })) << endl;
  cout << "{1} & {2} <=> 1.5 <=> "
       << find_median_elem(vector<int>({ 1 }), vector<int>({ 2 })) << endl;

  test_calc_two_sum();
  test_calc_two_sum_ii();
  test_calc_triplets_by_sum();
  test_calc_quadruplets_by_sum();

  cout << "8. calc_max_trapping_water" << endl;
  cout << "6 <=> " << calc_max_trapping_water(vector<int>({ 0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1 })) << endl;
  cout << "11 <=> " << calc_max_trapping_water(vector<int>({ 4, 3, 2, 1, 0, 3, 7 })) << endl;
  cout << "2 <=> " << calc_max_trapping_water(vector<int>({ 2, 0, 2 })) << endl;
  cout << "10 <=> " << calc_max_trapping_water(vector<int>({ 3, 0, 0, 2, 0, 4 })) << endl;

  cout << "9. sort_colors" << endl;
  cout << "[ 0 0 1 1 2 2 ] <=>" << endl;
  print_all_elem(sort_colors(vector<int>({ 2, 0, 2, 1, 1, 0 })));
  cout << "[ 0 0 0 0 0 1 1 1 1 1 2 2 ] <=>" << endl;
  print_all_elem(sort_colors(vector<int>({ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2 })));

  cout << "10. fast_sort_colors" << endl;
  cout << "[ 0 0 1 1 2 2 ] <=>" << endl;
  print_all_elem(fast_sort_colors(vector<int>({ 2, 0, 2, 1, 1, 0 })));
  cout << "[ 0 0 0 0 0 1 1 1 1 1 2 2 ] <=>" << endl;
  print_all_elem(fast_sort_colors(vector<int>({ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2 })));

  test_is_sudoku_valid();
  test_solve_sudoku();

  cout << "13. find_duplicate" << endl;
  cout << "2 <=> " << find_duplicate(vector<int>({ 1, 3, 4, 2, 2 })) << endl;
  cout << "3 <=> " << find_duplicate(vector<int>({ 3, 1, 3, 5, 2 })) << endl;

  cout << "14. check_and_update_board" << endl;
  vector<vector<int>> board({{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
                             { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                             { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
                             { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
                             { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }});
  print_all_elem_vec<int>(board);
  check_and_update_board(board);
  print_all_elem_vec<int>(board);

  test_calendar();
  test_find_dup_number();
  test_find_missing_num();
  test_find_major_num();
  test_find_major_numbers_ii();
  test_find_min_window_exceed_sum();
  test_find_max_product();
  test_find_max_product_dp();
  test_find_lces();
  test_calc_max_profit();
  test_calc_max_profit_iii();
  test_lean_calc_max_profit_iii();
  test_convert_fraction_to_decimal();
  test_calc_friend_circle_cnt();
  test_get_redundant_edges();
  test_is_value_existed();
  test_fast_is_value_existed();
  test_gen_subsets();
  test_gen_uniq_subsets();
  test_set_zeroes();
  test_lean_set_zeroes();
  test_calc_min_sum_path();
  test_cnt_uniq_paths();
  test_cnt_uniq_paths_ii();
  test_insert_new_interval();
  test_merge_intervals();
  test_is_last_pos_reachable();
  test_get_min_jump();
  test_find_k_closest_elems();

  return 0;
}
