#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <deque>
#include <unordered_set>

using namespace std;

namespace parenthesis_util {
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
   * goal: print all combination of well formed parentheses.
   * (()()), (())(), ()()(), ((())), good 
   * ()()(), ()(()),  (()()), ((()))
   * ())((), ()))((, )))(((, bad
   *
   * Observation:
   * - all valid combination has to start with '(', end with ')', # has to match.
   * - at any time, # of '(' is no less than # of ')'
   * - we form a good comb if reach end & conform all valid conditions @ each step
   */
  static void gen_valid_parenthesis_recur(int left_char_cnt, int right_char_cnt,
                                          int pair_cnt, string curr_parenthesis,
                                          vector<string> & paren_comb_ret) {
    if (left_char_cnt > pair_cnt || right_char_cnt > pair_cnt) { return; }
    if (left_char_cnt == pair_cnt && right_char_cnt == pair_cnt) {
      paren_comb_ret.push_back(curr_parenthesis); return;
    }
    if (left_char_cnt == right_char_cnt) {
      gen_valid_parenthesis_recur(left_char_cnt + 1, right_char_cnt, pair_cnt,
                                  curr_parenthesis + "(", paren_comb_ret);
    } else if (left_char_cnt > right_char_cnt) {
       gen_valid_parenthesis_recur(left_char_cnt + 1, right_char_cnt, pair_cnt,
                                   curr_parenthesis + "(", paren_comb_ret);
       gen_valid_parenthesis_recur(left_char_cnt, right_char_cnt + 1, pair_cnt,
                                   curr_parenthesis + ")", paren_comb_ret);
    }
  }

  static vector<string> generate_parenthesis(int pair_cnt) {
    vector<string> paren_comb_ret;
    gen_valid_parenthesis_recur(0, 0, pair_cnt, "", paren_comb_ret);
    return paren_comb_ret;
  }

  static void test_generate_parenthesis() {
    cout << "1. test_generate_parenthesis" << endl;
    for (auto & pair : parenthesis_util::generate_parenthesis(10)) {
      cout << pair << endl;
    }
  }

  /**
   * 301. Remove Invalid Parentheses
   * - Remove the minimum number of invalid parentheses in order to make the
   *   input string valid. Return all possible results.
   * - Note: The input string contain letters other than parentheses ( and ).
   * Example 1:
   * - Input: "()())()"
   * - Input: "()()())()" "()()()()" "()(())()" "(()())()"
   * - Output: ["()()()", "(())()"]
   * Example 2:
   * - Input: "(a)())()"
   * - Output: ["(a)()()", "(a())()"]
   * Example 3:
   * - Input: ")("
   * - Output: [""]
   * Intuition:
   * - as we need to get the actual valid parenthesis comb, recursion is a
   *   natrual fit for this. the key would be how to form the right way to
   *   remove min amount of char to make it valid.
   * - scan from l to r, one obvservation is that we could see different
   *   kinds of situation, either:
   *   ((()()( -> incomplete, needs to remove char from both head & tail
   *   ((()))( -> incomplete, but could be valid
   *   ()()()) -> completely invalid, cannot be reversed, has to be removed
   *   ()()()  -> valid
   * - In all, invalid chars does not needs to be in the start pos or the end
   *   can be in any position.
   * - Actually it could be a DP like problem, if we switch our way of thiking
   *   for every par token, we can check to see if it is valid, if not, remove
   *   for that invalid char, either remove it or some other char in its mirror
   * - DP not seems to be helpful, then bfs/dfs + careful pruning.
   */
  static bool is_parenthesis_valid(const string & str, int & left_cnt,
                                                       int & right_cnt) {
    left_cnt = 0; right_cnt = 0;
    for (auto & chr : str) {
      switch(chr) {
        case '(' : { left_cnt += 1; break; }
        case ')' : { if (right_cnt >= left_cnt) { return false; }
                     else { right_cnt += 1; break; } }
        default: { break; }
      }
    }
    return (left_cnt == right_cnt);
  }

  static vector<string> validate_parenthesis(const string & str) {
    deque<string> substr_arr = { str };
    unordered_set<string> valid_set, logic_set;
    int left_cnt = 0, right_cnt = 0;
    string curr_substr, new_substr;
    while (false == substr_arr.empty()) {
      curr_substr = substr_arr.front();
      substr_arr.pop_front();
      if (!valid_set.empty() && curr_substr.size() <
           valid_set.begin()->size()) { break; }
      if (true == is_parenthesis_valid(curr_substr, left_cnt, right_cnt)) {
        valid_set.insert(curr_substr);
      } else {
        for (int i = 0; i < curr_substr.size(); i++) {
          switch(curr_substr[i]) {
            case '(' :
            case ')' : {
              new_substr = curr_substr.substr(0, i) + curr_substr.substr(i + 1);
              if (0 >= logic_set.count(new_substr)) {
                logic_set.insert(new_substr);
                substr_arr.push_back(new_substr);
              }
              break;
            }
            default: { break; }
          }
        }
      }
    }
    return vector<string>(valid_set.begin(), valid_set.end());
  }

  static void test_validate_parenthesis() {
    cout << "2. test_validate_parenthesis" << endl;
    vector<string> result;
    vector<string> test_input = { "()(((((((()", "(((k()((", "", "()())()", "(a)())()", ")(" };
    vector<vector<string>> test_output = {
      { "()()", }, { "(k)", "k()", }, { "" },
      {"()()()", "(())()"}, {"(a)()()", "(a())()"}, {""},
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = validate_parenthesis(test_input[i]);
      print_all_elem<string>(result); cout << " <=>" << endl;
      print_all_elem<string>(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) { assert(result[j] == test_output[i][j]); }
    }
  }
};

int main(void) {
  using parenthesis_util::test_generate_parenthesis;
  using parenthesis_util::test_validate_parenthesis;
  test_generate_parenthesis();
  test_validate_parenthesis();
  return 0;
}
