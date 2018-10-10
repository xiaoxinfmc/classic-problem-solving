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
  vector<int> calc_two_sum(vector<int> input, int target) {
    vector<int> index_pair({});
    if (true == input.empty()) { return index_pair; }
    unordered_map<int, int> value_to_id_lookup;
    for (int i = 0; i < input.size(); i++) { value_to_id_lookup[input[i]] = i; }
    for (int i = 0; i < input.size(); i++) {
      if (value_to_id_lookup.end() != value_to_id_lookup.find(target - input[i]) &&
          i < value_to_id_lookup[target - input[i]]) {
        index_pair.push_back(i + 1);
        index_pair.push_back(value_to_id_lookup[target - input[i]] + 1);
      }
    }
    return index_pair;
  }

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
    vector<vector<int>> triplets_arr;
    sort(input.begin(), input.end());
    int curr_sum = 0;
    for (int i = 0; i < input.size(); i++) {
      for (int left_idx = i + 1, right_idx = input.size() - 1; left_idx < right_idx;) {
        curr_sum = input[i] + input[left_idx] + input[right_idx];
        if (target == curr_sum) {
          triplets_arr.push_back({ input[i], input[left_idx], input[right_idx] });
          while (left_idx < right_idx && input[right_idx] == input[right_idx - 1]) { right_idx--; }
          while (left_idx < right_idx && input[left_idx] == input[left_idx + 1]) { left_idx++; }
          right_idx--; left_idx++;
        }
        if (curr_sum < target) { left_idx++; }
        if (curr_sum > target) { right_idx--; }
      }
      while (i < input.size() - 1 && input[i] == input[i + 1]) { i++; }
    }
    return triplets_arr;
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
            while (k < l && k + 1 < input.size() && input[k] == input[k + 1]) { k++; }
            while (k < l && l - 1 > 0 && input[l] == input[l - 1]) { l--; }
            k++; l--;
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

  /**
   * ending term: all blanks been filled already
   * each round fill exact 1 number if curr cell not been filled yet, and if we
   * actually filled a value, then we run validation, if true contiue return
   * false otherwise.
   */
  static bool is_current_fill_valid(vector<vector<char>> & board, int i, int j) {
    int board_size = board.size();
    for (int k = 0; k < board_size; k++) {
      if (k != j && board[i][j] == board[i][k]) { return false; }
      if (k != i && board[i][j] == board[k][j]) { return false; }
    }
    int box_start_row = (i / 3) * 3, box_start_col = (j / 3) * 3;
    for (int r = box_start_row; r < box_start_row + 3; r++) {
      for (int c = box_start_col; c < box_start_col + 3; c++) {
        if (i != r && j != c && board[i][j] == board[r][c]) { return false; }
      }
    }
    return true;
  }

  static vector<char> get_all_values_to_fill(vector<vector<char>> & board, int i, int j) {
    int board_size = board.size();
    vector<bool> number_lookup(board_size + 1, false);
    vector<char> values_to_fill;
    for (int k = 0; k < board_size; k++) {
      if ('.' != board[i][k]) { number_lookup[get_number_val(board, i, k)] = true; }
      if ('.' != board[k][j]) { number_lookup[get_number_val(board, k, j)] = true; }
    }
    int box_start_row = (i / 3) * 3, box_start_col = (j / 3) * 3;
    for (int r = box_start_row; r < box_start_row + 3; r++) {
      for (int c = box_start_col; c < box_start_col + 3; c++) {
        if ('.' != board[r][c]) { number_lookup[get_number_val(board, r, c)] = true; }
      }
    }
    for (int i = 1; i < board_size + 1; i++) {
      if (false == number_lookup[i]) { values_to_fill.push_back((char)(i + (int)'0')); }
    }
    return values_to_fill;
  }

  static bool solve_sudoku_recur(vector<vector<char>> & board,
                                 vector<pair<int, int>> & cells_to_fill) {
    int board_size = board.size();
    if (true == cells_to_fill.empty()){ return true; }

    bool is_good_fill_found = false;
    pair<int, int> cell = cells_to_fill.back();
    vector<char> numbers_to_fill = get_all_values_to_fill(board, cell.first, cell.second);
    if (true == numbers_to_fill.empty()) { return false; }

    char original_value = board[cell.first][cell.second];
    for (auto value : numbers_to_fill) {
      board[cell.first][cell.second] = value;
      if (true == is_current_fill_valid(board, cell.first, cell.second)) {
        cells_to_fill.pop_back();
        is_good_fill_found = solve_sudoku_recur(board, cells_to_fill);
        if (false == is_good_fill_found) { cells_to_fill.push_back(cell); }
      }
      if (true == is_good_fill_found) { break; }
      board[cell.first][cell.second] = original_value;
    }
    return is_good_fill_found;
  }

  static bool solve_sudoku(vector<vector<char>> & board) {
    vector<pair<int, int>> cells_to_fill;
    for (int i = 0; i < board.size(); i++) {
      for (int j = 0; j < board.size(); j++) {
        if ('.' == board[i][j]) { cells_to_fill.push_back(pair<int, int>(i, j)); }
      }
    }
    return solve_sudoku_recur(board, cells_to_fill);
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
};

