#include <map>
#include <list>
#include <deque>
#include <string>
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

  template <class type>
  static void print_all_elem_vec(const vector<vector<type>> & input) {
    cout << "[" << endl;
    for (auto & arr : input){ cout << "  "; print_all_elem<type>(arr); }
    cout << "]" << endl;
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

  return 0;
}
