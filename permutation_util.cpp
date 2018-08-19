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

namespace permutation_util {
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
   * 8.1 Subsets
   * Given a set of distinct integers, S, return all possible subsets. Note:
   * Elements in a subset must be in non-descending order.
   * The solution set must not contain duplicate subsets.
   * For example, If S = [1, 2, 3], a solution is:
   * [ [3], [1], [2], [1,2,3], [1,3], [2,3], [1,2], [] ]
   * C(5, 5) + C(5, 4) ... + C(5, 0)
   */
  static void enumerate_all_subsets_recur(vector<int> values, vector<int> curr_set,
                                          vector<vector<int>> & subsets) {
    while (false == values.empty()) {
      subsets.push_back(curr_set);
      subsets.back().push_back(values.back());
      values.pop_back();
      enumerate_all_subsets_recur(values, subsets.back(), subsets);
    }
  }

  static vector<vector<int>> enumerate_all_subsets(vector<int> values) {
    vector<vector<int>> subsets({ vector<int>() });
    sort(values.rbegin(), values.rend());
    enumerate_all_subsets_recur(values, vector<int>(), subsets);
    return subsets;
  }

  /**
   *
   * 8.5 Combinations
   * Given two integers n and k, return all possible combinations of k numbers
   * out of 1...n. For example, If n = 4 and k = 2, a solution is:
   * [ [2,4], [3,4], [2,3], [1,2], [1,3], [1,4] ]
   */
  static void enumerate_all_subsets_of_size_k_recur(int max, int k,
                                                    vector<int> curr_set,
                                                    vector<vector<int>> & subsets) {
    if (curr_set.size() > k) { return; }
    if (curr_set.size() == k) { subsets.push_back(curr_set); return; }
    for (int i = curr_set.empty() ? 1 : curr_set.back() + 1; i <= max; i++) {
      curr_set.push_back(i);
      enumerate_all_subsets_of_size_k_recur(max, k, curr_set, subsets);
      curr_set.pop_back();
    }
  }

  static vector<vector<int>> enumerate_all_subsets_of_size_k(int max, int k) {
    vector<vector<int>> subsets;
    if (k > max) { return subsets; }
    enumerate_all_subsets_of_size_k_recur(max, k, vector<int>(), subsets);
    return subsets;
  }

  /**
   * 8.3 Permutations
   * Given a collection of numbers, return all possible permutations.
   * For example, [1,2,3] have the following permutations:
   * [1,2,3], [1,3,2], [2,1,3], [2,3,1], [3,1,2], and [3,2,1].
   * 1, (2, 3)
   * 1, 2 (3) ret out of for [ (1, 2, 3) ]
   * 1, 2, 3() ret
   * 1, 3 (2)
   * A(5, 5)
   */
  static void enumerate_all_comb_recur(list<int> values, vector<int> curr_set,
                                       vector<vector<int>> & subsets) {
    if (true == values.empty()) { subsets.push_back(curr_set); return; }
    for (list<int>::iterator itr = values.begin(); itr != values.end(); itr++) {
      curr_set.push_back(* itr);
      itr = values.erase(itr);
      enumerate_all_comb_recur(values, curr_set, subsets);
      itr = values.insert(itr, curr_set.back());
      curr_set.pop_back();
    }
  }

  static vector<vector<int>> enumerate_all_comb(const vector<int> values) {
    vector<vector<int>> subsets;
    enumerate_all_comb_recur(list<int>(values.begin(), values.end()), vector<int>(), subsets);
    return subsets;
  }

  /**
   * 8.4 Permutations II
   * Given a collection of numbers that might contain duplicates, return all
   * possible unique permutations. For example, [1,1,2] have the following
   * unique permutations: [1,1,2], [1,2,1], and [2,1,1].
   */
  static void enumerate_all_uniq_comb_recur(list<int> values,
                                            vector<int> curr_set,
                                            vector<vector<int>> & subsets) {
    unordered_set<int> used_value_set;
    if (true == values.empty()) { subsets.push_back(curr_set); return; }
    int curr_value = 0;
    for (list<int>::iterator itr = values.begin(); itr != values.end(); itr++) {
      curr_value = * itr;
      if (used_value_set.end() != used_value_set.find(curr_value)) { continue; }
      curr_set.push_back(curr_value);
      itr = values.erase(itr);
      enumerate_all_uniq_comb_recur(values, curr_set, subsets);
      itr = values.insert(itr, curr_value);
      curr_set.pop_back();
      used_value_set.insert(curr_value);
    }
  }

