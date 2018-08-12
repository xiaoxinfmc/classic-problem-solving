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
};

int main(void) {
  using array_util::print_all_elem;
  using array_util::print_all_elem_vec;
  using array_util::get_next_permutation_asc;
  using array_util::find_in_rotated_sorted_arr;
  using array_util::calc_longest_seq_len;

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
  return 0;
}
