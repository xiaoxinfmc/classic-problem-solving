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
  static void enumerate_all_subsets_recur(deque<int> values,
                                          vector<int> prev_subset,
                                          vector<vector<int>> & all_subsets) {
    while(false == values.empty()) {
      all_subsets.push_back(prev_subset);
      all_subsets.back().push_back(values.front());
      values.pop_front();
      enumerate_all_subsets_recur(values, all_subsets.back(), all_subsets);
    }
  }

  static vector<vector<int>> enumerate_all_subsets(vector<int> values) {
    vector<vector<int>> all_subsets({ vector<int>() });
    if (true == values.empty()) { return all_subsets; }
    sort(values.begin(), values.end());
    enumerate_all_subsets_recur(
      deque<int>(values.begin(), values.end()), vector<int>(), all_subsets
    );
    return all_subsets;
  }

  /**
   *
   * 8.5 Combinations
   * Given two integers n and k, return all possible combinations of k numbers
   * out of 1...n. For example, If n = 4 and k = 2, a solution is:
   * [ [2,4], [3,4], [2,3], [1,2], [1,3], [1,4] ]
   */
  static void enumerate_all_subsets_of_size_k_recur(list<int> values,
                                                    vector<int> prev_subset, int k,
                                                    vector<vector<int>> & subsets_of_size_k) {
    if (prev_subset.size() == k) { subsets_of_size_k.push_back(prev_subset); }
    if (prev_subset.size() >= k) { return; }
    while (false == values.empty()) {
      vector<int> curr_subset(prev_subset.begin(), prev_subset.end());
      curr_subset.push_back(values.front());
      values.pop_front();
      enumerate_all_subsets_of_size_k_recur(values, curr_subset, k, subsets_of_size_k);
    }
  }

  static vector<vector<int>> enumerate_all_subsets_of_size_k(int max, int k) {
    vector<vector<int>> subsets_of_size_k;
    if (max < 0 || k < 0 || max < k) { return subsets_of_size_k; }
    list<int> elem_list;
    for (int i = 1; i <= max; i++) { elem_list.push_back(i); }
    enumerate_all_subsets_of_size_k_recur(elem_list, vector<int>(), k, subsets_of_size_k);
    return subsets_of_size_k;
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
  static void enumerate_all_comb_recur(list<int> values,
                                       vector<int> curr_comb_vect,
                                       vector<vector<int>> & all_comb_vects){
    if (true == values.empty()) {
      all_comb_vects.push_back(curr_comb_vect); return;
    }
    int curr_val = 0;
    for (list<int>::iterator itr = values.begin(); itr != values.end(); itr++){
      curr_val = * itr;
      curr_comb_vect.push_back(curr_val);
      itr = values.erase(itr);
      enumerate_all_comb_recur(values, curr_comb_vect, all_comb_vects);
      itr = values.insert(itr, curr_val);
      curr_comb_vect.pop_back();
    }
  }

  static vector<vector<int>> enumerate_all_comb(const vector<int> values) {
    vector<vector<int>> all_comb_vects;
    if (true == values.empty()) { return all_comb_vects; }
    enumerate_all_comb_recur(
      list<int>(values.begin(), values.end()), vector<int>(), all_comb_vects
    );
    return all_comb_vects;
  }

  /**
   * 8.4 Permutations II
   * Given a collection of numbers that might contain duplicates, return all
   * possible unique permutations. For example, [1,1,2] have the following
   * unique permutations: [1,1,2], [1,2,1], and [2,1,1].
   */
  static void enumerate_all_uniq_comb_recur(list<int> values,
                                            vector<int> curr_comb_vect,
                                            vector<vector<int>> & all_comb_vects){
    if (true == values.empty()) {
      all_comb_vects.push_back(curr_comb_vect); return;
    }
    unordered_set<int> selected_elems;
    int curr_val = 0;
    for (list<int>::iterator itr = values.begin(); itr != values.end(); itr++){
      curr_val = * itr;
      if (selected_elems.end() != selected_elems.find(curr_val)) { continue; }
      curr_comb_vect.push_back(curr_val);
      itr = values.erase(itr);
      enumerate_all_uniq_comb_recur(values, curr_comb_vect, all_comb_vects);
      itr = values.insert(itr, curr_val);
      curr_comb_vect.pop_back();
      selected_elems.insert(curr_val);
    }
  }

  static vector<vector<int>> enumerate_all_uniq_comb(const vector<int> values) {
    vector<vector<int>> all_comb_vects;
    if (true == values.empty()) { return all_comb_vects; }
    enumerate_all_uniq_comb_recur(
      list<int>(values.begin(), values.end()), vector<int>(), all_comb_vects
    );
    return all_comb_vects;
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
                                       string curr_prefix,
                                       vector<string> & mapping_comb_ret) {
    if (curr_prefix.size() == phone.size()) {
      mapping_comb_ret.push_back(curr_prefix);
    }
    if (curr_prefix.size() >= phone.size()) { return; }
    int curr_digit_idx = int(phone[curr_prefix.size()]) - int('0');
    if (curr_digit_idx < 0 || curr_digit_idx >= keyboard.size()) { return; }
    for (int i = 0; i < keyboard[curr_digit_idx].size(); i++) {
      translate_all_comb_recur(
        keyboard, phone, curr_prefix + keyboard[curr_digit_idx][i], mapping_comb_ret
      );
    }
  }

  static vector<string> translate_all_comb(const string & phone) {
    vector<string> mapping_comb_ret;
    if (true == phone.empty()) { return mapping_comb_ret; }
    const vector<string> keyboard({
      "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
    });
    translate_all_comb_recur(keyboard, phone, "", mapping_comb_ret);
    return mapping_comb_ret;
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
