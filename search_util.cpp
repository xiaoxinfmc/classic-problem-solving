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
   */
  static vector<string> find_shortest_ladder(const string & start_str,
                                             const string & target_str,
                                             const vector<string> & dict){
    vector<string> shortest_ladder;
    return shortest_ladder;
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
   */
  class elem {
  public:
    elem(int curr_levl,const string & val) : level(curr_levl), value(val){}
    virtual ~elem(){}
    int level; string value;
    friend ostream & operator<< (ostream & os, const elem & elem) {
      os << elem.value << ":" << elem.level; return os;
    }
  };
  class elem_hash_func {
  public:
    size_t operator() (const elem & elem) const {
      // return hash<string>() (to_string(elem.level) + elem.value);
      return hash<string>() (elem.value);
    }
  };
  class elem_comparator {
  public:
    bool operator() (const elem & l_elem, const elem & r_elem) const {
      // return (l_elem.level == r_elem.level && l_elem.value == r_elem.value);
      return (l_elem.value == r_elem.value);
    }
  };

  static vector<vector<string>> all_shortest_ladder(const string & start_str,
                                                    const string & target_str,
                                                    const vector<string>& dict){
    vector<vector<string>> ladder_vec_ret;
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
   */
  static vector<vector<int>> find_nsum(vector<int> values, int target){
    vector<vector<int>> nsum_ret;
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
  static vector<vector<int>> find_sum_paths(vector<int> values, int target){
    vector<vector<int>> paths_ret;
    return paths_ret;
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
  static bool is_word_existed(vector<string> board, string input) {
    bool is_word_existed = false;
    return is_word_existed;
  }

  /**
   * 11.1 calc_pow(x, n) = { calc_pow(x, 2/n)^2, n > 1 | x, n == 1 }
   */
  static int calc_pow(int base, int p) {
    return 0;
  }

  /**
   * 11.2 Sqrt(x)
   * Implement int sqrt(int x).
   * Compute and return the square root of x.
   */
  static double calc_sqrt(int base) {
    double sqrt_low = 0, sqrt_high = base;
    double sqrt_val = (sqrt_low + sqrt_high) / 2;
    double margin_of_error_limit = 0.0000001, margin_of_error = 0.0;
    return sqrt_val;
  }

  /**
   * 10.1 Palindrome Partitioning
   * Given a string s, partition s such that every substring of the partition is
   * a palindrome. Return all possible palindrome partitioning of s.
   * For example, given s = "aab", Return [ ["aa","b"], ["a","a","b"] ]
   * based on current index each step, either not partition or partition.
   */
  static vector<vector<string>> get_all_palindrome_partition(const string input) {
    vector<vector<string>> partition_set;
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
    return 0;
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
    return 0;
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
  static vector<string> restore_ips(const string ipstr) {
    vector<string> ip_arr;
    return ip_arr;
  }
};

int main(void) {
  using search_util::print_all_elem;
  using search_util::print_all_elem_vec;
  using search_util::find_shortest_ladder;
  using search_util::all_shortest_ladder;
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

  cout << "1. find_shortest_ladder" << endl;
  vector<string> d0({ "hot","dot","dog","lot","log" });
  cout << "-->> hit [ hot dot dog lot log ] cog" << endl;
  cout << "[ hit hot dot dog cog ] <=>" << endl;
  print_all_elem<string>(find_shortest_ladder("hit", "cog", d0));

  cout << "2. find_all_shortest_ladder" << endl;
  cout << "-->> hit [ hot dot dog lot log ] cog" << endl;
  cout << "[" << endl << "  [ hit hot dot dog cog ]" << endl
       << "  [ hit hot lot log cog ]" << endl << "] <=>" << endl;
  print_all_elem_vec<string>(all_shortest_ladder("hit", "cog", d0));

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
  cout << "[ 255.255.11.135, 255.255.111.35 ] <=>" << endl;
  print_all_elem<string>(restore_ips("25525511135"));

  return 0;
}