int main(void) {
  using array_util::print_all_elem;
  using array_util::print_all_elem_vec;
  using array_util::get_next_permutation_asc;
  using array_util::find_in_rotated_sorted_arr;
  using array_util::calc_longest_seq_len;
  using array_util::find_median_elem;
  using array_util::calc_two_sum;
  using array_util::calc_triplets_by_sum;
  using array_util::calc_quadruplets_by_sum;
  using array_util::calc_max_trapping_water;
  using array_util::sort_colors;
  using array_util::fast_sort_colors;
  using array_util::is_sudoku_valid;
  using array_util::solve_sudoku;
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

  cout << "5. calc_two_sum" << endl;
  cout << "[ 2 3 ] <=> "; print_all_elem<int>(calc_two_sum(vector<int>({ 3, 2, 4 }), 6));
  cout << "[ 1 2 ] <=> "; print_all_elem<int>(calc_two_sum(vector<int>({ 2, 7, 11, 15 }), 9));

  cout << "6. calc_triplets_by_sum" << endl;
  cout << "[ [ -1 0 1 ], [ -1 -1 2 ] ] <=> " << endl;
  print_all_elem_vec<int>(calc_triplets_by_sum(vector<int>({ -1, 0, 1, 2, -1, -4 })));
  cout << "[ [ 3 9 12 ], [ 6 6 12 ] ] <=> " << endl;
  print_all_elem_vec<int>(calc_triplets_by_sum(vector<int>({ 12, 3, 6, 1, 6, 9 }), 24));
  cout << "[ [ -2 0 2 ], [ -2 1 1 ] ] <=> " << endl;
  print_all_elem_vec<int>(calc_triplets_by_sum(vector<int>({ -2, 0, 1, 1, 2 })));
  cout << "[ ] <=> " << endl;
  print_all_elem_vec<int>(calc_triplets_by_sum(vector<int>({ -2, 0, 1, 1, 2 }), 10));

  cout << "7. calc_quadruplets_by_sum" << endl;
  cout << "[ [ -1 0 0 1 ], [ -2 -1 1 2 ], [ -2 0 0 2 ] ] <=> " << endl;
  print_all_elem_vec<int>(calc_quadruplets_by_sum(vector<int>({ 1, 0, -1, 0, -2, 2 })));
  cout << "[ ] <=> " << endl;
  print_all_elem_vec<int>(calc_quadruplets_by_sum(vector<int>({ -2, 0, 1, 1, 2 }), 10));

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

  cout << "11. is_sudoku_valid" << endl;
  vector<vector<char>> sudoku_0({vector<char>({'5','3','.','.','7','.','.','.','.'}),
                                 vector<char>({'6','.','.','1','9','5','.','.','.'}),
                                 vector<char>({'.','9','8','.','.','.','.','6','.'}),
                                 vector<char>({'8','.','.','.','6','.','.','.','3'}),
                                 vector<char>({'4','.','.','8','.','3','.','.','1'}),
                                 vector<char>({'7','.','.','.','2','.','.','.','6'}),
                                 vector<char>({'.','6','.','.','.','.','2','8','.'}),
                                 vector<char>({'.','.','.','4','1','9','.','.','5'}),
                                 vector<char>({'.','.','.','.','8','.','.','7','9'})});
  vector<vector<char>> sudoku_1({vector<char>({'8','3','.','.','7','.','.','.','.'}),
                                 vector<char>({'6','.','.','1','9','5','.','.','.'}),
                                 vector<char>({'.','9','8','.','.','.','.','6','.'}),
                                 vector<char>({'8','.','.','.','6','.','.','.','3'}),
                                 vector<char>({'4','.','.','8','.','3','.','.','1'}),
                                 vector<char>({'7','.','.','.','2','.','.','.','6'}),
                                 vector<char>({'.','6','.','.','.','.','2','8','.'}),
                                 vector<char>({'.','.','.','4','1','9','.','.','5'}),
                                 vector<char>({'.','.','.','.','8','.','.','7','9'})});
  vector<vector<char>> sudoku_2({vector<char>({'3', '.', '6', '5', '.', '8', '4', '.', '.'}),
                                 vector<char>({'5', '2', '.', '.', '.', '.', '.', '.', '.'}),
                                 vector<char>({'.', '8', '7', '.', '.', '.', '.', '3', '1'}),
                                 vector<char>({'.', '.', '3', '.', '1', '.', '.', '8', '.'}),
                                 vector<char>({'9', '.', '.', '8', '6', '3', '.', '.', '5'}),
                                 vector<char>({'.', '5', '.', '.', '9', '.', '6', '.', '.'}),
                                 vector<char>({'1', '3', '.', '.', '.', '.', '2', '5', '.'}),
                                 vector<char>({'.', '.', '.', '.', '.', '.', '.', '7', '4'}),
                                 vector<char>({'.', '.', '5', '2', '.', '6', '3', '.', '.'})});
  cout << "1 <=> " << is_sudoku_valid(sudoku_0) << endl;
  cout << "0 <=> " << is_sudoku_valid(sudoku_1) << endl;

  cout << "12. solve_sudoku_row_recur" << endl;
  cout << "1 <=> " << solve_sudoku(sudoku_0) << " <=> " << is_sudoku_valid(sudoku_0) <<  endl;
  cout << "0 <=> " << solve_sudoku(sudoku_1) << " <=> " << is_sudoku_valid(sudoku_1) << endl;
  cout << "1 <=> " << solve_sudoku(sudoku_2) << " <=> " << is_sudoku_valid(sudoku_2) <<  endl;

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

  return 0;
}
