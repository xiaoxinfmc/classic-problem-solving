#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <map>
#include <cmath>
#include <list>
#include <deque>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <climits>

using namespace std;

namespace search_util{
  template <class type>
  static void print_all_elem(const vector<type>& input) {
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
   * 9.1 Word Ladder
   * Given two words (start and end), and a dictionary, find the length of
   * shortest transformation sequence from start to end, such that:
   * - Only one letter can be changed at a time
   * - Each intermediate word must exist in the dictionary
   * For example, Given: start = "hit" end = "cog"
   * - dict = ["hot","dot","dog","lot","log"]
   * As one shortest transformation is "hit" -> "hot" -> "dot" -> "dog" -> "cog"
   * return its length 5. Note:
   * Return 0 if there is no such transformation sequence.
   * All words have the same length.
   * All words contain only lowercase alphabetic characters.
   *
   * Observation:
   * - each word could be seen as a vertex with edges connected to another word
   * - all edges are equal weights, to get its shortest len, bfs will be a good
   *   way once we buildup the graph.
   */
  static int str_char_diff_cnt(const string & str_l, const string & str_r) {
    if (str_l.size() != str_r.size()) { return -1; }
    int diff_char_cnt = 0;
    for (int i = 0; i < str_l.size(); i++) {
      if (str_l[i] != str_r[i]) { diff_char_cnt++; }
    }
    return diff_char_cnt;
  }

  static int find_shortest_ladder(const string start_str,
                                  const string target_str,
                                  vector<string> dict) {
    if (1 == str_char_diff_cnt(start_str, target_str)) { return 2; }

    dict.push_back(start_str); dict.push_back(target_str);

    unordered_map<string, int> str_to_idx_map;
    for (int i = 0; i < dict.size(); i++) { str_to_idx_map[dict[i]] = i; }

    vector<vector<bool>> char_diff_graph(dict.size(), vector<bool>(dict.size(), false));
    for (int i = 0; i < dict.size(); i++) {
      for (int j = i + 1; j < dict.size(); j++) {
        if (1 == str_char_diff_cnt(dict[i], dict[j])) {
          char_diff_graph[i][j] = true; char_diff_graph[j][i] = true;
        }
      }
    }

    unordered_map<string, bool> node_visited_lookup;

    bool is_target_str_found = false;
    deque<pair<string, int>> ladder_buffer;
    ladder_buffer.push_back(pair<string, int>(start_str, 1));

    int ladder_len = 0, target_str_id = dict.size() - 1;

    while (!ladder_buffer.empty() && !is_target_str_found) {
      string curr_str = ladder_buffer.front().first;
      int    curr_lvl = ladder_buffer.front().second;
      ladder_buffer.pop_front();
      if (node_visited_lookup.end() != node_visited_lookup.find(curr_str)) { continue; }

      for (int i = 0; i < char_diff_graph[str_to_idx_map[curr_str]].size(); i++) {
        if (false == char_diff_graph[str_to_idx_map[curr_str]][i]) { continue; }
        if (target_str_id == i) {
          is_target_str_found = true;
          ladder_len = curr_lvl + 1;
          break;
        } else {
          ladder_buffer.push_back(pair<string, int>(dict[i], curr_lvl + 1));
        }
      }
      node_visited_lookup[curr_str] = true;
    }

    return ladder_len;
  }

  /**
   * 9.2 Word Ladder II
   * Given two words (start and end), and a dictionary, find all shortest
   * transformation sequence(s) from start to end, such that:
   * - Only one letter can be changed at a time
   * - Each intermediate word must exist in the dictionary
   * For example, Given:
   * - start = "hit"
   * - end = "cog"
   * - dict = ["hot","dot","dog","lot","log"]
   * Return
   * [ ["hit","hot","dot","dog","cog"],
   *   ["hit","hot","lot","log","cog"] ]
   * Note:
   * - All words have the same length.
   * - All words contain only lowercase alphabetic characters.
   * - BFS to get the shortest ladder length, then DFS to get all ladders.
   */
  static void dfs_find_all_ladders(int max_ladder_len,
                                   const string & target_str,
                                   vector<string> & curr_ladder,
                                   vector<string> & dict,
                                   unordered_set<string> & visited_lookup,
                                   vector<vector<string>> & ladder_vec_ret) {
    if (curr_ladder.size() > max_ladder_len) { return; }

    string curr_str = curr_ladder.back();
    if (curr_ladder.size() == max_ladder_len && target_str == curr_str) {
      ladder_vec_ret.push_back(vector<string>(curr_ladder.begin(), curr_ladder.end()));
      return;
    }
    for (int i = 0; i < dict.size(); i++) {
      if (1 != str_char_diff_cnt(dict[i], curr_str)) { continue; }
      if (visited_lookup.end() != visited_lookup.find(dict[i])) { continue; }
      curr_ladder.push_back(dict[i]);
      visited_lookup.insert(dict[i]);
      dfs_find_all_ladders(max_ladder_len, target_str, curr_ladder,
                           dict, visited_lookup, ladder_vec_ret);
      visited_lookup.erase(dict[i]);
      curr_ladder.pop_back();
    }
  }

  static vector<vector<string>> find_all_shortest_ladder(const string & start_str,
                                                         const string & target_str,
                                                         vector<string> dict){
    dict.push_back(start_str); dict.push_back(target_str);

    /* BFS here to calc the depth of all shortest ladders. */
    int ladder_len = -1, curr_lvl = 1, target_str_id = dict.size() - 1;
    unordered_set<string> visited_lookup;
    deque<pair<string, int>> search_buffer;
    search_buffer.push_back(pair<string, int>(start_str, 1));
    bool is_target_str_reached = false; string curr_str;
    while (false == search_buffer.empty() && -1 == ladder_len) {
      curr_lvl = search_buffer.front().second;
      curr_str = search_buffer.front().first;
      search_buffer.pop_front();
      if (visited_lookup.end() != visited_lookup.find(curr_str)) { continue; }
      for (int i = 0; i < dict.size(); i++) {
        if (1 != str_char_diff_cnt(dict[i], curr_str)) { continue; }
        if (i == target_str_id) { ladder_len = curr_lvl + 1; break; }
        search_buffer.push_back(pair<string, int>(dict[i], curr_lvl + 1));
      }
      visited_lookup.insert(curr_str);
    }

    /* DFS here to calc all real ladders */
    visited_lookup.clear();
    vector<vector<string>> ladder_vec_ret;
    vector<string> curr_ladder({ start_str });
    visited_lookup.insert(start_str);
    dfs_find_all_ladders(ladder_len, target_str, curr_ladder,
                         dict, visited_lookup, ladder_vec_ret);
    return ladder_vec_ret;
  }

  /**
   * 10.7 Combination Sum
   * Given a set of candidate numbers (C) and a target number (T), find all
   * unique combinations in C where the candidate numbers sums to T.
   * The same repeated number may be chosen from C unlimited number of times.
   * Note:
   * - All numbers (including target) will be positive integers.
   * - elements in a combination(a1,a2,...,ak) must be in non-descending order.
   *   (ie,a1 ≤a2 ≤...≤ak).
   * - The solution set must not contain duplicate combinations.
   *   For example, given candidate set 2,3,6,7 and target 7, A solution set is:
   *   [7] [2, 2, 3]
   * - C(n, 1) ... C(n, n) & pick set sum up to T
   */
  static void find_nsum_recur(int target, int curr_sum,
                              vector<int>::iterator curr_itr,
                              vector<int>::iterator ends_itr,
                              vector<int> & curr_set,
                              vector<vector<int>> & nsum_ret) {
    if ((target < curr_sum) || (curr_itr == ends_itr)) { return; }
    if (target == curr_sum) {
      nsum_ret.push_back(vector<int>(curr_set.begin(), curr_set.end()));
    } else {
      curr_set.push_back(* curr_itr);
      find_nsum_recur(target, curr_sum + curr_set.back(),
                      curr_itr, ends_itr, curr_set, nsum_ret);
      curr_set.pop_back();
      while ((curr_itr + 1 != ends_itr) &&
             (* curr_itr == * (curr_itr + 1))) { curr_itr++; }
      find_nsum_recur(target, curr_sum, curr_itr + 1,
                      ends_itr, curr_set, nsum_ret);
    }
  }

  static vector<vector<int>> find_nsum(vector<int> values, int target){
    vector<int> curr_set;
    vector<vector<int>> nsum_ret;
    sort(values.begin(), values.end());
    find_nsum_recur(target, 0, values.begin(), values.end(), curr_set, nsum_ret);
    return nsum_ret;
  }

  /**
   * 10.8 Combination Sum II
   * Given a collection of candidate numbers (C) and a target number (T), find
   * all unique combinations in C where the candidate numbers sums to T.
   * Each number in C may only be used once in the combination.
   * Note:
   * - All numbers(including target) will be positive integers.
   * - elements in a combination(a1,a2,...,ak) must be in non-descending order.
   *   (ie,a1 >a2 >...>ak).
   * - The solution set must not contain duplicate combinations.
   * - For example, given candidate set 10,1,2,7,6,1,5 and target 8, A solution
   *   set is: [1, 7] [1, 2, 5] [2, 6] [1, 1, 6]
   */
  static void find_cnsum_recur(int target, int curr_sum,
                               vector<int>::iterator curr_itr,
                               vector<int>::iterator ends_itr,
                               vector<int> & curr_set,
                               vector<vector<int>> & nsum_ret) {
    if ((target < curr_sum) || (curr_itr == ends_itr)) { return; }
    if (target == curr_sum) {
      nsum_ret.push_back(vector<int>(curr_set.begin(), curr_set.end()));
    } else {
      curr_set.push_back(* curr_itr);
      find_cnsum_recur(target, curr_sum + curr_set.back(),
                       curr_itr + 1, ends_itr, curr_set, nsum_ret);
      curr_set.pop_back();
      while ((curr_itr + 1 != ends_itr) &&
             (* curr_itr == * (curr_itr + 1))) { curr_itr++; }
      find_cnsum_recur(target, curr_sum, curr_itr + 1,
                       ends_itr, curr_set, nsum_ret);
    }
  }

  static vector<vector<int>> find_sum_paths(vector<int> values, int target){
    vector<int> curr_set;
    vector<vector<int>> nsum_ret;
    sort(values.begin(), values.end());
    find_cnsum_recur(target, 0, values.begin(), values.end(), curr_set, nsum_ret);
    return nsum_ret;
  }

  /**
   * 10.11 Word Search
   * Given a 2D board and a word, find if the word exists in the grid.
   * The word can be constructed from letters of sequentially adjacent cell,
   * where "adjacent" cells are those horizontally or vertically neighbouring.
   * The same letter cell may not be used more than once.
   * For example, Given board = [ ["ABCE"],
   *                              ["SFCS"],
   *                              ["ADEE"] ]
   * word = "ABCCED", -> returns true, word = "SEE", -> returns true,
   * word = "ABCB", -> returns false.
   * conduct search based on recursion, given any input star, start with (0, 0)
   * keep expanding if (row, col) fits first char, return true if expanding good
   * else keep moving left first, try expend for any position of (row, col...n)
   * each expanding will move either right or down as no cell reuse.
   */
  static bool search_word_recur(const string & input, int curr_idx,
                                int curr_row, int curr_col,
                                unordered_set<string> visited_lookup,
                                const vector<string> & board) {
    // if all input point been checked, then true
    if (curr_idx >= input.size()) { return true; }

    // if curr point is out of range, then false
    if (curr_row >= board.size() || curr_row < 0 ||
        curr_col >= board[curr_row].size() || curr_col < 0) { return false; }

    // if curr point does not match the value, then false;
    if (input[curr_idx] != board[curr_row][curr_col]) { return false; }

    // if curr point already been included in the path, then false
    string curr_point_key = to_string(curr_row) + "$" + to_string(curr_col);
    if (visited_lookup.end() != visited_lookup.find(curr_point_key)) { return false; }
    visited_lookup.insert(curr_point_key);

    bool is_word_existed = false;

    is_word_existed = search_word_recur(input, curr_idx + 1, curr_row + 1, curr_col, visited_lookup, board);
    if (true == is_word_existed) { return is_word_existed; }
    is_word_existed = search_word_recur(input, curr_idx + 1, curr_row, curr_col + 1, visited_lookup, board);
    if (true == is_word_existed) { return is_word_existed; }
    is_word_existed = search_word_recur(input, curr_idx + 1, curr_row - 1, curr_col, visited_lookup, board);
    if (true == is_word_existed) { return is_word_existed; }
    is_word_existed = search_word_recur(input, curr_idx + 1, curr_row, curr_col - 1, visited_lookup, board);

    return is_word_existed;
  }

  static bool is_word_existed(vector<string> board, string input) {
    unordered_set<string> visited_lookup;
    for (int i = 0; i < board.size(); i++) {
      for (int j = 0; j < board[i].size(); j++) {
        visited_lookup.clear();
        if (true == search_word_recur(input, 0, i, j, visited_lookup, board)) { return true; }
      }
    }
    return false;
  }

  /**
   * 11.1 calc_pow(x, n) = { calc_pow(x, 2/n)^2, n > 1 | x, n == 1 }
   */
  static int calc_pow(int base, int p) {
    if (0 == p) { return 1; }
    if (0 == p % 2) {
      int half = calc_pow(base, p / 2);
      return half * half;
    } else {
      return base * calc_pow(base, p - 1);
    }
  }

  /**
   * 11.2 Sqrt(x)
   * Implement int sqrt(int x).
   * Compute and return the square root of x.
   */
  static double calc_sqrt(int base) {
    double sqrt_low = 0, sqrt_high = base, sqrt_val = 0, curr_val = 0;
    double margin_of_error_limit = 0.0000001, margin_of_error = 0.0;
    do {
      sqrt_val = (sqrt_low + sqrt_high) / 2;
      curr_val = sqrt_val * sqrt_val;
      if (curr_val - base > margin_of_error_limit) {
        sqrt_high = sqrt_val;
      } else {
        sqrt_low = sqrt_val;
      }
      margin_of_error = abs(curr_val - base);
    } while (margin_of_error > margin_of_error_limit);
    return sqrt_val;
  }

  /**
   * 10.1 Palindrome Partitioning
   * Given a string s, partition s such that every substring of the partition is
   * a palindrome. Return all possible palindrome partitioning of s.
   * For example, given s = "aab", Return [ ["aa","b"], ["a","a","b"] ]
   * based on current index each step, either not partition or partition.
   */
  static void search_all_palindromes(const string & input,
                                     string curr_token, int curr_idx,
                                     vector<string> & curr_plain_arr,
                                     vector<vector<bool>> & palin_lookup,
                                     vector<vector<string>> & all_palin_groups) {
    if (curr_idx >= input.size()) {
      if (true == curr_token.empty()) {
        all_palin_groups.push_back(
          vector<string>(curr_plain_arr.begin(), curr_plain_arr.end())
        );
      }
      return;
    }
    // given existing partition [ x, xx, xxx ] & y,
    // if xxxy is a palin, then either [ x, xx, xxxy ] or [ x, xx, xxx, y ]
    // else [ x, xx, xxx, y ]
    int token_start_pos = curr_idx - curr_token.size();
    if (true == palin_lookup[token_start_pos][curr_idx]) {
      curr_plain_arr.push_back(curr_token + input[curr_idx]);
      search_all_palindromes(input, "", curr_idx + 1, curr_plain_arr,
                             palin_lookup, all_palin_groups);
      curr_plain_arr.pop_back();
    }
    search_all_palindromes(input, curr_token + input[curr_idx],
                           curr_idx + 1, curr_plain_arr,
                           palin_lookup, all_palin_groups);
  }

  static vector<vector<string>> get_all_palindrome_partition(const string input) {
    vector<string> curr_plain_arr;
    vector<vector<string>> partition_set;
    vector<vector<bool>> palin_lookup(input.size(), vector<bool>(input.size(), false));
    for (int i = 0; i < input.size(); i++) { palin_lookup[i][i] = true; }
    for (int i = input.size() - 1; i >= 0; i--) {
      for (int j = i + 1; j < input.size(); j++) {
        if (j == i + 1) { palin_lookup[i][j] = (input[i] == input[j]); continue; }
        palin_lookup[i][j] = ((input[i] == input[j]) && palin_lookup[i + 1][j - 1]);
      }
    }
    search_all_palindromes(input, "", 0, curr_plain_arr, palin_lookup, partition_set);
    return partition_set;
  }

  /**
   * 10.2 Unique Paths
   * A robot is located at the top-left corner of a m × n grid (marked Start in
   * the diagram below).
   * The robot can only move either down or right at any point in time. The
   * robot is trying to reach the bottom-right corner of the grid (marked
   * Finish in the diagram below).
   * How many possible unique paths are there?
   * A . . . . . .
   *   . . . . . .
   *   . . . . . .
   *   . . . . . .
   *   . . . . . . B
   *
   * path_cnt(i, j) => path_cnt(i, j + 1) + path_cnt(i + 1, j) if j < n - 2 || i < n - 2
   *                   2 if j == n - 2 && i == n - 2
   *                   0 if j == n - 1 && i == n - 1
   */
  static int calc_unique_paths(int row, int col) {
    vector<vector<int>> path_cnt_lookup(row, vector<int>(col, 0));
    /* once reach the far right, then only downward */
    for (int i = 0; i < row; i++) { path_cnt_lookup[i][col - 1] = 1; }
    /* once reach the bottom, then only right */
    for (int i = 0; i < col; i++) { path_cnt_lookup[row - 1][i] = 1; }
    for (int i = row - 2; i >= 0; i--) {
      for (int j = col - 2; j >= 0; j--) {
        path_cnt_lookup[i][j] = path_cnt_lookup[i + 1][j] + path_cnt_lookup[i][j + 1];
      }
    }
    return path_cnt_lookup[0][0];
  }

  /**
   * 10.3 Unique Paths II
   * Follow up for Unique Paths:
   * Now consider if some obstacles are added to the grids. How many unique
   * paths would there be? An obstacle and empty space is marked as 1 and 0
   * respectively in the grid.
   * For example,
   * There is one obstacle in the middle of a 3 × 3 grid as illustrated below.
   * [ [0,0,0],
   *   [0,1,0],
   *   [0,0,0] ]
   * The total number of unique paths is 2. Note: m and n will be at most 100.
   */
  static int calc_unique_paths_by_board(vector<vector<int>> input) {
    int row = input.size(), col = input.back().size();
    vector<vector<int>> path_cnt_lookup(row, vector<int>(col, 0));

    /* once reach the far right, then only downward */
    for (int i = row - 1; i >= 0; i--) {
      if (input[i][col - 1] == 1) { path_cnt_lookup[i][col - 1] = 0; break; }
      path_cnt_lookup[i][col - 1] = 1;
    }
    /* once reach the bottom, then only right */
    for (int i = col - 1; i >= 0; i--) {
      if (input[row - 1][i] == 1) { path_cnt_lookup[row - 1][i] = 0; break; }
      path_cnt_lookup[row - 1][i] = 1;
    }

    for (int i = row - 2; i >= 0; i--) {
      for (int j = col - 2; j >= 0; j--) {
        if (input[i][j] == 1) { path_cnt_lookup[i][j] = 0; continue; }
        if (input[i + 1][j] == 0) { path_cnt_lookup[i][j] += path_cnt_lookup[i + 1][j]; }
        if (input[i][j + 1] == 0) { path_cnt_lookup[i][j] += path_cnt_lookup[i][j + 1]; }
      }
    }
    return path_cnt_lookup[0][0];

  }

  /**
   * 10.4 N-Queens
   * The n-queens puzzle is the problem of placing n queens on an n × n
   * chessboard such that no two queens attack each other.
   * Given an integer n, return all distinct solutions to the n-queens puzzle.
   * Each solution contains a distinct board configuration of the n-queens
   * placement, where Q and . both indicate a queen and an empty space.
   * For example, There exist two distinct solutions to the 4-queens puzzle:
   * [ [".Q..",           . Q . .      . . Q .
   *    "...Q",           . . . Q      Q . . .
   *    "Q...",           Q . . .      . . . Q
   *    "..Q."],          . . Q .      . Q . .
   *   ["..Q.",
   *    "Q...",
   *    "...Q",
   *    ".Q.."]
   * ]
   * Observation:
   * - Any queen placed on the board needs to satisfy:
   *   no two queens share the same row, column, or diagonal
   * - An valid placement must place all queens on the board.
   * - dfs style recursion would be straightfoward, with buffer to store all
   *   rows & columns used for pruning, validates diagonal after each try.
   * - place one queen at a time from top -> bottom.
   */
  static bool is_curr_queen_valid(int curr_row, int curr_col,
                                  vector<string> & curr_chess_board) {
    for (int r = curr_row - 1, c = curr_col + 1;
         r >= 0 && c < curr_chess_board.size(); r--, c++) {
      if (curr_chess_board[r][c] == 'Q') { return false; }
    }
    for (int r = curr_row - 1, c = curr_col - 1; r >= 0 && c >= 0; r--, c--) {
      if (curr_chess_board[r][c] == 'Q') { return false; }
    }
    return true;
  }

  static void search_n_queens_recur(int curr_queen_id, int queen_cnt,
                                    vector<bool> & used_cols_lookup,
                                    vector<string> & curr_chess_board,
                                    vector<vector<string>> & placements) {
    if (curr_queen_id >= queen_cnt) { return; }
    for (int i = 0; i < queen_cnt; i++) {
      if (true == used_cols_lookup[i]) { continue; }
      curr_chess_board[curr_queen_id][i] = 'Q';
      used_cols_lookup[i] = true;
      if (true == is_curr_queen_valid(curr_queen_id, i, curr_chess_board)) {
        if (curr_queen_id == queen_cnt - 1) {
          placements.push_back(curr_chess_board);
        } else {
          search_n_queens_recur(curr_queen_id + 1, queen_cnt, used_cols_lookup,
                                curr_chess_board, placements);
        }
      }
      curr_chess_board[curr_queen_id][i] = '.';
      used_cols_lookup[i] = false;
    }
  }

  static vector<vector<string>> search_n_queens(int n) {
    vector<vector<string>> placements;
    vector<string> curr_chess_board(n, string(n, '.'));
    vector<bool> used_cols_lookup(n, false);
    search_n_queens_recur(0, n, used_cols_lookup, curr_chess_board, placements);
    return placements;
  }

  static void print_board(const vector<string> & curr_board) {
    cout << "[ ";
    for (int i = 0; i < curr_board.size(); i++) {
      if (i > 0) { cout << "  "; }
      for (char c : curr_board[i]) { cout << c << " "; }
      if (i < curr_board.size() - 1) { cout << endl; }
    }
    cout << "]" << endl;
  }

  static void print_board_vec(const vector<vector<string>> & boards_ret) {
    for (const vector<string> & curr_board: boards_ret) { print_board(curr_board); }
  }

  /**
   * 10.6 Restore IP Addresses
   * Given a string containing only digits, restore it by returning all possible
   * valid IP address combinations. For example: Given "25525511135",
   * return ["255.255.11.135", "255.255.111.35"]. (Order does not matter)
   */
  static void restore_ips_recur(const string & ipstr,
                                int curr_idx, int token_cnt,
                                string curr_token, string & curr_ip,
                                vector<string> & ip_arr) {
    if (curr_idx >= ipstr.size() && 4 == token_cnt &&
        true == curr_token.empty()) {
      ip_arr.push_back(string(curr_ip.begin(), curr_ip.end()));
      return;
    }
    if (curr_idx > ipstr.size() || 4 < token_cnt) { return; }

    if (!curr_token.empty() && atoi(curr_token.c_str()) <= 255) {
      int curr_token_size = curr_token.size();
      curr_ip.append(curr_token);
      if (false == curr_ip.empty() && 3 != token_cnt) { curr_ip.append("."); curr_token_size++; }
      string new_token = (curr_idx < ipstr.size()) ? string("") + ipstr[curr_idx] : string("");
      restore_ips_recur(ipstr, curr_idx + 1, token_cnt + 1,
                        new_token, curr_ip, ip_arr);
      for (int i = 0; i < curr_token_size; i++) { curr_ip.pop_back(); }
    }

    if (atoi((curr_token + ipstr[curr_idx]).c_str()) <= 255 && curr_idx < ipstr.size()) {
      restore_ips_recur(ipstr, curr_idx + 1, token_cnt,
                        curr_token + ipstr[curr_idx], curr_ip, ip_arr);
    }
  }

  static vector<string> restore_ips(const string ipstr) {
    vector<string> ip_arr; string curr_ip;
    restore_ips_recur(ipstr, 0, 0, "", curr_ip, ip_arr);
    return ip_arr;
  }

  /**
   * Find the smallest positive number missing from an unsorted array
   * You are given an unsorted array with both positive and negative elements.
   * You have to find the smallest positive number missing from the array in
   * O(n) time using constant extra space. You can modify the original array.
   * Examples
   * - Input:  {2, 3, 7, 6, 8, -1, -10, 15}
   * - Output: 1
   * - Input:  { 2, 3, -7, 6, 8, 1, -10, 15 }
   * - Output: 4
   * - Input: {1, 1, 0, -1, -2}
   * - Output: 2 
   * Intuition:
   * - bf -> sorting & scan nlogn
   * - maybe we can do it via swapping?
   * - 1st missing positive int -> counting sort ?
   *   for an array of size n, say we have m negative, then (n - m) positives
   *   ideally (n - m) pos # should be 1 ... (n - m) -> should fit within arr
   *   { 2, 3, -7, 6, 8, 1, -10, 15 }
   *     ^                        ^
   *     ^                   ^
   *   { -10, 3, -7, 6, 8, 1, 2, 15 }
   *          ^   ^
   *       1   2  3  4  5  6
   *   { -10, -7, 3, 6, 8, 1, 2, 15 }
   *              ^k
   *              1  2  3  8  15  6
   * - even to make it simple, for every val, try to put it to its index which
   *   should be idx of val - 1 (say 5 -> idx of 4), swap it with origin.
   *   { 2, 3, -7, 6, 8, 1, -10, 15 }
   *     ^
   *   { 3, 2, -7, 6, 8, 1, -10, 15 }
   *     ^
   *   { -7, 2, 3, 6, 8, 1, -10, 15 }
   *         ^  ^  ^...
   */
  static int find_first_missing_positive(vector<int> & arr) {
    int first_miss = 1;
    if (arr.empty()) { return first_miss; }
    int id_to_swap = 0, i = 0;
    for (i = 0; i < arr.size(); ) {
      if (arr[i] <= 0) { i++; continue; }
      id_to_swap = arr[i] - 1;
      if (id_to_swap < arr.size() && id_to_swap != i &&
          arr[i] > 0 && arr[i] != arr[id_to_swap]) {
         swap(arr[i], arr[id_to_swap]);
      } else { i++; }
    }
    for (i = 0; i < arr.size(); i++) {
      if (arr[i] - 1 != i) { first_miss = i + 1; return first_miss; }
    }
    first_miss = i + 1;
    return first_miss;
  }

  static void test_find_first_missing_positive() {
    cout << "13. find_first_missing_positive" << endl;
    int result = 0;
    vector<int> test_output = { 1, 4, 2, 3, 2, 1, 1, 2, 3 };
    vector<vector<int>> test_input = {
      { 2, 3, 7, 6, 8, -1, -10, 15 }, { 2, 3, -7, 6, 8, 1, -10, 15 }, { 1, 1, 0, -1, -2 }, { 1, 2, 0 }, { 3, 4, -1, 1 }, {0}, {}, {1}, {1,2}
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = find_first_missing_positive(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 200. Number of Islands
   * Given a 2d grid map of '1's (land) and '0's (water), count the number of
   * islands. An island is surrounded by water and is formed by connecting
   * adjacent lands horizontally or vertically. You may assume all four edges
   * of the grid are all surrounded by water.
   * Example 1:
   * - Input:
   *   11110
   *   11010
   *   11000
   *   00000
   * - Output: 1
   * Example 2:
   * - Input:
   *   11000
   *   11000
   *   00100
   *   00011
   * - Output: 3
   * Observation:
   * - scope: the full grid, O(n) is no less than O(mn)
   * - def: an island <=> 1s connected each other horiz / vertically
   * - goal: count all different islands.
   * - for each point, keep expanding 1s, & mark all points visited.
   * - needs to use a buffer to log all points visited, islands id.
   * - 1s => island id, 0s => 0, un-visited => -1
   * - when to incr. island_seq? when no expansion in any direction.
   * Intuition:
   * - islands are cells filled with 1 & connected vert|horiz
   * - one cell can at most belongs to 1 island, so need a buffer to log it
   * - key can be its id (func(r, c))
   * - for each cell greedy search all possible direction to the end
   * - if new islands found, then return true (incr. cnt), otherwise false
   */
  static bool is_new_island_found(const vector<vector<char>> & grid,
                                  int row, int col,
                                  vector<bool> & cell_usage_lookup) {
    int curr_cell_key = row * grid.front().size() + col;
    if (!(row < grid.size() && col < grid[row].size())) { return false; }
    if (grid[row][col] == '0' || cell_usage_lookup[curr_cell_key]) { return false; }
    cell_usage_lookup[curr_cell_key] = true;
    is_new_island_found(grid, row + 1, col, cell_usage_lookup);
    is_new_island_found(grid, row, col + 1, cell_usage_lookup);
    is_new_island_found(grid, row - 1, col, cell_usage_lookup);
    is_new_island_found(grid, row, col - 1, cell_usage_lookup);
    return true;
  }

  static int calc_num_of_islands(const vector<vector<char>> grid) {
    int total_cnt = 0;
    if (grid.empty()) { return total_cnt; }
    vector<bool> cell_usage_lookup(grid.size() * grid.front().size(), false);
    for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid[i].size(); j++) {
        if (is_new_island_found(grid, i, j, cell_usage_lookup)) { total_cnt += 1; }
      }
    }
    return total_cnt;
  }

  static void test_calc_num_of_islands() {
    cout << "14. calc_num_of_islands" << endl;
    int result = 0;
    vector<vector<vector<char>>> test_input = {
      { vector<char>({'1', '1', '1', '1', '0'}), vector<char>({'1', '1', '0', '1', '0'}),
        vector<char>({'1', '1', '0', '0', '0'}), vector<char>({'0', '0', '0', '0', '0'})},
      { vector<char>({'1', '1', '0', '0', '0'}), vector<char>({'1', '1', '0', '0', '0'}),
        vector<char>({'0', '0', '1', '0', '0'}), vector<char>({'0', '0', '0', '1', '1'})},
      { vector<char>({'1', '1', '1'}), vector<char>({'0', '1', '0'}), vector<char>({'0', '1', '0'})},
      { vector<char>({'1', '0', '0', '0', '0'}), vector<char>({'0', '1', '1', '0', '0'}),
        vector<char>({'0', '1', '1', '0', '0'}), vector<char>({'0', '0', '0', '1', '1'})}
    };
    vector<int> test_output = { 1, 3, 1, 3 };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_num_of_islands(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   *
   * Description
   * - Given a n,m which means the row and column of the 2D matrix and an array
   *   of pair A( size k). Originally, the 2D matrix is all 0 which means there
   *   is only sea in the matrix.
   * - The list pair has k operator and each operator has two integer A[i].x,
   *   A[i].y means that you can change the grid matrix[A[i].x][A[i].y] from sea
   *   to island.
   * - Return how many island are there in the matrix after each operator.
   * Notice
   * - 0 is represented as the sea, 1 is represented as the island. 
   * - If two 1 is adjacent, we consider them in the same island. 
   * - We only consider up/down/left/right adjacent.
   * Example
   *   0 0 0    1 0 0    1 1 0    1 1 0    1 1 0
   *   0 0 0 -> 0 0 0 -> 0 0 0 -> 0 0 0 -> 0 0 0
   *   0 0 0    0 0 0    0 0 0    0 0 1    0 1 1
   * - Given n = 3, m = 3, array of pair A = [(0,0),(0,1),(2,2),(2,1)].
   * - return [1,1,2,2].
   */
  class disjoint_set {
  public:
    disjoint_set(int size) {
      comp_cnt = 0; components = vector<int>(size, INT_MIN);
    }

    int get_comp_cnt() { return comp_cnt; }

    void set_comp(int key) {
      if (INT_MIN == components[key]) { components[key] = -1; comp_cnt++; }
    }

    bool is_curr_cell_belongs_to_sea(int key) { return (INT_MIN == components[key]); }

    bool union_set(int l_comp_id, int r_comp_id) {
      bool is_comp_dec = false;
      if (INT_MIN == components[l_comp_id] ||
          INT_MIN == components[r_comp_id]) { return false; }
      int l_root_id = union_find(l_comp_id);
      int r_root_id = union_find(r_comp_id);
      if (l_root_id == r_root_id) { return false; }
      /* merge small tree to larger one */
      if (abs(components[l_root_id]) > abs(components[r_root_id])) {
        components[l_root_id] += components[r_root_id];
        components[r_root_id] = l_root_id;
      } else {
        components[r_root_id] += components[l_root_id];
        components[l_root_id] = r_root_id;
      }
      comp_cnt--;
      return true;
    }

    int union_find(int comp_id) {
      int root_id = comp_id;
      while (components[root_id] >= 0) { root_id = components[root_id]; }
      while (components[comp_id] >= 0) { components[comp_id] = root_id;
                                         comp_id = components[comp_id]; }
      return root_id;
    }

    virtual ~disjoint_set() {}
    /* for root, size of group will be negative number, non-neg means parent id */
    vector<int> components;
    int comp_cnt;
  };

  static void check_and_link_neighbor_cells(disjoint_set & islands_set,
                                            int curr_row, int curr_col,
                                            int next_row, int next_col,
                                            int row_cnt, int col_cnt) {
    if ((curr_row < 0 || curr_col < 0 || curr_row >= row_cnt || curr_col >= col_cnt) ||
        (next_row < 0 || next_col < 0 || next_row >= row_cnt || next_col >= col_cnt)) { return; }
    int curr_cell_key = curr_row * col_cnt + curr_col;
    int next_cell_key = next_row * col_cnt + next_col;
    if (islands_set.is_curr_cell_belongs_to_sea(next_cell_key)) { return; }
    islands_set.union_set(curr_cell_key, next_cell_key);
  }

  static vector<int> calc_num_of_islands_adp(const vector<vector<int>> & ops_arr,
                                             int row, int col) {
    vector<int> islands_cnt;
    if (ops_arr.empty() || row <= 0 || col <= 0) { return islands_cnt; }
    disjoint_set islands_set(row * col);
    int curr_cell_key = 0;
    for (vector<int> op : ops_arr) {
      curr_cell_key = op.front() * col + op.back();
      /* only union find if existing cell change from sea to land */
      if (islands_set.is_curr_cell_belongs_to_sea(curr_cell_key)) {
        islands_set.set_comp(curr_cell_key);
        check_and_link_neighbor_cells(islands_set, op.front(), op.back(), op.front() + 1, op.back(), row, col);
        check_and_link_neighbor_cells(islands_set, op.front(), op.back(), op.front() - 1, op.back(), row, col);
        check_and_link_neighbor_cells(islands_set, op.front(), op.back(), op.front(), op.back() + 1, row, col);
        check_and_link_neighbor_cells(islands_set, op.front(), op.back(), op.front(), op.back() - 1, row, col);
      }
      islands_cnt.push_back(islands_set.get_comp_cnt());
    }

    return islands_cnt;
  }

  static void test_calc_num_of_islands_adp() {
    cout << "15. calc_num_of_islands" << endl;
    vector<int> result;
    vector<vector<int>> test_output = {
      { 1, 1, 2, 2, 2 },
      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 11, 12, 12, 12, 12, 12, 13, 13, 14,
        15, 16, 17, 18, 18, 18, 19, 19, 18, 17, 16, 16, 16, 16, 16, 16, 17, 17,
        16, 16, 17, 18, 18, 18, 17, 17, 17 } };
    vector<vector<vector<int>>> test_input = {
      { { 0, 0 }, { 0, 1 }, { 2, 2 }, { 2, 2 }, { 2, 1 } },
      { {0,9},{5,4},{0,12},{6,9},{6,5},{0,4},{4,11},{0,0},
        {3,5},{6,7},{3,12},{0,5},{6,13},{7,5},{3,6},{4,4},
        {0,8},{3,1},{4,6},{6,1},{5,12},{3,8},{7,0},{2,9},
        {1,4},{3,0},{1,13},{2,13},{6,0},{6,4},{0,13},{0,3},
        {7,4},{1,8},{5,5},{5,7},{5,10},{5,3},{6,10},{6,2},
        {3,10},{2,7},{1,12},{5,0},{4,5},{7,13},{3,2} }
    };
    vector<vector<int>> test_input_cfg = { { 3, 3 }, { 8, 14 } };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_num_of_islands_adp(test_input[i], test_input_cfg[i].front(), test_input_cfg[i].back());
      print_all_elem<int>(result);
      cout << "<=>" << endl;
      print_all_elem<int>(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) { assert(result[j] == test_output[i][j]); }
    }
  }

  /**
   * 295. Find Median from Data Stream
   * - Median is the middle value in an ordered integer list. If the size of
   *   the list is even, there is no middle value. So the median is the mean
   *   of the two middle value.
   * For example,
   * - [2,3,4], the median is 3
   * - [2,3], the median is (2 + 3) / 2 = 2.5
   * - Design a data structure that supports the following two operations:
   * - void addNum(int num) - Add a integer number from the data stream
   * - double findMedian() - Return the median of all elements so far.
   * Example:
   * - addNum(1)
   * - addNum(2)
   * - findMedian() -> 1.5
   * - addNum(3) 
   * - findMedian() -> 2
   * - Follow up:
   * - If all integer numbers from the stream are between 0 and 100
   * - If 99% of all integer numbers from the stream are between 0 and 100
   * Intuition:
   * - goal is to find a way to maintain the ability to do quick median query
   *   for dynamic number set.
   * - use 2 heap?
   *   max-heap : [ ......... ] min-heap : [ .............. ]
   *                1 2 3 4 5                 6 7 8 9 10
   *   each time add a new number, if > max-heap -> push to min-heap, vice versa.
   *   unless it cause heap to diff > 1 in size, 
   *   { 5 } {} < 6
   *   { 5 } { 6 } < 1
   *   { 1, 5 } { 6 } < 2 => cause size to diff > 1
   *   { 1, 2 }, { 5 }
   * - add-number => O(logn), query -> O(1)
   */
  class median_finder {
  public:
    median_finder() : curr_median(numeric_limits<double>::max()) { }
    virtual ~median_finder() { }
    void add_number(int new_value) { append_and_adjust_heaps(new_value); }
    double find_median() { return curr_median; }
  private:
    struct min_heap_less_op {
      bool operator() (const int & a, const int & b) { return b < a; }
    };
    void append_and_adjust_heaps(int new_value) {
      /* simply push the heap 1st */
      if (new_value < curr_median) {
        max_heap.push_back(new_value);
        push_heap(max_heap.begin(), max_heap.end());
        curr_median = max_heap.front();
      } else {
        min_heap.push_back(new_value);
        push_heap(min_heap.begin(), min_heap.end(), min_heap_less_op());
        curr_median = min_heap.front();
      }
      /* check to see any adjust needed for heaps */
      int size_diff = abs((int)max_heap.size() - (int)min_heap.size());
      if (size_diff >= 2) {
        if (max_heap.size() > min_heap.size()) {
          min_heap.push_back(max_heap.front());
          push_heap(min_heap.begin(), min_heap.end(), min_heap_less_op());
          pop_heap(max_heap.begin(), max_heap.end());
          max_heap.pop_back();
        } else {
          max_heap.push_back(min_heap.front());
          push_heap(max_heap.begin(), max_heap.end());
          pop_heap(min_heap.begin(), min_heap.end(), min_heap_less_op());
          min_heap.pop_back();
        }
        curr_median = (min_heap.front() + max_heap.front()) / 2.0;
      } else if (size_diff == 0) {
        curr_median = (min_heap.front() + max_heap.front()) / 2.0;
      }
    }
    double curr_median;
    vector<int> max_heap, min_heap;
  };

  static void test_median_finder() {
    cout << "16. test_median_finder" << endl;
    median_finder mf;
    for (int i = 0; i < 999; i++) {
      mf.add_number(i);
      if (i % 2 == 0) { assert(i / 2 == mf.find_median()); }
      else { assert((i / 2 + i / 2 + 1)/2.0 == mf.find_median()); }
    }
  }

  /**
   * 302. Smallest Rectangle Enclosing Black Pixels
   * - An image is represented by a binary matrix with 0 as a white pixel and 1
   *   as a black pixel. The black pixels are connected, i.e., there is only one
   *   black region. Pixels are connected horizontally and vertically. Given the
   *   location (x, y) of one of the black pixels, return the area of the
   *   smallest (axis-aligned) rectangle that encloses all black pixels.
   * - For example, given the following image:
   *   { "0010", "0110", "0100" } and x = 0, y = 2,
   *   Return 6.
   * Intuition:
   * - goal is to calc the bounding box of black pixels, obviously, it is to get
   *   the min/max-x-pos/y-pos.
   * - as it is mentioned, all black pixels are connected, such that identify
   *   the area should be easy, similar to flood-fill + dfs/bfs.
   */
  static bool is_pixel_black(const vector<string> & image, int x, int y) {
    return (x >= 0 && x < image.size() && y >= 0 &&
            y < image[x].size() && '1' == image[x][y]);
  }

  static int get_pixel_key(const vector<string> & image, int x, int y) {
    return (x * image.front().size() + y);
  }

  static int get_enclosed_area(const vector<string> & image, int x, int y) {
    int total_pixel_cnt = image.size() * image.front().size();
    vector<bool> visit_lookup(total_pixel_cnt, false);
    vector<pair<int, int>> visit_buffer = { pair<int, int>(x, y) };
    pair<int, int> curr_pixel;
    int curr_pixel_key = 0, min_x_pos = INT_MAX, min_y_pos = INT_MAX,
                            max_x_pos = INT_MIN, max_y_pos = INT_MIN;
    while (!visit_buffer.empty()) {
      curr_pixel = visit_buffer.back();
      visit_buffer.pop_back();
      curr_pixel_key = get_pixel_key(image, curr_pixel.first, curr_pixel.second);
      if (true == visit_lookup[curr_pixel_key]) { continue; }
      else { visit_lookup[curr_pixel_key] = true; }
      if (true == is_pixel_black(image, curr_pixel.first + 1, curr_pixel.second)) {
        visit_buffer.push_back(pair<int, int>(curr_pixel.first + 1, curr_pixel.second));
      }
      if (true == is_pixel_black(image, curr_pixel.first, curr_pixel.second + 1)) {
        visit_buffer.push_back(pair<int, int>(curr_pixel.first, curr_pixel.second + 1));
      }
      if (true == is_pixel_black(image, curr_pixel.first - 1, curr_pixel.second)) {
        visit_buffer.push_back(pair<int, int>(curr_pixel.first - 1, curr_pixel.second));
      }
      if (true == is_pixel_black(image, curr_pixel.first, curr_pixel.second - 1)) {
        visit_buffer.push_back(pair<int, int>(curr_pixel.first, curr_pixel.second - 1));
      }
      min_x_pos = min(min_x_pos, curr_pixel.first);
      min_y_pos = min(min_y_pos, curr_pixel.second);
      max_x_pos = max(max_x_pos, curr_pixel.first);
      max_y_pos = max(max_y_pos, curr_pixel.second);
    }
    return (max_x_pos - min_x_pos + 1) * (max_y_pos - min_y_pos + 1);
  }

  static void test_get_enclosed_area() {
    cout << "17. test_get_enclosed_area" << endl;
    int result = 0;
    vector<vector<string>> test_input = {
      { "0010", "0110", "0100" }
    };
    vector<vector<int>> test_pos_input = { { 0, 2 } };
    vector<int> test_output = { 6 };
    for (int i = 0; i < test_input.size(); i++) {
      result = get_enclosed_area(test_input[i], test_pos_input[i][0],
                                                test_pos_input[i][1]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  enum BOUNDARY_SEARCH_TYPE { MIN_X_POS = 0, MIN_Y_POS, MAX_X_POS, MAX_Y_POS };

  static int binary_search_for_boundary(const vector<string> & image,
                                        int x, int y, int search_type = MIN_X_POS) {
    int low = 0, high = 0, mid = 0, boundary_idx = 0;
    switch(search_type) {
      case MIN_X_POS: { low = 0; high = x; break; }
      case MIN_Y_POS: { low = 0; high = y; break; }
      case MAX_X_POS: { low = x; high = image.size() - 1; break; }
      case MAX_Y_POS: { low = y; high = image.front().size() - 1; break; }
      default: break;
    }
    bool is_black_pixel_exists = false;
    while (low <= high) {
      is_black_pixel_exists = false;
      mid = (high + low) / 2;
      switch(search_type) {
        case MIN_X_POS: case MAX_X_POS: {
          /* to get the boundary of x pos (vertically), we need to check all possible columns */
          for (int i = 0; i < image.front().size(); i++) {
            is_black_pixel_exists = is_pixel_black(image, mid, i);
            if (true == is_black_pixel_exists) { break; }
          }
          break;
        }
        case MIN_Y_POS: case MAX_Y_POS: {
          /* to get the boundary of y pos (horizontally), we need to check all possible rows */
          for (int i = 0; i < image.size(); i++) {
            is_black_pixel_exists = is_pixel_black(image, i, mid);
            if (true == is_black_pixel_exists) { break; }
          }
          break;
        }
        default: break;
      }
      switch(search_type) {
        /* if mid row/col has black pixel, then push either direction to check higher boundary */
        case MIN_X_POS: case MIN_Y_POS: {
          if (true == is_black_pixel_exists) { high = mid - 1; } else { low = mid + 1; }; break;
        }
        case MAX_X_POS: case MAX_Y_POS: {
          if (true == is_black_pixel_exists) { low = mid + 1; } else { high = mid - 1; }; break;
        }
        default: break;
      }
      if (true == is_black_pixel_exists) { boundary_idx = mid; }
    }
    return boundary_idx;
  }

  static int fast_get_enclosed_area(const vector<string> & image, int x, int y) {
    int min_x_pos = binary_search_for_boundary(image, x, y, MIN_X_POS);
    int min_y_pos = binary_search_for_boundary(image, x, y, MIN_Y_POS);
    int max_x_pos = binary_search_for_boundary(image, x, y, MAX_X_POS);
    int max_y_pos = binary_search_for_boundary(image, x, y, MAX_Y_POS);
    return (max_x_pos - min_x_pos + 1) * (max_y_pos - min_y_pos + 1);
  }

  static void test_fast_get_enclosed_area() {
    cout << "18. test_fast_get_enclosed_area" << endl;
    int result = 0;
    vector<vector<string>> test_input = {
      { "0010", "0110", "0100" }
    };
    vector<vector<int>> test_pos_input = { { 0, 2 } };
    vector<int> test_output = { 6 };
    for (int i = 0; i < test_input.size(); i++) {
      result = fast_get_enclosed_area(test_input[i], test_pos_input[i][0],
                                                     test_pos_input[i][1]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 174. Dungeon Game
   * - The demons had captured the princess (P) and imprisoned her in the
   *   bottom-right corner of a dungeon. The dungeon consists of M x N rooms
   *   laid out in a 2D grid. Our valiant knight (K) was initially positioned
   *   in the top-left room and must fight his way through the dungeon to
   *   rescue the princess.
   * - The knight has an initial health point represented by positive integer.
   *   If any point his health point drops to 0 or below, he dies immediately.
   * - Some of the rooms are guarded by demons, so the knight loses health
   *   (negative integers) upon entering these rooms; other rooms are either
   *   empty (0's) or contain magic orbs that increase the knight's health
   *   (positive integers).
   * - In order to reach the princess as quickly as possible, the knight
   *   decides to move only rightward or downward in each step.
   * - Write a function to determine the knight's minimum initial health so
   *   that he is able to rescue the princess.
   * - For example, given the dungeon below, the initial health of the knight
   *   must be at least 7 if he follows the optimal path
   *   RIGHT-> RIGHT -> DOWN -> DOWN.
   *   -2(K)   -3   3
   *   -5     -10   1
   *   10      30  -5 (P)
   * Intuition:
   * - obviously, it is a search problem, goal is to find path from top-left
   *   to bottom-right with min cost(only move down or right).
   * - the cost function here needs some attention, as it is not simply a sum
   *   but the min-sum-value at any step in the middle, we want at any step the
   *   knight does not lose too much life (say -7 is better than -30, even next
   *   step it can added up 10 and 60), because kinght needs to keep alive at
   *   every step, and if the value drop below 10, he dies.
   * - cost function should really be prefix-sum-of-path(i), a min-path means:
   *   for a path from (cell 0 ... cell j), it will have the largest value of
   *   min{ 0 < k < j | prefix-sum-of-path(k) }
   * - if all cells are pos, then knight only need 0, any path is good anyway.
   * - Whether it is a DP depends on if we can breakdown it into subproblem and
   *   find the optimal structure. Also we only want a value instead of full
   *   path, then DP is a natural solution here.
   * - Let min-sum-cost(i, j) denote the min-prefix-sum for path ends at (i,j)
   *   min-sum-cost(i, j) = max(min-sum-cost(i - 1, j), min-sum-cost(i, j - 1))
   *   goal is to calc bottom right value.
   * - for each cell, prefix-sum & min-prefix-sum can from diff. path.
   * - however, hard part being that it is very difficult to nail down optimal
   *   structure, as on each point of path, we have current hp(prefix-sum) and
   *   min-prefix-sum along the path, without futrue context, we cannot make
   *   a move.
   *
   * - A DIFF strategy would be searching the path in reverse order, and change
   *   the meaning of each cell in lookup.
   * - !!!! Let min-hp-lookup(i, j) represents min-hp knight needs to have
   *   BEFORE reaching the target cell. min-hp-lookup(0, 0) will be the goal.
   * - For initial status in bottom right cell, the knight needs min hp of
   *   0 - dungeon[i][j] + 1. Plus, to enter this cell(i, j), knight may move
   *   from cell(i - 1, j) or cell(i, j - 1), and each cell has its own cost
   *   then we can alos calc. the min-hp for cell(i - 1, j) & cell(i, j - 1)
   * - to calc min-hp-lookup(i, j), we know it may move in 2 directions,
   *   either cell(i + 1, j) or cell(i, j + 1). assume we already know the min
   *   hp needed in these 2 cells, then min-hp-lookup(i, j) should be able to
   *   satisfy the min requirement so as to move forward (to bottom-right cell)
   * - Or rather straight-forwardly to say, when we have 2 cell, both could lead
   *   to the target cell, obviously we want to be in the cell with smallest
   *   min-hp limit to finish the path.
   * - VERY important thing here is that we don't choose the actual path here,
   *   ONLY to calc min. hp requirement on each room to finish rest of journey.
   *
   *   min-hp-lookup(i, j) + cell(i, j) = min(min-hp-lookup(i + 1, j), min-hp-lookup(i, j + 1))
   *                                    v
   *   min-hp-lookup(i, j) = min(min-hp-lookup(i + 1, j), min-hp-lookup(i, j + 1)) - cell(i, j)
   */
  static int calc_min_hp(const vector<vector<int>>& dungeon) {
    if (dungeon.empty() || dungeon.front().empty()) { return 1; }
    vector<int> min_cost_lookup = dungeon.back();
    min_cost_lookup.back()  = 1 - min_cost_lookup.back();
    if (min_cost_lookup.back() <= 0) { min_cost_lookup.back() = 1; }

    for (int i = dungeon.size() - 1; i >= 0; i--) {
      for (int j = dungeon[i].size() - 1; j >= 0; j--) {
        if (j + 1 == dungeon[i].size() && i + 1 == dungeon.size()) { continue; }
        int tentative_value = INT_MAX;
        if (j + 1 < dungeon[i].size()) { tentative_value = min(tentative_value, min_cost_lookup[j + 1]); }
        if (i + 1 < dungeon.size()) { tentative_value = min(tentative_value, min_cost_lookup[j]); }
        min_cost_lookup[j] = (0 - dungeon[i][j]);
        if (INT_MAX != tentative_value) { min_cost_lookup[j] += tentative_value; }
        if (min_cost_lookup[j] <= 0) { min_cost_lookup[j] = 1; }
      }
    }
    return min_cost_lookup.front() > 0 ? min_cost_lookup.front() : 1;
  }

  static void test_calc_min_hp() {
    cout << "19. test_calc_min_hp" << endl;
    int result = 0;
    vector<int> test_output = { 7, 3, 4, 2 };
    vector<vector<vector<int>>> test_input = {
      { { -2, -3, 3 }, { -5, -10, 1 }, { 10, 30, -5 } },
      { { 1, -3, 3 }, { 0, -2, 0 }, { -3, -3, -3 } },
      { { -3, 5 } },
      { { -1, 1 } }
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_min_hp(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }
};

int main(void) {
  using search_util::print_all_elem;
  using search_util::print_all_elem_vec;
  using search_util::find_shortest_ladder;
  using search_util::find_all_shortest_ladder;
  using search_util::find_nsum;
  using search_util::find_sum_paths;
  using search_util::is_word_existed;
  using search_util::calc_pow;
  using search_util::calc_sqrt;
  using search_util::get_all_palindrome_partition;
  using search_util::calc_unique_paths;
  using search_util::calc_unique_paths_by_board;
  using search_util::search_n_queens;
  using search_util::print_board_vec;
  using search_util::print_board;
  using search_util::restore_ips;
  using search_util::test_find_first_missing_positive;
  using search_util::test_calc_num_of_islands;
  using search_util::test_calc_num_of_islands_adp;
  using search_util::test_median_finder;
  using search_util::test_get_enclosed_area;
  using search_util::test_fast_get_enclosed_area;
  using search_util::test_calc_min_hp;

  cout << "1. find_shortest_ladder" << endl;
  vector<string> d0({ "hot","dot","dog","lot","log" });
  cout << "-->> hit [ hot dot dog lot log ] cog" << endl;
  cout << "[ hit hot dot dog cog ] <=> " << find_shortest_ladder("hit", "cog", d0) << endl;

  cout << "2. find_all_shortest_ladder" << endl;
  cout << "-->> hit [ hot dot dog lot log ] cog" << endl;
  cout << "[" << endl << "  [ hit hot dot dog cog ]" << endl
       << "  [ hit hot lot log cog ]" << endl << "] <=>" << endl;
  print_all_elem_vec<string>(find_all_shortest_ladder("hit", "cog", d0));

  cout << "3. find_nsum" << endl;
  cout << "[ [ 7 ] [ 2, 2, 3 ] ] <=>" << endl;
  print_all_elem_vec<int>(find_nsum(vector<int>({ 2, 3, 6, 7 }), 7));

  cout << "4. find_sum_paths" << endl;
  cout << "[ [ 1, 7 ] [ 1, 2, 5 ] [ 2, 6 ] [ 1, 1, 6 ] ] <=>" << endl;
  print_all_elem_vec<int>(find_sum_paths(vector<int>({ 10, 1, 2, 7, 6, 1, 5 }), 8));

  cout << "5. is_word_existed" << endl;
  cout << "1 <=> " << is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "ABCCED") << endl;
  cout << "1 <=> " << is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "SEE") << endl;
  cout << "0 <=> " << is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "ABCB") << endl;
  cout << "0 <=> " << is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "ABCD") << endl;
  cout << "0 <=> " << is_word_existed(vector<string>({ "ABCE", "SFCS", "BFE" }), "ABCD") << endl;

  cout << "6. calc_pow" << endl;
  cout << "2 ^ 0: " << calc_pow(2, 0) << endl;
  cout << "2 ^ 1: " << calc_pow(2, 1) << endl;
  cout << "2 ^ 2: " << calc_pow(2, 2) << endl;
  cout << "2 ^ 3: " << calc_pow(2, 3) << endl;
  cout << "2 ^ 4: " << calc_pow(2, 4) << endl;

  cout << "7. calc_sqrt" << endl;
  cout << "0: " << calc_sqrt(0) << endl;
  cout << "1: " << calc_sqrt(1) << endl;
  cout << "2: " << calc_sqrt(2) << endl;
  cout << "3: " << calc_sqrt(3) << endl;
  cout << "4: " << calc_sqrt(4) << endl;

  cout << "8. get_all_palindrome_partition" << endl;
  cout << "aab | [ [ aa, b ], [ a, a, b ] ] <=>" << endl;
  print_all_elem_vec<string>(get_all_palindrome_partition("aab"));
  cout << "a | [ [ a ] ] <=>" << endl;
  print_all_elem_vec<string>(get_all_palindrome_partition("a"));
  cout << "aaa | [ [ a, a, a ], [ aa, a ], [a, aa ], [ aaa ] ] <=>" << endl;
  print_all_elem_vec<string>(get_all_palindrome_partition("aaa"));
  cout << "aca | [ [ a, c, a ], [ aca ] ]" << endl;
  print_all_elem_vec<string>(get_all_palindrome_partition("aca"));
  cout << "aacaa | [ [a, a, c, a, a ], [ a, aca, a ], [ a, a, c, aa ], [aa, c, a, a ], [ aa, c, aa ], [ aacaa ] ]" << endl;
  print_all_elem_vec<string>(get_all_palindrome_partition("aacaa"));

  cout << "9. calc_unique_paths" << endl;
  cout << "28 <=> " << calc_unique_paths(3, 7) << endl;

  cout << "10. calc_unique_paths_by_board" << endl;
  vector<vector<int>> path_input({
    vector<int>({ 0, 0, 0 }), vector<int>({ 0, 1, 0 }), vector<int>({ 0, 0, 0 })
  });
  print_all_elem_vec<int>(path_input);
  cout << "2 <=> " << calc_unique_paths_by_board(path_input) << endl;

  cout << "11. search_n_queens" << endl;
  cout << "[ . . Q .\n" << "  Q . . .\n" << "  . . . Q\n" << "  . Q . . ]\n"
       << "[ . Q . .\n" << "  . . . Q\n" << "  Q . . .\n" << "  . . Q . ]\n <=>" << endl;
  print_board_vec(search_n_queens(4));
  cout << "92 <=> " << search_n_queens(8).size() << endl;
  cout << "724 <=> " << search_n_queens(10).size() << endl;

  cout << "12. restore_ips" << endl;
  cout << "[ 255.255.11.135 255.255.111.35 ] <=>" << endl;
  print_all_elem<string>(restore_ips("25525511135"));

  test_find_first_missing_positive();
  test_calc_num_of_islands();
  test_calc_num_of_islands_adp();
  test_median_finder();
  test_get_enclosed_area();
  test_fast_get_enclosed_area();
  test_calc_min_hp();

  return 0;
}
