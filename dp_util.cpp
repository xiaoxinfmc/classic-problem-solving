#include <cstdio>
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

namespace dp_util{
  template <class Type>
  static void print_all_elem(const vector<Type> input) {
    cout << "[ ";
    for (auto arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }
  template <class Type>
  static void print_all_elem_vec(const vector<vector<Type>> input) {
    cout << "[" << endl;
    for (auto arr : input){ cout << "  "; print_all_elem<Type>(arr); }
    cout << "]" << endl;
  }

  /**
   * 13.3 Palindrome Partitioning II
   * - Given a string s, partition s such that every substring of the partition
   *   is a palindrome. Return the minimum cuts needed for a palindrome
   *   partitioning of s.
   * - For example, given s = "aab",
   *   Return 1 since the palindrome partitioning ["aa","b"] could be produced
   *   using 1 cut.
   * Obvervation:
   * - Only need to get the min cuts instead of actual plan, min-cut of s[0..i]
   *   can be seen as a sub-problem of s[0..n], then considering DP.
   * - Let min_plain_cnt[i] denotes the # of palindromes produced by opt. cutting
   *   based on input of s[0..i], then s[0..i + 1] depends on: {
   *   - if (s[0..i + 1] is a palindrome) => 1
   *   - if (s[0..i + 1] not a palindrom) =>
   *       min{ 0 < k <= i | s[0..k] + 1 if (s[k + 1..i + 1] is a palindrome) }
   *       we won't discuss the case where s[k + 1..i + 1] is not a palindrome
   *       due to the fact that
   *       1. always a base case of s[0..i] + 1
   *       2. more refined partition of s[k + 1..i + 1] is part of subproblem
   *          xxx xxx xxx | kk yyy z => xxx xxx xxx kk yyy | z
   */
  static int get_min_palindrome_cnt(const string & input_str) {
    int str_len = input_str.size();
    if (str_len <= 0) { return 0; }
    vector<int> min_palin_token_cnt(str_len, INT_MAX);

    /* pre-calc all plaindrome check for lookup for every substr */
    vector<vector<bool>> plaindrome_check_lookup(str_len, vector<bool>(str_len, false));
    for (int i = 0; i < str_len; i++) { plaindrome_check_lookup[i][i] = true; }
    for (int i = str_len - 1; i >= 0; i--) {
      for (int j = i + 1; j < str_len; j++) {
        plaindrome_check_lookup[i][j] = (input_str[i] == input_str[j]);
        if (j > i + 1) { plaindrome_check_lookup[i][j] = ((true == plaindrome_check_lookup[i][j]) &&
                                                          (true == plaindrome_check_lookup[i + 1][j - 1])); }
      }
    }
    /* start dp, if (s[0..i + 1] is a palindrome) => 1
     *           else min{ 0 < k <= i | s[0..k] + 1 if (s[k + 1..i + 1] is a palindrome) } */
    for (int i = 0; i < str_len; i++) {
      if (0 == i) { min_palin_token_cnt[0] = 1; continue; }
      if (true == plaindrome_check_lookup[0][i]) { min_palin_token_cnt[i] = 1; continue; }
      for (int k = 0; k < i; k++) {
        if (true == plaindrome_check_lookup[k + 1][i]) {
          min_palin_token_cnt[i] = min(min_palin_token_cnt[i], min_palin_token_cnt[k] + 1);
        }
      }
    }

    /* 5 palindrome tokens means 4 cuts */
    return min_palin_token_cnt.back() - 1;
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
   * Observation:
   * - s1 & s2 are scramble-pair iff exisit at least an i, such that
   *   s1[0..i] is-scramble-of s2[0..i] &&
   *   s1[i + 1..n] is-scramble-of s2[i + 1..n]
   *
   * - Let scramble_lookup[i][j] denotes s1[i..j] is-scramble-of s2[i..j]
   *   goal is to calc. true|false for scramble_lookup[0][n - 1]
   * - to calc. scramble_lookup[i][j], we have
   * - (i == j) iff s1 == s2 => scramble_lookup[i][i] => true
   * - (j - i == 1) iff s1 == s2 || s1.reverse == s2.reverse
   * - (j - i >= 2) iff s1 == s2 || { exists i <= k < j | scramble_lookup[i][k] && scramble_lookup[k + 1][j]
   * - O(n^3)
   *
   * - Or let scramble_lookup[i] denotes s1[i..n - 1] is-scramble-of s2[i..n - 1]
   *   Goal is to calc. the value of scramble_lookup[0].
   * - to calc. scramble_lookup[i], we already know scramble_lookup[i + 1] ... scramble_lookup[n - 1]
   * - scramble_lookup[i] is true iff
   *   1. exists an j such that { i <= j < n | scramble_lookup[k] && scramble_lookup[j + 1] }
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
  /*
  static bool is_input_via_scramble(const string & base,const string & str_chk){
    if (base.size() != str_chk.size()) { return false; }
    if (base.empty() && str_chk.empty()) { return true; }
    int str_len = base.size();
    vector<vector<bool>> scramble_lookup(str_len, vector<bool>(str_len, false));
    for (int i = 0; i < str_len; i++) { scramble_lookup[i][i] = (base[i] == str_chk[i]); }
    for (int i = str_len - 1; i >= 0; i--) {
      for (int j = i + 1; j < str_len; j++) {
        if (j == i + 1) {
          scramble_lookup[i][j] = ((base[i] == str_chk[i] && base[j] == str_chk[j]) ||
                                   (base[i] == str_chk[j] && base[j] == str_chk[i]));
        } else {
          // - (j - i >= 2) iff s1 == s2 || { exists i <= k < j | scramble_lookup[i][k] && scramble_lookup[k + 1][j]
          for (int k = i; k < j; k++) {
            scramble_lookup[i][j] = ((scramble_lookup[i][k] && scramble_lookup[k + 1][j]) ||
                                     (scramble_lookup[i][k] && scramble_lookup[k + 1][j]))
            if (true == scramble_lookup[i][j]) { break; }
          }
        }
      }
    }
    return scramble_lookup.front().back();
  }
  */
  enum { STRATEGY_UNDEF = -1, STRATEGY_FAIL, STRATEGY_SUCCESS };

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

  static bool is_input_via_scramble(const string & base,const string & str_chk){
    if (base.size() != str_chk.size()) { return false; }
    unordered_map<string, int> strategy_lookup;
    bool is_scramble = check_scramble(base, str_chk, strategy_lookup);
    return is_scramble;
  }

  /**
   * Given s1, s2, s3, find whether s3 is formed by the interleaving of
   * s1 and s2. For example, Given: s1 = "aabcc", s2 = "dbbca",
   * When s3 = "aadbbcbcac", return true.
   * When s3 = "aadbbbaccc", return false.
   * - input -> T1 T2 check if P is a interleave of T1 & T2
   *   P[0..i + j + 1] is a interleave of T1[0..i] & T2[0..j] then
   *   P[0..i + j] must be a interleave of T1[0..i - 1] & T2[0..j] OR
   *                                       T1[0..i] & T2[0..j + 1]
   *   P(i, j) {
   *   - true if (((T1[i] == P[i + j]) && P(i - 1, j)) ||
   *              ((T2[j] == P[i + j]) && P(i, j - 1)))
   * - chk(i, j) => p[i + j ... n] is an interleave of t[i..n - 1] & s[j..n - 1]
   * - chk(i, j) => {
   *   - t[i] == p[i + j] && chk(i + 1, j) || s[j] == p[i + j] && chk(i, j + 1);
   *   }
   */
  static string get_check_signature(int pos_l, int pos_r, int pos_s) {
    return to_string(pos_l) + "$" + to_string(pos_r) + "$" + to_string(pos_s);
  }

  static bool is_input_via_interleave(const string & base_l,
                                      const string & base_r,
                                      const string & str_chk,
                                      unordered_map<string, bool> & lookup,
                                      int pos_l = 0,
                                      int pos_r = 0,
                                      int pos_s = 0) {

    if ((base_l.size() - pos_l) + (base_r.size() - pos_r) !=
        (str_chk.size() - pos_s)) { return false; }
    if (str_chk.size() == pos_s) { return true; }
    if ((str_chk[pos_s] != base_l[pos_l]) &&
        (str_chk[pos_s] != base_r[pos_r])) { return false; }
    string signature = get_check_signature(pos_l, pos_r, pos_s);
    if (lookup.end() != lookup.find(signature)) { return lookup[signature]; }
    // return true if all chars str_chk been checked already.
    bool is_interleave = (
      ((str_chk[pos_s] == base_l[pos_l]) &&
       (is_input_via_interleave(base_l, base_r, str_chk, lookup,
                                pos_l + 1, pos_r, pos_s + 1))) ||
      ((str_chk[pos_s] == base_r[pos_r]) &&
       (is_input_via_interleave(base_l, base_r, str_chk, lookup,
                                pos_l, pos_r + 1, pos_s + 1)))
    );
    lookup[signature] = is_interleave;
    return is_interleave;
  }

  static bool _check_input_via_interleave(const string & base_l,
                                          const string & base_r,
                                          const string & str_chk) {
    unordered_map<string, bool> lookup;
    return is_input_via_interleave(base_l, base_r, str_chk, lookup, 0, 0, 0);
  }
  /**
   *   P(i, j) { T1[0..i] & T2[0..j] could form P[0..i + j + 1]
   *   - true if (((T1[i] == P[i + j]) && P(i - 1, j)) ||
   *              ((T2[j] == P[i + j]) && P(i, j - 1)))
   */
  static bool check_input_via_interleave(const string & base_l,
                                         const string & base_r,
                                         const string & str_chk) {
    // vector, cnt & value
    vector<vector<bool>> lookup(base_l.size(), vector<bool>(base_r.size(), false));
    for (int i = 0; i < base_l.size(); i++) {
      for (int j = 0; j < base_r.size(); j++) {
        if (i == 0 && j == 0) {
          lookup[i][j] = (base_l[i] == str_chk[i] && base_r[j] == str_chk[j + 1]);
        } else if (i == 0) {
          lookup[i][j] = (((base_l[i] == str_chk[i + j + 1])) ||
                          ((base_r[j] == str_chk[i + j + 1]) && lookup[i][j - 1]));
        } else if (j == 0) {
          lookup[i][j] = (((base_l[i] == str_chk[i + j + 1]) && lookup[i - 1][j]) ||
                          ((base_r[j] == str_chk[i + j + 1])));
        } else {
          lookup[i][j] = (((base_l[i] == str_chk[i + j + 1]) && lookup[i - 1][j]) ||
                          ((base_r[j] == str_chk[i + j + 1]) && lookup[i][j - 1]));
        }
      }
    }
    return lookup.back().back();
  }

  /**
   * Given a triangle, find the maximum path sum from top to bottom. Each step you
   * may move to adjacent numbers on the row below.
   * For example, given the following triangle
   * [
   *     [2], ---- 0 i
   *    [3,4], --- 1
   *   [6,5,7], -- 2, i => (k, k || k + 1)
   *  [4,1,8,3]  - 3 (0, 1, 2, 3, j)
   * ]
   * The maximum path sum from top to bottom is 11 (i.e., 2 + 3 + 5 + 1 = 11).
   * Note: Bonus point if you are able to do this using only O(n) extra space,
   *       where n is the total number of rows in the triangle.
   * max_sum(i, k) = { m[0, 0], i == 0;
   *                   max{ max_sum(i - 1, k - 1) + m[i, k],
   *                        max_sum(i - 1, k)     + m[i, k] }
   * - i => current row id of input of vectors.
   * - k => current index @ end of the path on row i.
   */
  static int find_max_elem(const vector<int> & int_vec) {
    int max_val = int_vec.front();
    for (auto & val : int_vec) { if (val > max_val ) { max_val = val; } }
    return max_val;
  }

  static int calc_max_sum_triangle(const vector<vector<int>> input_matrix){
    if (input_matrix.empty()) { return INT_MAX; }
    vector<int> prev_sum_buf(input_matrix.back().size(), INT_MAX);
    vector<int> curr_sum_buf(input_matrix.back().size(), INT_MAX);
    vector<int> & curr_sum_buf_ref = curr_sum_buf,
                & prev_sum_buf_ref = prev_sum_buf,
                & temp_sum_buf_ref = prev_sum_buf;

    prev_sum_buf[0] = input_matrix[0][0];
    for (size_t row_id = 1; row_id < input_matrix.size(); row_id++) {
      for (size_t col_id = 0; col_id < input_matrix[row_id].size(); col_id++) {
        if (0 == col_id) {
          curr_sum_buf[col_id] = input_matrix[row_id][col_id] + prev_sum_buf[col_id];
        } else if (input_matrix[row_id].size() - 1 == col_id) {
          curr_sum_buf[col_id]=input_matrix[row_id][col_id]+prev_sum_buf[col_id - 1];
        } else {
          curr_sum_buf[col_id] = max(
            input_matrix[row_id][col_id] + prev_sum_buf[col_id],
            input_matrix[row_id][col_id] + prev_sum_buf[col_id - 1]
          );
        }
      }
      temp_sum_buf_ref = curr_sum_buf_ref;
      curr_sum_buf_ref = prev_sum_buf_ref;
      prev_sum_buf_ref = temp_sum_buf_ref;
    }

    return find_max_elem(prev_sum_buf_ref);
  }

  /**
   * 13.12 Word Break
   * Given a string s and a dictionary of words dict, determine if s can be
   * segmented into a space-separated sequence of one or more dictionary words.
   * For example, given s = "leetcode", dict = ["leet", "code"].
   * Return true because "leetcode" can be segmented as "leet code".
   *
   * Observation:
   * - break-lookup[i] => denotes s[0..i] is breakable, then we have
   *   goal is to calc. break-lookup[n - 1]
   * - break-lookup[i] is true iff exists an j, such that
   *   { 0 <= j < i | break-lookup[j] && s[j..i] is in our dict }
   *
   * for w[0..n], assume w[0..n - 1] is breakable, then good if w[n] in dict
   * w[0..n] can be break up if exist j, where w[0..j] is breakable and
   * w[j + 1..n] form a token found in a dict. => calc. w[0..n]
   *
   * if w.size = 1 ==>> good if w[0] in dict
   * 0 1 2 3 => 0..3   0 <= i <= 3   0 <= j <= i
   * if w.size = i + 1, ==>> good if w[0..j - 1] breakable && w[j..i] in dict
   *
   * w[0..n] is breakable if w[0..j] is breakable && dict has token w[j + 1..n]
   * enum { STRATEGY_UNDEF = -1, STRATEGY_FAIL, STRATEGY_SUCCESS };
   */
  static bool is_word_breakable(const unordered_set<string> dict,
                                const string word) {
    if (true == word.empty()) { return true; }
    vector<bool> break_lookup(word.size(), STRATEGY_UNDEF);
    for (int i = 0; i < word.size(); i++) {
      break_lookup[i] = (dict.end() != dict.find(word.substr(0, i + 1)));
      if (true == break_lookup[i]) { continue; }
      for (int j = i; j > 0; j--) {
        if (true == break_lookup[i]) { break; }
        break_lookup[i] = (dict.end() != dict.find(word.substr(j, (i - j + 1))) &&
                           true == break_lookup[j - 1]);
      }
    }
    return break_lookup.back();
  }

  /**
   * 13.11 Distinct Subsequences
   * Given a string S and a string T, count the number of distinct subsequences
   * of T in S. A subsequence of a string is a new string which is formed from
   * the original string by deleting some (can be none) of the characters
   * without disturbing the relative positions of the remaining characters.
   * (ie, "ACE" is a subsequence of "ABCDE" while "AEC" is not).
   * Here is an example: S = "rabbbit", T = "rabbit" Return 3.
   * - S[0..i..m] & T[0..j..n], F(i, j) => # of dist subseq by S[0..i] & T[0..j]
   *   for F[i, j], i >= j (i < j, all value will be 0 as token is longer)
   *     if S[i] == T[j] => F[n - 1, m - 1] + F[n - 1, m]
   *     else            => F[n - 1, m]
   *   rabbbit
   *   rabbit
   * Observation:
   * - seq-cnt(i, j) => dist seq cnt for t[0..i] in s[0..j], then
   *   goal is to calc seq-cnt(m - 1, n - 1);
   * - say i from 0 ~ m - 1, j from i ~ n - 1;
   *   if t[i] == s[j], seq-cnt(i, j) == seq-cnt(i, j - 1) + seq-cnt(i - 1, j - 1)
   *   if t[i] != s[j], seq-cnt(i, j) == seq-cnt(i, j - 1)
   */
  static int count_distinct_subseq(const string text, const string seq) {
    int text_len = text.size(), seq_len = seq.size();
    if ((true == seq.empty()) || (true == text.empty()) ||
        (seq_len > text_len)) { return 0; }
    vector<vector<int>> seq_cnt_lookup(text_len, vector<int>(seq_len, 0));
    for (int i = 0; i < text_len; i++) {
      for (int j = 0; ((j <= i) && (j < seq.size())); j++) {
        if (0 == j && 0 == i) {
          seq_cnt_lookup[i][j] = (text[i] == seq[j]) ? 1 : 0;
        } else if (0 == j) {
          seq_cnt_lookup[i][j] = ((text[i] == seq[j]) ?
            (1 + seq_cnt_lookup[i - 1][j]) : (seq_cnt_lookup[i - 1][j]));
        } else {
          seq_cnt_lookup[i][j] = ((text[i] == seq[j]) ?
            (seq_cnt_lookup[i - 1][j - 1] + seq_cnt_lookup[i - 1][j]) :
            (seq_cnt_lookup[i - 1][j]));
        }
      }
    }
    return seq_cnt_lookup[text_len - 1][seq_len - 1];
  }

  /**
   * 13.10 Decode Ways
   * A message containing letters from A-Z is being encoded to numbers
   * using the following mapping: 'A' -> 1 'B' -> 2 ... 'Z' -> 26
   * Given an encoded message containing digits, determine the total number
   * of ways to decode it. For example, Given encoded message "12", it could
   * be decoded as AB(1 2) or L(12). The number of ways decoding "12" is 2.
   * - T[n] => # of ways to decode input str[0..n]
   *   Assume we already know T[n - 1] & T[n - 2], then
   *   if token[n-1..n] is valid, T[n] = T[n - 2];
   *   if token[n..n] is valid, T[n] += T[n - 1];
   *   return 0 if none of them is good
   *
   * Observation:
   * - Only total # of decoding plans are needed, with subproblem nature, dp.
   * - let s -> input digits of n, decode-cnt(i) => # of ways to decode s[0..i]
   * - decode-cnt(i) = {
   *     if (last-2 digit valid) { decode-cnt(i - 1) + decode-cnt(i - 2) }
   *     else { decode-cnt(i - 1) }
   *   }
   */
  static bool is_single_char_valid(const string & number_str, int start_pos) {
    return (number_str[start_pos] >= '1' && number_str[start_pos] <= '9');
  }

  static bool is_double_char_valid(const string & number_str, int start_pos) {
    return (
      (number_str[start_pos] == '1' && number_str[start_pos + 1] >= '0'
                                    && number_str[start_pos + 1] <= '9') ||
      (number_str[start_pos] == '2' && number_str[start_pos + 1] >= '0'
                                    && number_str[start_pos + 1] <= '6'));
  }

  static int get_decoding_count(const string & number_str) {
    vector<int> decode_cnt_lookup(number_str.size(), 0);
    int input_len = number_str.size();
    if (0 == input_len) { return 0; }
    if (1 == input_len) {
      return (true == is_single_char_valid(number_str, 0) ? 1 : 0);
    }
    decode_cnt_lookup[0] = (true == is_single_char_valid(number_str, 0) ? 1 : 0);
    if (0 == decode_cnt_lookup[0]) { return 0; }
    decode_cnt_lookup[1] = ((true == is_single_char_valid(number_str, 1)) ? 1 : 0) +
                           ((true == is_double_char_valid(number_str, 0)) ? 1 : 0);
    for (int i = 2; i < input_len; i++) {
      if (true == is_single_char_valid(number_str, i)) {
        decode_cnt_lookup[i] = decode_cnt_lookup[i - 1];
      }
      if (true == is_double_char_valid(number_str, i - 1)) {
        decode_cnt_lookup[i] += decode_cnt_lookup[i - 2];
      }
      if (0 == decode_cnt_lookup[i]) { return 0; }
    }
    return decode_cnt_lookup.back();
  }

  /**
   * Given an input string (s) and a pattern (p), implement regular expression
   * matching with support for '.' and '*'.
   * '.' Matches any single character.
   * '*' Matches zero or more of the preceding element.
   * The matching should cover the entire input string (not partial).
   * Note:
   * - s could be empty and contains only lowercase letters a-z.
   * - p could be empty and contains only lowercase letters a-z, like . or *.
   * EG:
   * - s ->   a    bc
   * - p -> k*a.*k*bc*
   * - for p to cover whole parts of input s
   *   let lookup(i, j) be p[0..j] covers s[0..i], goal is to calc lookup[i][j]
   *   assume we know all results for lookup[0..i - 1][0..j - 1], lookup[i][j]?
   *   # abc && abc i > 0 & j > 0
   *   lookup[i][j] => good? for s[0..i-1] && p[0..j], s[0..i-1] && p[0..j-1], s[0..i] && p[0..j-1]
   *   # abc k*ab*k*c.*k*          a aa
   *                            |===============================v--+===========================v--+
   *                            v                                  |                              |
   *     ( a, k) F ( a, k*) F ( a, k*a) T ( a, k*ab) F ( a, k*ab*) T ( a, k*ab*k) F ( a, k*ab*k*) T
   *      ^      |  ^       |   ^           ^            ^             ^(match end)   ^
   *             v          v
   *     (ab, k) F (ab, k*) F (ab, k*a) F (ab, k*ab) T (ab, k*ab*) T (ab, k*ab*k) F (ab, k*ab*k*) T
   *     ^         ^-----------^(not full)--^------------^-------------^--------------^
   *                    a != *, (a, k*) && (b !=a) => F => lookup[i - 1][j - 1] F, not max-cover
   *                          b != *, (a, k*a) && (b == a) => T lookup[i - 1][j - 1] && S[i] == P[j] max-cover
   *                                         * == *, ((ab, k*a) F || (ab, k*ab) T) => T
   *                                                              k != *, (a, k*ab*) || (k != b) => F
   *                                                              * == *, (ab, k*ab*) T || (ab, k*ab*k) F => T
   * - aa a* => (aa, a) F [ (a, a) T &&
   * - 1. p[j] != '*', then
   *        lookup[i][j] = (s[i] == p[j]) && (true == lookup[i - 1][j - 1]);
   * - 2. else
   *        lookup[i][j] = lookup[i][j - 2] || lookup[i][j - 1] ||
   *                       lookup[i - 1][j] && (text[i] == text[i - 1] || '.' == pattern[j - 1] (repeat)
   * - when 0 == i && 0 == j then lookup_buffer[0][0] = (text[0] == pattern[0]);
   * - when 0 == i && j > 0 then
   *   if (* == pattern[j]) then
   *     lookup[i][j] = (j > 1) ? (lookup[i][j - 1] || lookup[i][j - 2]) : (lookup[i][j - 1]);
   *   else  // a <=> a a && a <=> a * a || b * a
   *     lookup[i][j] = ((* == pattern[j - 1]) && (text[i] == pattern[j]))
   */
  static bool is_char_pair_match(char l, char r) { return ((l == r) || ('.' == r)); }

  static bool is_pattern_matched(const string text, const string pattern) {
    if (!text.empty() && pattern.empty()) { return false; }
    if ( text.empty() && pattern.empty()) { return true; }

    int text_len = text.size(); int pattern_len = pattern.size();
    if (text.empty()) {
      if (0 != pattern_len % 2) { return false; }
      for (int i = 0; i < pattern_len; i++) {
        if (1 == i % 2 && pattern[i] != '*') { return false; }
      }
      return true;
    }

    vector<vector<bool>> lookup(text_len, vector<bool>(pattern_len, false));

    lookup[0][0] = is_char_pair_match(text[0], pattern[0]);
    int min_good_char_cnt = lookup[0][0] ? 1 : 0; int min_bad_char_cnt = lookup[0][0] ? 0 : 1;
    for (int j = 1; j < pattern_len; j++) {
      if (is_char_pair_match(text[0], pattern[j])) {
        lookup[0][j] = (0 == min_good_char_cnt && 0 == min_bad_char_cnt);
        min_good_char_cnt += 1;
      } else if ('*' == pattern[j]) {
        if (is_char_pair_match(text[0], pattern[j - 1])) {
          lookup[0][j] = (((j > 1) && (lookup[0][j - 2])) ||
                          ((lookup[0][j - 1])));
          min_good_char_cnt -= 1;
        } else {
          lookup[0][j] = ((j > 1) && (lookup[0][j - 2]));
          min_bad_char_cnt -= 1;
        }
      } else { min_bad_char_cnt += 1; }
    }
    for (int i = 1; i < text_len; i++) {
      for (int j = 1; j < pattern_len; j++) {
        if ('*' != pattern[j]) {
          lookup[i][j] = ((true == is_char_pair_match(text[i], pattern[j])) &&
                          (true == lookup[i - 1][j - 1]));
        } else {
          lookup[i][j] = (((true == is_char_pair_match(text[i], pattern[j - 1])) &&
                           (true == is_char_pair_match(text[i - 1], pattern[j - 1])) &&
                                      (true == lookup[i - 1][j])) || // pat[j - 1] appear at least twice
                          ((j > 1) && (true == lookup[i][j - 2])) || // pat[j - 1] did not appear
                                      (true == lookup[i][j - 1]));   // pat[j - 1] appear once
        }
      }
    }
    return lookup.back().back();
  }

  /**
   * Dynamic Programming | Set 29 (Longest Common Substring)
   * Given two strings 'X' and 'Y', find length of longest common substring.
   * Examples :
   * Input : X = "GeeksforGeeks", y = "GeeksQuiz"
   * Output : 5
   * The longest common substring is "Geeks" and is of
   * length 5
   * Input : X = "abcdxyz", y = "xyzabcd"
   * Output : 4
   * The longest common substring is "abcd" and is of
   * length 4
   * Input : X = "zxabcdezy", y = "yzabcdezx"
   * Output : 6
   * The longest common substring is "abcdez" and is of length 6
   *
   * Analysis:
   * - Let lcs(i, j) denote the length of lcs for x[0..i] & y[0..j]
   *   with x[i] and y[j] matched
   *   lcs(i, j) = {
   *     if x[i] == y[j] lcs(i - 1, j - 1) + 1
   *     else 0
   *   }
   */
  static int calc_lcstr_len(string l_str, string r_str) {
    int lcs_max = 0;
    vector<int> lcs_lookup(r_str.size(), 0);
    for (int i = 0; i < l_str.size(); i++) {
      for (int j = r_str.size() - 1; j >= 0; j--) {
        if (l_str[i] == r_str[j]) {
          lcs_lookup[j] = (j > 0) ? lcs_lookup[j - 1] + 1 : 1;
          lcs_max = max(lcs_max, lcs_lookup[j]);
        } else { lcs_lookup[j] = 0; }
      }
    }
    return lcs_max;
  }

  /**
   * def lcs_buf[i][j] => lcs-len of l_str[0..i] & r_str[0..j],
   * to calc lcs_buf[i][j], assume we already know lcs_buf[0..i - 1][0..j - 1]
   *                                               lcs_buf[0..i - 1][j]
   *                                               lcs_buf[0..i][j - 1]
   * lcs_buf[0][0] = (l_str[0] == r_str[0]) ? 1 : 0;
   * if l_str[i] == r_str[j]
   *   lcs_buf[i][j] = lcs_buf[0..i - 1][0..j - 1] + 1
   * else
   *   lcs_buf[i][j] = max{ lcs_buf[0..i - 1][0..j],
   *                        lcs_buf[0..i][j - 1] }
   * end
   */
  static int calc_lcs_len(const string l_str, const string r_str) {
    if (l_str.empty() || r_str.empty()) { return 0; }
    vector<vector<int>> lcs_buffer(l_str.size(), vector<int>(r_str.size(), 0));
    for (int i = 0; i < l_str.size(); i++) {
      for (int j = 0; j < r_str.size(); j++) {
        if (i == 0 && j == 0) {
          lcs_buffer[0][0] = (l_str[0] == r_str[0]) ? 1 : 0; continue;
        }
        if (l_str[i] == r_str[j]) {
          if (0 == i || j == 0) {
            lcs_buffer[i][j] = (j > 0) ? max(lcs_buffer[i][j - 1], 1) :
                                         max(lcs_buffer[i - 1][j], 1);
          } else {
            lcs_buffer[i][j] = lcs_buffer[i - 1][j - 1] + 1;
          }
        } else {
          lcs_buffer[i][j] = (j > 0) ? lcs_buffer[i][j - 1] : 0;
          if (i > 0) {
            lcs_buffer[i][j] = max(lcs_buffer[i - 1][j], lcs_buffer[i][j]);
          }
        }
      }
    }
    return lcs_buffer.back().back();
  }

  /**
   * Given an unsorted array of int, find the len of longest increasing subseq.
   * Example:
   * Input: [10,9,2,5,3,7,101,18,0 ]
   * Output: 4
   * Explanation: The longest increasing subsequence is [2,3,7,101], therefore
   *              the length is 4.
   * Note:
   * - There may be more than one LIS combination, it is only necessary for you to
   *   return the length. Your algorithm should run in O(n2) complexity.
   *   Follow up: Could you improve it to O(nlogn) time complexity?
   *
   * Solution:
   * - let lis(i) denote the len of longest increasing subseq for input(0..i),
   *   with input[i] been selected, then the goal is to calc the max of lis(i)
   * - assume we know lis(0)..lis(i - 1), with base case of lis(0) -> 1, and to
   *   calc. lis(i), we need to go over lis(0) -> lis(i - 1), to see max ext.
   * - lis(i) = max{ 0 <= k <= i - 1, input(i) > input(k) ? lis(k) + 1 : lis(i) }
   */
  static int calc_longest_incr_subseq_len(vector<int> input) {
    vector<int> lis_lookup(input.size(), 1);
    int max_lis_len = INT_MIN;
    for (int i = 1; i < input.size(); i++) {
      for (int k = 0; k < i; k++) {
        if (input[i] > input[k]) {
          lis_lookup[i] = max(lis_lookup[i], lis_lookup[k] + 1);
        }
      }
      max_lis_len = max(max_lis_len, lis_lookup[i]);
    }
    return max_lis_len;
  }

  /**
   * Partition a set into two subsets such that diff. of subset sums is minimum
   * Given a set of integers, the task is to divide it into two sets S1 and S2
   * such that the absolute difference between their sums is minimum.
   *
   * If there is a set S with n elements, then if we assume Subset1 has m
   * elements, Subset2 must have n-m elements and the value of
   * abs(sum(Subset1) – sum(Subset2)) should be minimum.
   *
   * Example:
   * Input:  arr[] = {1, 6, 11, 5}
   * Output: 1
   * Explanation:
   * Subset1 = {1, 5, 6}, sum of Subset1 = 12 
   * Subset2 = {11}, sum of Subset2 = 11
   * Analysis:
   * - 0/1 knapsack with a special goal to optimize
   * - Let subset_diff_lookup(i, j) be the diff of subset-1 given input(0..i) and
   *   current max possible sum of j, subset-1 give us the smallest diff.
   * - subset_diff_lookup(0, j) => j
   * - subset_diff_lookup(i, j) = {
   *     if (abs(subset_diff_lookup[i - 1][j - input[i]] - 2 * input[i]) <
   *         abs(j - 2 * subset_diff_lookup[i - 1][j])) {
   *       subset_diff_lookup(i, j) = subset_diff_lookup[i - 1][j - input[i]] - 2 * input[i]
   *     } else {
   *       subset_diff_lookup(i, j) = subset_diff_lookup[i - 1][j]
   *     }
   *   }
   * - goal is to calc all subset_diff_lookup, then return bottom right
   */
  static int calc_min_partition_diff(vector<int> input) {
    int sum = 0, min_diff = INT_MAX;
    for (auto & val : input) { sum += val; }
    vector<vector<int>> subset_diff_lookup(input.size() + 1, vector<int>(sum + 1, 0));
    // do not have anything to pack
    for (int i = 0; i <= sum; i++) { subset_diff_lookup[0][i] = sum; }
    // do not have enough cap. to pack anything
    for (int i = 0; i <= input.size(); i++) { subset_diff_lookup[i][0] = sum; }
    for (int i = 1; i <= input.size(); i++) {
      for (int j = 1; j <= sum; j++) {
        if (j - input[i] < 0) {
          subset_diff_lookup[i][j] = subset_diff_lookup[i - 1][j];
        } else if (abs(subset_diff_lookup[i - 1][j - input[i]] - 2 * input[i]) <
                   abs(subset_diff_lookup[i - 1][j])) {
          subset_diff_lookup[i][j] = subset_diff_lookup[i - 1][j - input[i]] - 2 * input[i];
        } else {
          subset_diff_lookup[i][j] = subset_diff_lookup[i - 1][j];
        }
      }
    }
    for (int i = 1; i <= sum; i++) {
      min_diff = min(min_diff, abs(subset_diff_lookup.back()[i]));
    }
    return min_diff;
  }

  /**
   * Find the longest path in a matrix with given constraints
   * Given a n*n matrix where all numbers are distinct, find the maximum length
   * path (starting from any cell) such that all cells along the path are in
   * increasing order with a difference of 1.
   *
   * We can move in 4 directions from a given cell (i, j), i.e., we can move to
   * (i+1, j) or (i, j+1) or (i-1, j) or (i, j-1) with the condition that the
   * adjacent cells have a difference of 1.
   *
   * Example:
   * Input:  mat[][] = {{1, 2, 9}
   *                    {5, 3, 8}
   *                    {4, 6, 7}}
   * Output: 4
   * The longest path is 6-7-8-9
   *
   * Analysis:
   * - each value is distinct, and only 4 possible moving direction, as step is
   *   1 and only 1, so each move is 1 direction ONLY!
   * - let path_len_lookup(i, j) denote the path len starts with (i, j), ->
   *   path_len_lookup(i, j) = {
   *     if (i < n - 1 && input[i][j] == 1 + input[i + 1][j]) { return path_len_lookup(i + 1, j) + 1; }
   *     if (i > 0 && input[i][j] == 1 + input[i - 1][j]) { return path_len_lookup(i - 1, j) + 1; }
   *     if (j < n - 1 && input[i][j] == 1 + input[i][j + 1]) { return path_len_lookup(i, j + 1) + 1; }
   *     if (j > 0 && input[i][j] == 1 + input[i][j - 1]) { return path_len_lookup(i, j - 1) + 1; }
   *     return 1
   *   }
   */
  static int calc_path_start_from_recur(int i, int j,
                                        const vector<vector<int>> & input,
                                              vector<vector<int>> & lookup) {
    if (lookup[i][j] > 0) { return lookup[i][j]; }
    if (i < input.size() - 1 && input[i][j] == 1 + input[i + 1][j]) {
      return calc_path_start_from_recur(i + 1, j, input, lookup) + 1;
    }
    if (j < input.size() - 1 && input[i][j] == 1 + input[i][j + 1]) {
      return calc_path_start_from_recur(i, j + 1, input, lookup) + 1;
    }
    if (i > 0 && input[i][j] == 1 + input[i - 1][j]) {
      return calc_path_start_from_recur(i - 1, j, input, lookup) + 1;
    }
    if (j > 0 && input[i][j] == 1 + input[i][j - 1]) {
      return calc_path_start_from_recur(i, j - 1, input, lookup) + 1;
    }
    return 1;
  }

  static int calc_max_incr_path_len(vector<vector<int>> input) {
    vector<vector<int>> lookup(
      input.size(), vector<int>(input.front().size(), 0)
    );
    int max_len = 0;
    for (int i = 0; i < input.size(); i++) {
      for (int j = 0; j < input.size(); j++) {
        lookup[i][j] = calc_path_start_from_recur(i, j, input, lookup);
        max_len = max(max_len, lookup[i][j]);
      }
    }
    return max_len;
  }

  /**
   * Given a set of non-negative integers, and a value sum, determine if there
   * is a subset of the given set with sum equal to given sum.
   * Examples: set[] = {3, 34, 4, 12, 5, 2}, sum = 9
   * Output:  True => There is a subset (4, 5) with sum 9.
   * Analysis:
   * - Given input[0..i] & sum, check if any subset mounts to sum?
   * - let lookup(i, j) => i : subset from input[0..i]
   *                       j : target sum
   *       lookup(i, j) => sum of the corresponding subset.
   * - constraint is 9, then simply pack as much as possible to see if we reach 9
   * - goal is to calc and check for anything in lookup equals to 9
   * - init all values to -1;
   *   if lookup(i - 1, j - input[i]) + input[i] <= j {
   *     lookup(i, j) = max(lookup(i - 1, j - input[i]) + input[i], lookup(i - 1, j))
   *   } else {
   *     lookup(i, j) = lookup(i - 1, j)
   *   }
   */
  static bool check_subset_sum(vector<int> input, int sum) {
    bool is_sum_existed = false;
    vector<vector<int>> lookup(input.size(), vector<int>(sum + 1, 0));
    for (int i = 0; i < input.size(); i++) {
      for (int j = 0; j <= sum; j++) {
        if (0 == i) {
          lookup[i][j] = (j >= input[i]) ? input[i] : 0;
        } else {
          lookup[i][j] = lookup[i - 1][j];
          if (j >= input[i] && lookup[i - 1][j - input[i]] + input[i] <= j) {
            lookup[i][j] = max(lookup[i - 1][j - input[i]] + input[i], lookup[i][j]);
          }
        }
        is_sum_existed = (!is_sum_existed && sum == lookup[i][j]);
      }
    }
    return is_sum_existed;
  }

  /**
   * Dynamic Programming | Set 8 (Matrix Chain Multiplication)
   * Given a sequence of matrices, find the most efficient way to multiply
   * matrices together. The problem is not actually to perform multiplications,
   * but merely to decide in which order to perform the multiplications.
   * We have many options to multiply a chain of matrices because matrix
   * multiplication is associative. In other words, no matter how parenthesize
   * the product, the result will be the same. For example, if we had four
   * matrices A, B, C, and D, we would have:
   * (ABC)D = (AB)(CD) = A(BCD) = ....
   * However, the order in which we parenthesize the product affects number
   * of simple arithmetic operations needed to compute the product, or the
   * efficiency. For example, suppose A is a 10 * 30 matrix, B is a 30 * 5
   * matrix, and C is a 5 * 60 matrix. Then,
   * (AB)C = (10×30×5) + (10×5×60) = 1500 + 3000 = 4500 operations
   * A(BC) = (30×5×60) + (10×30×60) = 9000 + 18000 = 27000 operations.
   * Input: p[] = {40, 20, 30, 10, 30}
   * Output: 26000
   * - There are 4 matrices of dimensions 40x20, 20x30, 30x10 and 10x30.
   *   Let the input 4 matrices be A, B, C and D.  The minimum number of
   *   multiplications are obtained by putting parenthesis in following way
   *   (A(BC))D --> 20*30*10 + 40*20*10 + 40*10*30
   * Input: p[] = {10, 20, 30, 40, 30}
   * Output: 30000
   * - There are 4 matrices of dimensions 10x20, 20x30, 30x40 and 40x30.
   *   Let the input 4 matrices be A, B, C and D.  The minimum number of
   *   multiplications are obtained by putting parenthesis in following way
   *   ((AB)C)D --> 10*20*30 + 10*30*40 + 10*40*30
   * Input: p[] = {10, 20, 30}
   * Output: 6000
   * - There are only two matrices of dimensions 10x20 and 20x30. So there
   *   is only one way to multiply the matrices, cost of which is 10*20*30
   *
   * Let min_mult_ops(i, j) be the min mult ops need for matrix from i..j
   * to calc min_mult_ops(i, j), we know min_mult_ops(i, k), i < k < j
   * As matrix(j) will always be multiplied with another one produced by the
   * mult of subsets of matrices(by any consequtive matrices next to each).
   * - Goal is to calc min_mult_ops(0, j)
   *   20 x 30, 30 x 40, 40 x 3, 3 x 1
   *            30 x 40 x 1 40 x 3 x 1
   * - min_mult_ops(i, j) = {
   *     0 if j == i
   *     ops_cnt(m[i], m[j]) if j == i + 1
   *     min { i <= k < j | min_mult_ops(i, k) +
   *                        min_mult_ops(k + 1, j) +
   *                        row[i] * col[k + 1] * col[j + 1] }
   *  }
   *            0   1   2   3
   * 26000 <=> 40, 20, 30, 10, 30
   * [   0     1     2     3                       1 - k             40 * 30 * 10 -> 12k                         6k        8k
   * 0 [ 0 24000 36000 48000 ] (0, 1) (0, 2) { m(0, 1) 24k + m[1][1] + m[i] * m[k + 1] * m[3] => 36k, | m(0, 0) + m(1, 2) + 40 * 20 * 10 => 14k }
   * 1 [ 0     0  6000 12000 ] (1, 2) (1, 3) { 20 * 30 * 10 + 20 * 10 * 30 => 12000, 9000 + 20 * 30 * 30 } -> i => 1, j = 3, k -> (2) m(1, 2) + m(3, 3) + 20 * 10 * 30 => 12000
   * 2 [ 0     0     0  9000 ] (2, 3)
   * 3 [ 0     0     0     0 ]
   * ]
   */
  static int calc_min_multiply_ops(vector<int> matrix) {
    vector<vector<int>> min_mult_ops(
      matrix.size() - 1, vector<int>(matrix.size() - 1, 0)
    );
    for (int i = min_mult_ops.size() - 1; i >= 0; i--) {
      for (int j = i + 1; j < min_mult_ops.size(); j++) {
        if (j == i + 1) {
          min_mult_ops[i][j] = matrix[i] * matrix[j] * matrix[j + 1]; continue;
        }
        min_mult_ops[i][j] = INT_MAX;
        for (int k = i; k < j; k++) {
          //int ops_cnt = min_mult_ops[i][k] + matrix[i] * matrix[k + 1] * matrix[j + 1];
          //if (k + 1 < j) { ops_cnt += min_mult_ops[k + 1][j]; }
          int ops_cnt = min_mult_ops[i][k] + min_mult_ops[k + 1][j] + matrix[i] * matrix[k + 1] * matrix[j + 1];
          min_mult_ops[i][j] = ops_cnt < min_mult_ops[i][j] ? ops_cnt : min_mult_ops[i][j];
        }
      }
    }
    return min_mult_ops.front().back();
  }

  /**
   * Partition problem is to determine whether a given set can be partitioned
   * into two subsets such that the sum of elements in both subsets is same.
   * Examples
   * - arr[] = {1, 5, 11, 5}
   *   Output: true
   *   The array can be partitioned as {1, 5, 5} and {11}
   * - arr[] = {1, 5, 3}
   *   Output: false 
   *   The array cannot be partitioned into equal sum sets.
   * - Let diff_lookup(i) be the diff. between 2 sets, which consists
   *   a subset from arr[0..i - 1] with arr[i] selected, then its value should
   *   be min{ 0 <= k <= i - 1 | abs(arr[k] + 2 * arr[i]) }
   *   we selected the plan with min abs, but store its actual diff instead of abs.
   *   A + B = total
   *   A - B = arr[i - 1]
   *   arr[i] => (A + arr[i] - (B - arr[i])) => arr[i - 1] + 2 * arr[i]
   */
  static bool is_set_evenly_partitioned(vector<int> arr) {
    bool is_set_evenly_partitioned = false;
    // reserve 1 more represents no elem selected
    vector<int> diff_lookup(arr.size() + 1, 0);
    int total_sum = 0;
    for (auto & val : arr) { total_sum += val; }
    // when we do not choose anything, then diff would be 0 - total.
    diff_lookup[0] = -1 * total_sum;
    for (int i = 1; i <= arr.size(); i++) {
      int min_diff_abs = INT_MAX;
      for (int j = 0; j < i; j++) {
        int curr_diff_abs = abs(diff_lookup[j] + 2 * arr[i]);
        if (curr_diff_abs < min_diff_abs) {
          min_diff_abs = curr_diff_abs;
          diff_lookup[i] = diff_lookup[j] + 2 * arr[i];
        }
      }
      if (0 == diff_lookup[i]) { is_set_evenly_partitioned = true; break; }
    }
    return is_set_evenly_partitioned;
  }
  /**
   * Dynamic Programming | Set 13 (Cutting a Rod)
   *
   * Given a rod of length n inches and an array of prices that contains prices
   * of all pieces of size smaller than n. Determine maximum value obtainable
   * by cutting up the rod and selling the pieces.
   *
   * For example, if length of the rod is 8 and the values of different pieces
   * are given as following, then the maximum obtainable value is 22 (by cutting
   * in two pieces of 2 and 6)
   *
   * length   | 1   2   3   4   5   6   7   8
   * --------------------------------------------
   * price    | 1   5   8   9  10  17  17  20
   *
   * And if the prices are as following, then the maximum obtainable value is
   * 24 (by cutting in eight pieces of length 1)
   *
   * length   | 1   2   3   4   5   6   7   8
   * --------------------------------------------
   * price    | 3   5   8   9  10  17  17  20
   *
   * Reduced to weighted knapsnack problem, having number of diff. items, with
   * max capacity of rod length, try to pack as much value as possible.
   * max_cut_lookup(i, j) => max value by picking subset from 0 ... i with j cap.
   */
  static int __max_cut_value(vector<int> value_arr, int max_len) {
    vector<int> max_items_cnt(max_len + 1, 0);
    for (int i = 1; i <= max_len; i++) { max_items_cnt[i] = max_len / i; }
    int total_items = 0;
    for (auto & i : max_items_cnt){ total_items += i; }
    vector<int> item_value_arr, item_len_arr;
    for (int i = 1; i <= max_len; i++) {
      for (int j = 0; j < max_items_cnt[i]; j++) {
        item_len_arr.push_back(i);
        item_value_arr.push_back(value_arr[i - 1]);
      }
    }
    vector<vector<int>> max_cut_lookup(
      item_value_arr.size() + 1, vector<int>(max_len + 1, 0)
    );
    for (int i = 1; i <= total_items; i++) {
      for (int j = 1; j <= max_len; j++) {
        int log_length = item_len_arr[i - 1];
        int log_value = item_value_arr[i - 1];
        // not adding log[i]
        max_cut_lookup[i][j] = max_cut_lookup[i - 1][j];
        // adding log[i] if we have cap large-enough
        if (j >= log_length) {
          max_cut_lookup[i][j] = max(
            max_cut_lookup[i - 1][j], max_cut_lookup[i - 1][j - log_length] + log_value
          );
        }
      }
    }
    return max_cut_lookup.back().back();
  }

  static int _max_cut_value(vector<int> value_arr, int max_len) {
    vector<vector<int>> max_cut_lookup(
      value_arr.size() + 1, vector<int>(max_len + 1, 0)
    );
    for (int i = 1; i <= value_arr.size(); i++) {
      for (int j = 1; j <= max_len; j++) {
        max_cut_lookup[i][j] = max_cut_lookup[i - 1][j];
        if (j >= i) {
          max_cut_lookup[i][j] = max(
            max_cut_lookup[i][j], max_cut_lookup[i][j - i] + value_arr[i - 1]
          );
        }
      }
    }
    return max_cut_lookup.back().back();
  }

  static int max_cut_value(vector<int> value_arr, int max_len) {
    vector<int> max_cut_lookup(value_arr.size() + 1, 0);
    for (int i = 0; i < value_arr.size(); i++) {
      for (int j = 1; j <= max_len; j++) {
        if (j >= i + 1) {
          max_cut_lookup[j] = max(
            max_cut_lookup[j], max_cut_lookup[j - i - 1] + value_arr[i]
          );
        }
      }
    }
    return max_cut_lookup.back();
  }

  /**
   * Dynamic Programming | Set 7 (Coin Change)
   * Given a value N, if we want to make change for N cents, we have infinite
   * supply of each of S = { S1, S2, .. , Sm} valued coins, how many ways can
   * we make the change? The order of coins doesn't matter
   *
   * For example, for N = 4 and S = {1,2,3}, there are four solutions:
   * {1,1,1,1},{1,1,2},{2,2},{1,3}.
   * So output should be 4.
   *
   * For N = 10 and S = {2, 5, 3, 6}, there are five solutions:
   * {2,2,2,2,2}, {2,2,3,3}, {2,2,6}, {2,3,5} and {5,5}.
   * So the output should be 5.
   *
   * Reduced to Knapsack to pick unlimited items(weighted, from well known
   * categories) with constraint of maximum capacity, instead of check if
   * we can fully pack it or the maximum packed value, the goal here is to
   * count how many ways we can fully pack it. Actually it is the same when
   * compared to check if we can fully pack it, we only needs to count them
   * after we fill out the table.
   *
   * - coin_change_cnt_lookup(i, j) => with i diff. kinds of coins, target of j cents
   *   coin_change_cnt_lookup(i, j) = {
   *     if (value(i) <= j)
   *       // # of cnt when use coin i(unlimited) + # of cnt when not using coin i
   *       coin_change_cnt_lookup(i, j - value(i)) + coin_change_cnt_lookup(i - 1, j);
   *     else
   *       coin_change_cnt_lookup(i - 1, j);
   *   }
   *
   * 1 Reduced to 0/1 knapsack as only limited # of each coins are possible.
   * - Let coin_change_lookup(i, j) be added sum of max change(<=N) based on
   *   coins 0...i, with target sum of j. goal is to calc. whole table then
   *   count coin_change_lookup(i, N) with value equals to N.
   *
   * if (j > value(i)) {
   *   coin_change_lookup(i, j) = max{ coin_change_lookup(i - 1, j),
   *                                   coin_change_lookup(i - 1, j - value(i)) + value(i) }
   * } else {
   *   coin_change_lookup(i, j) = coin_change_lookup(i - 1, j)
   * }
   *
   * 2 Change the constraints, as the capacity will not change, while each
   *   we can choose to pick diff. coins each time, then:
   * - Let coin_change_lookup(i, j) be the max # of full-packing for capacity
   *   j based on i kinds of coin with > 0 # of coin i is chosen.
   *   To calc. coin_change_lookup(i, j), we know
   *   coin_change_lookup(0..i, 0..j), then
   * - coin_change_lookup(i, j) = coin_change_lookup(i - 1, j)
   *                              + 1 if coin_change_lookup(i - 1, j - coin(i)) > 0
   *                              + 1 if coin_change_lookup(i, j - coin(i)) > 0 || 0 == N % coin(i)
   *             0   1   2   3   4
   *         { } 1   0   0   0   0
   *       { 1 } 1   1   1   1   1
   *    { 1, 2 } 1   1   2   2   3 (2, 1 1) (1 1 1, 1 2) (1 1 1 1, 2 2, 1 1 2) (1 1 1 1, 2 2, 1 1 2, 1 3)
   * { 1, 2, 3 } 1   1   2   3   3 + 1
   */
  static int calc_coin_change(vector<int> coin_values, int target) {
    vector<vector<int>> coin_change_lookup(coin_values.size() + 1, vector<int>(target + 1, 0));
    for (int i = 0; i <= coin_values.size(); i++) { coin_change_lookup[i][0] = 1; } // no exc needed.
    for (int i = 1; i <= coin_values.size(); i++) {
      for (int j = 1; j <= target; j++) {
        coin_change_lookup[i][j] = coin_change_lookup[i - 1][j];
        if (coin_values[i - 1] <= j) {
          coin_change_lookup[i][j] += coin_change_lookup[i][j - coin_values[i - 1]];
        }
      }
    }
    return coin_change_lookup.back().back();
  }

  /**
   * Dynamic Programming | Set 36 (Maximum Product Cutting)
   * Given a rope of length n meters, cut the rope in different parts of
   * integer lengths in a way that maximizes product of lengths of all
   * parts. You must make at least one cut. Assume that the length of rope
   * is more than 2 meters.
   * Examples:
   * Input: n = 2
   * Output: 1 (Maximum obtainable product is 1*1)
   *
   * Input: n = 3
   * Output: 2 (Maximum obtainable product is 1*2)
   *
   * Input: n = 4
   * Output: 4 (Maximum obtainable product is 2*2)
   *
   * Input: n = 5
   * Output: 6 (Maximum obtainable product is 2*3)
   *
   * Input: n = 10
   * Output: 36 (Maximum obtainable product is 3*3*4)
   *
   * Reduced to knapsack, pack unlimited items(bounded categories) into a bag
   * with cap. N. the goal is to maximize the value of product.
   * let product_lookup(i, j) denote max product based on subset 0..i line
   * fragments with line length of j. Goal is to calc product_lookup(N, N)
   * - max_cut_lookup(i, j) = max{ max_cut_lookup(i - 1, j),
   *                               max_cut_lookup(i, j - len(i)) * len(i),
   *                               len(i) * (j - len(i)) }
   * - for line frag. size i, must be cut from j > i, otherwise default to 0
   * -   0 1 2 3 4 5 6 7 8 9 10
   *   0 1 1 1 1 1 1 1 1 1 1  1
   *   1 1 1 1 1 1 1 1 1 1 1  1 (all 1m long)
   *   2 1 1 1 2 4 6 8 (1, 5 * 2 => 10, 6 * 2
   *   3 1
   */
  static int calc_max_line_cut(int length) {
    vector<vector<int>> max_cut_lookup(length + 1, vector<int>(length + 1, 1));
    for (int i = 1; i <= length; i++) {
      for (int j = 1; j <= length; j++) {
        max_cut_lookup[i][j] = max_cut_lookup[i - 1][j];
        if (j > i) {
          max_cut_lookup[i][j] = max(max_cut_lookup[i][j], i * (j - i));
          max_cut_lookup[i][j] = max(max_cut_lookup[i][j],
                                     max_cut_lookup[i][j - i] * i);
        }
      }
    }
    return max_cut_lookup.back().back();
  }

  /**
   * Dynamic Programming | Set 32 (Word Break Problem)
   * Given an input string and a dictionary of words, find out if the input
   * string can be segmented into a space-separated sequence of dictionary
   * words. See following examples for more details.
   *
   * Consider the following dictionary
   * { i, like, sam, sung, samsung, mobile, ice,
   *   cream, icecream, man, go, mango}
   *
   * Input:  ilike
   * Output: Yes 
   * The string can be segmented as "i like"
   *
   * Input:  ilikesamsung
   * Output: Yes
   * The string can be segmented as "i like samsung"
   * or "i like sam sung"
   *
   * - input: text of size n, & dict of size m words
   * - breakable_lookup(i) denote if text(0..i) is breakable, implies that
   *   exists a j, such that { 0 <= j <= i | text(0..j) && dict.find(text[j + 1..i])
   * - goal is to calc breakable_lookup(n)
   */
  static bool is_text_breakable(string text, unordered_set<string> dict) {
    if (text.empty()) { return true; }
    vector<bool> breakable_lookup(text.size(), false);
    breakable_lookup[0] = (dict.end() != dict.find(text.substr(0, 1)));
    for (int i = 1; i < text.size(); i++) {
      for (int j = 0; j <= i; j++) {
        if (j < i) {
          breakable_lookup[i] = (dict.end() != dict.find(text.substr(j + 1, (i - j))) &&
                                 breakable_lookup[j]);
        } else {
          breakable_lookup[i] = (dict.end() != dict.find(text.substr(0, j + 1)));
        }
        if (true == breakable_lookup[i]) { break; }
      }
    }
    return breakable_lookup.back();
  }

  /**
   * Dynamic Programming | Set 30 (Dice Throw)
   * Given n dice each with m faces, numbered from 1 to m, find the number of
   * ways to get sum X. X is the summation of values on each face when all the
   * dice are thrown.
   * brutal force: face_per_dice ^ dice_cnt, combination.
   * we see overlapping effort to do the summation, DP.
   *
   * Reduce to knapsack, say we have m different kinds of item to pack, with
   * each kind items totaled n. We need to pick n items from each category with
   * backpack size of x, we want to count how many ways to pack it full.
   *
   * Let dice-sum-lookup(i, j) be the # of plans throw i times with sum of j
   * if there is no plan/path, then default to 0, goal is calc to bottom-right
   * dice-sum-lookup(i, j) = {
   *   sum{ 1 < k < m, dice-sum-lookup(i - 1, j - k) }
   * }
   */
  static int calc_sum_of_throws(int face_per_dice, int dice_cnt, int target_num) {
    vector<vector<int>> dice_sum_lookup(
      dice_cnt + 1, vector<int>(target_num + 1, 0)
    );
    for (int i = 1; i <= dice_cnt; i++) {
      for (int j = 1; j <= target_num; j++) {
        if (i == 1) { dice_sum_lookup[i][j] = (j <= face_per_dice) ? 1 : 0; continue; }
        for (int k = 1; k <= face_per_dice && k <= j; k++) {
          dice_sum_lookup[i][j] += dice_sum_lookup[i - 1][j - k];
        }
      }
    }
    return dice_sum_lookup[dice_cnt][target_num];
  }

  /**
   * Dynamic Programming | Set 11 (Egg Dropping Puzzle)
   * The following is a description of the instance of this famous puzzle
   * involving n=2 eggs and a building with k=36 floors.
   *
   * Suppose that we wish to know which stories in a 36-story building are safe
   * to drop eggs from, and which will cause the eggs to break on landing. We
   * make a few assumptions:
   * - An egg that survives a fall can be used again.
   * - A broken egg must be discarded.
   * - The effect of a fall is the same for all eggs.
   * - If an egg breaks when dropped, then it break if dropped from higher floor
   * - If an egg survives a fall then it would survive a shorter fall.
   * - It is not ruled out that the first-floor windows break eggs, nor is it
   *   ruled out that the 36th-floor do not cause an egg to break.
   *
   * If only one egg is available and we wish to be sure of obtaining the right
   * result, the experiment can be carried out in only one way. Drop the egg
   * from the first-floor window; if it survives, drop it from the second floor
   * window. Continue upward until it breaks. In the worst case, this method may
   * require 36 droppings. Suppose 2 eggs are available. What is the least
   * number of egg-droppings that is guaranteed to work in all cases?
   *
   * The problem is not actually to find the critical floor, but merely to
   * decide floors from which eggs should be dropped so that total number of
   * trials are minimized.
   *
   * - steps to test of floor 1 2 3 is same as steps needed to test floor 4 5 6
   *
   * - During our test procedure, let min-egg-drops(i, j) be total # of drops
   *   needed for i eggs available and j floors to be tested. By dropping
   *   the egg for test(we always drop an egg no matter where our position is),
   *   it could either survive or not. AGAIN, the position we are dropping the
   *   egg could be any, say 4 from 2 3 4 5 6 7, i -> 3, j -> 6
   *
   * - if egg breaks, we only have i - 1 eggs and only 0..j - 1 floors to check
   *   depends on the position we are dropping the ball, and position could be
   *   from 2 ... 7, if break @ 2, then we done, means 0 left, if break @ 7,
   *   then we have j - 1 left, no matter which plan we use, we need worst #.
   *   => then min-egg-drops(i, j) = max{ 1 <= x <= j | min-egg-drops(i - 1, x - 1) + 1 }
   *
   * - if egg survived, then we have i eggs, with j - 1 possible floors to check
   *   depends on the position we are dropping the ball, and position could be
   *   from 2 ... 7, if survived @ 2, then we have j - 1 left, if break @ 7,
   *   then done 0 left, no matter which plan we use, we need worst #.
   *
   *   => then min-egg-drops(i, j) = max{ 1 <= x <= j | min-egg-drops(i, j - x) + 1 }
   *
   * - min-egg-drops(i, j) = min{
   *     max{ 1 <= x <= j | min-egg-drops(i - 1, x - 1) + 1, # worst when egg breaks
   *                        min-egg-drops(i, j - x) + 1 }    # worst when egg good
   *   }
   *
   * - when i => 1, then min-egg-drops(i, j) => j
   *   when j => 1, then min-egg-drops(i, j) => 1
   *
   * - no matter how many eggs actually been used, we will always perform last
   *   drop of test which leads us to critical floor, and this step will either
   *   breaks our egg (which identify the previous floor) or succeed (we reach
   *   the max possible testing steps and curr floor is the only one left test)
   * - case 1, if last test breaks our egg (which identify the previous floor)
   *   then the last test must be failed during one-floor lag up phase, means
   *   for same floor but 1 egg less case, it fail all binary test & last one.
   *   also for current case, we also fail all binary test & last one, which
   *   implies we basically wasted 1 more binary test compared to previous 1
   *   | |x vx    vx
   *   1 2  3 4 5 6 7 8 9 10 11
   *   | |x       vx
   *   1 2  3 4 5 6 7 8 9 10 11
   *   else previous egg survivie, 
   *   then min-egg-drops(i, j) => min-egg-drops(i - 1, j) || min-egg-drops(i - 1, j )
   *        v v x vx
   *   1 2  3 4 5 6 7 8 9 10 11
   *   |      | x vx
   *   1 2  3 4 5 6 7 8 9 10 11
   * - case 2, if last egg survived, our egg survive for all subset of floors.
   *   it implies for same floor but 1 egg less case, it must try 1 more time.
   *   but it has less by 1 binary test.
   *   then min-egg-drops(i, j) => min-egg-drops(i - 1, j)
   *     | vx    vx
   *   1 2 3 4 5 6 7 8 9 1011
   *     | |x    vx
   *   1 2 3 4 5 6 7 8 9 10 11
   * - if 1 egg left, then we can only start from the safest untested level,
   * - if we have multiple eggs, then we could use extra ones to reducing the
   *   searching scope of floors. caes 1, egg survives on floor j / 2, then
   *   we will search (j / 2 < floor <= j) else test (1 <= floor < j / 2)
   * - worst case scenario, each test breaks an egg & last egg needs test all
   * - when floors does not change, every addition of egg guarantees >= 1 test
   *   which could reduce drops by ((i - 1) + 2 ^ (logk - i + 1)) - ((i) + 2 ^ (logk - i)) => 2^(logk - i) - 1
   *   min-egg-drops(i - 1, j) - 2^(logk - i) - 1, if i <= logk else logk
   * - when eggs does not change, every addition of floor requires >= 1 test
   *   min-egg-drops(i, j - 1) + 1
   */
  static int calc_min_egg_drops_for_critical_floor(int egg_cnt, int floor_cnt) {
    /**
     * - min-egg-drops(i, j) = min{
     *     max{ 1 <= x <= j | min-egg-drops(i - 1, x - 1) + 1, # worst when egg breaks
     *                        min-egg-drops(i, j - x) + 1 }    # worst when egg good
     *   }
     *   min steps for worst case senario
     * - when i => 1, then min-egg-drops(i, j) => j
     *   when j => 1, then min-egg-drops(i, j) => 1
     */
    vector<vector<int>> min_egg_drops(egg_cnt + 1, vector<int>(floor_cnt + 1, 0));
    for (int i = 1; i <= floor_cnt; i++) { min_egg_drops[1][i] = i; }
    for (int i = 1; i <= egg_cnt; i++) { min_egg_drops[i][1] = 1; }
    for (int i = 2; i <= egg_cnt; i++) {
      for (int j = 2; j <= floor_cnt; j++) {
        min_egg_drops[i][j] = INT_MAX;
        for (int c = 1; c <= j; c++) {
          min_egg_drops[i][j] = min(
            min_egg_drops[i][j], max(min_egg_drops[i - 1][c - 1] + 1,
                                     min_egg_drops[i][j - c] + 1)
          );
        }
      }
    }
    return min_egg_drops.back().back();
  }

  /**
   * 312. Burst Balloons
   *
   * Given n balloons, indexed from 0 to n-1. Each balloon is painted with a
   * number on it represented by array nums. You are asked to burst all the
   * balloons. If the you burst balloon i you will get
   * - nums[left] * nums[i] * nums[right] coins.
   * Here left and right are adjacent indices of i. After the burst, the left
   * and right then becomes adjacent.
   * Find the maximum coins you can collect by bursting the balloons wisely.
   *
   * Note:
   * - You may imagine nums[-1] = nums[n] = 1. They are not real therefore you
   *   can not burst them.
   * - 0 ≤ n ≤ 500, 0 ≤ nums[i] ≤ 100
   * Example:
   * - Input: [3,1,5,8]
   * - Output: 167 
   * - Explanation: nums  = [3,1,5,8] --> [3,5,8] -->   [3,8]   -->  [8]  --> []
   *                coins =  3*1*5      +  3*5*8    +   1*3*8     + 1*8*1   = 167
   * Obvservation:
   * - we need to burst balloons from 0 -> n - 1, each time we burst any balloon
   *   we collect coins which is dependent on its nearest un-bursted balloons.
   * - the goal is to get the max coins by bursting balloons in certain order.
   * - obvious, ALL balloons needs to be bursted to maximize coins.
   * - we only need the max coins instead of actual plan, then DP is handy.
   * Subproblem & Optimal Structure Backtracking:
   * - { } { b0 }            { b0 b1 }
   * -  0    v0    max(v0 * v1 + v1, v0 * v1 + v0)
   * - reduce to knapsack? pick items in certain order so that values maximized?
   * - each time we burst a balloon from existing set { b0, ... bi }
   * - max-coin(ballon-set){ b from ballon-set | max{ v(prev-b) * v(b) * v(next-b) + max-coin(ballon-set exclude b) } }
   * Subproblem & Optimal Structure DP:
   * - the hard point been at any stage, balloons may not be next to each other
   * - howere, we burst at most 1 balloon at a time, such that each time we
   *   burst a balloon, we break them to 2 parts, and each part is connected.
   * - Let max-coin(i, j, k) denotes the max coins we can get from connected set
   *   of balloons bi...bj, then the goal is to calc max-coin(0, n - 1), where
   *   bk is the last balloon to burst.
   * - max-coin(i, j) = { i <= k <= j | max{ nums[i] + max(i + 1, j),
   *                                         nums[j] + max(i, j - 1),
   *                                         max(i, k - 1) + nums[k - 1] * nums[k] * nums[k + 1] + max(k + 1, j) } }
   *
  { (8,2,6,8,9,) 8 (1,4,1,5,3,0,7,7,0,4,2) }
         ^       ^              ^
         |       |              |
  { (8) (2,6,8,9,8,1,4,1,5,3,0,7,7,0,4,2) }
     ^               ^
     |               |
20 <=> [
  [ 3 8 ]
  [ x 5 ]
]
   */
  static int fast_maximum_coins_via_burst_balloons(vector<int> nums) {
    int balloon_cnt = nums.size();
    if (0 == balloon_cnt) { return 0; }
    vector<vector<int>> max_coins_lookup(balloon_cnt, vector<int>(balloon_cnt, INT_MIN));
    for (int i = 0; i < balloon_cnt; i++) {
      max_coins_lookup[i][i] = nums[i];
      if (i - 1 >= 0) { max_coins_lookup[i][i] *= nums[i - 1]; }
      if (i + 1 <  balloon_cnt) { max_coins_lookup[i][i] *= nums[i + 1]; }
    }
    for (int i = balloon_cnt - 1; i >= 0; i--) {
      for (int j = i + 1; j < balloon_cnt; j++) {
        for (int k = i; k <= j; k++) {
          int tentative_value = nums[k];
          if (i - 1 >= 0) { tentative_value *= nums[i - 1]; }
          if (j + 1 <  balloon_cnt) { tentative_value *= nums[j + 1]; }

          if (k == i) { max_coins_lookup[i][j] = max(max_coins_lookup[i][j], tentative_value + max_coins_lookup[i + 1][j]); continue; }
          if (k == j) { max_coins_lookup[i][j] = max(max_coins_lookup[i][j], tentative_value + max_coins_lookup[i][j - 1]); continue; }
          if (k - 1 >= i) { tentative_value = max_coins_lookup[i][k - 1] + tentative_value; }
          if (k + 1 <= j) { tentative_value = tentative_value + max_coins_lookup[k + 1][j]; }
          max_coins_lookup[i][j] = max(max_coins_lookup[i][j], tentative_value);
        }
      }
    }
    return max_coins_lookup.front().back();
  }

  static int maximum_coins_via_burst_balloons_recur(vector<int> & nums,
                                                   list<int> unburst_balloons,
                                                   unordered_map<string, int> & subset_max) {
    int max_coins = 0, remaining_max_coins = 0, curr_burst_value = 0;
    int first_balloon_id = 0, last_balloon_id = 0, curr_balloon_id = 0;
    switch (unburst_balloons.size()) {
      case 0: { break; }
      case 1: { max_coins = nums[unburst_balloons.front()]; break; }
      case 2: { max_coins = max(nums[unburst_balloons.front()] * nums[unburst_balloons.back()] +
                                nums[unburst_balloons.front()],
                                nums[unburst_balloons.front()] * nums[unburst_balloons.back()] +
                                nums[unburst_balloons.back()]);
                break;
      }
      default: {
        string subset_key = "";
        for (auto & balloon_id : unburst_balloons) { subset_key.append(to_string(balloon_id) + "$"); }
        if (subset_max.end() != subset_max.find(subset_key)) { max_coins = subset_max[subset_key]; break; }

        list<int>::iterator prev_itr = unburst_balloons.begin(),
                            next_itr = unburst_balloons.begin();
        first_balloon_id = unburst_balloons.front(),
        last_balloon_id  = unburst_balloons.back();

        for (list<int>::iterator itr = unburst_balloons.begin(); itr != unburst_balloons.end(); itr++) {

          curr_balloon_id = * itr;
          prev_itr = itr; prev_itr--;
          next_itr = itr; next_itr++;

          if (curr_balloon_id == first_balloon_id) {
            curr_burst_value = nums[curr_balloon_id] * nums[* next_itr];
          } else if (curr_balloon_id == last_balloon_id) {
            curr_burst_value = nums[* prev_itr] * nums[curr_balloon_id];
          } else {
            curr_burst_value = nums[* prev_itr] * nums[curr_balloon_id] * nums[* next_itr];
          }

          itr = unburst_balloons.erase(itr);
          remaining_max_coins = maximum_coins_via_burst_balloons_recur(nums, unburst_balloons, subset_max);
          max_coins = max(max_coins, remaining_max_coins + curr_burst_value);

          itr = unburst_balloons.insert(itr, curr_balloon_id);
        }

        subset_max[subset_key] = max_coins;
        break;
      }
    }
    return max_coins;
  }

  static int maximum_coins_via_burst_balloons(vector<int> nums) {
    unordered_map<string, int> subset_max;
    list<int> unburst_balloons;
    for (int i = 0; i < nums.size(); i++) { unburst_balloons.push_back(i); }
    return maximum_coins_via_burst_balloons_recur(nums, unburst_balloons, subset_max);
  }
};

int main(void) {
  using dp_util::print_all_elem;
  using dp_util::print_all_elem_vec;

  using dp_util::get_min_palindrome_cnt;
  using dp_util::is_input_via_scramble;
  using dp_util::check_input_via_interleave;
  using dp_util::calc_max_sum_triangle;
  using dp_util::is_word_breakable;
  using dp_util::count_distinct_subseq;
  using dp_util::get_decoding_count;
  using dp_util::is_pattern_matched;
  using dp_util::calc_lcs_len;
  using dp_util::calc_longest_incr_subseq_len;
  using dp_util::calc_min_partition_diff;
  using dp_util::calc_max_incr_path_len;
  using dp_util::check_subset_sum;
  using dp_util::calc_min_multiply_ops;
  using dp_util::is_set_evenly_partitioned;
  using dp_util::_max_cut_value;
  using dp_util::max_cut_value;
  using dp_util::calc_coin_change;
  using dp_util::calc_max_line_cut;
  using dp_util::is_text_breakable;
  using dp_util::calc_sum_of_throws;
  using dp_util::calc_min_egg_drops_for_critical_floor;
  using dp_util::calc_lcstr_len;
  using dp_util::maximum_coins_via_burst_balloons;
  using dp_util::fast_maximum_coins_via_burst_balloons;

  cout << "1. get_min_palindrome_cnt" << endl;
  string test_input[] = { "", "a", "aab", "aaa", "aaacdfe", "aaacaaafe",
                          "abcde", "abacabaca", "abacakacaba" };
  int test_output[] = { 0, 0, 1, 0, 4, 2, 4, 2, 0 };
  cout << "get_min_palin_cnt" << endl;
  for (int i = 0; i < sizeof(test_input) / sizeof(string); i++) {
    int min_palin_cnt = get_min_palindrome_cnt(test_input[i]);
    cout << "1-" << i << ": " << test_input[i] << " | min-palin-cuts: "
         << test_output[i] << " vs " << min_palin_cnt << endl;
    assert(min_palin_cnt == test_output[i]);
  }

  cout << "2. is_input_via_scramble" << endl;
  cout << "1 <=> " << is_input_via_scramble("great", "rgeat") << endl;
  cout << "1 <=> " << is_input_via_scramble("great", "rgtae") << endl;
  cout << "0 <=> " << is_input_via_scramble("great", "rtage") << endl;

  cout << "3. is_input_via_interleave" << endl;
  cout << "1 <=> " << check_input_via_interleave("aabcc", "dbbca", "aadbbcbcac") << endl;
  cout << "0 <=> " << check_input_via_interleave("aabcc","dbbca","aadbbbaccc") << endl;

  cout << "4. calc_max_sum_triangle" << endl;
  cout << "21 <=> " << calc_max_sum_triangle(vector<vector<int>>(
    { vector<int>({ 2 }), vector<int>({ 3, 4 }), vector<int>({ 6, 5, 7 }),
      vector<int>({ 4, 1, 8, 3 }) })) << endl;
  cout << "23 <=> " << calc_max_sum_triangle(vector<vector<int>>(
    { vector<int>({ 3 }), vector<int>({ 7, 4 }), vector<int>({ 2, 4, 6 }),
      vector<int>({ 8, 5, 9, 3 }) })) << endl;
  cout << "19 <=> " << calc_max_sum_triangle(vector<vector<int>>(
    { vector<int>({ 8 }), vector<int>({ -4, 4 }), vector<int>({ 2, 2, 6 }),
      vector<int>({ 1, 1, 1, 1 }) })) << endl;

  cout << "5. is_word_breakable" << endl;
  unordered_set<string> dict = { "mobile","samsung","sam","sung","man","mango",
                                 "icecream","and","go","i","like","ice","cream" };
  cout << "1 <=> " << is_word_breakable(dict, "") << endl;
  cout << "1 <=> " << is_word_breakable(dict, "iiiiiiii") << endl;
  cout << "1 <=> " << is_word_breakable(dict, "ilikesamsung") << endl;
  cout << "1 <=> " << is_word_breakable(dict, "ilikelikeimangoiii") << endl;
  cout << "1 <=> " << is_word_breakable(dict, "samsungandmango") << endl;
  cout << "0 <=> " << is_word_breakable(dict, "samsungandmangok") << endl;

  cout << "6. count_distinct_subseq" << endl;
  cout << "6 <=> " << count_distinct_subseq("geeksforgeeks", "ge") << endl;
  cout << "0 <=> " << count_distinct_subseq("geeksforgeeks", "xe") << endl;
  cout << "0 <=> " << count_distinct_subseq("geeksforgeeks", "") << endl;
  cout << "3 <=> " << count_distinct_subseq("rabbbit", "rabbit") << endl;

  cout << "7. get_decoding_count" << endl;
  cout << "4 <=> " << get_decoding_count(string("1923")) << endl;
  cout << "1 <=> " << get_decoding_count(string("1")) << endl;
  cout << "2 <=> " << get_decoding_count(string("12")) << endl;
  cout << "4 <=> " << get_decoding_count(string("1072512")) << endl;
  cout << "1 <=> " << get_decoding_count(string("10")) << endl;
  cout << "0 <=> " << get_decoding_count(string("100")) << endl;
  cout << "0 <=> " << get_decoding_count(string("0")) << endl;
  cout << "0 <=> " << get_decoding_count(string("010")) << endl;
  cout << "1 <=> " << get_decoding_count(string("110")) << endl;
  cout << "3 <=> " << get_decoding_count(string("111")) << endl;
  cout << "3 <=> " << get_decoding_count(string("1234")) << endl;

  cout << "8. is_pattern_matched" << endl;

  cout << "1 <=> " << is_pattern_matched("", "x*") << endl;
  cout << "1 <=> " << is_pattern_matched("", "x*x*x*") << endl;
  cout << "1 <=> " << is_pattern_matched("", "") << endl;
  cout << "0 <=> " << is_pattern_matched("x", "") << endl;
  cout << "1 <=> " << is_pattern_matched("", "") << endl;
  cout << "0 <=> " << is_pattern_matched("a", "") << endl;
  cout << "0 <=> " << is_pattern_matched("a", "aa") << endl;
  cout << "0 <=> " << is_pattern_matched("aa", "a") << endl;
  cout << "1 <=> " << is_pattern_matched("aa", "a*") << endl;
  cout << "1 <=> " << is_pattern_matched("aa", ".*") << endl;
  cout << "1 <=> " << is_pattern_matched("ab", ".*") << endl;
  cout << "0 <=> " << is_pattern_matched("mississippi", "mis*is*p*.") << endl;
  cout << "1 <=> " << is_pattern_matched("aab", "c*a*b") << endl;
  cout << "1 <=> " << is_pattern_matched("aab", "a*b") << endl;
  cout << "0 <=> " << is_pattern_matched("aaa", "ab*a") << endl;
  cout << "1 <=> " << is_pattern_matched("aaaa", ".*") << endl;
  cout << "0 <=> " << is_pattern_matched("b", "ab*b") << endl;
  cout << "1 <=> " << is_pattern_matched("aaa", "ab*ac*a") << endl;
  cout << "1 <=> " << is_pattern_matched("a", "..*") << endl;

  cout << "9. calc_lcs_len" << endl;
  cout << "4 <=> " << calc_lcs_len("AGGTAB", "GXTXAYB") << endl;
  cout << "3 <=> " << calc_lcs_len("ABCDGH", "AEDFHR") << endl;
  cout << "1 <=> " << calc_lcs_len("A", "AEDFHR") << endl;
  cout << "1 <=> " << calc_lcs_len("ABCDGH", "G") << endl;
  cout << "0 <=> " << calc_lcs_len("ABCDGH", "") << endl;
  cout << "0 <=> " << calc_lcs_len("", "AEDFHR") << endl;
  cout << "0 <=> " << calc_lcs_len("", "") << endl;

  cout << "10. calc_longest_incr_subseq_len" << endl;
  cout << "6 <=> " << calc_longest_incr_subseq_len(
                        vector<int>({ 2, 5, 3, 7, 11, 8, 10, 13, 6 })
                      ) << endl;
  cout << "5 <=> " << calc_longest_incr_subseq_len(
                        vector<int>({ 10, 22, 9, 33, 21, 50, 41, 60 })
                      ) << endl;
  cout << "3 <=> " << calc_longest_incr_subseq_len(
                        vector<int>({ 4, 10, 4, 3, 8, 9 })
                      ) << endl;
  cout << "6 <=> " << calc_longest_incr_subseq_len(
                        vector<int>({ -1, 9, 1, 2, 3, 4, 5 })
                      ) << endl;
  cout << "6 <=> " << calc_longest_incr_subseq_len(
                        vector<int>({ 3,5,6,2,5,4,19,5,6,7,12 })
                      ) << endl;

  cout << "11. calc_min_partition_diff" << endl;
  cout << "1 <=> " << calc_min_partition_diff(
                        vector<int>({ 3, 1, 4, 2, 2, 1 })
                      ) << endl;
  cout << "0 <=> " << calc_min_partition_diff(
                        vector<int>({ 4, 5, 6, 8, 10, 11 })
                      ) << endl;

  cout << "12. calc_max_incr_path_len" << endl;
  cout << "4 <=> " << calc_max_incr_path_len(
                        vector<vector<int>>({ vector<int>({1, 2, 9}),
                                              vector<int>({5, 3, 8}),
                                              vector<int>({4, 6, 7}) })
                      ) << endl;
  cout << "8 <=> " << calc_max_incr_path_len(
                        vector<vector<int>>({ vector<int>({3, 2, 9}),
                                              vector<int>({4, 5, 8}),
                                              vector<int>({4, 6, 7}) })
                      ) << endl;
  cout << "9 <=> " << calc_max_incr_path_len(
                        vector<vector<int>>({ vector<int>({40, 30, 20, 25, 90}),
                                              vector<int>({40, 3, 2, 1, 90}),
                                              vector<int>({40, 4, 9, 8, 90}),
                                              vector<int>({40, 5, 6, 7, 90}),
                                              vector<int>({40, 30, 20, 25, 90}) })
                      ) << endl;

  cout << "13. check_subset_sum" << endl;
  cout << "1 <=> " << check_subset_sum(vector<int>({3, 34, 4, 12, 5, 2}), 9) << endl;

  cout << "14. calc_min_multiply_ops" << endl;
  cout << "30000 <=> " << calc_min_multiply_ops(vector<int>({10, 20, 30, 40, 30})) << endl;
  cout << "26000 <=> " << calc_min_multiply_ops(vector<int>({40, 20, 30, 10, 30})) << endl;
  cout << "6000 <=> " << calc_min_multiply_ops(vector<int>({10, 20, 30})) << endl;
  cout << "18 <=> " << calc_min_multiply_ops(vector<int>({1, 2, 3, 4})) << endl;

  cout << "15. is_set_evenly_partitioned" << endl;
  cout << "1 <=> " << is_set_evenly_partitioned(vector<int>({1, 5, 11, 5})) << endl;
  cout << "0 <=> " << is_set_evenly_partitioned(vector<int>({1, 5, 3})) << endl;
  cout << "1 <=> " << is_set_evenly_partitioned(vector<int>({1, 5, 4})) << endl;
  cout << "0 <=> " << is_set_evenly_partitioned(vector<int>({1, 2, 3, 5})) << endl;
  cout << "0 <=> " << is_set_evenly_partitioned(vector<int>({1, 2, 5})) << endl;
  cout << "1 <=> " << is_set_evenly_partitioned(vector<int>({1, 1})) << endl;

  cout << "16. max_cut_value" << endl;
  cout << "22 <=> " << _max_cut_value(vector<int>({1, 5, 8, 9, 10, 17, 17, 20}), 8) << endl;
  cout << "22 <=> " << max_cut_value(vector<int>({1, 5, 8, 9, 10, 17, 17, 20}), 8) << endl;
  cout << "24 <=> " << _max_cut_value(vector<int>({3, 5, 8, 9, 10, 17, 17, 20}), 8) << endl;
  cout << "24 <=> " << max_cut_value(vector<int>({3, 5, 8, 9, 10, 17, 17, 20}), 8) << endl;

  cout << "17. calc_coin_change" << endl;
  cout << "2 <=> " << calc_coin_change(vector<int>({1, 2, 3}), 3) << endl;
  cout << "4 <=> " << calc_coin_change(vector<int>({1, 2, 3}), 4) << endl;
  cout << "5 <=> " << calc_coin_change(vector<int>({2, 5, 3, 6}), 10) << endl;

  cout << "18. calc_max_line_cut" << endl;
  cout << "1 <=> " << calc_max_line_cut(2) << endl;
  cout << "2 <=> " << calc_max_line_cut(3) << endl;
  cout << "4 <=> " << calc_max_line_cut(4) << endl;
  cout << "6 <=> " << calc_max_line_cut(5) << endl;
  cout << "36 <=> " << calc_max_line_cut(10) << endl;

  cout << "19. is_text_breakable" << endl;
  cout << "1 <=> " << is_text_breakable(
    "ilike", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                     "mobile", "ice", "cream", "icecream",
                                     "man", "go", "mango" })) << endl;
  cout << "1 <=> " << is_text_breakable(
    "ilikesamsung", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                            "mobile", "ice", "cream", "icecream",
                                            "man", "go", "mango" })) << endl;
  cout << "1 <=> " << is_text_breakable(
    "iiiiiiii", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                        "mobile", "ice", "cream", "icecream",
                                        "man", "go", "mango" })) << endl;
  cout << "1 <=> " << is_text_breakable(
    "", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                "mobile", "ice", "cream", "icecream",
                                "man", "go", "mango" })) << endl;
  cout << "1 <=> " << is_text_breakable(
    "ilikelikeimangoiii", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                                "mobile", "ice", "cream", "icecream",
                                                "man", "go", "mango" })) << endl;
  cout << "1 <=> " << is_text_breakable(
    "samsungandmango", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                                "mobile", "ice", "cream", "icecream",
                                                "and", "man", "go", "mango" })) << endl;
  cout << "0 <=> " << is_text_breakable(
    "samsungandmangok", unordered_set<string>({ "i", "like", "sam", "sung", "samsung",
                                                "mobile", "ice", "cream", "icecream",
                                                "and", "man", "go", "mango" })) << endl;

  cout << "20. calc_sum_of_throws" << endl;
  cout << "0 <=> " << calc_sum_of_throws(4, 2, 1) << endl;
  cout << "2 <=> " << calc_sum_of_throws(2, 2, 3) << endl;
  cout << "21 <=> " << calc_sum_of_throws(6, 3, 8) << endl;
  cout << "4 <=> " << calc_sum_of_throws(4, 2, 5) << endl;
  cout << "6 <=> " << calc_sum_of_throws(4, 3, 5) << endl;

  cout << "21. calc_min_egg_drops_for_critical_floor" << endl;
  cout << "8 <=> " << calc_min_egg_drops_for_critical_floor(2, 36) << endl;
  cout << "36 <=> " << calc_min_egg_drops_for_critical_floor(1, 36) << endl;
  cout << "1 <=> " << calc_min_egg_drops_for_critical_floor(2, 1) << endl;

  cout << "22. calc_lcstr_len" << endl;
  cout << "5 <=> " << calc_lcstr_len("GeeksforGeeks", "GeeksQuiz") << endl;
  cout << "4 <=> " << calc_lcstr_len("abcdxyz", "xyzabcd") << endl;
  cout << "6 <=> " << calc_lcstr_len("zxabcdezy", "yzabcdezx") << endl;

  cout << "23. maximum_coins_via_burst_balloons" << endl;
  cout << "167 <=> " << maximum_coins_via_burst_balloons(vector<int>({ 3, 1, 5, 8 })) << endl;
  cout << "0 <=> " << maximum_coins_via_burst_balloons(vector<int>({ })) << endl;
  cout << "3 <=> " << maximum_coins_via_burst_balloons(vector<int>({ 3 })) << endl;
  cout << "20 <=> " << maximum_coins_via_burst_balloons(vector<int>({ 3, 5 })) << endl;
  cout << "152 <=> " << maximum_coins_via_burst_balloons(vector<int>({ 3, 5, 8 })) << endl;
  cout << "1088290 <=> " << maximum_coins_via_burst_balloons(vector<int>({ 9, 76, 64, 21, 97, 60, 5 })) << endl;
  cout << "3414 <=> " << maximum_coins_via_burst_balloons(vector<int>({ 8,2,6,8,9,8,1,4,1,5,3,0,7,7,0,4,2 })) << endl;

  cout << "24. fast_maximum_coins_via_burst_balloons" << endl;
  cout << "167 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ 3, 1, 5, 8 })) << endl;
  cout << "0 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ })) << endl;
  cout << "3 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ 3 })) << endl;
  cout << "20 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ 3, 5 })) << endl;
  cout << "152 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ 3, 5, 8 })) << endl;
  cout << "1088290 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ 9, 76, 64, 21, 97, 60, 5 })) << endl;
  cout << "3414 <=> " << fast_maximum_coins_via_burst_balloons(vector<int>({ 8,2,6,8,9,8,1,4,1,5,3,0,7,7,0,4,2 })) << endl;

  return 0;
}
