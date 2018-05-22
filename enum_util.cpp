#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class EnumUtil{
public:
  /**
   * 8.3 Permutations
   * Given a collection of numbers, return all possible permutations.
   * For example, [1,2,3] have the following permutations:
   * [1,2,3], [1,3,2], [2,1,3], [2,3,1], [3,1,2], and [3,2,1].
   */
  static vector<vector<int>> enum_all_comb(const vector<int> & value_arr) {
    vector<vector<int>> group_arr;
    if (true == value_arr.empty()) { return group_arr; }
    get_all_comb(list<int>(value_arr.begin(), value_arr.end()),
                           vector<int>(), group_arr);
    return group_arr;
  }

  static void get_all_comb(list<int> value_list, vector<int> curr_group,
                           vector<vector<int>> & group_arr_ret) {
    if (true == value_list.empty()) { return; }
    int tmp_val = 0;
    for (list<int>::iterator itr = value_list.begin();
                             itr != value_list.end(); itr++) {
      tmp_val = *itr; curr_group.push_back(tmp_val);
      itr = value_list.erase(itr);
      if (true == value_list.empty()) { group_arr_ret.push_back(curr_group); }
      else { get_all_comb(value_list, curr_group, group_arr_ret); }
      curr_group.pop_back();
      value_list.insert(itr, tmp_val);
      itr--;
    }
  }

  static vector<vector<int>> enum_all_uniq_comb(const vector<int> & value_arr) {
    vector<vector<int>> group_arr;
    if (true == value_arr.empty()) { return group_arr; }
    unordered_set<string> signature_set;
    get_all_uniq_comb(list<int>(value_arr.begin(), value_arr.end()),
                      vector<int>(), "", signature_set, group_arr);
    return group_arr;
  }

  /**
   * 8.4 Permutations II
   * Given a collection of numbers that might contain duplicates, return all
   * possible unique permutations. For example, [1,1,2] have the following
   * unique permutations: [1,1,2], [1,2,1], and [2,1,1].
   */
  static void get_all_uniq_comb(
    list<int> value_list, vector<int> curr_group, string curr_signature,
    unordered_set<string> & signature_set, vector<vector<int>> & group_arr_ret
  ) {
    if (true == value_list.empty()) { return; }
    int curr_value = 0; string new_signature;
    for (list<int>::iterator itr = value_list.begin();
                             itr != value_list.end(); itr++) {
      curr_value = * itr;
      itr = value_list.erase(itr);
      curr_group.push_back(curr_value);
      new_signature = (curr_signature + "$" + to_string(curr_value));
			if (true == value_list.empty()) {
        if (signature_set.end() == signature_set.find(new_signature)) {
          group_arr_ret.push_back(curr_group);
          signature_set.insert(new_signature);
        }
      } else {
        get_all_uniq_comb(
          value_list, curr_group, new_signature, signature_set, group_arr_ret
        );
      }
      curr_group.pop_back();
      value_list.insert(itr, curr_value); // insert to the front of itr
      itr--;
    }
  }

  class Elem {
  public:
    Elem(int val, int idx) : value(val), index(idx) {}
    virtual ~Elem() {}
    int value, index;
  };
  /**
   *
   * 8.5 Combinations
   * Given two integers n and k, return all possible combinations of k numbers
   * out of 1...n. For example, If n = 4 and k = 2, a solution is:
   * [ [2,4], [3,4], [2,3], [1,2], [1,3], [1,4], ]
   */
  static vector<vector<int>> enum_all_partial_comp(int max, int target_size) {
    vector<vector<int>> group_arr_ret;
    if (0 >= max || 0 >= target_size || target_size > max) {
      return group_arr_ret;
    }
    list<Elem> elem_list;
    for (int i = 1; i <= max; i++) {
      elem_list.push_back(Elem(i, i - 1));
    }
    unordered_set<string> signature_set;
    get_all_partial_comp(elem_list, vector<int>(), vector<bool>(max, false),
                         signature_set, target_size, group_arr_ret);
    return group_arr_ret;
  }

  static void get_all_partial_comp(list<Elem> elem_list, vector<int> curr_set,
                                   vector<bool> selected_elem_idx_arr,
                                   unordered_set<string>& signature_set,int max,
                                   vector<vector<int>> & group_arr_ret) {
    if (max == curr_set.size()) { return; }
    for (list<Elem>::iterator itr = elem_list.begin();
                              itr != elem_list.end(); itr++) {
      Elem new_elem_val = * itr;
      itr = elem_list.erase(itr);
      selected_elem_idx_arr[new_elem_val.index] = true;
      curr_set.push_back(new_elem_val.value);
      if (max == curr_set.size()) {
        string new_signature = "";
        for (int i = 0; i < selected_elem_idx_arr.size(); i++){
          if (selected_elem_idx_arr[i]) { new_signature += to_string(i) + "$"; }
        }
        if (signature_set.end() == signature_set.find(new_signature)) {
          group_arr_ret.push_back(curr_set);
          signature_set.insert(new_signature);
        }
      } else {
        get_all_partial_comp(elem_list, curr_set, selected_elem_idx_arr,
                             signature_set, max, group_arr_ret);
      }
      selected_elem_idx_arr[new_elem_val.index] = false;
      itr = elem_list.insert(itr, new_elem_val);
      curr_set.pop_back();
    }
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
  static vector<string> translate_all_comb(const string & phone) {
    vector<string> mapping_comb_ret;
    if (true == phone.empty()) { return mapping_comb_ret; }
    const vector<string> keyboard {
      "", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"
    };
    gen_comb_via_digits(phone, "", keyboard, 0, mapping_comb_ret);
    return mapping_comb_ret;
  }

  static void gen_comb_via_digits(const string input_digits,
                                  const string curr_digits_in_str,
                                  const vector<string> & keyboard,
                                  int curr_digit_idx,
                                  vector<string> & mapping_comb_ret) {
    if (curr_digit_idx >= input_digits.size()) { return; }
    int curr_digit = int(input_digits[curr_digit_idx]) - int('0');
    if (curr_digit < 2 || curr_digit > 9) { return; }

    for (auto & curr_chr : keyboard[curr_digit]) {
      if (curr_digit_idx == input_digits.size() - 1) {
        mapping_comb_ret.push_back(curr_digits_in_str + curr_chr);
      } else {
        gen_comb_via_digits(input_digits, (curr_digits_in_str + curr_chr),
                            keyboard, curr_digit_idx + 1, mapping_comb_ret);
      }
    }
  }

  /**
   * 8.1 Subsets
   * Given a set of distinct integers, S, return all possible subsets. Note:
   * Elements in a subset must be in non-descending order.
   * The solution set must not contain duplicate subsets.
   * For example, If S = [1, 2, 3], a solution is:
   * [ [3], [1], [2], [1,2,3], [1,3], [2,3], [1,2], [] ]
   */
  static vector<vector<int>> enum_all_subsets(const vector<int> & input) {
    vector<vector<int>> subset_group_ret{ vector<int>() };
    if (true == input.empty()) { return subset_group_ret; }
    list<Elem> elem_list; unordered_set<string> signature_set;
    for (int i = 0; i < input.size(); i++) {
      elem_list.push_back(Elem(input[i], i));
    }
    gen_all_subsets(elem_list, vector<int>(),
                    vector<bool>(input.size(), false),
                    signature_set, subset_group_ret);
    return subset_group_ret;
  }

  static void gen_all_subsets(list<Elem> elem_list,
                              vector<int> curr_subset,
                              vector<bool> selected_idx_arr,
                              unordered_set<string> & signature_set,
                              vector<vector<int>> & subset_group_ret) {
    for (list<Elem>::iterator curr_iter  = elem_list.begin();
                              curr_iter != elem_list.end(); curr_iter++) {
      Elem curr_elem = * curr_iter;
      curr_iter = elem_list.erase(curr_iter);
      selected_idx_arr[curr_elem.index] = true;
      curr_subset.push_back(curr_elem.value);

      string curr_subset_sign = "";
      for (int i = 0; i < selected_idx_arr.size(); i++) {
        if (false == selected_idx_arr[i]) { continue; }
        curr_subset_sign += (to_string(i) + "$");
      }
      if (signature_set.end() == signature_set.find(curr_subset_sign)) {
        subset_group_ret.push_back(curr_subset);
        signature_set.insert(curr_subset_sign);
        gen_all_subsets(elem_list, curr_subset, selected_idx_arr,
                        signature_set, subset_group_ret);
      }
      curr_subset.pop_back();
      selected_idx_arr[curr_elem.index] = false;
      curr_iter = elem_list.insert(curr_iter, curr_elem);
    }
  }

  static void print_all_str(const vector<string> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }

  static void print_all_group(const vector<vector<int>> & input) {
    for (auto & arr : input) {
      cout << "[ ";
      for (auto & val : arr) { cout << val << " "; }
      cout << "]";
    }
    cout << endl;
  }
};

