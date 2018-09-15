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
   */
  static int find_first_missing_positive(vector<int> input) {
    for (int i = 0, j = input.size() - 1; i < j;) {
      if (input[j] < 0) { j--; continue; }
      if (input[i] > 0) { i++; continue; }
      swap(input[i], input[j]);
    }
    for (int i = 0; i < input.size(); i++) {
      if (input[i] > 0 && input[i] < input.size()) {
        input[input[i]] = input[input[i]] * -1;
      }
    }
    for (int i = 1; i < input.size(); i++) {
      if (input[i] >= 0) { return i; }
    }
    return false;
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
   */
  static void search_islands_recur(vector<vector<char>> & grid,
                                   int curr_row, int curr_col, int * curr_area_ptr,
                                   vector<vector<int>> & islands_mark) {
    if (curr_row < 0 || curr_row >= grid.size() ||
        curr_col < 0 || curr_col >= grid[curr_row].size()) { return; }
    if (-1 != islands_mark[curr_row][curr_col]) { return; }

    if ('1' == grid[curr_row][curr_col]) {
      islands_mark[curr_row][curr_col] = 1;
      * curr_area_ptr += 1;
      search_islands_recur(grid, curr_row + 1, curr_col, curr_area_ptr, islands_mark);
      search_islands_recur(grid, curr_row, curr_col + 1, curr_area_ptr, islands_mark);
      search_islands_recur(grid, curr_row - 1, curr_col, curr_area_ptr, islands_mark);
      search_islands_recur(grid, curr_row, curr_col - 1, curr_area_ptr, islands_mark);
    } else {
      islands_mark[curr_row][curr_col] = 0;
    }
  }

  static int calc_num_of_islands(vector<vector<char>> grid) {
    int curr_area = 0, islands_cnt = 0;
    if (grid.empty()) { return islands_cnt; }
    vector<vector<int>> islands_mark(grid.size(), vector<int>(grid.front().size(), -1));
    for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid[i].size(); j++) {
        curr_area = 0;
        search_islands_recur(grid, i, j, & curr_area, islands_mark);
        if (curr_area > 0) { islands_cnt += 1; }
      }
    }
    return islands_cnt;
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
      assert(size > 0);
      components_cnt = size;
      union_set = vector<int>(size, -1);
    }

    virtual ~disjoint_set() {}

    /* when elem 1st added, reset its component size to 1 <-> 1 */
    bool add_to_set(int id) {
      if (!is_id_within_range(id)) { return false; }
      union_set[id] = -1;
      return false;
    }

    /* merge prev with next to same component */
    bool union_sets(int prev_id, int next_id) {
      if (!is_id_within_range(prev_id) ||
          !is_id_within_range(next_id)) { return false; }
      int prev_src = union_find(prev_id);
      int next_src = union_find(next_id);
      if (prev_src == next_src) { return false; }
      union_by_root(prev_src, next_src);
      components_cnt--;
      return true;
    }

    int get_components_cnt() { return components_cnt; }

    int get_max_slots() { return union_set.size(); }

  private:

    /* un-connected means belongs to itself */
    int union_find(int id) {
      if (!is_id_within_range(id)) { return -1; }
      if (union_set[id] < 0) { return id; }
      union_set[id] = union_find(union_set[id]);
      return union_set[id];
    }

    void union_by_root(int prev_root, int next_root) {
      if (union_set[prev_root] > union_set[next_root]) {
        /* size(-2) < size(-3), -2 > -3 => merge small(prev) to big(next), merge next to prev */
        union_set[next_root] += union_set[prev_root];
        union_set[prev_root]  = next_root;
      } else {
        union_set[prev_root] += union_set[next_root];
        union_set[next_root] = prev_root;
      }
    }

    bool is_id_within_range(int id) {
      return ((id >= 0) && (id < union_set.size()));
    }

    vector<int> union_set;
    int components_cnt;
  };

  static vector<int> calc_num_of_islands_adp(vector<vector<int>> ops_arr,
                                             int row, int col) {
    vector<vector<int>> board(row, vector<int>(col, 0));
    vector<int> islands_cnts;
    disjoint_set islands_union(row * col);
    int curr_row = 0, curr_col = 0, total_islands = 0;
    for (vector<int> & op : ops_arr) {
      curr_row = op.front(); curr_col = op.back();

      if (1 == board[curr_row][curr_col]) { islands_cnts.push_back(total_islands); continue; }
      total_islands += 1;
      board[curr_row][curr_col] = 1;

      if (curr_row + 1 < row && board[curr_row + 1][curr_col] == 1) {
        if (true == islands_union.union_sets(curr_row * col + curr_col, (curr_row + 1) * col + curr_col)) { total_islands--; }
      }
      if (curr_row - 1 >= 0  && board[curr_row - 1][curr_col] == 1) {
        if (true == islands_union.union_sets(curr_row * col + curr_col, (curr_row - 1) * col + curr_col)) { total_islands--; }
      }
      if (curr_col + 1 < col && board[curr_row][curr_col + 1] == 1) {
        if (true == islands_union.union_sets(curr_row * col + curr_col, curr_row * col + (curr_col + 1))) { total_islands--; }
      }
      if (curr_col - 1 >= 0  && board[curr_row][curr_col - 1] == 1) {
        if (true == islands_union.union_sets(curr_row * col + curr_col, curr_row * col + (curr_col - 1))) { total_islands--; }
      }
      islands_cnts.push_back(total_islands);
    }
    return islands_cnts;
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
  using search_util::find_first_missing_positive;
  using search_util::calc_num_of_islands;
  using search_util::calc_num_of_islands_adp;

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

  cout << "13. find_first_missing_positive" << endl;
  cout << "1 <=> " << find_first_missing_positive(vector<int>({ 2, 3, 7, 6, 8, -1, -10, 15 })) << endl;
  cout << "4 <=> " << find_first_missing_positive(vector<int>({ 2, 3, -7, 6, 8, 1, -10, 15 })) << endl;
  cout << "2 <=> " << find_first_missing_positive(vector<int>({ 1, 1, 0, -1, -2 })) << endl;

  cout << "14. calc_num_of_islands" << endl;
  cout << "1 <=> " << calc_num_of_islands(vector<vector<char>>({ vector<char>({'1', '1', '1', '1', '0'}),
                                                                 vector<char>({'1', '1', '0', '1', '0'}),
                                                                 vector<char>({'1', '1', '0', '0', '0'}),
                                                                 vector<char>({'0', '0', '0', '0', '0'})})) << endl;
  cout << "3 <=> " << calc_num_of_islands(vector<vector<char>>({ vector<char>({'1', '1', '0', '0', '0'}),
                                                                 vector<char>({'1', '1', '0', '0', '0'}),
                                                                 vector<char>({'0', '0', '1', '0', '0'}),
                                                                 vector<char>({'0', '0', '0', '1', '1'})})) << endl;
  cout << "1 <=> " << calc_num_of_islands(vector<vector<char>>({ vector<char>({'1', '1', '1'}),
                                                                 vector<char>({'0', '1', '0'}),
                                                                 vector<char>({'0', '1', '0'})})) << endl;
  cout << "3 <=> " << calc_num_of_islands(vector<vector<char>>({ vector<char>({'1', '0', '0', '0', '0'}),
                                                                 vector<char>({'0', '1', '1', '0', '0'}),
                                                                 vector<char>({'0', '1', '1', '0', '0'}),
                                                                 vector<char>({'0', '0', '0', '1', '1'})})) << endl;

  cout << "15. calc_num_of_islands" << endl;
  cout << "[ 1 1 2 2 2 ] <=> " << endl;
  print_all_elem<int>(calc_num_of_islands_adp({ { 0, 0 }, { 0, 1 }, { 2, 2 }, { 2, 2 }, { 2, 1 } }, 3, 3));
  cout << "[ 1 2 3 4 5 6 7 8 9 10 11 11 12 12 12 12 12 13 13 14 15 16 17 18 18 18 19 19 18 17 16 16 16 16 16 16 17 17 16 16 17 18 18 18 17 17 17 ] <=> " << endl;
  print_all_elem<int>(calc_num_of_islands_adp({{0,9},{5,4},{0,12},{6,9},{6,5},{0,4},{4,11},{0,0},{3,5},{6,7},{3,12},{0,5},{6,13},{7,5},{3,6},{4,4},{0,8},{3,1},{4,6},{6,1},{5,12},{3,8},{7,0},{2,9},{1,4},{3,0},{1,13},{2,13},{6,0},{6,4},{0,13},{0,3},{7,4},{1,8},{5,5},{5,7},{5,10},{5,3},{6,10},{6,2},{3,10},{2,7},{1,12},{5,0},{4,5},{7,13},{3,2}}, 8, 14));

  return 0;
}
