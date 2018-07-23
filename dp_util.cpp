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
  template <class Type>
  static void print_all_elem(const vector<Type> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }
  template <class Type>
  static void print_all_elem_vec(const vector<vector<Type>> & input) {
    cout << "[" << endl;
    for (auto & arr : input){ cout << "  "; print_all_elem<Type>(arr); }
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
   * - input -> T1 T2 check if P is a interleave of T1 & T2
   *   P[0..i + j + 1] is a interleave of T1[0..i] & T2[0..j] then
   *   P[0..i + j] must be a interleave of T1[0..i - 1] & T2[0..j] OR
   *                                       T1[0..i] & T2[0..j + 1]
   *   P(i, j) {
   *   - true if (((T1[i] == P[i + j]) && P(i - 1, j)) ||
   *              ((T2[j] == P[i + j]) && P(i, j - 1)))
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

  static int find_max_elem(const vector<int> & int_vec) {
    int max_val = int_vec.front();
    for (auto & val : int_vec) { if (val > max_val ) { max_val = val; } }
    return max_val;
  }

  /**
   * 13.12 Word Break
   * Given a string s and a dictionary of words dict, determine if s can be
   * segmented into a space-separated sequence of one or more dictionary words.
   * For example, given s = "leetcode", dict = ["leet", "code"].
   * Return true because "leetcode" can be segmented as "leet code".
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

  static bool _is_word_breakable(const unordered_set<string> dict,
                                const string word) {
    if (true == word.empty()) { return true; }
    vector<vector<int>> break_lookup(
      word.size(), vector<int>(word.size(), STRATEGY_UNDEF)
    );
    return _check_word_breakable(dict, word, 0, word.size() - 1, break_lookup);
  }

  static bool _check_word_breakable(const unordered_set<string> & dict,
                                    const string & word,
                                    int start_pos, int end_pos,
                                    vector<vector<int>> & break_lookup) {
    if (STRATEGY_UNDEF != break_lookup[start_pos][end_pos]) {
      return (STRATEGY_SUCCESS == break_lookup[start_pos][end_pos]);
    }
    bool is_word_breakable = false;
    if (dict.find(word.substr(start_pos, end_pos - start_pos + 1)) != dict.end()) {
      is_word_breakable = true;
    } else {
      for (int i = start_pos; i < end_pos; i++) {
        is_word_breakable = (
          _check_word_breakable(dict, word, start_pos, i, break_lookup) &&
          _check_word_breakable(dict, word, i + 1, end_pos, break_lookup)
        );
        if (true == is_word_breakable) { break; }
      }
    }
    break_lookup[start_pos][end_pos] = (
      (true == is_word_breakable) ? STRATEGY_SUCCESS : STRATEGY_FAIL
    );
    return is_word_breakable;
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
   */
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

  static bool is_char_pair_match(char l, char r) { return ((l == r) || ('.' == r)); }

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
   * Input: [10,9,2,5,3,7,101,18]
   * Output: 4
   * Explanation: The longest increasing subsequence is [2,3,7,101], therefore
   *              the length is 4. 
   * Note:
   * - There may be more than one LIS combination, it is only necessary for you to
   *   return the length. Your algorithm should run in O(n2) complexity.
   *   Follow up: Could you improve it to O(nlogn) time complexity?
   * Analysis:
   * - let lis(i, j) be the lis-len for arr[i..j], then goal is to get lis(0, j)
   * - to calc lis(i, j), assume we already know lis(i + 1, j) && lis(i, j - 1);
   *     4 5 7 2 1 0 6
   *   4 1         3 ?
   *   5   1         3
   *   7     1
   *   2       1
   *   1         1
   *   0           1 2
   *   6             1
   *
   *   4 5 7         arr[i][j]         ? -> 4, 5, 7
   *   |---------|   arr[i][j - 1]     3 -> 4, 5, 7 -> max 7
   *   | |-------|-| arr[i + 1][j]     2 -> 5, 7    -> max 7
   *   4 5 7 2 1 0 6
   * - arr[i + 1][j]  < arr[i][j - 1] => seq starts from beginning
   *                                     ( -1 <0 1 2 3 5) 4> => use arr[i][j - 1];
   * - arr[i + 1][j]  > arr[i][j - 1] => seq ends with curr num arr[j]
   *                                     (  7 <0 1 2 3 5) 6> => use arr[i + 1][j];
   * - arr[i + 1][j] == arr[i][j - 1] => seq has nothing to do with begin & end
   *                                     ( -1 <0 1 2 3 4)  5> arr[j] > arr[i] && arr[j] > arr[j - 1] => +1
   *                                     (100 <0 1 2 3 4) -1> arr[j] < arr[i] => same
   */
  static int calc_longest_incr_subseq_len(vector<int> input) {
    int total_cnt = input.size();
    if (0 == total_cnt) { return 0; }
    vector<vector<int>> lis_buffer(total_cnt, vector<int>(total_cnt, 0));
    for (int i = total_cnt - 1; i >= 0; i--) {
      for (int j = i; j < total_cnt; j++) {
        if (i == j) { lis_buffer[i][j] = 1; continue; }
        if (lis_buffer[i + 1][j] < lis_buffer[i][j - 1]) {
          lis_buffer[i][j] = lis_buffer[i][j - 1]; continue;
        } else if (lis_buffer[i + 1][j] > lis_buffer[i][j - 1]) {
          lis_buffer[i][j] = lis_buffer[i + 1][j]; continue;
        } else {
          if (input[i] < input[j] && input[j] > input[j - 1]) {
            lis_buffer[i][j] = lis_buffer[i + 1][j] + 1;
          } else {
            lis_buffer[i][j] = lis_buffer[i + 1][j];
          }
        }
      }
    }
    return lis_buffer.front().back();
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
  assert(true == dp_util::check_input_via_interleave("aabcc", "dbbca", "aadbbcbcac"));
  assert(false == dp_util::check_input_via_interleave("aabcc","dbbca","aadbbbaccc"));

  cout << "4. dp_util::calc_max_sum_triangle" << endl;
  cout << "21 <=> " << dp_util::calc_max_sum_triangle(vector<vector<int>>(
    { vector<int>({ 2 }), vector<int>({ 3, 4 }), vector<int>({ 6, 5, 7 }),
      vector<int>({ 4, 1, 8, 3 }) })) << endl;
  cout << "23 <=> " << dp_util::calc_max_sum_triangle(vector<vector<int>>(
    { vector<int>({ 3 }), vector<int>({ 7, 4 }), vector<int>({ 2, 4, 6 }),
      vector<int>({ 8, 5, 9, 3 }) })) << endl;
  cout << "19 <=> " << dp_util::calc_max_sum_triangle(vector<vector<int>>(
    { vector<int>({ 8 }), vector<int>({ -4, 4 }), vector<int>({ 2, 2, 6 }),
      vector<int>({ 1, 1, 1, 1 }) })) << endl;

  cout << "5. dp_util::is_word_breakable" << endl;
  unordered_set<string> dict = { "mobile","samsung","sam","sung","man","mango",
                                 "icecream","and","go","i","like","ice","cream" };
  cout << "1 <=> " << dp_util::is_word_breakable(dict, "") << endl;
  cout << "1 <=> " << dp_util::is_word_breakable(dict, "iiiiiiii") << endl;
  cout << "1 <=> " << dp_util::is_word_breakable(dict, "ilikesamsung") << endl;
  cout << "1 <=> " << dp_util::is_word_breakable(dict, "ilikelikeimangoiii") << endl;
  cout << "1 <=> " << dp_util::is_word_breakable(dict, "samsungandmango") << endl;
  cout << "0 <=> " << dp_util::is_word_breakable(dict, "samsungandmangok") << endl;

  cout << "6. dp_util::count_distinct_subseq" << endl;
  cout << "6 <=> " << dp_util::count_distinct_subseq("geeksforgeeks", "ge") << endl;
  cout << "0 <=> " << dp_util::count_distinct_subseq("geeksforgeeks", "xe") << endl;
  cout << "0 <=> " << dp_util::count_distinct_subseq("geeksforgeeks", "") << endl;
  cout << "3 <=> " << dp_util::count_distinct_subseq("rabbbit", "rabbit") << endl;

  cout << "7. dp_util::get_decoding_count" << endl;
  cout << "4 <=> " << dp_util::get_decoding_count(string("1923")) << endl;
  cout << "1 <=> " << dp_util::get_decoding_count(string("1")) << endl;
  cout << "2 <=> " << dp_util::get_decoding_count(string("12")) << endl;
  cout << "4 <=> " << dp_util::get_decoding_count(string("1072512")) << endl;
  cout << "1 <=> " << dp_util::get_decoding_count(string("10")) << endl;
  cout << "0 <=> " << dp_util::get_decoding_count(string("100")) << endl;
  cout << "0 <=> " << dp_util::get_decoding_count(string("0")) << endl;
  cout << "0 <=> " << dp_util::get_decoding_count(string("010")) << endl;
  cout << "1 <=> " << dp_util::get_decoding_count(string("110")) << endl;
  cout << "3 <=> " << dp_util::get_decoding_count(string("111")) << endl;
  cout << "3 <=> " << dp_util::get_decoding_count(string("1234")) << endl;

  cout << "8. dp_util::is_pattern_matched" << endl;

  cout << "1 <=> " << dp_util::is_pattern_matched("", "x*") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("", "x*x*x*") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("", "") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("x", "") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("", "") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("a", "") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("a", "aa") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("aa", "a") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("aa", "a*") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("aa", ".*") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("ab", ".*") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("mississippi", "mis*is*p*.") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("aab", "c*a*b") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("aab", "a*b") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("aaa", "ab*a") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("aaaa", ".*") << endl;
  cout << "0 <=> " << dp_util::is_pattern_matched("b", "ab*b") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("aaa", "ab*ac*a") << endl;
  cout << "1 <=> " << dp_util::is_pattern_matched("a", "..*") << endl;

  cout << "9. dp_util::calc_lcs_len" << endl;
  cout << "4 <=> " << dp_util::calc_lcs_len("AGGTAB", "GXTXAYB") << endl;
  cout << "3 <=> " << dp_util::calc_lcs_len("ABCDGH", "AEDFHR") << endl;
  cout << "1 <=> " << dp_util::calc_lcs_len("A", "AEDFHR") << endl;
  cout << "1 <=> " << dp_util::calc_lcs_len("ABCDGH", "G") << endl;
  cout << "0 <=> " << dp_util::calc_lcs_len("ABCDGH", "") << endl;
  cout << "0 <=> " << dp_util::calc_lcs_len("", "AEDFHR") << endl;
  cout << "0 <=> " << dp_util::calc_lcs_len("", "") << endl;

  cout << "10. dp_util::calc_longest_incr_subseq_len" << endl;
  cout << "6 <=> " << dp_util::calc_longest_incr_subseq_len(
                        vector<int>({ 2, 5, 3, 7, 11, 8, 10, 13, 6 })
                      ) << endl;
  cout << "5 <=> " << dp_util::calc_longest_incr_subseq_len(
                        vector<int>({ 10, 22, 9, 33, 21, 50, 41, 60 })
                      ) << endl;
  return 0;
}
