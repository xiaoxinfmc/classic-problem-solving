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

class SearchUtil{
public:
  template <class Type>
  static void print_all_elem(const vector<Type>& input) {
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
    deque<string> search_buffer;
    vector<string> ladder_buffer;
    vector<string> shortest_ladder;
    unordered_set<string> existing_token_set(dict.begin(), dict.end());
    unordered_set<string> selected_token_set;
    unordered_map<string, string> path;

    if (existing_token_set.end() != existing_token_set.find(start_str) ||
        existing_token_set.end() != existing_token_set.find(target_str)) {
      return ladder_buffer;
    }
    existing_token_set.insert(start_str);
    existing_token_set.insert(target_str);
    search_buffer.push_back(start_str);
    selected_token_set.insert(start_str);
    while (false == search_buffer.empty()) {
      string curr_str = search_buffer.front();
      ladder_buffer.push_back(curr_str);
      for (auto & token : existing_token_set){
        if ((selected_token_set.end() == selected_token_set.find(token)) &&
            (is_diff_by_one_char_only(ladder_buffer.back(), token))) {
          search_buffer.push_back(token);
          selected_token_set.insert(token);
          path[token] = curr_str;
        }
      }
      if (target_str == curr_str) { break; }
      search_buffer.pop_front();
    }
    if (target_str != ladder_buffer.back()) { return shortest_ladder; }
    shortest_ladder.push_back(ladder_buffer.back());
    while (path.end() != path.find(shortest_ladder.back())) {
      shortest_ladder.push_back(path[shortest_ladder.back()]);
    }
    reverse(shortest_ladder.begin(), shortest_ladder.end());
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
  class Elem {
  public:
    Elem(int curr_levl,const string & val) : level(curr_levl), value(val){}
    virtual ~Elem(){}
    int level; string value;
    friend ostream & operator<< (ostream & os, const Elem & elem) {
      os << elem.value << ":" << elem.level; return os;
    }
  };
  class ElemHashFunc {
  public:
    size_t operator() (const Elem & elem) const {
      // return hash<string>() (to_string(elem.level) + elem.value);
      return hash<string>() (elem.value);
    }
  };
  class ElemComparator {
  public:
    bool operator() (const Elem & l_elem, const Elem & r_elem) const {
      // return (l_elem.level == r_elem.level && l_elem.value == r_elem.value);
      return (l_elem.value == r_elem.value);
    }
  };

  static unordered_map<string, unordered_set<string>>
    build_graph_off_by_one_char(const string & start_str,
                                const string & target_str,
                                const vector<string> & dict)
  {
    unordered_map<string, unordered_set<string>> graph_ret;
    vector<string> tokens_avail(dict.begin(), dict.end());
    tokens_avail.push_back(start_str); tokens_avail.push_back(target_str);
    for (int i = 0; i < tokens_avail.size(); i++) {
      for (int c = 0; c < tokens_avail.size(); c++) {
        if (true == is_diff_by_one_char_only(tokens_avail[i], tokens_avail[c])){
          if (graph_ret.end() == graph_ret.find(tokens_avail[i])) {
            graph_ret[tokens_avail[i]] = unordered_set<string>();
          }
          graph_ret[tokens_avail[i]].insert(tokens_avail[c]);
        }
      }
    }
    return graph_ret;
  }

  static vector<vector<string>> all_shortest_ladder(const string & start_str,
                                                    const string & target_str,
                                                    const vector<string>& dict){
    unordered_map<string, unordered_set<string>> graph_ret =
      build_graph_off_by_one_char(start_str, target_str, dict);

    vector<vector<string>> ladder_vec_ret;
    if (graph_ret.end() == graph_ret.find(start_str) ||
        graph_ret.end() == graph_ret.find(target_str)){ return ladder_vec_ret; }

    ladder_vec_ret = dfs_generate_path_via_subgraph(
      start_str, target_str, bfs_search_and_clip_subgraph(start_str, target_str, graph_ret)
    );

    return ladder_vec_ret;
  }

  static vector<vector<string>> dfs_generate_path_via_subgraph(
    const string & start_str, const string & target_str,
    const unordered_map<string, unordered_set<string>> & sub_dag)
  {
    vector<Elem> ladder_search_buffer;
    vector<Elem> curr_ladder_vect;
    vector<string> new_ladder_arr;
    unordered_map<int, vector<vector<string>>> curr_ladder_vect_map;
    unordered_map<string, unordered_set<string>>::const_iterator vertex_pair_itr;

    int min_path_len = INT_MAX;
    ladder_search_buffer.push_back(Elem(0, start_str));
    while (false == ladder_search_buffer.empty()) {
      Elem curr_elem = ladder_search_buffer.back();
      ladder_search_buffer.pop_back();
      curr_ladder_vect.push_back(curr_elem);
      if (curr_elem.value == target_str) {
        min_path_len = min((int)curr_ladder_vect.size(), min_path_len);
        if (curr_ladder_vect_map.find(curr_ladder_vect.size()) ==
            curr_ladder_vect_map.end()) {
          curr_ladder_vect_map[curr_ladder_vect.size()] = vector<vector<string>>();
        }
        new_ladder_arr.clear();
        for (auto & token : curr_ladder_vect) { new_ladder_arr.push_back(token.value); }
        curr_ladder_vect_map[curr_ladder_vect.size()].push_back(new_ladder_arr);
      }
      vertex_pair_itr = sub_dag.find(curr_elem.value);
      if (sub_dag.end() != vertex_pair_itr) {
        const unordered_set<string> & vertex_ref = vertex_pair_itr->second;
        for (unordered_set<string>::const_iterator neighbor_itr = vertex_ref.begin();
             neighbor_itr != vertex_ref.end(); neighbor_itr++) {
          ladder_search_buffer.push_back(Elem(curr_elem.level + 1, * neighbor_itr));
        }
      }
      if (false == ladder_search_buffer.empty()) {
        while (curr_ladder_vect.size() > ladder_search_buffer.back().level) {
          curr_ladder_vect.pop_back();
        }
      }
    }
    if (INT_MAX == min_path_len) { return vector<vector<string>>(); }
    return curr_ladder_vect_map[min_path_len];
  }

  static unordered_map<string, unordered_set<string>>
    bfs_search_and_clip_subgraph(const string & start_str,
                                 const string & target_str,
                                 const unordered_map<string,
                                       unordered_set<string>> & graph_input)
  {
    unordered_map<string, unordered_set<string>> sub_dag_ret;
    deque<Elem> ladder_search_buffer;
    unordered_set<Elem, ElemHashFunc, ElemComparator> selected_elem_set;
    unordered_set<Elem, ElemHashFunc, ElemComparator>::iterator elem_itr;
    unordered_map<string, unordered_set<string>>::const_iterator vertex_pair_itr;

    Elem curr_elem(0, "");
    ladder_search_buffer.push_back(Elem(0, start_str));
    selected_elem_set.insert(Elem(0, start_str));
    while (false == ladder_search_buffer.empty()) {
      curr_elem = ladder_search_buffer.front();
      if (curr_elem.value == target_str) { break; }
      vertex_pair_itr = graph_input.find(curr_elem.value);
      if (graph_input.end() != vertex_pair_itr) {
        const unordered_set<string> & vertex_ref = vertex_pair_itr->second;
        for (unordered_set<string>::const_iterator neighbor_itr = vertex_ref.begin();
             neighbor_itr != vertex_ref.end(); neighbor_itr++) {
          Elem new_elem(curr_elem.level + 1, * neighbor_itr);
          if (selected_elem_set.end() == selected_elem_set.find(new_elem)){
            ladder_search_buffer.push_back(new_elem);
            selected_elem_set.insert(new_elem);
          }
        }
      }
      ladder_search_buffer.pop_front();
    }
    if (curr_elem.value != target_str) { return sub_dag_ret; }
    for (auto & vertex_elem: selected_elem_set) {
      vertex_pair_itr = graph_input.find(vertex_elem.value);
      if (graph_input.end() == vertex_pair_itr){ continue; }
      const unordered_set<string> & vertex_ref = vertex_pair_itr->second;
      for (unordered_set<string>::const_iterator neighbor_itr = vertex_ref.begin();
           neighbor_itr != vertex_ref.end(); neighbor_itr++) {
        elem_itr = selected_elem_set.find(Elem(0, * neighbor_itr));
        /**
         * filter out any link point back or in the same level.
         *           1
         *          / \
         *         v   v
         *         2   3
         *          \ /
         *           v
         *           4
         */
        if (selected_elem_set.end() != elem_itr && vertex_elem.level < elem_itr->level) {
          if (sub_dag_ret.end() == sub_dag_ret.find(elem_itr->value)) {
            sub_dag_ret[vertex_elem.value] = unordered_set<string>();
          }
          sub_dag_ret[vertex_elem.value].insert(elem_itr->value);
        }
      }
    }
    return sub_dag_ret;
  }

  /**
   * 10.7 Combination Sum
   * Given a set of candidate numbers (C) and a target number (T), find all
   * unique combinations in C where the candidate numbers sums to T.
   * The same repeated number may be chosen from C unlimited number of times.
   * Note:
   * - All numbers (including target) will be positive integers.
   * - Elements in a combination(a1,a2,...,ak) must be in non-descending order.
   *   (ie,a1 ≤a2 ≤...≤ak).
   * - The solution set must not contain duplicate combinations.
   *   For example, given candidate set 2,3,6,7 and target 7, A solution set is:
   *   [7] [2, 2, 3]
   */
  static vector<vector<int>> find_nsum(vector<int> values, int target){
    sort(values.begin(), values.end());
    vector<vector<int>> nsum_ret;
    if (values.front() > target) { return nsum_ret; }
    // find_sum(0, target, 0, values, vector<int>(), nsum_ret);
    vector<int> path_buffer; find_sum_comb(0, target, 0, values, path_buffer, nsum_ret);
    return nsum_ret;
  }

  static void find_sum_comb(int start_pos, int target, int curr_sum,
                            vector<int> & values, vector<int> & curr_path,
                            vector<vector<int>> & paths_ret) {
print_all_elem<int>(curr_path);
    if (curr_sum > target || start_pos >= values.size()) { return; }
    if (curr_sum == target) { paths_ret.push_back(curr_path); return; }
    for (int i = start_pos; i < values.size(); i++) {
      curr_path.push_back(values[i]);
      find_sum_comb(
        i, target, curr_sum + values[i], values, curr_path, paths_ret
      );
      curr_path.pop_back();
    }
  }

  static void find_sum(int start_pos, int target, int curr_sum,
                       const vector<int> & values, vector<int> curr_sum_arr,
                       vector<vector<int>> & nsum_ret) {
print_all_elem<int>(curr_sum_arr);
    if (curr_sum > target) { return; }
    if (curr_sum == target) { nsum_ret.push_back(curr_sum_arr); return; }
    if (start_pos >= values.size()) { return; } else {
      // search a path when this num is skipped.
      find_sum(start_pos + 1, target, curr_sum, values, curr_sum_arr, nsum_ret);
      curr_sum_arr.push_back(values[start_pos]);
      // search a path when this num will be reused.
      find_sum(start_pos, target, curr_sum + values[start_pos], values, curr_sum_arr, nsum_ret);
      // search a path when this num will be used and we move to next .
      find_sum(start_pos + 1, target, curr_sum + values[start_pos], values, curr_sum_arr, nsum_ret);
    }
  }

  /**
   * 10.8 Combination Sum II
   * Given a collection of candidate numbers (C) and a target number (T), find
   * all unique combinations in C where the candidate numbers sums to T.
   * Each number in C may only be used once in the combination.
   * Note:
   * - All numbers(including target) will be positive integers.
   * - Elements in a combination(a1,a2,...,ak) must be in non-descending order.
   *   (ie,a1 >a2 >...>ak).
   * - The solution set must not contain duplicate combinations.
   * - For example, given candidate set 10,1,2,7,6,1,5 and target 8, A solution
   *   set is: [1, 7] [1, 2, 5] [2, 6] [1, 1, 6]
   */
  static vector<vector<int>> find_sum_paths(vector<int> values, int target){
    sort(values.begin(), values.end());
    vector<vector<int>> paths_ret;
    if (true == values.empty() || values.front() > target) { return paths_ret; }
    int start_pos = 0, curr_sum = 0; vector<int> path_buffer;
    gen_sum_paths(start_pos, curr_sum, target, values, path_buffer, paths_ret);
    return paths_ret;
  }

  static void gen_sum_paths(int start_pos, int curr_sum, int target,
                            vector<int> & values, vector<int> & curr_path,
                            vector<vector<int>> & paths_ret) {
print_all_elem<int>(curr_path);
    if (start_pos >= values.size() || curr_sum > target) { return; }
    if (curr_sum == target) { paths_ret.push_back(curr_path); return; }
    int prev_num = INT_MAX;
    for (int i = start_pos; i < values.size(); i++) {
      if (values[i] == prev_num){ continue; }
      curr_path.push_back(values[i]);
      gen_sum_paths(
        i + 1, curr_sum + curr_path.back(), target, values, curr_path, paths_ret
      );
      curr_path.pop_back();
      prev_num = values[i];
    }
  }

  static bool is_diff_by_one_char_only(const string & src,const string & cmp){
    if (src.size() != cmp.size()) { return false; }
    int diff_char_cnt = 0;
    for (int i = 0; i < src.size(); i++){ if(src[i]!=cmp[i]){diff_char_cnt++;} }
    return (1 == diff_char_cnt);
  }

  /**
   * 10.9 Generate Parentheses
   * Given n pairs of parentheses, write a function to generate all combinations
   * of well-formed parentheses. For example, given n = 3, a solution set is:
   * "((()))", "(()())", "(())()", "()(())", "()()()"
   * Well-Formed Parentheses:
   * - # of ( & # of ) has to match, always start with ( and enclose with )
   * - at any given position of existing prefixes, # of ( always >= # of )
   */
  static vector<vector<char>> gen_parentheses(int pair_cnt){
    vector<vector<char>> pair_comb_ret; vector<char> curr_pair;
    if (pair_cnt <= 0) { return pair_comb_ret; }
    gen_pair_recur(pair_cnt, 0, 0, curr_pair, pair_comb_ret);
    return pair_comb_ret;
  }

  static void gen_pair_recur(int pair_cnt, int left_cnt, int right_cnt,
                             vector<char> & curr_pair,
                             vector<vector<char>> & pair_comb_ret) {
    if (left_cnt > pair_cnt || right_cnt > pair_cnt ||
        left_cnt < right_cnt) { return; }
    if (false == curr_pair.empty() && curr_pair.front() == ')') { return; }
    if ((left_cnt == right_cnt) && (left_cnt == pair_cnt) &&
        (curr_pair.front() == '(') && (curr_pair.back() == ')')) {
      pair_comb_ret.push_back(curr_pair); return;
    }
print_all_elem<char>(curr_pair);
    curr_pair.push_back('(');
    gen_pair_recur(pair_cnt, left_cnt + 1, right_cnt, curr_pair, pair_comb_ret);
    curr_pair.pop_back();
    curr_pair.push_back(')');
    gen_pair_recur(pair_cnt, left_cnt, right_cnt + 1, curr_pair, pair_comb_ret);
    curr_pair.pop_back();
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
    if (input.size() > board.size() * board.front().size()) {
      return is_word_existed;
    }
    unordered_set<string> invalid_path_set, chosen_coord_set;
    is_word_existed = check_word(
      board, input, 0, 0, 0, invalid_path_set, chosen_coord_set
    );
    return is_word_existed;
  }

  static bool check_word(vector<string> & board, string & input,
                         int board_row, int board_col, int check_pos,
                         unordered_set<string> & invalid_path_set,
                         unordered_set<string> & chosen_coord_set) {
    // reaching here means all char from input been matched
    if (check_pos >= input.size()){ return true; }
    if (board_row < 0 || board_row >= board.size() ||
        board_col < 0 || board_col >= board.back().size()){ return false; }
    bool is_word_matched = false;
cout << "input: " << input << " curr-pos: " << check_pos << endl;
    for (int r = board_row; r < board.size(); r++) {
      for (int c = board_col; c < board[r].size(); c++) {
        string coord_signature = to_string(r) + string("$") + to_string(c);
        string check_signature = coord_signature + string("$") + to_string(check_pos);
        if (chosen_coord_set.end() == chosen_coord_set.find(coord_signature) &&
            invalid_path_set.end() == invalid_path_set.find(check_signature)) {
          if (board[r][c] == input[check_pos]) {
            chosen_coord_set.insert(coord_signature);
cout << "input: " << input << " board: " << board[r][c] << " char: " << input[check_pos] << " key: " << check_signature << endl;
            is_word_matched = (
              check_word(board, input, r, c + 1, check_pos + 1, invalid_path_set, chosen_coord_set) ||
              check_word(board, input, r, c - 1, check_pos + 1, invalid_path_set, chosen_coord_set) ||
              check_word(board, input, r + 1, c, check_pos + 1, invalid_path_set, chosen_coord_set)
            );
            if (true == is_word_matched) { return is_word_matched; }
            chosen_coord_set.erase(coord_signature);
          }
        }
        invalid_path_set.insert(check_signature);
        if (false == chosen_coord_set.empty()) { return is_word_matched; }
      }
    }
    return is_word_matched;
  }

  /**
   * 11.1 calc_pow(x, n) = { calc_pow(x, 2/n)^2, n > 1 | x, n == 1 }
   */
  static int calc_pow(int base, int p) {
    if (0 == p) { return 1; }
    if (1 == p) { return base; }
    if (0 == p % 2){
      return (pow(calc_pow(base, p / 2), 2));
    } else {
      return base * (pow((calc_pow(base, (p - 1) / 2)), 2));
    }
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
    do {
      sqrt_val = (sqrt_low + sqrt_high) / 2.0;
      margin_of_error = (sqrt_val * sqrt_val - base);
      if (margin_of_error > 0) {
        sqrt_high = sqrt_val;
      } else {
        sqrt_low = sqrt_val;
      }
    } while (abs(margin_of_error) > margin_of_error_limit);
    return sqrt_val;
  }

  /**
   * 10.1 Palindrome Partitioning
   * Given a string s, partition s such that every substring of the partition is
   * a palindrome. Return all possible palindrome partitioning of s.
   * For example, given s = "aab", Return [ ["aa","b"], ["a","a","b"] ]
   * based on current index each step, either not partition or partition.
   */
  static vector<vector<string>> gen_palin_pars(string input) {
    vector<vector<string>> pairs_ret; vector<string> curr_pair;
    if (true == input.empty()) { return pairs_ret; }
    int prefix_len = 0, curr_pos = 0, total_token_len = 0;
    search_all_palin_pairs(input, prefix_len, curr_pos, total_token_len, curr_pair, pairs_ret);
    // unordered_set<string> invalid_break_set; _search_all_palin_pairs(input, prefix_len, curr_pos, total_token_len, invalid_break_set, curr_pair, pairs_ret);
    return pairs_ret;
  }

  static vector<vector<bool>> find_all_valid_palin(const string & input) {
cout << input << endl;
    vector<vector<bool>> lookup(input.size(), vector<bool>(input.size(),false));
    for (int i = 0; i < input.size(); i++){ lookup[i][i] = true; }
    for (int i = 0; i < input.size(); i++) {
      // 0 ... c ... i ... n, know if any pair in 0 ... i - 1 is a palin
      for (int c = i; c >= 0; c--) {
        if (i - c <= 1){ lookup[c][i] = (input[c] == input[i]); }
        else {
          lookup[c][i] = ((input[c] == input[i]) && (lookup[c + 1][i - 1]));
        }
      }
    }
// for (int i = 0; i < input.size(); i++) { for (int c = i; c >= 0; c--) { cout << c << " : " << i << " : " << input.substr(c, (i - c + 1)) << " : " << lookup[c][i] << endl; } }
    return lookup;
  }

  /*
0 : 0 : a : 1
0 : 1 : aa : 1
0 : 4 : aacaa : 1
1 : 1 : a : 1
1 : 3 : aca : 1
2 : 2 : c : 1
3 : 3 : a : 1
3 : 4 : aa : 1
4 : 4 : a : 1

[ a aa aacaa ]
<aacaa> [ a aa ]
aa [ a c ]
aa c [ a a aa ]
<aa c aa> [ a a ] pop_back until curr len is 1 less of the last in buffer
aa c a [ a a ]
<aa c a a> [ a ]
a [a aca ]
  */
  static vector<vector<string>> search_paths(const string & input,
                                             const vector<vector<bool>> lookup){
    vector<vector<string>> palin_pair_path;
    vector<string> palin_buffer, curr_path;
    vector<int> start_posbuf;
    int curr_path_len = 0;
    for (int i = 0; i < input.size(); i++) {
      if (true == lookup[0][i]){
        palin_buffer.push_back(input.substr(0, i + 1));
        start_posbuf.push_back(0);
      }
    }
    while (false == palin_buffer.empty()) {
      curr_path.push_back(palin_buffer.back());
      curr_path_len += palin_buffer.back().size();
      palin_buffer.pop_back(); start_posbuf.pop_back();
      for (int i = curr_path_len; i < input.size(); i++) {
        if (false == lookup[curr_path_len][i]) { continue; }
        palin_buffer.push_back(input.substr(curr_path_len, i-curr_path_len+1));
        start_posbuf.push_back(curr_path_len);
      }
      if (curr_path_len == input.size()){
// print_all_elem<string>(curr_path);
        palin_pair_path.push_back(curr_path);
        if (false == start_posbuf.empty()) {
          while (curr_path_len > start_posbuf.back()) {
            curr_path_len -= curr_path.back().size();
            curr_path.pop_back();
          }
        }
      }
    }
    return palin_pair_path;
  }

  // a ? a ? b ? b ? a ? a
  static void _search_all_palin_pairs(const string & input, int prefix_len,
                                      int curr_pos, int total_token_len,
                                      vector<string> & curr_pair,
                                      vector<vector<string>> & pairs_ret) {
    if (curr_pos >= input.size()) {
      if (total_token_len == input.size()) { pairs_ret.push_back(curr_pair); }
    } else {
      // a b c d e f
      //     ^ (prefix_len(ab) => 2, curr_pos(c) => 2)
      // - put a bar to partition
      string token = input.substr(curr_pos - prefix_len, prefix_len + 1);
      if (true == is_palindrome(token)) {
        curr_pair.push_back(token);
        _search_all_palin_pairs(input, 0, curr_pos + 1,
                                total_token_len + prefix_len + 1,
                                curr_pair, pairs_ret);
        curr_pair.pop_back();
      }
      // - not put a bar to partition
      _search_all_palin_pairs(input, prefix_len + 1, curr_pos + 1,
                              total_token_len, curr_pair, pairs_ret);
    }
  }

  static void search_all_palin_pairs(const string & input, int prefix_len,
                                 int curr_pos, int total_token_len,
                                 vector<string> & curr_pair,
                                 vector<vector<string>> & pairs_ret) {
    vector<vector<bool>> lookup = find_all_valid_palin(input);
    search_palin_paths(
      input, prefix_len, curr_pos, total_token_len, lookup, curr_pair, pairs_ret
    );
  }

  static void search_palin_paths(const string & input, int prefix_len,
                                 int curr_pos, int total_token_len,
                                 const vector<vector<bool>> & lookup,
                                 vector<string> & curr_pair,
                                 vector<vector<string>> & pairs_ret) {
    if (curr_pos >= input.size()) {
      if (total_token_len == input.size()) { pairs_ret.push_back(curr_pair); }
    } else {
      // a b c d e f
      //     ^ (prefix_len(ab) => 2, curr_pos(c) => 2)
      // - put a bar to partition
      string token = input.substr(curr_pos - prefix_len, prefix_len + 1);
      if (true == lookup[curr_pos - prefix_len][curr_pos]) {
        curr_pair.push_back(token);
        search_palin_paths(input, 0, curr_pos + 1,
                           total_token_len + prefix_len + 1,
                           lookup, curr_pair, pairs_ret);
        curr_pair.pop_back();
      }
      // - not put a bar to partition
      search_palin_paths(input, prefix_len + 1, curr_pos + 1,
                         total_token_len, lookup, curr_pair, pairs_ret);
    }
  }

  // a ? a ? b ? b ? a ? a
  static bool _search_all_palin_pairs(const string & input, int prefix_len,
                                      int curr_pos, int total_token_len,
                                      unordered_set<string> & invalid_break_set,
                                      vector<string> & curr_pair,
                                      vector<vector<string>> & pairs_ret) {
    bool is_breakdown_good = false;
    string curr_break_signature = to_string(prefix_len) + "$" + to_string(curr_pos);
    if (invalid_break_set.end() == invalid_break_set.find(curr_break_signature)) {

    if (curr_pos >= input.size()) {
      if (total_token_len == input.size()) {
        pairs_ret.push_back(curr_pair); is_breakdown_good = true;
      }
    } else {
      string new_signature;
      // a b c d e f
      //     ^ (prefix_len(ab) => 2, curr_pos(c) => 2)
      // - put a bar to partition
      bool is_break_good = false;
      string token = input.substr(curr_pos - prefix_len, prefix_len + 1);
      if (true == is_palindrome(token)) {
        curr_pair.push_back(token);
        new_signature = to_string(0) + "$" + to_string(curr_pos + 1);
        if (invalid_break_set.end() == invalid_break_set.find(new_signature)) {
        is_break_good = _search_all_palin_pairs(
          input, 0, curr_pos + 1, total_token_len + prefix_len + 1,
          invalid_break_set, curr_pair, pairs_ret
        );
        if (false == is_break_good) { invalid_break_set.insert(new_signature); }
        }
        curr_pair.pop_back();
      }
      bool is_non_break_good = false;
      // - not put a bar to partition
      new_signature = to_string(prefix_len + 1) + "$" + to_string(curr_pos + 1);
      if (invalid_break_set.end() == invalid_break_set.find(new_signature)) {
      is_non_break_good = _search_all_palin_pairs(
        input, prefix_len + 1, curr_pos + 1, total_token_len,
        invalid_break_set, curr_pair, pairs_ret
      );
      if (false == is_non_break_good) { invalid_break_set.insert(new_signature); }
      }
      is_breakdown_good = (is_break_good || is_non_break_good);
      if (false == is_breakdown_good) { invalid_break_set.insert(curr_break_signature); }
    }

    }
    return is_breakdown_good;
  }

  // a b b a, size => 4, ls => 4/2 - 1, rs => 4/2
  // a c a, size => 3, ls => 3/2 - 1, rs => 3/2 + 1
  static bool is_palindrome(const string & token) {
    if (true == token.empty() || 1 == token.size()) { return true; }
    int right_span = token.size() / 2, left_span = right_span - 1;
    if (0 != token.size() % 2) { right_span += 1; }
    while (right_span < token.size() && left_span >= 0) {
      if (token[left_span] != token[right_span]) { return false; }
      left_span--; right_span++;
    }
    return true;
  }

  static vector<vector<string>> partition(string s) {
    int n = s.size();
    /*
    bool p[n][n]; // whether s[i,j] is palindrome
    fill_n(&p[0][0], n * n, false);
    for (int i = n - 1; i >= 0; --i)
      for (int j = i; j < n; ++j)
        p[i][j] = s[i] == s[j] && ((j - i < 2) || p[i + 1][j - 1]);
    */
    vector<vector<bool>> p = find_all_valid_palin(s);

    vector<vector<string>> sub_palins(n, vector<string>());
    for (int i = n - 1; i >= 0; --i) {
      for (int j = i; j < n; ++j) {
        if (p[i][j]) {
          string palindrome = s.substr(i, j - i + 1);
          if (j + 1 < n) {
            for (auto v : sub_palins[j + 1]) {
              //v.insert(v.begin(), palindrome);
              v.insert(0, palindrome);
              sub_palins[i].push_back(v);
            }
          } else {
            // sub_palins[i].push_back(vector<string>{ palindrome });
            sub_palins[i].push_back(palindrome);
          }
        }
      }
    }
    return sub_palins;
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
    vector<vector<int>> path_cnt_buf(row, vector<int>(col, 0));
    for (int r = row - 1; r >= 0; r--) {
      for (int c = col - 1; c >= 0; c--) {
        if ((row - 1 == r) && (col - 1 == c)){ path_cnt_buf[r][c] = 0; }
        else if ((row - 1 == r) || (col - 1 == c)) { path_cnt_buf[r][c] = 1; }
        else {
          path_cnt_buf[r][c] = path_cnt_buf[r + 1][c] + path_cnt_buf[r][c + 1];
        }
      }
    }
    return path_cnt_buf[0][0];
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
  static int calc_unique_paths(vector<vector<int>> input) {
    int row = input.size(); int col = input.back().size();
    vector<vector<int>> path_cnt_buf(row, vector<int>(col, 0));
    if (1 == input[0][0]) { return 0; }
    for (int r = row - 1; r >= 0; r--) {
      for (int c = col - 1; c >= 0; c--) {
        if ((row - 1 == r) && (col - 1 == c)) {
          path_cnt_buf[r][c] = 0;
        } else if ((row - 1 == r) || (col - 1 == c)) {
          if (row - 1 == r) {
            path_cnt_buf[r][c] = (input[r][c + 1] == 0) ? 1 : 0;
          } else if (col - 1 == c) {
            path_cnt_buf[r][c] = (input[r + 1][c] == 0) ? 1 : 0;
          }
        } else {
          if (input[r + 1][c] == 0) { path_cnt_buf[r][c] += path_cnt_buf[r + 1][c]; }
          if (input[r][c + 1] == 0) { path_cnt_buf[r][c] += path_cnt_buf[r][c + 1]; }
        }
      }
    }
    return path_cnt_buf[0][0];
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
diag-up:

(0, 0)
(0, 1), (1, 0)
(0, 2), (1, 1), (2, 0)
(0, 3), (1, 2), (2, 1), (3, 0)
(0, 4), (1, 3), (2, 2), (3, 1), (4, 0)
(0, 5), (1, 4), (2, 3), (3, 2), (4, 1), (5, 0)

(5, 0), (4, 1), (3, 2), (2, 3), (1, 4), (0, 5)
(5, 1), (4, 2), (3, 3), (2, 4), (1, 5)
(5, 2), (4, 3), (3, 4), (2, 5)
(5, 3), (4, 4), (3, 5)
(5, 4), (4, 5)
(5, 5)


diag-down:

(0, 5)
(0, 4), (1, 5)
(0, 3), (1, 4), (2, 5)
(0, 2), (1, 3), (2, 4), (3, 5)
(0, 1), (1, 2), (2, 3), (3, 4), (4, 5)
(0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5)

(0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5)
(1, 0), (2, 1), (3, 2), (4, 3), (5, 4)
(2, 0), (3, 1), (4, 2), (5, 3)
(3, 0), (4, 1), (5, 2)
(4, 0), (5, 1)
(5, 0)

[ . . . x x .
  . . x x . .
  . x x . . .
  x x . . . .
  x . . . . .
  Q . . . . . ]
cout << "queuens-left: " << queuens_left << " avail-pos: ";
cout << "[ "; for (auto & x : avail_pos) { cout << x << " "; } cout << "]" << endl;
print_board(curr_board);
cout << "==>> verified-board:" << endl; print_board(curr_board);
else { cout << "<<== invalids-board:" << endl; print_board(curr_board); }
cout << "== [ "; for (auto & x : avail_pos) { cout << x << " "; } cout << "]" << endl;
   */
  static vector<vector<string>> search_n_queuens(int n) {
    vector<vector<string>> boards_ret;
    vector<string> curr_board(n, string(n, '.'));
    list<int> avail_pos; for (int i = 0; i < n; i++){ avail_pos.push_back(i); }
    search_queuen_placement(avail_pos, n, curr_board, boards_ret);
    return boards_ret;
  }

  static void search_queuen_placement(list<int> avail_pos, int queuens_left,
                                      vector<string> & curr_board,
                                      vector<vector<string>> & boards_ret) {
    if (true == avail_pos.empty() && 0 == queuens_left){
      boards_ret.push_back(curr_board); return;
    }
    int curr_pos_val = 0;
    for (list<int>::iterator curr_pos_itr  = avail_pos.begin();
                             curr_pos_itr != avail_pos.end(); curr_pos_itr++) {
      curr_pos_val = * curr_pos_itr;
      curr_pos_itr = avail_pos.erase(curr_pos_itr);
      curr_board[queuens_left - 1][curr_pos_val] = 'Q';
      if (true == is_curr_placement_valid(curr_board)) {
        search_queuen_placement(avail_pos, queuens_left - 1, curr_board, boards_ret);
      }
      curr_board[queuens_left - 1][curr_pos_val] = '.';
      curr_pos_itr = avail_pos.insert(curr_pos_itr, curr_pos_val);
    }
  }

  static vector<vector<string>> search_queuens(int n) {
    vector<vector<string>> boards_ret;
    vector<string> curr_board(n, string(n, '.'));
    list<int> avail_pos; for (int i = 0; i < n; i++){ avail_pos.push_back(i); }
    vector<int> row_arr, col_arr;
    search_queuen_place(avail_pos, n, row_arr, col_arr, curr_board, boards_ret);
    return boards_ret;
  }

  static void search_queuen_place(list<int> avail_pos, int queuens_left,
                                  vector<int> & row_arr, vector<int> & col_arr,
                                  vector<string> & curr_board,
                                  vector<vector<string>> & boards_ret) {
    if (true == avail_pos.empty() && 0 == queuens_left){
      boards_ret.push_back(curr_board); return;
    }
    int curr_pos_val = 0;
    for (list<int>::iterator curr_pos_itr  = avail_pos.begin();
                             curr_pos_itr != avail_pos.end(); curr_pos_itr++) {
      curr_pos_val = * curr_pos_itr;
      curr_pos_itr = avail_pos.erase(curr_pos_itr);
      if (true == is_placement_valid(queuens_left - 1, curr_pos_val, row_arr, col_arr)) {
        curr_board[queuens_left - 1][curr_pos_val] = 'Q';
        row_arr.push_back(queuens_left - 1); col_arr.push_back(curr_pos_val);
        search_queuen_place(avail_pos, queuens_left - 1, row_arr, col_arr, curr_board, boards_ret);
        row_arr.pop_back(); col_arr.pop_back();
      }
      curr_board[queuens_left - 1][curr_pos_val] = '.';
      curr_pos_itr = avail_pos.insert(curr_pos_itr, curr_pos_val);
    }
  }

  static bool is_placement_valid(int row, int col, const vector<int> & row_arr,
                                                   const vector<int> & col_arr) {
    for (int i = 0; i < col_arr.size(); i++) {
      if ((row_arr[i] == row) || (col_arr[i] == col) ||
          (abs(row_arr[i] - row) == abs(col_arr[i] - col))) { return false; }
    }
    return true;
  }

  static bool is_curr_placement_valid(const vector<string> & curr_board) {
    int board_size = curr_board.size();
    int row_queuen_cnt = 0, col_queuen_cnt = 0;
    for (int i = 0; i < board_size; i++) {
      row_queuen_cnt = 0; col_queuen_cnt = 0;
      for (int c = 0; c < board_size; c++) {
        if ('Q' == curr_board[i][c]) { row_queuen_cnt += 1; }
        if (row_queuen_cnt > 1) { return false; }
        if ('Q' == curr_board[c][i]) { col_queuen_cnt += 1; }
        if (col_queuen_cnt > 1) { return false; }
      }
    }
    int diag_quenen_cnt = 0;
    for (int i = 0; i < board_size; i++) {
      // diag_bottom_left_and_top_right
      diag_quenen_cnt = 0;
      for (int c = 0; c <= i; c++) {
        if ('Q' == curr_board[c][i - c]) { diag_quenen_cnt++; }
        if (diag_quenen_cnt > 1) { return false; }
      }
      // diag_bottom_left_and_top_right
      diag_quenen_cnt = 0;
      for (int c = 0; c <= i; c++) { // i -> 1 | c -> 0, 1 <5, 4> <4, 5>
        if ('Q' == curr_board[board_size - 1 - c][board_size - 1 - i + c]) {
          diag_quenen_cnt++;
        }
        if (diag_quenen_cnt > 1) { return false; }
      }
      // diag_bottom_right_and_top_left
      diag_quenen_cnt = 0;
      for (int c = 0; c <= i; c++) {
        if ('Q' == curr_board[c][board_size - 1 - i + c]) { diag_quenen_cnt++; }
        if (diag_quenen_cnt > 1) { return false; }
      }
      // diag_bottom_right_and_top_left
      diag_quenen_cnt = 0;
      for (int c = 0; c <= i; c++) {
        if ('Q' == curr_board[board_size - 1 - i + c][c]) { diag_quenen_cnt++; }
        if (diag_quenen_cnt > 1) { return false; }
      }
    }
    return true;
  }

  static void print_board_vec(const vector<vector<string>> & boards_ret) {
    for (const vector<string> & curr_board: boards_ret) { print_board(curr_board); }
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

  /**
   * 10.6 Restore IP Addresses
   * Given a string containing only digits, restore it by returning all possible
   * valid IP address combinations. For example: Given "25525511135",
   * return ["255.255.11.135", "255.255.111.35"]. (Order does not matter)
   */
  static vector<string> restore_ips(const string ipstr) {
    vector<string> ip_tokens_set, curr_ip_tokens;
    search_valid_ips(ipstr, 0, curr_ip_tokens, ip_tokens_set);
    return ip_tokens_set;
  }

  static void search_valid_ips(const string & ipstr, int curr_pos,
                               vector<string> & curr_ip_tokens,
                               vector<string> & ip_tokens_set) {
    const static int IP_TOKEN_LEN_LIMIT = 3, IP_TOKEN_CNT = 4;
    if ((curr_pos >= ipstr.size()) && (IP_TOKEN_CNT == curr_ip_tokens.size())){
      string valid_ip = "";
      for (auto token : curr_ip_tokens) { valid_ip += (token + "."); }
      ip_tokens_set.push_back(valid_ip.substr(0, valid_ip.size() - 1));
    } else {
      for (int i = 0; i < IP_TOKEN_LEN_LIMIT; i++) {
        if (false == verify_ip_token(ipstr, curr_pos, curr_pos + i)){ break; }
        curr_ip_tokens.push_back(ipstr.substr(curr_pos, i + 1));
        search_valid_ips(ipstr, curr_pos + i + 1, curr_ip_tokens,ip_tokens_set);
        curr_ip_tokens.pop_back();
      }
    }
  }

  static bool verify_ip_token(const string & ipstr, int start_pos, int end_pos){
    bool is_token_valid = false;
    int token_len = end_pos - start_pos + 1;

    if (end_pos > ipstr.size() - 1) { return is_token_valid; }
    if (!((token_len >= 1) && (token_len <= 3))) { return is_token_valid; }
    for (int i = start_pos; i <= end_pos; i++) {
      if (!(ipstr[i] >= '0' && ipstr[i] <= '9')) { return is_token_valid; }
    }
    switch(ipstr[start_pos]) {
      case '0' : {
        if (1 == token_len){ is_token_valid = true; } break;
      }
      case '1' : {
        is_token_valid = true; break;
      }
      case '2' : {
        if (1 == token_len){ is_token_valid = true; }
        if (2 <= token_len){ is_token_valid = (ipstr[start_pos + 1] <= '5'); }
        if (3 <= token_len){ is_token_valid = ((is_token_valid) &&
                                               (ipstr[start_pos + 2] <= '5')); }
        break;
      }
      default : {
        is_token_valid = (token_len < 3) ? true : false;
        break;
      }
    }
    return is_token_valid;
  }
};

int main(void) {

  // cout << SearchUtil::partition("aacaaaacaaaacaaaacaaaacaaaacaa").size() << endl; return 0;
  // cout << SearchUtil::gen_palin_pars("aacaaaacaaaacaaaacaaaacaaaacaa").size() << endl; return 0;
  // cout << SearchUtil::search_paths("aacaaaacaaaacaaaacaaaacaaaacaa", SearchUtil::find_all_valid_palin("aacaaaacaaaacaaaacaaaacaaaacaa")).size() << endl; return 0;

  // cout << SearchUtil::search_n_queuens(10).size() << endl; return 0;

  vector<string> d0({ "hot","dot","dog","lot","log" });

  cout << "==>> find_shortest_ladder" << endl;
  cout << "-->> hit [ hot dot dog lot log ] cog" << endl;
  cout << "[ hit hot dot dog cog ]" << endl;
  SearchUtil::print_all_elem<string>(
    SearchUtil::find_shortest_ladder("hit", "cog", d0)
  );
  cout << "==>> find_all_shortest_ladder" << endl;
  cout << "-->> hit [ hot dot dog lot log ] cog" << endl;
  cout << "[" << endl << "  [ hit hot dot dog cog ]" << endl
       << "  [ hit hot lot log cog ]" << endl << "]" << endl;
  SearchUtil::print_all_elem_vec<string>(
    SearchUtil::all_shortest_ladder("hit", "cog", d0)
  );
  cout << "==>> find_nsum" << endl;
  SearchUtil::print_all_elem_vec<int>(
    SearchUtil::find_nsum(vector<int>({ 2, 3, 6, 7 }), 7)
  );
  cout << "==>> find_sum_paths" << endl;
  SearchUtil::print_all_elem_vec<int>(
    SearchUtil::find_sum_paths(vector<int>({ 10, 1, 2, 7, 6, 1, 5 }), 8)
  );
  cout << "==>> find_pair_comb" << endl;
  SearchUtil::print_all_elem_vec<char>(SearchUtil::gen_parentheses(3));
  /* For example, Given board = [ ["ABCE"],
   *                              ["SFCS"],
   *                              ["ADEE"] ]
   * word = "ABCCED", -> returns true, word = "SEE", -> returns true,
   * word = "ABCB", -> returns false.
   */
  cout << "==>> is_word_existed" << endl;
  cout << SearchUtil::is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "ABCCED") << endl;
  cout << SearchUtil::is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "SEE") << endl;
  cout << SearchUtil::is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "ABCB") << endl;
  cout << SearchUtil::is_word_existed(vector<string>({ "ABCE", "SFCS", "ADEE" }), "ABCD") << endl;
  cout << SearchUtil::is_word_existed(vector<string>({ "ABCE", "SFCS", "BFE" }), "ABCD") << endl;

  cout << "==>> calc_pow" << endl;
  cout << "2 ^ 0: " << SearchUtil::calc_pow(2, 0) << endl;
  cout << "2 ^ 1: " << SearchUtil::calc_pow(2, 1) << endl;
  cout << "2 ^ 2: " << SearchUtil::calc_pow(2, 2) << endl;
  cout << "2 ^ 3: " << SearchUtil::calc_pow(2, 3) << endl;
  cout << "2 ^ 4: " << SearchUtil::calc_pow(2, 4) << endl;

  cout << "==>> calc_sqrt" << endl;
  cout << "0: " << SearchUtil::calc_sqrt(0) << endl;
  cout << "1: " << SearchUtil::calc_sqrt(1) << endl;
  cout << "2: " << SearchUtil::calc_sqrt(2) << endl;
  cout << "3: " << SearchUtil::calc_sqrt(3) << endl;
  cout << "4: " << SearchUtil::calc_sqrt(4) << endl;

  /**
   * For example, given s = "aab", Return [ ["aa","b"], ["a","a","b"] ]
   * based on current index each step, either not partition or partition.
   */
  cout << "==>> gen_palin_pars" << endl;
  cout << "aab | [ [ aa, b ], [ a, a, b ] ]" << endl;
  SearchUtil::print_all_elem_vec<string>(SearchUtil::gen_palin_pars("aab"));
  cout << "a | [ [ a ] ]" << endl;
  SearchUtil::print_all_elem_vec<string>(SearchUtil::gen_palin_pars("a"));
  cout << "aaa | [ [ a, a, a ], [ aa, a ], [a, aa ], [ aaa ] ]" << endl;
  SearchUtil::print_all_elem_vec<string>(SearchUtil::gen_palin_pars("aaa"));
  cout << "aca | [ [ a, c, a ], [ aca ] ]" << endl;
  SearchUtil::print_all_elem_vec<string>(SearchUtil::gen_palin_pars("aca"));
  cout << "aacaa | [ [a, a, c, a, a ], [ a, aca, a ], [ a, a, c, aa ], [aa, c, a, a ], [ aa, c, aa ], [ aacaa ] ]" << endl;
  SearchUtil::print_all_elem_vec<string>(SearchUtil::gen_palin_pars("aacaa"));

  SearchUtil::find_all_valid_palin("aacaa");
  SearchUtil::print_all_elem_vec<string>(
    SearchUtil::search_paths("aacaa", SearchUtil::find_all_valid_palin("aacaa"))
  );

  vector<vector<int>> path_input({ vector<int>({ 0, 0, 0 }), vector<int>({ 0, 1, 0 }), vector<int>({ 0, 0, 0 }) });

  SearchUtil::print_all_elem_vec<int>(path_input);
  cout << SearchUtil::calc_unique_paths(path_input) << endl;

  cout << 3 << " : " << 7 << " : " << SearchUtil::calc_unique_paths(3, 7) << endl;

  SearchUtil::print_board_vec(SearchUtil::search_queuens(4));

  SearchUtil::print_all_elem<string>(SearchUtil::restore_ips("25525511135"));

  return 0;
}