  static vector<vector<int>> enumerate_all_uniq_comb(const vector<int> values) {
    vector<vector<int>> subsets;
    enumerate_all_uniq_comb_recur(list<int>(values.begin(), values.end()), vector<int>(), subsets);
    return subsets;
  }

  /**
   * 8.6 Letter Combinations of a Phone Number
   * Given a digit string, return all possible letter combinations that the
   * number could represent. A mapping of digit to letters (just like on the
   * telephone buttons) is given below.
   * Input:Digit string "23"
   * Output: ["ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf"].
   * Note: Although the above answer is in lexicographical order, your answer
   *       could be in any order you want.
   * const vector<string> keyboard {
   *   " ", "", "abc", "def", // '0','1','2',...
   *   "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz" };
   */

  static void translate_all_comb_recur(const vector<string> & keyboard,
                                       const string & phone,
                                       int curr_phone_digit_idx,
                                       string curr_prefix,
                                       vector<string> & char_sets) {
    if (curr_phone_digit_idx >= phone.size()) {
      char_sets.push_back(curr_prefix); return;
    }
    int curr_phone_num = (int)(phone[curr_phone_digit_idx] - '0');
    for (int i = 0; i < keyboard[curr_phone_num].size(); i++) {
      translate_all_comb_recur(keyboard, phone, curr_phone_digit_idx + 1,
                               curr_prefix + keyboard[curr_phone_num][i],
                               char_sets);
    }
  }

  static vector<string> translate_all_comb(const string & phone) {
    const vector<string> keyboard({ "+", "", "abc", "def", "ghi", "jkl",
                                    "mno", "pqrs", "tuv", "wxyz" });
    vector<string> char_sets;
    translate_all_comb_recur(keyboard, phone, 0, "", char_sets);
    return char_sets;
  }
};

int main(void) {
  using permutation_util::print_all_elem;
  using permutation_util::print_all_elem_vec;
  using permutation_util::enumerate_all_comb;
  using permutation_util::enumerate_all_uniq_comb;
  using permutation_util::enumerate_all_subsets;
  using permutation_util::enumerate_all_subsets_of_size_k;
  using permutation_util::translate_all_comb;

  cout << "1. all possible permutation" << endl;
  print_all_elem_vec<int>(enumerate_all_comb(vector<int>({ })));
  print_all_elem_vec<int>(enumerate_all_comb(vector<int>({ 1 })));
  print_all_elem_vec<int>(enumerate_all_comb(vector<int>({ 1, 2, 3 })));

  cout << "2. all unique-- permutation" << endl;
  print_all_elem_vec<int>(enumerate_all_uniq_comb(vector<int>({ })));
  print_all_elem_vec<int>(enumerate_all_uniq_comb(vector<int>({ 1 })));
  print_all_elem_vec<int>(enumerate_all_uniq_comb(vector<int>({ 1, 2, 3 })));
  print_all_elem_vec<int>(enumerate_all_uniq_comb(vector<int>({ 1, 1, 3 })));
  print_all_elem_vec<int>(enumerate_all_uniq_comb(vector<int>({ 1, 1, 1 })));

  cout << "3. all possible subsets" << endl;
  print_all_elem_vec<int>(enumerate_all_subsets(vector<int>({ })));
  print_all_elem_vec<int>(enumerate_all_subsets(vector<int>({ 1 })));
  print_all_elem_vec<int>(enumerate_all_subsets(vector<int>({ 1, 2, 3 })));

  cout << "4. enumerate_all_subsets_of_size_k" << endl;
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(4, 0));
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(4, 1));
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(4, 2));
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(4, 3));
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(4, 4));
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(1, 1));
  print_all_elem_vec<int>(enumerate_all_subsets_of_size_k(1, 4));

  cout << "5. translate_all_comb" << endl;
  print_all_elem<string>(translate_all_comb(""));
  print_all_elem<string>(translate_all_comb("0"));
  print_all_elem<string>(translate_all_comb("2"));
  print_all_elem<string>(translate_all_comb("23"));
  print_all_elem<string>(translate_all_comb("234"));

  return 0;
}