int main(void) {
  cout << "==>> enum_all_comb" << endl;
  vector<int> input { 1, 2, 3 };
  EnumUtil::print_all_group(EnumUtil::enum_all_comb(input));
  vector<int> input_1 { 1 };
  EnumUtil::print_all_group(EnumUtil::enum_all_comb(input_1));
  vector<int> input_2 { };
  EnumUtil::print_all_group(EnumUtil::enum_all_comb(input_2));

  cout << "==>> enum_all_uniq_comb" << endl;
  vector<int> input_3 { 1, 1, 1 };
  EnumUtil::print_all_group(EnumUtil::enum_all_uniq_comb(input_3));
  vector<int> input_4 { 1, 1, 2 };
  EnumUtil::print_all_group(EnumUtil::enum_all_uniq_comb(input_4));
  vector<int> input_5 { 1 };
  EnumUtil::print_all_group(EnumUtil::enum_all_uniq_comb(input_5));
  vector<int> input_6 { };
  EnumUtil::print_all_group(EnumUtil::enum_all_uniq_comb(input_6));

  cout << "==>> enum_all_partial_comp" << endl;
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(4, 2));
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(4, 0));
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(0, 2));
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(4, 2));
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(4, 1));
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(1, 1));
  EnumUtil::print_all_group(EnumUtil::enum_all_partial_comp(4, 4));

  cout << "==>> translate_all_comb" << endl;
  EnumUtil::print_all_str(EnumUtil::translate_all_comb("23"));
  EnumUtil::print_all_str(EnumUtil::translate_all_comb("2"));
  EnumUtil::print_all_str(EnumUtil::translate_all_comb("234"));
  EnumUtil::print_all_str(EnumUtil::translate_all_comb(""));
  EnumUtil::print_all_str(EnumUtil::translate_all_comb("0"));

  cout << "==>> enum_all_subsets" << endl;
  vector<int> input_7 { 1, 2, 3 };
  EnumUtil::print_all_group(EnumUtil::enum_all_subsets(input_7));
  vector<int> input_8 { 1, 1, 2 };
  EnumUtil::print_all_group(EnumUtil::enum_all_subsets(input_8));
  vector<int> input_9 { 1 };
  EnumUtil::print_all_group(EnumUtil::enum_all_subsets(input_9));
  vector<int> input_10 { };
  EnumUtil::print_all_group(EnumUtil::enum_all_subsets(input_10));
  return 0;
}
