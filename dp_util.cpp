#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <map>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class DPUtil{
public:
  enum { STRATEGY_UNDEF = -1, STRATEGY_FAIL, STRATEGY_SUCCESS };
  /**
   * 13.3 Palindrome Partitioning II
   * - Given a string s, partition s such that every substring of the partition
   *   is a palindrome. Return the minimum cuts needed for a palindrome
   *   partitioning of s.
   * - For example, given s = "aab",
   *   Return 1 since the palindrome partitioning ["aa","b"] could be produced
   *   using 1 cut.
   * - min_palin_cnt(i, j) {
   *     j - i > 2, min {
   *       i <= k <= j | min_palin_cnt(i, k) + min_palin_cnt(k + 1, j)
   *     }
   *     j - i == 2, if (s[i] == s[j]) { min_palin_cnt[i][j] = 0; }
   *                 else if (s[j - 1] == s[j]) { min_palin_cnt[i][j] = 1; }
   *                 else { min_palin_cnt[i][j] = 2; }
   *     j - i <= 1, if (s[i] == s[j]) { min_palin_cnt[i][j] = 0; }
   *                 else { min_palin_cnt[i][j] = 1; }
   *   }
   * - min_palin_cnt[j] {
   *   1. min_palin_cnt(0, j - 1) + 1,
   *   2. min_palin_cnt(0, j - 1),
   *      if s[j] fits previous palin
   *      - prev size == 1 && j = 1, a ?(j)
   *        ==> only when s[j] == s[j - 1]
   *      - prev size == 1 && j > 1, a c ?(j)
   *        ==> only when s[j] == s[j - 2]
   *      - prev size == 2 && j > 1, a a ?(j)
   *        ==> only when s[j] == s[j - 1]
   *      - prev size >= 3 && j > 1, a a a ?(j)
   *        ==>> only when s[j - 1] == s[j - size + 1] && s[j] == s[j - 1]
   *   3. 1, 0 == j
   * - min_cut_cnt(i, n) = min{ i <= j <= n, | min_cut_cnt(j + 1, n) + 1 }
   */
  static int get_min_palin_cnt(const string & input_str) {
    const int input_str_len = input_str.size();
    int min_palin_cnt_buf[input_str_len + 1]; // min cuts for str[i..n]
    bool is_token_a_palin[input_str_len][input_str_len];
    for (int i = 0; i < input_str_len; i++) {
      min_palin_cnt_buf[i] = input_str_len - i - 1;
      for (int j = 0; j < input_str_len; j++) {
        if (i == j) { is_token_a_palin[i][j] = true; }
        else { is_token_a_palin[i][j] = false; }
      }
    }
    min_palin_cnt_buf[input_str_len] = -1;
    for (int i = input_str_len - 1; i >= 0; i--) {
      for (int j = i; j < input_str_len; j++) {
        if (((input_str[i] == input_str[j])) &&
            ((j - i < 2) || (is_token_a_palin[i + 1][j - 1]))) {
          is_token_a_palin[i][j] = true;
          min_palin_cnt_buf[i] = std::min(min_palin_cnt_buf[i],
                                          min_palin_cnt_buf[j + 1] + 1);
        }
      }
    }
    return min_palin_cnt_buf[0];
  }

  /** Given a 2D binary matrix filled with 0's and 1's, find the largest
    * rectangle containing all ones and return its area.
    * max_radius[r][c] = min{ max_radius[r - 1][c] + 1, max_radius[r + 1][c] + 1
    *                         max_radius[r][c - 1] - 1, max_radius[r][c + 1] + 1
    *                    }, for any value[x][y] == 1
    */

  /** Say you have an array for which the i-th element is the price of a given
    * stock on day i. Design an algorithm to find the maximum profit. You may
    * complete at most two transactions.
    * Note: You may not engage in multiple transactions at the same time (ie,
    * you must sell the stock before you buy again).
    */

  /** 13.7 Scramble String
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
    string lookup_key = get_hash_key(base, str_chk);
    string alt_lk_key = get_hash_key(str_chk, base);
    if (strategy_lookup.end() != strategy_lookup.find(lookup_key)) {
      return (STRATEGY_SUCCESS == strategy_lookup[lookup_key]);
    }
    if (strategy_lookup.end() != strategy_lookup.find(alt_lk_key)) {
      return (STRATEGY_SUCCESS == strategy_lookup[alt_lk_key]);
    }
    bool is_scramble = false;
    if (base == str_chk) { is_scramble = true; }
    if (false == is_scramble) {
      int start_idx = 0; int end_idx = base.size() - 1;
      for (int i = start_idx; i < end_idx; i++) {
        if ((check_scramble(base.substr(start_idx, i - start_idx + 1),
                            str_chk.substr(start_idx, i - start_idx + 1),
                            strategy_lookup) &&
             check_scramble(base.substr(i + 1, end_idx - i),
                            str_chk.substr(i + 1, end_idx - i),
                            strategy_lookup)) ||
            (check_scramble(base.substr(start_idx, i - start_idx + 1),
                            str_chk.substr(end_idx - i, i - start_idx + 1),
                            strategy_lookup) &&
             check_scramble(base.substr(i + 1, end_idx - i),
                            str_chk.substr(start_idx, end_idx - i),
                            strategy_lookup))) { is_scramble = true; break; }
      }
    }
    strategy_lookup[lookup_key] = (
      (true == is_scramble) ? STRATEGY_SUCCESS : STRATEGY_FAIL
    );
    strategy_lookup[alt_lk_key] = strategy_lookup[lookup_key];
// cout << lookup_key << " : " << is_scramble << " | " << base << " | " << str_chk << endl;
    return is_scramble;
  }

  /** Given s1, s2, s3, find whether s3 is formed by the interleaving of
    * s1 and s2. For example, Given: s1 = "aabcc", s2 = "dbbca",
    * When s3 = "aadbbcbcac", return true.
    * When s3 = "aadbbbaccc", return false.
    */
  static bool is_input_via_interleave(const string & base_l,
                                      const string & base_r,
                                      const string & str_chk) {
    const int l_len = base_l.size(), r_len = base_r.size();
    const int t_len = str_chk.size();
    if (l_len + r_len != t_len) { return false; }
    int ** strategy_lookup = new int * [l_len];
    for (int i = 0; i < l_len; i++) {
      strategy_lookup[i] = new int[r_len]; // 1 -> SUCCESS 0 -> FAIL -1 -> NA
      for (int j = 0; j < r_len; j++) { strategy_lookup[i][j]=STRATEGY_UNDEF; }
    }
    bool is_interleave = check_interleave(
      base_l, base_r, str_chk, 0, 0, 0, (int **)strategy_lookup
    );
    for (int i = 0; i < l_len; i++) { delete [] strategy_lookup[i]; }
    delete [] strategy_lookup;
    return is_interleave;
  }

  static bool check_interleave(const string & base_l,
                               const string & base_r,
                               const string & str_chk,
                               int curr_lidx, int curr_ridx, int chck_idx,
                               int ** strategy_lookup) {
    if (curr_lidx >= base_l.size() && curr_ridx >= base_r.size()){ return true; }
    if ((curr_lidx < base_l.size()) && (curr_ridx < base_r.size()) &&
        (str_chk[chck_idx] != base_l[curr_lidx]) &&
        (str_chk[chck_idx] != base_r[curr_ridx])) { return false; }
    if ((curr_lidx < base_l.size()) && (curr_ridx < base_r.size()) &&
        (STRATEGY_UNDEF != strategy_lookup[curr_lidx][curr_ridx])) {
      return (STRATEGY_SUCCESS == strategy_lookup[curr_lidx][curr_ridx]);
    }
    bool is_interleave = false;
    if ((curr_lidx < base_l.size()) &&
        (str_chk[chck_idx] == base_l[curr_lidx])) {
      is_interleave = check_interleave(
        base_l, base_r, str_chk, curr_lidx + 1,
        curr_ridx, chck_idx + 1, strategy_lookup
      );
    }
    if ((false == is_interleave) && (curr_ridx < base_r.size()) &&
        (str_chk[chck_idx] == base_r[curr_ridx])) {
      is_interleave = check_interleave(
        base_l, base_r, str_chk, curr_lidx,
        curr_ridx + 1, chck_idx + 1, strategy_lookup
      );
    }
    if ((curr_lidx < base_l.size()) && (curr_ridx < base_r.size())) {
      strategy_lookup[curr_lidx][curr_ridx] = (
        (true == is_interleave) ? STRATEGY_SUCCESS : STRATEGY_FAIL
      );
    }
    return is_interleave;
  }

  /**
    * 13.8 Minimum Path Sum
    * Given a m × n grid filled with non-negative numbers, find a path from top
    * left to bottom right which minimizes the sum of all numbers along its path
    * Note: You can only move either down or right at any point in time
    *
    * min_sum(i, j) = min{ grid[i][j] + min_sum(i, j - 1),
    *                      grid[i][j] + min_sum(i - 1, j) }
    */
  static int find_min_path_sum(const vector<vector<int>> & grid) {
    int row_cnt = grid.size(); int col_cnt = grid.front().size();
    vector<vector<int>> sum_buffer;
    for (int i = 0; i < row_cnt; i++) {
      sum_buffer.push_back(vector<int>(col_cnt, INT_MAX));
    }
    for (int r = 0; r < row_cnt; r++) {
      for (int c = 0; c < col_cnt; c++) {
        sum_buffer[r][c] = grid[r][c];
        if ((0 == r) && (0 == c)) { continue; }
        else if (0 == r) { sum_buffer[r][c] += sum_buffer[r][c - 1]; }
        else if (0 == c) { sum_buffer[r][c] += sum_buffer[r - 1][c]; }
        else {
          sum_buffer[r][c] += std::min(sum_buffer[r][c - 1], sum_buffer[r - 1][c]);
        }
      }
    }
    return sum_buffer[row_cnt - 1][col_cnt - 1];
  }

  /**
   * 13.11 Distinct Subsequences
   * Given a string S and a string T, count the number of distinct subsequences
   * of T in S. A subsequence of a string is a new string which is formed from
   * the original string by deleting some (can be none) of the characters
   * without disturbing the relative positions of the remaining characters.
   * (ie, "ACE" is a subsequence of "ABCDE" while "AEC" is not).
   * Here is an example: S = "rabbbit", T = "rabbit" Return 3.
   *
   * Let seq_cnt[i][j] be max common seq len for seq[0..j] found in text[0..i]
   *
   * if (i == 0 || j == 0) {
   *   seq_len[i][j] = (seq[i] == text[j]) ? 1 : 0
   * }
   * if (text[i] == seq[j]) {
   *   seq_len[i][j] = seq_len[i - 1][j - 1] + 1
   * } else {
   *   seq_len[i][j] = max(seq_len[i - 1][j], seq_len[i][j - 1])
   * }
   */
  static int count_distinct_subseq(const string & text, const string & seq) {
cout << text << " | " << seq << endl;
    int seq_cnt  = 0;
    int text_len = text.size();
    int seq_len  = seq.size();
    if (seq_len == 0 || seq_len > text_len) { return seq_cnt; }
    vector<vector<int>> common_seq_len;
    common_seq_len.push_back(vector<int>(seq_len + 1, 0));
    for (int i = 1; i <= text_len; i++) {
      common_seq_len.push_back(vector<int>(seq_len + 1, 0));
      for (int j = 1; j <= seq_len; j++) {
        if (i == 1 && j == 1) {
          common_seq_len[i][j] = (seq[j - 1] == text[i - 1]) ? 1 : 0;
        } else if (text[i - 1] == seq[j - 1]) {
          // cnt of seq[0..j-1] in text[0..i-1] and seq[0..j] in text[0..i-1]
          common_seq_len[i][j] = (
            common_seq_len[i - 1][j - 1] + common_seq_len[i - 1][j]
          );
        } else {
          common_seq_len[i][j] = common_seq_len[i - 1][j];
        }
cout << common_seq_len[i][j] << "  ";
      }
cout << endl;
    }
cout << endl;
    seq_cnt = common_seq_len[text_len][seq_len];
    return seq_cnt;
  }

  static int calc_max_subseq_len(const string & text, const string & seq) {
cout << text << " | " << seq << endl;
    int seq_cnt  = 0;
    int text_len = text.size();
    int seq_len  = seq.size();
    if (seq_len == 0 || seq_len > text_len) { return seq_cnt; }
    vector<vector<int>> common_seq_len;
    for (int i = 0; i < text_len; i++) {
      common_seq_len.push_back(vector<int>(seq_len, 0));
      for (int j = 0; j < seq_len; j++) {
        if (i == 0 && j == 0) {
          common_seq_len[i][j] = (seq[j] == text[i]) ? 1 : 0;
        } else if (text[i] == seq[j]) {
          common_seq_len[i][j] = common_seq_len[i - 1][j - 1] + 1;
        } else {
          if (i == 0 || j == 0) {
            common_seq_len[i][j] = (
              (j == 0) ? common_seq_len[i - 1][j] : common_seq_len[i][j - 1]
            );
          } else {
            common_seq_len[i][j] = std::max(common_seq_len[i - 1][j],
                                            common_seq_len[i][j - 1]);
          }
          // longest common substr, common_seq_len[i][j] = 0;
        }
        if (common_seq_len[i][j] == seq_len){ seq_cnt++; }
cout << common_seq_len[i][j] << "  ";
      }
cout << endl;
    }
cout << endl;
    return seq_cnt;
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
   */
  static bool is_word_breakable(const unordered_set<string> & dict,
                                const string & word) {
    bool is_breakable = (
      (dict.end() != dict.find(word)) || (true == word.empty())
    );
    if (true == is_breakable) { return is_breakable; }
    string token; vector<bool> lookup(word.size(), false);
    for (int i = 0; i < word.size(); i++) {
      if (i == 0) { lookup[i] = (
                      dict.end() != dict.find(word.substr(0, 1))
                    ); continue; }
      for (int j = 0; j <= i; j++) {
        token = word.substr(j, i - j + 1);
        lookup[i] = (dict.end() != dict.find(token));
        if (j > 0) { lookup[i] = (lookup[j - 1] && lookup[i]); }
        if (true == lookup[i]) { break; }
      }
    }
    is_breakable = lookup.back();
    return is_breakable;
  }

  /**
   * if w.max-id == 0 ==>> good if w[0] in dict
   * else w.max-id == i ==>> exist a j, where w[0..j-1] is breakable and also
   *      w[j..i] is in dict, whether w[0..j] is breakable can be cached in the
   *      meantime, we can keep append the token until the end.
   */
  static vector<vector<string>> break_word(const unordered_set<string> & dict,
                                           const string & word) {
    vector<string> curr_token_path;
    vector<vector<string>> token_path_arr;

    string sub_token; int word_len = word.size(); bool tmp_flag = false;

    if (0 == word_len) { return token_path_arr; }

    vector<bool> lookup(word_len, false);
    map<int, vector<string>> start_pos_to_token_arr_map;
    for (int i = 0; i < word_len; i++) {
      for (int j = 0; j <= i; j++) {
        sub_token = word.substr(j, i - j + 1);
        tmp_flag = lookup[i];
        lookup[i] = (dict.end() != dict.find(sub_token));
        if (j > 0) { lookup[i] = lookup[i] && lookup[j - 1]; }
        if (true == lookup[i]) {
          if (start_pos_to_token_arr_map.find(j) == start_pos_to_token_arr_map.end()){
            start_pos_to_token_arr_map[j] = vector<string>();
          }
          start_pos_to_token_arr_map[j].push_back(sub_token);
        }
        // reset every time when false
        if (false == lookup[i]) { lookup[i] = tmp_flag; }
      }
    }
    search_all_results(start_pos_to_token_arr_map,
                       start_pos_to_token_arr_map.begin(),
                       token_path_arr, curr_token_path, word_len);
    for (auto & token_arr: token_path_arr) {
      cout << "[ "; for (auto & token: token_arr) { cout << token << " "; } cout << "] ";
    }; cout << endl;
    return token_path_arr;
  }

  static void search_all_results(map<int, vector<string>> & start_pos_to_token_arr_map,
                                 map<int, vector<string>>::const_iterator pos_token_pair_itr,
                                 vector<vector<string>> & token_path_arr,
                                 vector<string> curr_token_path, int target_size) {
    if (start_pos_to_token_arr_map.end() == pos_token_pair_itr) { return; }
    int next_start_pos = pos_token_pair_itr->first;
    map<int, vector<string>>::const_iterator next_pair_itr;
    for (auto token : pos_token_pair_itr->second) {
      next_start_pos = pos_token_pair_itr->first + token.size();
      curr_token_path.push_back(token);
      if (target_size == next_start_pos) {
        token_path_arr.push_back(curr_token_path);
      } else {
        next_pair_itr = start_pos_to_token_arr_map.find(next_start_pos);
        if (start_pos_to_token_arr_map.end() != next_pair_itr) {
          search_all_results(start_pos_to_token_arr_map, next_pair_itr,
                             token_path_arr, curr_token_path, target_size);
        }
      }
      curr_token_path.pop_back();
    }
  }
};



