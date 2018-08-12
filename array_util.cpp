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
  return 0;
}
