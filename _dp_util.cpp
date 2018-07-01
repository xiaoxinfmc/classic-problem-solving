#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <map>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class dp_util{
public:
  /**
   * 13.3 Palindrome Partitioning II
   * - Given a string s, partition s such that every substring of the partition
   *   is a palindrome. Return the minimum cuts needed for a palindrome
   *   partitioning of s.
   * - For example, given s = "aab",
   *   Return 1 since the palindrome partitioning ["aa","b"] could be produced
   *   using 1 cut.
   * - min_palin_cut[j] -> min palin cuts to break str[0..j] {
   *   1. j == 0, => 0
   *   2. j == 1, => 0 | 1
   *   3. j >= 2, => {
   *        for i in (0 ... j - 1)
   *          if str[i..j] is a palindrome,
   *            min_palin_cut[j] <= min(min_palin_cut[j], min_palin_cut[i - 1] + 1)
   *          else
   *            min_palin_cut[j] <= min(min_palin_cut[j], min_palin_cut[j - 1] + 1)
   *          end
   *      }
   */
  static int get_min_palindrome_cnt(const string & input_str) {
    const int input_str_len = input_str.size();
    if (0 >= input_str_len) { return 0; }
    int min_palindrome_cuts[input_str_len];
    bool is_token_palindrome[input_str_len][input_str_len];
    for (int i = 0; i < input_str_len; i++) {
      min_palindrome_cuts[i] = i;
      for (int c = 0; c < input_str_len; c++) {
        if (i == c) { is_token_palindrome[i][c] = true; }
        else { is_token_palindrome[i][c] = false; }
      }
    }
    /* i from 1 -> last */
    for (int i = 1; i < input_str_len; i++) {
      /* c from i - 1 to 0 */
      for (int c = i - 1; c >= 0; c--) {
        if ((input_str[c] == input_str[i]) &&
            ((c == i - 1) || (is_token_palindrome[c + 1][i - 1]))) {
          is_token_palindrome[c][i] = true;
          if (c - 1 >= 0) {
            min_palindrome_cuts[i] = min(min_palindrome_cuts[i],
                                         min_palindrome_cuts[c - 1] + 1);
          } else { // whole piece is a palindrome, no cut needed.
            min_palindrome_cuts[i] = min(min_palindrome_cuts[i], 0);
          }
        }
      }
      min_palindrome_cuts[i] = min(min_palindrome_cuts[i],
                                   min_palindrome_cuts[i - 1] + 1);
    }
    return min_palindrome_cuts[input_str_len - 1];
  }

  /**
   * 13.7 Scramble String
   * Given a string s1, we may represent it as a binary tree by partitioning
   * it to two non-empty substrings recursively.
   * Below is one possible representation of s1 = "great":
   *   gr eat
   *    /  \
   *  gr   eat
   *  /\    /\
   * g  r  e  at
   *          /\
   *         a  t
   * To scramble the string, we may choose any non-leaf node and swap its two
   * children. For example, if we choose the node "gr" and swap its two
   * children, it produces a scrambled string "rgeat".
   *       rg eat
   *        /  \
   *      rg   eat
   *      /\    /\
   *     r  g  e  at
   *              /\
   *             a  t
   * We say that "rgeat" is a scrambled string of "great".
   * Similarly, if we continue to swap the children of nodes "eat" and "at",
   * it produces a scrambled string "rgtae".
   *       rg tae
   *        /  \
   *      rg    tae
   *      /\    / \
   *     r  g  ta  e
   *           /\
   *          t  a
   * We say that "rgtae" is a scrambled string of "great".
   * Given two strings s1 and s2 of the same length, determine if s2 is a
   * scrambled string of s1.
   *
   * true if s1 == s2
   * if str.size > 2 {
   *   is_scramble(s1[0..n], s2[0..n]) <=> any {
   *     0 <= i < n | (is_scramble(s1[0..i], s2[0..i]) &&
   *                   is_scramble(s1[i + 1..n], s2[i + 1..n])) ||
   *                  (is_scramble(s1[0..i], s2[n - i..n]) &&
   *                   is_scramble(s1[i + 1..n], s2[0..n - i - 1]))
   *   }
   * } else {
   *   s1.reverse == s2.reverse
   * }
   */
  enum { STRATEGY_UNDEF = -1, STRATEGY_FAIL, STRATEGY_SUCCESS };
  static bool is_input_via_scramble(const string & base,const string & str_chk){
    if (base.size() != str_chk.size()) { return false; }
    unordered_map<string, int> strategy_lookup;
    bool is_scramble = check_scramble(base, str_chk, strategy_lookup);
    return is_scramble;
  }

  static string get_hash_key(const string & base, const string & str_chk) {
    return (base + "$" + str_chk);
  }

  static bool check_scramble(const string & base, const string & str_chk,
                             unordered_map<string, int> & strategy_lookup){
    if (base.size() != str_chk.size()) { return false; }
    if (base == str_chk) { return true; }
    string lookup_key = get_hash_key(base, str_chk);
    if (strategy_lookup.find(lookup_key) != strategy_lookup.end()) {
      return strategy_lookup[lookup_key];
    }
    bool is_pair_scramble = false;
    int str_len = base.size();
    for (int i = 0; i < str_len - 1; i++) {
      is_pair_scramble = (
        (check_scramble(base.substr(0, i + 1),
                        str_chk.substr(0, i + 1),
                        strategy_lookup) &&
         check_scramble(base.substr(i + 1, str_len - i - 1),
                        str_chk.substr(i + 1, str_len - i - 1),
                        strategy_lookup)) ||
        (check_scramble(base.substr(0, i + 1),
                        str_chk.substr(str_len - 1 - i, i + 1),
                        strategy_lookup) &&
         check_scramble(base.substr(i + 1, str_len - i - 1),
                        str_chk.substr(0, str_len - i - 1),
                        strategy_lookup))
      );
      if (true == is_pair_scramble) { break; }
    }
    strategy_lookup[lookup_key] = is_pair_scramble;
    return is_pair_scramble;
  }

  /**
   * Given s1, s2, s3, find whether s3 is formed by the interleaving of
   * s1 and s2. For example, Given: s1 = "aabcc", s2 = "dbbca",
   * When s3 = "aadbbcbcac", return true.
   * When s3 = "aadbbbaccc", return false.
   */
  static bool is_input_via_interleave(const string & base_l,
                                      const string & base_r,
                                      const string & str_chk) {
    return false;
  }
};