int main(void) {
  string test_input[] = { "", "a", "aab", "aaa", "aaacdfe", "aaacaaafe",
                          "abcde", "abacabaca", "abacakacaba" };
  int test_output[] = { -1, 0, 1, 0, 4, 2, 4, 2, 0 };
  cout << "DPUtil::get_min_palin_cnt" << endl;
  for (int i = 0; i < sizeof(test_input) / sizeof(string); i++) {
    assert(DPUtil::get_min_palin_cnt(test_input[i]) == test_output[i]);
    cout << test_input[i] << endl;
    cout << DPUtil::get_min_palin_cnt(test_input[i]) << endl;
  }
  cout << "DPUtil::is_input_via_interleave" << endl;
  cout << DPUtil::is_input_via_interleave("aabcc", "dbbca", "aadbbcbcac") << endl; // T
  cout << DPUtil::is_input_via_interleave("aabcc", "dbbca", "aadbbbaccc") << endl; // F
  cout << "DPUtil::is_input_via_scramble" << endl;
  cout << DPUtil::is_input_via_scramble("great", "rgeat") << endl;
  cout << DPUtil::is_input_via_scramble("great", "rgtae") << endl;
  cout << DPUtil::is_input_via_scramble("great", "rtage") << endl;
  cout << "==>> DPUtil::count_distinct_subseq" << endl;
  cout << DPUtil::count_distinct_subseq("rabbbit", "rabbit") << endl;
  cout << DPUtil::count_distinct_subseq("ABCDE", "ACE") << endl;
  cout << DPUtil::count_distinct_subseq("ABCDE", "AEC") << endl;
  cout << DPUtil::count_distinct_subseq("ABCDE", "BE") << endl;
  cout << DPUtil::count_distinct_subseq("ABCDE", "A") << endl;
  cout << DPUtil::count_distinct_subseq("ABCDE", "") << endl;
  cout << "==>> is_word_breakable" << endl;
  unordered_set<string> dict = { "leet", "code" };
  cout << DPUtil::is_word_breakable(dict, "leetcode") << endl;
  cout << DPUtil::is_word_breakable(dict, "") << endl;
  cout << DPUtil::is_word_breakable(dict, "k") << endl;
  cout << "==>> break_word" << endl;
  unordered_set<string> dictc = {
    "x", "k", "leet", "code", "leetc", "cat", "cats", "and", "sand", "dog"
  };
  DPUtil::break_word(dictc, "leetcode");
  DPUtil::break_word(dictc, "");
  DPUtil::break_word(dictc, "k");
  DPUtil::break_word(dictc, "y");
  DPUtil::break_word(dictc, "catsanddog");
  return 0;
}
