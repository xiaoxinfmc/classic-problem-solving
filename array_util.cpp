#include <map>
#include <list>
#include <deque>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
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
    for (int i = 0; i < input.size() - 3; i++) {
      for (int j = i + 1; j < input.size() - 2; j++) {
        for (int k = j + 1, l = input.size() - 1; k < l;) {
          int curr_sum = (input[i] + input[j] + input[k] + input[l]);
          if (target == curr_sum) {
            quadruplets.push_back(vector<int>({ input[i], input[j], input[k], input[l]}));
            k++;
            l--;
          } else if (curr_sum > target) {
            l--;
          } else {
            k++;
          }
          while (k < l && input[k] == input[k + 1]) { k++; }
          while (l > k && input[l] == input[l - 1]) { l--; }
        }
        while (j < input.size() - 2 && input[j] == input[j + 1]) { j++; }
      }
      while (i < input.size() - 3 && input[i] == input[i + 1]) { i++; }
    }
    return quadruplets;
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
   */
  vector<int> calc_two_sum(vector<int> input, int target) {
    vector<int> index_pair({ -1, -1 });
    unordered_map<int, int> lookup;
    for (int i = 0; i < input.size(); i++) { lookup[input[i]] = i + 1; }
    for (auto & curr_val : input) {
      if (lookup.end() != lookup.find(target - curr_val) &&
          lookup[curr_val] != lookup[target - curr_val]) {
        if (lookup[curr_val] < lookup[target - curr_val]) {
          index_pair[0] = lookup[curr_val]; index_pair[1] = lookup[target - curr_val];
        } else {
          index_pair[1] = lookup[curr_val]; index_pair[0] = lookup[target - curr_val];
        }
        break;
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
   * - Or sort the input in place 1st, then 
   */
  vector<vector<int>> calc_triplets_by_sum(vector<int> input, int target = 0) {
    vector<vector<int>> triplets;
    sort(input.begin(), input.end());
    int arry_siz = input.size(), target_diff = 0;
    for (int curr_idx = 0; curr_idx < arry_siz - 2; curr_idx++) {
      target_diff = target - input[curr_idx];
      for (int midd_idx = curr_idx + 1, last_idx = arry_siz - 1;
               midd_idx < last_idx;) {
        if (input[midd_idx + 1] == input[midd_idx] && midd_idx < last_idx) {
          while (input[midd_idx + 1] == input[midd_idx] &&
                 midd_idx < last_idx){ midd_idx++; }
        }
        if (input[last_idx - 1] == input[last_idx] && midd_idx < last_idx) {
          while (input[last_idx - 1] == input[last_idx] &&
                 midd_idx < last_idx){ last_idx--; }
        }
        if ((input[midd_idx] + input[last_idx]) == target_diff) {
          triplets.push_back(vector<int>({ input[curr_idx], input[midd_idx], input[last_idx] }));
          midd_idx++; last_idx--;
        } else if ((input[midd_idx] + input[last_idx]) > target_diff) {
          last_idx--;
        } else {
          midd_idx++;
        }
      }
      if (input[curr_idx + 1] == input[curr_idx] && curr_idx < arry_siz - 2) {
        while (input[curr_idx + 1] == input[curr_idx] &&
               curr_idx < arry_siz - 2) { curr_idx++; }
      }
    }
    return triplets;
  }

  vector<vector<int>> _calc_triplets_by_sum(vector<int> input, int target = 0) {
    vector<vector<int>> triplets;
    //        sum of 2            smaller #      related idx
    unordered_map<int, unordered_map<int, unordered_set<int>>> lookup;
    for (int i = 0; i < input.size(); i++) {
      for (int j = i + 1; j < input.size(); j++) {
        int curr_sum = input[i] + input[j];
        if (lookup.end() == lookup.find(curr_sum)) {
          lookup[curr_sum] = unordered_map<int, unordered_set<int>>();
        }
        int num_to_check = min(input[i], input[j]);
        if (lookup[curr_sum].end() == lookup[curr_sum].find(num_to_check)) {
          lookup[curr_sum][num_to_check] = unordered_set<int>();
        }
        lookup[curr_sum][num_to_check].insert(i);
        lookup[curr_sum][num_to_check].insert(j);
      }
    }

    for (int i = 0; i < input.size(); i++) {
      int delta = target - input[i];
      if (lookup.end() == lookup.find(delta)) { continue; }
      unordered_map<int, unordered_set<int>> pair_map = lookup[delta];
      for (auto & sum_pair : pair_map) {
        if (sum_pair.second.end() != sum_pair.second.find(i)) { continue; }
        int third_val = delta - sum_pair.first;
        if (input[i] <= sum_pair.first) {
          triplets.push_back({ input[i], sum_pair.first, third_val });
        } else if (input[i] < third_val) {
          triplets.push_back({ sum_pair.first, input[i], third_val });
        } else {
          triplets.push_back({ sum_pair.first, third_val, input[i] });
        }
        lookup[input[i] + sum_pair.first].erase(min(input[i], sum_pair.first));
        lookup[input[i] + third_val].erase(min(input[i], third_val));
      }
      lookup.erase(delta);
    }
    return triplets;
  }

  /**
   * 75. Sort Colors
   * Given an array with n objects colored red, white or blue, sort them inplace
   * so that objects of the same color are adjacent, with the colors in the order
   * red, white and blue.
   * Here, we will use the integers 0, 1, and 2 to represent the color red,
   * white, and blue respectively.
   * Note: You are not suppose to use the library's sort function for this problem.
   * Example:                  i  j           k        i  j        k
   * Input: [2,0,2,1,1,0] -> { 2, 0, 2, 1, 1, 1 } -> { 1, 0, 2, 1, 1, 2 }
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
        case 0: { swap(nums[i], nums[j]); i++; j++; break; }
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
    int fast_ptr = 0, slow_ptr = 0, max_size = nums.size();
    if (1 >= nums.size()){ return -1; }
    do {
      slow_ptr = nums[slow_ptr] % max_size;
      fast_ptr = nums[nums[fast_ptr] % max_size] % max_size;
    } while (fast_ptr != slow_ptr);
    slow_ptr = 0;
    do {
      slow_ptr = nums[slow_ptr] % max_size;
      fast_ptr = nums[fast_ptr ] % max_size;
    } while (fast_ptr != slow_ptr);
    return slow_ptr;
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
  return 0;
}