int main(void) {
  cout << "1. dp_util::get_min_palindrome_cnt" << endl;
  string test_input[] = { "", "a", "aab", "aaa", "aaacdfe", "aaacaaafe",
                          "abcde", "abacabaca", "abacakacaba" };
  int test_output[] = { 0, 0, 1, 0, 4, 2, 4, 2, 0 };
  cout << "dp_util::get_min_palin_cnt" << endl;
  for (int i = 0; i < sizeof(test_input) / sizeof(string); i++) {
    int min_palin_cnt = dp_util::get_min_palindrome_cnt(test_input[i]);
    cout << "1-" << i << ": " << test_input[i] << " | min-palin-cuts: "
         << test_output[i] << " vs " << min_palin_cnt << endl;
    assert(min_palin_cnt == test_output[i]);
  }

  cout << "2. dp_util::is_input_via_scramble" << endl;
  assert(true == dp_util::is_input_via_scramble("great", "rgeat"));
  assert(true == dp_util::is_input_via_scramble("great", "rgtae"));
  assert(false == dp_util::is_input_via_scramble("great", "rtage"));

  cout << "3. dp_util::is_input_via_interleave" << endl;
  assert(true == dp_util::is_input_via_interleave("aabcc", "dbbca", "aadbbcbcac"));
  assert(false == dp_util::is_input_via_interleave("aabcc", "dbbca", "aadbbbaccc"));

  return 0;
}
