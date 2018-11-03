#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <unordered_map>
#include <cctype>
#include <list>
#include <cmath>
#include <set>

namespace string_util {
  using std::cout;
  using std::endl;
  using std::string;
  using std::vector;
  using std::unordered_set;
  using std::set;
  using std::list;
  using std::min;
  using std::max;
  using std::unordered_map;
  using std::pair;

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
   * 212. Word Search II
   * Given a 2D board and words from the dictionary, find all words in the board.
   * Each word must be constructed from letters of sequentially adjacent cell,
   * where "adjacent" cells are those horizontally or vertically neighboring. The
   * same letter cell may not be used more than once in a word.
   * Example:
   * Input:
   * words = ["oath","pea","eat","rain"] and board =
   * [
   *   ['o','a','a','n'],
   *   ['e','t','a','e'],
   *   ['i','h','k','r'],
   *   ['i','f','l','v']
   * ]
   * Output: ["eat","oath"]
   */
  class prefix_trie_node {
  public:
    prefix_trie_node(char val) : char_val(val) {
      parent_ptr = NULL;
      for (int i = 0; i < sizeof(adjacent_ptrs)/sizeof(adjacent_ptrs[0]); i++) {
        adjacent_ptrs[i] = NULL;
      }
      is_visited_flag = false;
      is_ending_char = false;
    }

    virtual ~prefix_trie_node() {}

    void insert_adjacent_char(prefix_trie_node * char_ptr) {
      adjacent_ptrs[int(char_ptr->get_char_val()) - int('a')] = char_ptr;
      char_ptr->parent_ptr = this;
    }

    bool is_adjacent_char_exists(char curr_char) {
      return (NULL != adjacent_ptrs[int(curr_char) - int('a')]);
    }

    prefix_trie_node * get_parent_ptr() { return parent_ptr; }

    prefix_trie_node * get_adjacent_char_ptr(char curr_char) {
      return adjacent_ptrs[int(curr_char) - int('a')];
    }

    char get_char_val() { return char_val; }

    bool get_is_ending_char() { return is_ending_char; }
    void mark_as_ending_char() { is_ending_char = true; }

    bool is_visited() { return is_visited_flag; }
    void mark_as_visited() { is_visited_flag = true; }

  private:
    char char_val;
    bool is_visited_flag;
    bool is_ending_char;
    prefix_trie_node * parent_ptr;
    prefix_trie_node * adjacent_ptrs[int('z') - int('a') + 1];
  };

  static prefix_trie_node * build_prefix_trie(const vector<string> & words) {
    prefix_trie_node * trie_root = new prefix_trie_node('$');
    assert(NULL != trie_root);
    if (true == words.empty()) { return trie_root; }
    for (auto & word : words) {
      prefix_trie_node * curr_trie_ptr = trie_root;
      for (auto & chr : word) {
        if (false == curr_trie_ptr->is_adjacent_char_exists(chr)) {
          curr_trie_ptr->insert_adjacent_char(new prefix_trie_node(chr));
        }
        curr_trie_ptr = curr_trie_ptr->get_adjacent_char_ptr(chr);
      }
      curr_trie_ptr->mark_as_ending_char();
    }
    return trie_root;
  }

  static int gen_position_key(int curr_row, int curr_col,
                              const vector<vector<char>> & board) {
    return curr_row * board.front().size() + curr_col;
  }

  static void search_word_in_batch(const vector<vector<char>> & board,
                                   prefix_trie_node * trie_root_ptr,
                                   vector<string> & existing_words,
                                   int curr_row, int curr_col,
                                   unordered_set<int> & position_taken) {
    if (NULL == trie_root_ptr) { return; }
    if (curr_row >= board.size() || curr_col >= board.front().size() ||
        curr_row < 0 || curr_col < 0) { return; }

    int curr_position_key = gen_position_key(curr_row, curr_col, board);

    if (position_taken.end() != position_taken.find(curr_position_key)) { return; }

    if (trie_root_ptr->get_char_val() != board[curr_row][curr_col]) { return; }

    position_taken.insert(curr_position_key);

    if (true == trie_root_ptr->get_is_ending_char() &&
        false == trie_root_ptr->is_visited()) {
      string curr_word;
      trie_root_ptr->mark_as_visited();
      for (prefix_trie_node * curr_ptr = trie_root_ptr; NULL != curr_ptr->get_parent_ptr();
           curr_ptr = curr_ptr->get_parent_ptr()) {
        curr_word.push_back(curr_ptr->get_char_val());
      }
      existing_words.push_back(string(curr_word.rbegin(), curr_word.rend()));
    }

    prefix_trie_node * adjacent_ptr = NULL;
    for (int chr = int('a'); chr <= int('z'); chr++) {
      adjacent_ptr = trie_root_ptr->get_adjacent_char_ptr(char(chr));
      if (NULL == adjacent_ptr) { continue; }
      search_word_in_batch(board, adjacent_ptr, existing_words, curr_row + 1, curr_col, position_taken);
      search_word_in_batch(board, adjacent_ptr, existing_words, curr_row - 1, curr_col, position_taken);
      search_word_in_batch(board, adjacent_ptr, existing_words, curr_row, curr_col + 1, position_taken);
      search_word_in_batch(board, adjacent_ptr, existing_words, curr_row, curr_col - 1, position_taken);
    }

    position_taken.erase(curr_position_key);
  }

  static vector<string> find_word_in_batch(const vector<vector<char>> & board,
                                           const vector<string> & words) {
    vector<string> existing_words;
    prefix_trie_node * trie_root_ptr = build_prefix_trie(words);
    unordered_set<int> position_taken;

    for (int i = 0; i < board.size(); i++) {
      for (int j = 0; j < board[i].size(); j++) {
        search_word_in_batch(board, trie_root_ptr->get_adjacent_char_ptr(board[i][j]),
                             existing_words, i, j, position_taken);
        if (existing_words.size() == words.size()) { return existing_words; }
      }
    }
    return existing_words;
  }

  static void test_find_word_in_batch() {
    cout << "1. test_find_word_in_batch" << endl;
    cout << "[ eat oath ] <=>" << endl;
    print_all_elem(find_word_in_batch(vector<vector<char>>({ { 'o','a','a','n' }, { 'e','t','a','e' },
                                                             { 'i','h','k','r' }, { 'i','f','l','v' } }),
                                      vector<string>({ "oath", "pea", "eat", "rain" })));
    cout << "[ a ] <=>" << endl;
    print_all_elem(find_word_in_batch(vector<vector<char>>({ { 'o','a','a','n' }, { 'e','t','a','e' },
                                                             { 'i','h','k','r' }, { 'i','f','l','v' } }),
                                      vector<string>({ "a" })));
    cout << "[ ] <=>" << endl;
    print_all_elem(find_word_in_batch(vector<vector<char>>({ { 'o','a','a','n' }, { 'e','t','a','e' },
                                                             { 'i','h','k','r' }, { 'i','f','l','v' } }),
                                      vector<string>({ })));

    cout << "[ ab ac bd ca db ] <=>" << endl;
    print_all_elem(find_word_in_batch(vector<vector<char>>({ { 'a','b' }, { 'c','d' } }),
                                      vector<string>({ "ab","cb","ad","bd","ac","ca","da","bc","db","adcb","dabc","abb","acb" })));

    cout << "[ anda anes anesis avener avine bena bend benda besa besan bowl daven embow inerm irene myst nane nanes neem reem reest renew rine riva rive riven sand sane sang seen seer send sise stob stow teil vine viner vire wadna wave wene wots ] <=>" << endl;
    print_all_elem(find_word_in_batch(vector<vector<char>>({ {'s', 'e', 'e', 'n', 'e', 'w'}, {'t', 'm', 'r', 'i', 'v', 'a'},
                                                             {'o', 'b', 's', 'i', 'b', 'd'}, {'w', 'm', 'y', 's', 'e', 'n'},
                                                             {'l', 't', 's', 'n', 's', 'a'}, {'i', 'e', 'z', 'l', 'g', 'n'} }),
                                      vector<string>({ "anda","anes","anesis","avener","avine","bena","bend","benda","besa","besan","bowl","daven","embow","inerm","irene","myst","nane","nanes","neem","reem","reest","renew","rine","riva","rive","riven","sand","sane","sang","seen","seer","send","sise","stob","stow","teil","vine","viner","vire","wadna","wave","wene","wots" })));
  }

  /**
   * 214. Shortest Palindrome
   * Given a string s, you are allowed to convert it to a palindrome by adding
   * characters in front of it. Find and return the shortest palindrome you can
   * find by performing this transformation.
   * Example 1:
   * - Input: "aacecaaa"
   * - Output: "aaacecaaa"
   * Example 2:
   * - Input: "abcd"
   * - Output: "dcbabcd"
   * Intuition:
   * - prepend chars to make whole str be a palindrome.
   * - no matter what the palindrome looks like, it has a pivot, worst case will
   *   be using 1st char as pivot, and will not exceed the mid chr.
   *      7
   *   ---|----
   * - aacecaaa
   * - a -> $a$
   * - a a -> $a$a$
   * - a b c -> $a$b$c$ -> size -> 5 -> max-pivot -> size / 2
   * - a a b -> $a$a$b$ -> i -> 2 -> 2 * 2 + 1
   */
  static bool is_curr_position_palindrome(const string & text, int pivot) {
    int text_len = text.size();
    int left_idx = pivot - 1, right_idx = pivot + 1;
    bool is_palindrome = true;
    while (left_idx >= 0 && right_idx < text_len) {
      if (text[left_idx] != text[right_idx]) { is_palindrome = false; break; }
      left_idx--; right_idx++;
    }
    return is_palindrome;
  }

  static string get_shortest_palindrome(string text_input) {
    if (true == text_input.empty()) { return ""; }
    string text("$");
    for (auto & chr : text_input) { text.append(1, chr); text.append("$"); }

    int pivot_pos = text.size() / 2;
    if (true == is_curr_position_palindrome(text, pivot_pos)) { return text_input; }

    for (int i = pivot_pos - 1; i >= 0; i--) {
      if (false == is_curr_position_palindrome(text, i)) { continue; }
      string prefix = text.substr(i * 2 + 1, string::npos);
      text = string(prefix.rbegin(), prefix.rend()) + text;
      break;
    }

    string result;
    for (auto & chr : text) { if ('$' != chr) { result.append(1, chr); } }
    return result;
  }

  static void test_get_shortest_palindrome() {
    cout << "2. test_get_shortest_palindrome" << endl;
    cout << "aaacecaaa <=> " << get_shortest_palindrome("aacecaaa") << endl;
    cout << "dcbabcd <=> " << get_shortest_palindrome("abcd") << endl;
    cout << "a <=> " << get_shortest_palindrome("a") << endl;
    cout << " <=> " << get_shortest_palindrome("") << endl;
    cout << "aaacecaaa <=> " << get_shortest_palindrome("aaacecaaa") << endl;
  }

  /**
   * s: $  a  $  b  $  a  $  b  $  a  $  b  $  a  $
   * i:    1  2
   * r: 0  1  ?
   *  2-1-1 2-1+1
   *    0     2
   *    0  1  2  3
   *    0  1  0  ?(i=3)
   *             1
   *      3-1-0 <=> 3-1+0 ~ 2
   * s: $  a  $  b  $  a  $  b  $  a  $  b  $  a  $
   * i: 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
   * l: 0  1  0  3  0  5  0  7  0  5  0  3  0  1  0
   *    |--------------|--?--x--------|
   *         m       m*
   *    |<<<<<<<<|>>>>>>>>|
   *                    m        m*
	 *             ***|<<<<<<<<|>>>>>>>>|***
   *    |????????????        ^         ???????????|
   *
   *    |<<<<<<<<<<<<<<<<<<<<|>>>>>>>>>>>>>>>>>>>>|
   *            |-----| r_without_cp
   *   lbi     c     rbi
   *    |<<<<<<|>>>>>>|      ^    **|<<<<<<|>>>>>>|**
   *                  |<<|>>|^|<<|>>|            ==>> case I
   */
  const static char DEF_DELIM_CHAR = '$';

  static string insert_delim(const string & input) {
    string result(1, DEF_DELIM_CHAR);
    for (auto & chr : input) {
      result.append(1, chr); result.append(1, DEF_DELIM_CHAR);
    }
    return result;
  }

  static string remove_delim(const string input) {
    string result;
    for (auto & chr : input) {
      if (DEF_DELIM_CHAR == chr) { continue; }
      result.append(1, chr);
    }
    return result;
  }

  static void calc_boundary_arr(const string & text,
                                vector<int> & palin_boundary_lookup,
                                int * max_palin_id, int * max_palin_len) {
    int curr_radius = 0, curr_pivot = 0, mirror_pivot = 0,
        left_boundary = 0, right_boundary = 0;
    int left_idx = 0, right_idx = 0;

    for (int i = 1; i < text.size() - 1; i++) {
      right_boundary = curr_pivot + curr_radius;
      if (i < right_boundary) {
        /* current pivot is covered by boundary */
        mirror_pivot = 2 * curr_pivot - i;
        right_idx = i + palin_boundary_lookup[mirror_pivot];
        if (right_idx <= right_boundary) {
          /*            x ... curr-pivot ...  i
           *   |---------|----------|----------|----------|
           *      <-------------->     <--------------> */
          palin_boundary_lookup[i] = palin_boundary_lookup[mirror_pivot];
          left_idx = i - palin_boundary_lookup[mirror_pivot] - 1;
          right_idx = i + palin_boundary_lookup[mirror_pivot] + 1;
        } else {
          /*            x ... curr-pivot ...  i
           *   |---------|----------|----------|----------|
           *                     <--------------------------->
           * <------------------------> */
          palin_boundary_lookup[i] = right_boundary - i;
          right_idx = right_boundary + 1;
          left_idx = 2 * i - right_boundary - 1;
        }
      } else {
        left_idx = i - 1; right_idx = i + 1;
      }
      while (left_idx >= 0 && right_idx < text.size()) {
        if (text[left_idx] != text[right_idx]) { break; }
        palin_boundary_lookup[i]++; left_idx--; right_idx++; curr_radius++;
      }
      if (right_idx >= right_boundary) {
        curr_pivot = i; curr_radius = palin_boundary_lookup[i];
      }
      if (* max_palin_len < palin_boundary_lookup[i]) {
        * max_palin_id = i; * max_palin_len = palin_boundary_lookup[i];
      }
    }
  }

  static string get_longest_palindrome(string input_str) {
    string text = insert_delim(input_str);

    vector<int> palin_boundary_lookup(text.size(), 0);
    int max_palin_id = 0, max_palin_len = 0;
    calc_boundary_arr(text, palin_boundary_lookup, & max_palin_id, & max_palin_len);

    string token_righ = text.substr(max_palin_id + 1, max_palin_len);
    string token_left = token_righ; reverse(token_left.begin(), token_left.end());

    return remove_delim(token_left + string(1, text[max_palin_id]) + token_righ);
  }

  static void test_get_longest_palindrome() {
    cout << "3. test_get_longest_palindrome" << endl;
    string test_input[] = {
      "babcbabcbaccba", "abaaba", "abababa", "abcbabcbabcba", "forgeeksskeegfor",
      "caba", "abacdfgdcaba", "abacdfgdcabba", "abacdedcaba", "a", "aa", "",
    };
    string test_output[] = {
      "abcbabcba", "abaaba", "abababa", "abcbabcbabcba", "geeksskeeg",
      "aba", "aba", "abba", "abacdedcaba", "a", "aa", "",
    };
    for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
      cout << test_output[i] << " <=> " << get_longest_palindrome(test_input[i]) << endl;
      assert(get_longest_palindrome(test_input[i]) == test_output[i]);
    }
  }

  static string gen_shortest_palindrome(string text_input) {
    string text = insert_delim(text_input);

    vector<int> mirror_len_arr(text.size(), 0);
    int max_id = 0, max_len = 0, target_id = 0;
    calc_boundary_arr(text, mirror_len_arr, & max_id, & max_len);
    for (int i = 0; i < text.size() / 2 + 1; i++) {
      if (0 == (mirror_len_arr[i] - i)) { target_id = i; }
    }

    string left_token = text.substr(target_id + mirror_len_arr[target_id] + 1, string::npos);
    string righ_token = left_token; reverse(righ_token.begin(), righ_token.end());
    return remove_delim(righ_token + text);
  }

  static void test_gen_shortest_palindrome() {
    cout << "4. test_gen_shortest_palindrome" << endl;
    cout << "aaacecaaa <=> " << gen_shortest_palindrome("aacecaaa") << endl;
    cout << "dcbabcd <=> " << gen_shortest_palindrome("abcd") << endl;
    cout << "a <=> " << gen_shortest_palindrome("a") << endl;
    cout << " <=> " << gen_shortest_palindrome("") << endl;
    cout << "aaacecaaa <=> " << gen_shortest_palindrome("aaacecaaa") << endl;
  }

  /**
   * 151. Reverse Words in a String
   * Given an input string, reverse the string word by word.
   * Example:
   * Input:  "the sky is blue",
   *         "elub si yks eth",
   * Output: "blue is sky the".
   * Note:
   * - A word is defined as a sequence of non-space characters.
   * - Input string may contain leading or trailing spaces. However, your
   *   reversed string should not contain leading or trailing spaces.
   * - You need to reduce multiple spaces between two words to a single space in
   *   the reversed string.
   * - Follow up: For C programmers, try to solve it in-place in O(1) space.
   * Intuition:
   * - reverse the text 1st then reverse each word
   */
  static void reverse_words_in_place(string & text) {
    int text_len = text.size(), word_cnt = 0;
    reverse(text.begin(), text.end());

    /* reverse each word 1st */
    for (string::iterator start_itr = text.begin(), end_itr = text.begin();
                                                    end_itr != text.end();) {
      bool is_token_added = false;
      while (start_itr != text.end() && ' ' == * start_itr) { start_itr++; end_itr = start_itr; }
      while (end_itr != text.end() && ' ' != * end_itr) { is_token_added = true; end_itr++; }
      reverse(start_itr, end_itr);
      if (end_itr != text.end()) { end_itr++; }
      start_itr = end_itr;
      if (true == is_token_added) { word_cnt++; }
    }

    int token_start_id = 0, token_end_id = 0, curr_id = 0, curr_token_id = 0;
    /* remove redundant space */
    for (curr_id = 0; curr_id < text_len && token_start_id < text_len; ) {
      bool is_token_added = false;
      while (token_start_id < text_len && ' ' == text[token_start_id]) {
        token_start_id++; token_end_id = token_start_id;
      }
      while (token_end_id < text_len && ' ' != text[token_end_id]) { token_end_id++; }
      for (int k = token_start_id; k < token_end_id; k++) {
        text[curr_id] = text[k]; curr_id++; is_token_added = true;
      }
      if (true == is_token_added && curr_id < text_len &&
          curr_token_id != word_cnt - 1) { text[curr_id] = ' '; curr_id++; }
      token_end_id++; token_start_id = token_end_id;
      if (true == is_token_added) { curr_token_id++; }
    }
    text.erase(curr_id);
  }

  static void test_reverse_words_in_place() {
    string test_input[] = { "the sky is blue", " ", "  the    sky is   blue ", "   dfksjf ", " 1" };
    string test_output[] = { "blue is sky the", "", "blue is sky the", "dfksjf", "1" };
    cout << "6. test_reverse_words_in_place" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
      reverse_words_in_place(test_input[i]);
      cout << test_output[i] << " <=> " << test_input[i] << endl;
      assert(test_input[i] == test_output[i]);
    }
  }

  static string reverse_words(string & text) {
    vector<string> token_arr;
    string result;
    int token_len = 0;

    for (int token_start_id = 0, token_end_id = 0; token_end_id < text.size();) {
      if (' ' == text[token_start_id]) {
        token_start_id++; token_end_id = token_start_id; continue;
      }
      if (' ' == text[token_end_id] || token_end_id == text.size() - 1) {
        token_len = token_end_id - token_start_id;
        if (' ' != text[token_end_id]) { token_len++; }
        token_arr.push_back(text.substr(token_start_id, token_len));
        token_start_id = token_end_id + 1; token_end_id = token_start_id;
        continue;
      }
      token_end_id++;
    }

    while (false == token_arr.empty()) {
      result.append(token_arr.back());
      token_arr.pop_back();
      if (false == token_arr.empty()) { result.append(" "); }
    }

    return result;
  }

  static void test_reverse_words() {
    string test_input[] = { "the sky is blue", " ", "  the    sky is   blue ", "   dfksjf ", " 1" };
    string test_output[] = { "blue is sky the", "", "blue is sky the", "dfksjf", "1" };
    cout << "5. test_reverse_words" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
      cout << test_output[i] << " <=> " << reverse_words(test_input[i]) << endl;
      assert(reverse_words(test_input[i]) == test_output[i]);
    }
  }
  /**
   * 97. Interleaving String
   * - Given s1, s2, s3, find whether s3 formed by interleaving of s1 and s2.
   * Example 1:
   * - Input: s1 = "aabcc", s2 = "dbbca", s3 = "aadbbcbcac"
   * - Output: true
   * Example 2:
   * - Input: s1 = "aabcc", s2 = "dbbca", s3 = "aadbbbaccc"
   * - Output: false
   * Intuition:
   * - only true|false needed, not a actual plan, DP?
   * - let lookup(i, j, k) denotes whether s3[k] is formed by s1[i] & s[j]
   * - lookup(i, j, k) => {
   *     lookup(i - 1, j, k - 1) && s1[i] == s3[k] && k == i + j ||
   *     lookup(i, j - 1, k - 1) && s2[j] == s3[k] && k == i + j
   *   }
   * - goal is to calc the last elem
   */
  static bool is_interleave(const string & s1, const string & s2,
                                               const string & s3) {
    vector<vector<vector<bool>>> lookup(s1.size() + 1,
      vector<vector<bool>>(s2.size() + 1, vector<bool>(s3.size() + 1, false))
    );
    if (s1.size() + s2.size() != s3.size()) { return false; }
    lookup[0][0][0] = true;
    for (int i = 0; i <= s1.size(); i++) {
      for (int j = 0; j <= s2.size(); j++) {
        for (int k = 1; k <= s3.size(); k++) {
          if (k != i + j) { continue; }
          lookup[i][j][k] = ((i - 1 >= 0 && lookup[i - 1][j][k - 1] && s1[i - 1] == s3[k - 1]) ||
                             (j - 1 >= 0 && lookup[i][j - 1][k - 1] && s2[j - 1] == s3[k - 1]));
        }
      }
    }
    return lookup.back().back().back();
  }

  static bool fast_is_interleave(const string & s1, const string & s2,
                                                    const string & s3) {
    vector<vector<bool>> lookup(s1.size() + 1, vector<bool>(s2.size() + 1, false));
    if (s1.size() + s2.size() != s3.size()) { return false; }
    lookup[0][0] = true;
    for (int i = 0; i <= s1.size(); i++) {
      for (int j = 0; j <= s2.size(); j++) {
        int k = i + j;
        if (k <= 0) { continue; }
        lookup[i][j] = ((i - 1 >= 0 && lookup[i - 1][j] && s1[i - 1] == s3[k - 1]) ||
                        (j - 1 >= 0 && lookup[i][j - 1] && s2[j - 1] == s3[k - 1]));
      }
    }
    return lookup.back().back();
  }

  static void test_is_interleave() {
    cout << "7. test_is_interleave" << endl;
    vector<vector<string>> postive_cases = { { "aabcc", "dbbca", "aadbbcbcac" }, { "aabcc", "", "aabcc" }, { "", "aabcc", "aabcc" }, { "", "a", "a" }, { "", "", "" } };
    vector<vector<string>> negative_cases = { { "aabcc", "dbbca", "aadbbbaccc" }, { "aabcc", "", "aadbbbaccc" }, { "", "aabcc", "aa" } };
    for (auto & input_case : postive_cases) {
      assert(true == is_interleave(input_case[0], input_case[1], input_case[2]));
    }
    for (auto & input_case : negative_cases) {
      assert(false == is_interleave(input_case[0], input_case[1], input_case[2]));
    }
  }

  static void test_fast_is_interleave() {
    cout << "8. test_fast_is_interleave" << endl;
    vector<vector<string>> postive_cases = { { "aabcc", "dbbca", "aadbbcbcac" }, { "aabcc", "", "aabcc" }, { "", "aabcc", "aabcc" }, { "", "a", "a" }, { "", "", "" } };
    vector<vector<string>> negative_cases = { { "aabcc", "dbbca", "aadbbbaccc" }, { "aabcc", "", "aadbbbaccc" }, { "", "aabcc", "aa" } };
    for (auto & input_case : postive_cases) {
      assert(true == fast_is_interleave(input_case[0], input_case[1], input_case[2]));
    }
    for (auto & input_case : negative_cases) {
      assert(false == fast_is_interleave(input_case[0], input_case[1], input_case[2]));
    }
  }
  /**
   * 76. Minimum Window Substring
   * Given a string S and a string T, find the minimum window in S which will
   * contain all the characters in T in complexity O(n).
   * Example:
   * - Input: S = "ADOBECODEBANC", T = "ABC"
   * - Output: "BANC"
   * Note:
   * - If there is no such window in S that covers all characters in T, return
   *   the empty string "".
   * - If there is such window, you are guaranteed that there will always be
   *   only one unique minimum window in S.
   * Intuition:
   * - a char set contains all chars need to be covered, we keep a lookup
   * - bf way calc all possible start point & keep matching to the end.
   * - we only need to cover all chars, order not matter.
   *     v     v   v       | |   |
   * - [ A D O B E C O D E B A N C ], T = [ A B C ]
   *     v     v v   v       | | |   |
   * - [ A D O B B E C O D E B B A N C ], T = [ A B C ]
   *     v v   v   v       |
   *     0 1 2 3 4 5 6 7 8 9 0 1 2
   * - [ A B O B E C O D E B A B C ] [ A B B C ]
   *     A B O B E C O
   *     A B O(B E C O D E B)
   *           B E C O D E B A B C
   *               C O D E B A B
   *               C O D E B A B C -> B A B C
   *     v v   v v |
   *     0 1 2 3 4 5 6 7 8 9 0 1 2
   * - [ A B O B C B A B X ] | [ A B B C ]
   *     A B O B C
   *           B C B A
   *             C B A B
   * - variable sized window keep sliding.
   */
  static void decr_remaining_char_cnt(char chr, vector<int> & curr_buf,
                                      int curr_pos_id, vector<list<int>> & curr_pos_lookup) {
    curr_buf[int(toupper(chr)) - int('A')]--;
    curr_pos_lookup[int(toupper(chr)) - int('A')].push_back(curr_pos_id);
  }

  static bool is_char_existed(char chr, const vector<int> & lookup) {
    return (lookup[int(toupper(chr)) - int('A')] > 0);
  }

  static int get_next_start_id(vector<int> & curr_buf, const vector<int> & lookup,
                               vector<list<int>> & curr_pos_lookup,
                               const string & text, int curr_start_id,
                               int * curr_char_cnt_ptr) {
    int curr_char_id = int(toupper(text[curr_start_id])) - int('A');
    int max_id = curr_pos_lookup[curr_char_id].front() + 1;
    int new_start_pos = INT_MAX;
    for (int i = 0; i <= int('Z') - int('A'); i++) {
      while (!curr_pos_lookup[i].empty() && curr_pos_lookup[i].front() < max_id) {
        curr_pos_lookup[i].pop_front(); curr_buf[i]++; * curr_char_cnt_ptr -= 1;
      }
      if (!curr_pos_lookup[i].empty()) { new_start_pos = min(new_start_pos, curr_pos_lookup[i].front()); }
    }
    new_start_pos = min(new_start_pos, curr_start_id);
    return new_start_pos;
  }

  static int get_buffer_size() { return int('Z') - int('A') + 1; }

  static string get_min_window(const string & text, const string & char_set) {
    if (text.size() < char_set.size() || text.empty() || char_set.empty()) { return ""; }

    int total_char_cnt = char_set.size();
    vector<int> lookup(get_buffer_size(), 0 );
    for (auto & chr : char_set) { lookup[int(toupper(chr)) - int('A')]++; }

    int curr_char_cnt = 0;
    vector<int> curr_buf = lookup;
    vector<list<int>> curr_pos_lookup(get_buffer_size(), list<int>());

    int start_id = 0, end_id = INT_MAX;
    for (int curr_start_id = 0, curr_end_id = 0; curr_end_id < text.size(); ) {
      while (curr_start_id < text.size() && !is_char_existed(text[curr_start_id], lookup)) {
        curr_start_id++; curr_end_id = curr_start_id;
      }

      /* if we already cover all chars in prev window, then we need to adjust it */
      if (curr_start_id < text.size() && !is_char_existed(text[curr_start_id], curr_buf)) {
        curr_start_id = get_next_start_id(curr_buf, lookup, curr_pos_lookup, text, curr_start_id, & curr_char_cnt);
      }

      /* keep expanding until we cover the full char set */
      int right_idx = curr_end_id; bool is_char_set_covered = false;
      while (curr_end_id < text.size() && curr_char_cnt != total_char_cnt) {
        if (is_char_existed(text[curr_end_id], curr_buf)) {
          decr_remaining_char_cnt(text[curr_end_id], curr_buf, curr_end_id, curr_pos_lookup);
          curr_char_cnt++; right_idx = curr_end_id;
          if (curr_char_cnt == total_char_cnt) { is_char_set_covered = true; }
        }
        curr_end_id++;
      }
      curr_end_id = right_idx;
      /* log the boundary if we see smaller ones */
      if ((true == is_char_set_covered) && (curr_end_id - curr_start_id) < (end_id - start_id)) {
        start_id = curr_start_id; end_id = curr_end_id;
      }
      curr_end_id++; curr_start_id = curr_end_id;
    }
    return (end_id == INT_MAX) ? "" : text.substr(start_id, end_id - start_id + 1);
  }

  static string fast_min_window(const string & text, const string & char_set) {
    if (text.size() < char_set.size() || text.empty() || char_set.empty()) { return ""; }

    unordered_map<char, int> char_cnt_tot, char_cnt_map;
    for (auto & chr : char_set) { char_cnt_tot[chr]++; char_cnt_map[chr] = 0; }

    int start_id = 0, end_id = INT_MAX, covered_char_cnt = 0, target_char_cnt = char_set.size();

    for (int curr_start_id = 0, curr_end_id = 0, curr_pos = 0; curr_pos < text.size(); ) {
      curr_end_id = curr_pos;
      while (curr_pos < text.size() && covered_char_cnt != target_char_cnt) {
        if (char_cnt_tot.end() != char_cnt_tot.find(text[curr_pos])) {
          if (char_cnt_map[text[curr_pos]] < char_cnt_tot[text[curr_pos]]) { covered_char_cnt++; }
          char_cnt_map[text[curr_pos]]++;
          if (covered_char_cnt == target_char_cnt) { curr_end_id = curr_pos; }
        }
        curr_pos++;
      }

      while (curr_start_id <= curr_end_id && covered_char_cnt == target_char_cnt) {
        if (curr_end_id - curr_start_id <= end_id - start_id) { start_id = curr_start_id; end_id = curr_end_id; }
        if (char_cnt_tot.end() != char_cnt_tot.find(text[curr_start_id])) {
          if (char_cnt_map[text[curr_start_id]] == char_cnt_tot[text[curr_start_id]]) { covered_char_cnt--; }
          char_cnt_map[text[curr_start_id]]--;
        }
        curr_start_id++;
      }
    }
    return (end_id == INT_MAX) ? "" : text.substr(start_id, end_id - start_id + 1);
  }

  static void test_get_min_window() {
    string test_input[] = { "ADOBECODEBANC", "ABC", "ABOBECODEBABC", "ABBC", "ABOBCBABX", "ABBC", "", "", "", "sss" };
    string test_output[] = { "BANC", "BABC", "BCBA", "", "" };
    cout << "9. test_get_min_window" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
      string resu = get_min_window(test_input[i * 2], test_input[i * 2 + 1]);
      cout << resu << " <=> " << test_output[i] << " <=> " << test_input[i * 2] << " | " << test_input[i * 2 + 1] << endl;
      assert(resu == test_output[i]);
    }
  }

  static void test_fast_min_window() {
    string test_input[] = { "ADOBECODEBANC", "ABC", "ABOBECODEBABC", "ABBC", "ABOBCBABX", "ABBC", "", "", "", "sss" };
    string test_output[] = { "BANC", "BABC", "CBAB", "", "" };
    cout << "10. fast_min_window" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
      string resu = fast_min_window(test_input[i * 2], test_input[i * 2 + 1]);
      cout << resu << " <=> " << test_output[i] << " <=> " << test_input[i * 2] << " | " << test_input[i * 2 + 1] << endl;
      assert(resu == test_output[i]);
    }
  }

  /**
   * 115. Distinct Subsequences
   * - Given a string S and a string T, count the number of distinct subseqs
   *   of S which equals T.
   * - A subsequence of a string is a new string which is formed from the
   *   original string by deleting some (can be none) of the characters without
   *   disturbing the relative positions of the remaining characters.
   *   (ie, "ACE" is a subsequence of "ABCDE" while "AEC" is not).
   * Example 1:
   * - Input: S = "rabbbit", T = "rabbit"
   * - Output: 3
   * - Explanation:
   *   As shown below, there are 3 ways you can generate "rabbit" from S.
   *   (The caret symbol ^ means the chosen letters)
   *   rabbbit
   *   ^^^^ ^^
   *   rabbbit
   *   ^^ ^^^^
   *   rabbbit
   *   ^^^ ^^^
   * Example 2:
   * - Input: S = "babgbag", T = "bag"
   * - Output: 5
   * - Explanation:
   *   As shown below, there are 5 ways you can generate "bag" from S.
   *   (The caret symbol ^ means the chosen letters)
   *   babgbag
   *   ^^ ^
   *   babgbag
   *   ^^    ^
   *   babgbag
   *   ^    ^^
   *   babgbag
   *     ^  ^^
   *   babgbag
   *       ^^^
   * Intuition:
   * - Let seq-cnt(i, j) be the len of common subseq for t[0..i] & p[0..j]
   * - seq-cnt(i, j) = {
   *     if t[i] == p[j]
   *       seq-cnt(i - 1, j - 1) + 1
   *     else
   *       max(seq-cnt(i - 1, j), seq-cnt(i, j - 1));
   *     end
   *   }
   * - # of seq -> # of elems with size == size of pattern.
   * - actually only 2 cases, as we cannot skip any char from pattern
   * - let seq-cnt(i, j) denote # of diff matching seq for t[0..i] & p[0..j]
   *   where p[0..j] is fully matched, & seq-cnt[i][0] == 1 => no need match
   * - seq-cnt(i, j) = {
   *     if t[i] == p[j]
   *       seq-cnt(i - 1, j - 1) + seq-cnt(i - 1, j)
   *     else
   *       seq-cnt(i - 1, j)
   *     end
   *   }
   * - return bottom right elem.
[
  [ 0 0 0 0 0 0 0 ]
  [ 0 1 1 1 1 1 1 ]
  [ 0 1 2 2 2 2 2 ]
  [ 0 1 2 3 3 3 3 ]
  [ 0 1 2 3 4 4 4 ]
  [ 0 1 2 3 4 4 4 ]
  [ 0 1 2 3 4 5 5 ]
  [ 0 1 2 3 4 5 6 ]
]
     - then start from (1, 1) try to reach all nodes where pattern is fully matched.
     - each step can only move downward (skip text) or diag (matched), then # of
       uniq paths is the # of seqs.
   */
  static int cnt_distinct_subseqs(const string & text, const string & pattern) {
    vector<vector<int>> lookup(text.size() + 1, vector<int>(pattern.size() + 1, 0));
    for (int i = 0; i <= text.size(); i++) { lookup[i][0] = 1; }
    for (int i = 1; i <= text.size(); i++) {
      for (int j = 1; j <= pattern.size(); j++) {
        if (text[i - 1] == pattern[j - 1]) {
          lookup[i][j] = lookup[i - 1][j - 1] + lookup[i - 1][j];
        } else {
          lookup[i][j] = lookup[i - 1][j];
        }
      }
    }
    return lookup.back().back();
  }

  static void test_cnt_distinct_subseqs() {
    string test_input[] = { "rabbbit", "rabbit", "babgbag", "bag" };
    int test_output[] = { 3, 5 };
    cout << "10. fast_min_window" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(int); i++) {
      int resu = cnt_distinct_subseqs(test_input[i * 2], test_input[i * 2 + 1]);
      cout << resu << " <=> " << test_output[i] << " <=> " << test_input[i * 2] << " | " << test_input[i * 2 + 1] << endl;
      assert(resu == test_output[i]);
    }
  }

  /**
   * 71. Simplify Path
   * - Given an absolute path for a file (Unix-style), simplify it.
   * For example,
   * - path = "/home/", => "/home"
   *   path = "/a/./b/../../c/", => "/c"
   *   path = "/a/../../b/../c//.//", => "/c"
   *   path = "/a//b////c/d//././/..", => "/a/b/c"
   *
   * - In a UNIX-style file system, a period ('.') refers to the current dir,
   *   so it can be ignored in a simplified path. Additionally, a double period
   *   ("..") moves up a directory, so it cancels out whatever the last dir was.
   *   For more information, look here:
   * - https://en.wikipedia.org/wiki/Path_(computing)#Unix_style
   *
   * Corner Cases:
   *
   * - Did you consider the case where path = "/../"?
   * - In this case, you should return "/".
   * - Another corner case is the path might contain multiple slashes '/'
   *   together, such as "/home//foo/", In this case, you should ignore
   *   redundant slashes and return "/home/foo".
   */

  const static string def_curr_dir = ".";
  const static string def_prev_dir = "..";
  const static string def_delim    = "/";
  static string simplify_path(const string & input) {
    string curr_token;
    vector<string> token_stack;
    if (input.empty() || input[0] != def_delim[0]) { return def_delim; }

    for (int i = 0; i < input.size(); ) {
      if (def_delim[0] == input[i]) { i++; continue; }

      curr_token = "";
      /* normal chars, loop until get all normal chars */
      while ((i < input.size()) && def_delim[0] != input[i]) { curr_token.append(1, input[i]); i++; }

      if (curr_token != def_prev_dir && curr_token != def_curr_dir && curr_token[0] != def_delim[0]) {
        token_stack.push_back(curr_token);
      } else if (curr_token == def_prev_dir) {
        if (!token_stack.empty()) { token_stack.pop_back(); }
      }
    }

    curr_token = "";
    for (int i = 0; i < token_stack.size(); i++) {
      curr_token.append(def_delim);
      curr_token.append(token_stack[i]);
    }

    if (curr_token.empty()) { curr_token = def_delim; }

    return curr_token;
  }

  static void test_simplify_path() {
    string test_input[] = { "/home/", "/a/./b/../../c/", "/a/../../b/../c//.//", "/a//b////c/d//././/..", "/../", "/home//foo/", "", "/../../../", "/...", "////home//...ss/foo///" };
    string test_output[] = { "/home", "/c", "/c", "/a/b/c", "/", "/home/foo", "/", "/", "/...", "/home/...ss/foo" };
    string path;
    cout << "11. test_simplify_path" << endl;
    for (int i = 0; i < sizeof(test_input)/sizeof(string); i++) {
      path = simplify_path(test_input[i]);
      cout << test_output[i] << " <=> " << path << endl;
      assert(test_output[i] == path);
    }
  }

  /**
   * 65. Valid Number
   * Validate if a given string can be interpreted as a decimal number.
   * Some examples:
   * "0" => true
   * " 0.1 " => true
   * "abc" => false
   * "1 a" => false
   * "2e10" => true
   * " -90e3   " => true
   * " 1e" => false
   * "e3" => false
   * " 6e-1" => true
   * " 99e2.5 " => false
   * "53.5e93" => true
   * " --6 " => false
   * "-+3" => false
   * "95a54e53" => false
   * Note:
   * - It is intended for the problem statement to be ambiguous. You should
   *   gather all requirements up front before implementing one. However, here
   *   is a list of characters that can be in a valid decimal number:
   * - Numbers 0-9
   * - Exponent - "e"
   * - Positive/negative sign - "+"/"-"
   * - Decimal point - "."
   * - Of course, the context of these characters also matters in the input.
   * - -/+? ((0(.*[0-9])?)|([1-9]+[0-9]*)) (e -/+? [1-9]+[0-9]*)?
   */
  static bool is_number_valid(const string & str) {
    vector<unordered_set<char>> input_arr({
      unordered_set<char>({ '0' }),
      unordered_set<char>({ '-', '+' }),
      unordered_set<char>({ '1', '2', '3', '4', '5', '6', '7', '8', '9' }),
      unordered_set<char>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }),
      unordered_set<char>({ '.' }),
      unordered_set<char>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }),
      unordered_set<char>({ 'e', 'E' }),
      unordered_set<char>({ '-', '+' }),
      unordered_set<char>({ '1', '2', '3', '4', '5', '6', '7', '8', '9' }),
      unordered_set<char>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' })
    });
    enum CHAR_TYPE { CHAR_INVALID = -1, CHAR_ZERO = 0, CHAR_VALUE_SIGN, CHAR_VALUE_NON_ZERO, CHAR_VALUE_INT,
                     CHAR_DOT, CHAR_VALUE_FRACTION_INT, CHAR_EXP_FLAG, CHAR_EXP_SIGN, CHAR_EXP_NON_ZERO, CHAR_EXP_INT };

    vector<int> char_type_mapping({
      CHAR_ZERO, CHAR_VALUE_SIGN, CHAR_VALUE_NON_ZERO, CHAR_VALUE_INT, CHAR_DOT,
      CHAR_VALUE_FRACTION_INT, CHAR_EXP_FLAG, CHAR_EXP_SIGN, CHAR_EXP_NON_ZERO, CHAR_EXP_INT });

    unordered_set<int> valid_starting_states({ CHAR_ZERO, CHAR_VALUE_SIGN, CHAR_VALUE_NON_ZERO });

    unordered_set<int> terminated_states({ CHAR_ZERO, CHAR_VALUE_NON_ZERO, CHAR_VALUE_INT,
                                           CHAR_VALUE_FRACTION_INT, CHAR_EXP_NON_ZERO, CHAR_EXP_INT });

    unordered_map<int, unordered_set<int>> transition_lookup({
      { CHAR_ZERO, unordered_set<int>({ CHAR_DOT }) },
      { CHAR_VALUE_SIGN, unordered_set<int>({ CHAR_ZERO, CHAR_VALUE_NON_ZERO }) },
      { CHAR_VALUE_NON_ZERO, unordered_set<int>({ CHAR_DOT, CHAR_VALUE_INT, CHAR_EXP_FLAG }) },
      { CHAR_VALUE_INT, unordered_set<int>({ CHAR_VALUE_INT, CHAR_DOT, CHAR_EXP_FLAG }) },
      { CHAR_DOT, unordered_set<int>({ CHAR_VALUE_FRACTION_INT }) },
      { CHAR_VALUE_FRACTION_INT, unordered_set<int>({ CHAR_VALUE_FRACTION_INT, CHAR_EXP_FLAG }) },
      { CHAR_EXP_FLAG, unordered_set<int>({ CHAR_EXP_SIGN, CHAR_EXP_NON_ZERO }) },
      { CHAR_EXP_SIGN, unordered_set<int>({ CHAR_EXP_NON_ZERO }) },
      { CHAR_EXP_NON_ZERO, unordered_set<int>({ CHAR_EXP_INT }) },
      { CHAR_EXP_INT, unordered_set<int>({ CHAR_EXP_INT }) }
    });

    bool is_number_valid = false;
    char curr_char;
    int curr_state = CHAR_INVALID;
    unordered_set<int> & valid_states_set_ref = valid_starting_states;

    int start_pos = 0, end_pos = str.size() - 1;
    while (start_pos < str.size() && ' ' == str[start_pos]) { start_pos++; }
    while (end_pos >= 0 && ' ' == str[end_pos]) { end_pos--; }
    for (int i = start_pos; i <= end_pos; i++) {
      curr_char = str[i];
      curr_state = CHAR_INVALID;
      for (int j = 0; j < char_type_mapping.size(); j++) {
        int state = char_type_mapping[j];
        if (input_arr[state].end() == input_arr[state].find(curr_char)) { continue; }
        if (valid_states_set_ref.end() == valid_states_set_ref.find(state)) { continue; }
        valid_states_set_ref = transition_lookup[state];
        curr_state = state; break;
      }

      if (CHAR_INVALID == curr_state) { break; }
      if (end_pos == i && terminated_states.end() !=
                          terminated_states.find(curr_state)) {
        is_number_valid = true;
      }
    }

    return is_number_valid;
  }

  static void test_is_number_valid() {
    string test_input[] = { "-10.0009e+50", "e53", "000", "0", " 0.1 ", "abc", "1 a", "2e10", " -90e3   ", " 1e", "e3", " 6e-1", " 99e2.5 ", "53.5e93", " --6 ", "-+3", "95a54e53" };
    bool test_output[] = { true, false, false, true, true, false, false, true, true, false, false, true, false, true, false, false, false };
    bool result = false;
    cout << "16. test_is_number_valid" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(test_output[0]); i++) {
      result = is_number_valid(test_input[i]);
      cout << test_output[i] << " <=> " << result << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 67. Add Binary
   * Given two binary strings, return their sum (also a binary string).
   *
   * The input strings are both non-empty and contains only characters 1 or 0.
   *
   * Example 1:
   *
   * Input: a = "11", b = "1"
   * Output: "100"
   * Example 2:
   *
   * Input: a = "1010", b = "1011"
   * Output: "10101"
   *
   * "1010"
   * "1011"
   *    01
   *   (1)
   */
  static string add_binary(const string & str_l, const string & str_r) {
    string output = ""; int carry = 0, sum = 0;
    /* a will always points to the longer one */
    const string & a = (str_l.size() >= str_r.size()) ? str_l : str_r;
    const string & b = (str_l.size() >= str_r.size()) ? str_r : str_l;

    for (int i = a.size() - 1, j = b.size() - 1; i >= 0; i--, j--) {
      sum = int(a[i]) - int('0') + carry;
      if (j >= 0) { sum += (int(b[j]) - int('0')); }
      switch(sum) {
        case 0:
        case 1: { output.append(1, char(sum + int('0'))); carry = 0; break; }
        case 2:
        case 3: { output.append(1, char(sum + int('0') - 2)); carry = 1; break; }
        default: { break; }
      }
    }
    if (carry == 1) { output.append(1, '1'); }
    reverse(output.begin(), output.end());

    return output;
  }

  static void test_add_binary() {
    vector<string> test_input({ "11", "1", "1010", "1011", "110111", "1", "1", "110111", "110111", "" });
    vector<string> test_output({ "100", "10101", "111000", "111000", "110111" });
    string result;
    cout << "12. test_add_binary" << endl;
    for (int i = 0; i < test_output.size(); i++) {
      result = add_binary(test_input[i * 2], test_input[i * 2 + 1]);
      cout << test_output[i] << " <=> " << result << endl;
      assert(test_output[i] == result);
    }
  }

  /**
   * 32. Longest Valid Parentheses
   * Given a string containing just the characters '(' and ')', find the length
   * of the longest valid (well-formed) parentheses substring.
   *
   * Example 1:
   *
   * Input: "(()"
   * Output: 2
   * Explanation: The longest valid parentheses substring is "()"
   * Example 2:
   *
   * Input: ")()())"
   * Output: 4
   * Explanation: The longest valid parentheses substring is "()()"
   *
   * Intuition:
   * - ONLY need # of valid plans, then DP
   * - goal is to get the size of well formated longest substring
   * - let lcs-lookup(j) denotes the size of lcs ends at str[j] then
   * - each time we get check a char ')', we try to close a existing unclosed
   *   '(' on left side, which should be on the left of lcs[j - 1]
   *   or if we get a '(' then we essentially start a new pair.
   * - lcs-lookup(j) = {
   *     lcs-lookup(j - 1) if (str[j] == '(')
   *     lcs-lookup(j - 1) + 2 + prev-lcs-size if (str[j] == ')' && str[j - 1 - lcs-lookup(j - 1)] == '(')
   *   }
   * - goal is to calc all elem & calc the max-non-stop-subsum
   * - (()() -> (()()( -> (()()() -> (()()())
   */
  static int calc_longest_valid_parentheses(const string & input) {
    int max_lcs_size = 0;
    vector<int> lcs_lookup(input.size(), 0);
    for (int i = 1; i < input.size(); i++) {
      if (input[i] == '(') {
        lcs_lookup[i] = 0;
      } else if (input[i - 1 - lcs_lookup[i - 1]] == '(') {
        lcs_lookup[i] = lcs_lookup[i - 1] + 2;
        /* check to see if prev-neighboring token are valid, sum it if yes */
        if (i - 2 - lcs_lookup[i - 1] >= 0) {
          lcs_lookup[i] += lcs_lookup[i - 2 - lcs_lookup[i - 1]];
        }
      }
      max_lcs_size = max(max_lcs_size, lcs_lookup[i]);
    }
    return max_lcs_size;
  }

  static void test_calc_longest_valid_parentheses() {
    cout << "13. test_calc_longest_valid_parentheses" << endl;
    string test_input[] = { "(()", ")()())", "(()()())", "((((()()())(()", "((())())", "(((())))", "", "((((()()())(()(())((()",  };
    int test_output[] = { 2, 4, 8, 8, 8, 8, 0, 8 };
    for (int i = 0; i < sizeof(test_output)/sizeof(int); i++) {
      int resu = calc_longest_valid_parentheses(test_input[i]);
      cout << test_output[i] << " <=> " << resu << endl;
      assert(test_output[i] == resu);
    }
  }

  /**
   * 44. Wildcard Matching
   * Given an input string (s) and a pattern (p), implement wildcard pattern matching with support for '?' and '*'.
   *
   * '?' Matches any single character.
   * '*' Matches any sequence of characters (including the empty sequence).
   * The matching should cover the entire input string (not partial).
   *
   * Note:
   *
   * s could be empty and contains only lowercase letters a-z.
   * p could be empty and contains only lowercase letters a-z, and characters like ? or *.
   * Example 1:
   *
   * Input:
   * s = "aa"
   * p = "a"
   * Output: false
   * Explanation: "a" does not match the entire string "aa".
   * Example 2:
   *
   * Input:
   * s = "aa"
   * p = "*"
   * Output: true
   * Explanation: '*' matches any sequence.
   * Example 3:
   *
   * Input:
   * s = "cb"
   * p = "?a"
   * Output: false
   * Explanation: '?' matches 'c', but the second letter is 'a', which does not match 'b'.
   * Example 4:
   *
   * Input:
   * s = "adceb"
   * p = "*a*b"
   * Output: true
   * Explanation: The first '*' matches the empty sequence, while the second '*' matches the substring "dce".
   * Example 5:
   *
   * Input:
   * s = "acdcb"
   * p = "a*c?b"
   * Output: false
   * acdcbcxb -> a*c?b -> true
   *
   * Intuition:
   * - the * essentially means .*, which disables the optimization of kmp etc, -> no O(n) then DP??
   * - Let mlookup(i, j) denotes if text[0..i] matched pattern[0..j], goal is to see bt elem in mlookup
   * - mlookup(i, j) = {
   *                                 use like ?             ignore
   *     if (pattern[j] == *) { mlookup(i - 1, j - 1) || mlookup(i, j - 1) }
   *     else if (pattern[j] == ?) { mlookup(i - 1, j - 1); }
   *     else { mlookup(i - 1, j - 1) && (pattern[j] == text[i]); }
   *   }
   * - base cases: '' & '' -> true, 'aaaa' & '' false, '' & 'bbb' -> false
   */
  static bool is_wild_card_matched(const string & text, const string & pattern) {
    if (text.empty()) {
      for (auto & chr : pattern) { if ('*' !=chr) { return false; } } return true;
    }
    if (pattern.empty()){ return false; }

    vector<vector<bool>> mlookup(text.size() + 1, vector<bool>(pattern.size() + 1, false));
    mlookup[0][0] = true;
    for (int i = 1; i <= pattern.size(); i++) {
      if ('*' == pattern[i - 1]) { mlookup[0][i] = true; } else { break; }
    }

    for (int i = 1; i <= text.size(); i++) {
      for (int j = 1; j <= pattern.size(); j++) {
        if ('*' == pattern[j - 1]) {
          /* either curr char from text can be skipped or * been ignored or matched */
          mlookup[i][j] = mlookup[i - 1][j] || mlookup[i][j - 1] || mlookup[i - 1][j - 1];
        } else if ('?' == pattern[j - 1] || text[i - 1] == pattern[j - 1]) {
          mlookup[i][j] = mlookup[i - 1][j - 1];
        }
      }
    }
    return mlookup.back().back();
  }

  static void test_is_wild_card_matched() {
    string test_input[] = { "aa", "a", "aa", "*", "cb", "?a", "adceb", "*a*b", "acdcb", "a*c?b", "acdcbcxb", "a*c?b", "", "**", "", "", "abc", "*", "a", "a*", "b", "*?*?", "ho", "**ho" };
    bool test_output[] = { false, true, false, true, false, true, true, true, true, true, false, true };
    cout << "14. test_is_wild_card_matched" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(test_output[0]); i++) {
      bool resu = is_wild_card_matched(test_input[2 * i], test_input[2 * i + 1]);
      cout << test_output[i] << " <=> " << resu << " | " << test_input[2 * i] << " : " << test_input[2 * i + 1]<<endl;
      assert(test_output[i] == resu);
    }
  }

  /**
   * 10. Regular Expression Matching
   * Given an input string (s) and a pattern (p), implement regular expression
   * matching with support for '.' and '*'.
   * '.' Matches any single character.
   * '*' Matches zero or more of the preceding element.
   * The matching should cover the entire input string (not partial).
   * Note:
   * - s could be empty and contains only lowercase letters a-z.
   * - p could be empty and contains only lowercase letters a-z, like . or *.
   *   { "aa", "a", "aa", "a*", "ab", ".*", "aab", "c*a*b", "mississippi", "mis*is*p*." }
   *   { false, true, true, true, false }
   */
  static bool is_regex_matched(const string & text, const string & pattern) {
    if (pattern.empty()) { return text.empty(); }

    if (text.empty()) {
      if (pattern.size() % 2 != 0) { return false; }
      for (int i = 1; i < pattern.size(); i += 2) { if ('*' != pattern[i]) { return false; } }
      return true;
    }

    vector<vector<bool>> mlookup(text.size() + 1, vector<bool>(pattern.size() + 1, false));
    mlookup[0][0] = true;
    for (int i = 1; i < pattern.size(); i += 2) {
      if ('*' == pattern[i]) { mlookup[0][i + 1] = true; } else { break; }
    }
    for (int i = 1; i <= text.size(); i++) {
      for (int j = 1; j <= pattern.size(); j++) {
        if ('*' == pattern[j - 1]) {
          if (j >= 2) {
            /* star matches zero or simply do nothing. */
            mlookup[i][j] = mlookup[i][j] || (mlookup[i][j - 2]) || (mlookup[i][j - 1]);
            /* start match with one or more of the preceding element, say aa & a*, (a)* == a(a) && a - a* */
            mlookup[i][j] = mlookup[i][j] || (
              ((pattern[j - 2] == '.') || (text[i - 1] == pattern[j - 2])) && (mlookup[i - 1][j])
            );
          }
        } else if ('.'== pattern[j - 1] || pattern[j - 1] == text[i - 1]) {
          mlookup[i][j] = mlookup[i - 1][j - 1];
        }
      }
    }
    return mlookup.back().back();
  }

  static void test_is_regex_matched() {
    string test_input[] = { "b", "ab*b", "aaa", "ab*ac*a", "a", "..*", "", "x*", "", "x*x*x*", "", "", "x", "", "aa", "a", "aa", "a*", "ab", ".*", "aab", "c*a*b", "mississippi", "mis*is*p*.", "ssip", "s*p", "mississ", "mis*is*", "mississi", "mis*is*", "aaa", "ab*a*c*a", "aaa", ".*" };
    bool test_output[] = { false, true, true, true, true, true, false, false, true, true, true, false, false, true, false, true, true };
    cout << "15. test_is_regex_matched" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(test_output[0]); i++) {
      bool resu = is_regex_matched(test_input[2 * i], test_input[2 * i + 1]);
      cout << test_output[i] << " <=> " << resu << " | " << test_input[2 * i] << " : " << test_input[2 * i + 1]<<endl;
      assert(test_output[i] == resu);
    }
  }

  /**
   * 472. Concatenated Words
   * - Given a list of words (without duplicates), please write a program that
   *   returns all concatenated words in the given list of words.
   * - A concatenated word is defined as a string that is comprised entirely of
   *   at least two shorter words in the given array.
   * Example:
   * - Input: ["cat","cats","catsdogcats","dog","dogcatsdog",
   *           "hippopotamuses","rat","ratcatdogcat"]
   *   Output: ["catsdogcats","dogcatsdog","ratcatdogcat"]
   *   catsdogcats : build up suffices O(m^2), n words & n pieces for -> O(n^2m^2)
   *             s (cats, catsdogcats, hippopotamuses)
   *            ts
   *           ats
   *          cats x
   *         gcats
   *        ogcats
   *       dogcats x -> trie contains all suffices of all words.
   *      sdogcats
   *     tsdogcats
   *    atsdogcats
   *   catsdogcats x
   *   suffix trie is too expensive & heavy.
   *   build a prefix trie for all words & do dfs for every word
   *            v  v
   *   { a abbc abbcd bbcd }
   * - actually a dfs should be good with available entries on each level of a path
   * - say we have a prefix trie, then we need apply DP to find a path to the end.
   * - for a word w[0..j], token[0..i] matched & token[i+1..j] can also be matched
   * Explanation:
   * - "catsdogcats" can be concatenated by "cats", "dog" and "cats"; 
   * - "dogcatsdog" can be concatenated by "dog", "cats" and "dog"; 
   * - "ratcatdogcat" can be concatenated by "rat", "cat", "dog" and "cat".
   * Note:
   * - The number of elements of the given array will not exceed 10,000
   * - The length sum of elements in the given array will not exceed 600,000.
   * - All the input string will only include lower case letters.
   * - The returned elements order does not matter.
   * Intuition:
   * - bf -> for every word, check if it is a concat from others.
   * - problem being that even check a single word is very expensive.
   * - we may want to optimize & figure out the redundant calc.
   * - seems word could be reused # times & its order does not matter.
   *   (unlikely to concat all tokens & run DP)
   * - then parse each word to a graph?
   * - { a b ab c d cd abcd abc abcc }
   *   base token: a  b    c  d
   *               +--^ +---^
   *   concat tok: ab   |  cd
   *               ^----+   |
   *               abc  +--abcd
   *               |----+
   *               abcc
   * - think of the size of each token? only to prune out some cases.
   * - the problem only require us to return true | false for any words
   *   instead of actual breakdown, so still DP?
   * - sort the input 1st such that we know what to do?
   * - build a suffix tree for every word?
   */
  const static int MAX_PREFIX_TRIE_CHAR_CNT = (int)'z' - (int)'a' + 1;

  class prefix_trie_vertex {
  public:
    prefix_trie_vertex(char val = '\0', int idx = -1) : value(val), word_idx(idx) {}
    virtual ~prefix_trie_vertex() {}
    prefix_trie_vertex * trie_ptrs[MAX_PREFIX_TRIE_CHAR_CNT] = { NULL };
    char value; int word_idx;
  };

  static prefix_trie_vertex * gen_prefix_trie(const vector<string> & words) {
    prefix_trie_vertex * trie_root_ptr = new prefix_trie_vertex(),
                       * trie_curr_ptr = NULL;
    assert(NULL != trie_root_ptr);

    for (int j = 0; j < words.size(); j++) {
      const string & word = words[j];
      trie_curr_ptr = trie_root_ptr;
      for (int i = 0; i < word.size(); i++) {
        char chr = word[i];
        int chr_idx = (int)chr - (int)'a', word_idx = -1;
        if (NULL == trie_curr_ptr->trie_ptrs[chr_idx]) {
          trie_curr_ptr->trie_ptrs[chr_idx] = new prefix_trie_vertex(chr, word_idx);
          assert(NULL != trie_curr_ptr->trie_ptrs[chr_idx]);
        }
        trie_curr_ptr = trie_curr_ptr->trie_ptrs[chr_idx];
        if (word.size() - 1 == i) { trie_curr_ptr->word_idx = j; }
      }
    }
    return trie_root_ptr;
  }

  static void delete_prefix_trie(prefix_trie_vertex * trie_root_ptr) {
    if (NULL == trie_root_ptr) { return; }
    vector<prefix_trie_vertex *> trie_ptr_buffer({ trie_root_ptr });
    prefix_trie_vertex * trie_ptr_to_del = NULL;
    while(false == trie_ptr_buffer.empty()) {
      trie_ptr_to_del = trie_ptr_buffer.back();
      trie_ptr_buffer.pop_back();
      for (int i = 0; i < (sizeof(trie_ptr_to_del->trie_ptrs) /
                           sizeof(prefix_trie_vertex *)); i++) {
        if (NULL != trie_ptr_to_del->trie_ptrs[i]) {
          trie_ptr_buffer.push_back(trie_ptr_to_del->trie_ptrs[i]);
        }
      }
      delete trie_ptr_to_del;
    }
  }

  static void print_prefix_trie(prefix_trie_vertex * trie_root_ptr) {
    if (NULL == trie_root_ptr) { return; }
    vector<prefix_trie_vertex *> trie_ptr_buffer({ trie_root_ptr });
    prefix_trie_vertex * trie_ptr_to_del = NULL;
    while(false == trie_ptr_buffer.empty()) {
      trie_ptr_to_del = trie_ptr_buffer.back();
      trie_ptr_buffer.pop_back();
      for (int i = 0; i < (sizeof(trie_ptr_to_del->trie_ptrs) /
                           sizeof(prefix_trie_vertex *)); i++) {
        if (NULL != trie_ptr_to_del->trie_ptrs[i]) {
          trie_ptr_buffer.push_back(trie_ptr_to_del->trie_ptrs[i]);
        }
      }
      cout << trie_ptr_to_del->value << " : ";
    }
    cout << endl;
  }

  static bool is_word_valid_recur(const vector<string> & words,
                                  int curr_word_idx, int curr_char_idx, int token_cnt,
                                  prefix_trie_vertex * trie_root_ptr) {
    const string & word = words[curr_word_idx];

    if (curr_char_idx >= word.size()) {
      if (2 <= token_cnt){ return true; } else { return false; }
    }
    /* curr word idx (char) -> curr trie node ptr */
    prefix_trie_vertex * curr_trie_ptr = trie_root_ptr->trie_ptrs[(int)word[curr_char_idx] - 'a'];
    while (NULL != curr_trie_ptr && curr_trie_ptr->value == word[curr_char_idx]) {
      if (curr_word_idx != curr_trie_ptr->word_idx && curr_trie_ptr->word_idx >= 0) {
        if (is_word_valid_recur(words, curr_word_idx, curr_char_idx + 1, token_cnt + 1, trie_root_ptr)) { return true; }
      }
      curr_char_idx++; if (curr_char_idx >= word.size()) { break; }
      curr_trie_ptr = curr_trie_ptr->trie_ptrs[(int)word[curr_char_idx] - 'a'];
    }
    return false;
  }

  static vector<string> find_all_concat_words(const vector<string> & words) {
    vector<string> concat_words;
    prefix_trie_vertex * trie_root_ptr = gen_prefix_trie(words);
    for (int i = 0; i < words.size(); i++) {
      if (is_word_valid_recur(words, i, 0, 0, trie_root_ptr)) { concat_words.push_back(words[i]); }
    }
    delete_prefix_trie(trie_root_ptr);
    return concat_words;
  }

  static void test_find_all_concat_words() {
    cout << "16. test_find_all_concat_words" << endl;
    vector<string> result;
    vector<vector<string>> test_output = { { "catsdogcats","dogcatsdog","ratcatdogcat" }, { "ab", "cd", "abcd", "abc", "abcc" }, { "abbcd" } };
    vector<vector<string>> test_input = { { "cat","cats","catsdogcats","dog","dogcatsdog","hippopotamuses","rat","ratcatdogcat" }, { "a", "b", "ab", "c", "d", "cd", "abcd", "abc", "abcc" }, { "a", "abbc", "abbcd", "bbcd" } };
    for (int i = 0; i < test_input.size(); i++) {
      result = find_all_concat_words(test_input[i]);
      sort(result.begin(), result.end());
      sort(test_output[i].begin(), test_output[i].end());
      print_all_elem<string>(result); cout << "<=>" << endl;
      print_all_elem<string>(test_output[i]);
    }
  }

  /**
   * 336. Palindrome Pairs
   * Given a list of unique words, find all pairs of distinct indices (i, j) in
   * the given list, so that the concatenation of the two words
   * i.e. words[i] + words[j] is a palindrome.
   * Example 1:
   * Input: ["abcd","dcba","lls","s","sssll"]
   * Output: [[0,1],[1,0],[3,2],[2,4]] 
   * Explanation: The palindromes are ["dcbaabcd","abcddcba","slls","llssssll"]
   * Example 2:
   * Input: ["bat","tab","cat"]
   * Output: [[0,1],[1,0]] 
   * Explanation: The palindromes are ["battab","tabbat"]
   * Intuition:
   * - BF -> for every pair O(n2), check its mirror property -> T(n) ~ O(n2*m)
   * - problem scope: worst case, all pairs are palindrome -> T(n) >= O(n2)
   * - optimization may relies on avoid redundant checking?
   * - abc + cba is good, not vice versa (same as abc & ba)
   * - directed graph? (bit too far)
   * - instead of matching against all diff word, matching all possible tokens
   * - for abccacc, start from end pos right, expanding left
   *     if str[j..n] is a palindrome, then check if we have str[0..j - 1].reverse
   * - after all done, check str.reverse.
   */
  static bool is_str_palin(const string & input, int i, int j) {
    if (input.empty()) { return true; }
    for (; i <= j; i++, j--) {
      if (input[i] != input[j]) { return false; }
    }
    return true;
  }

  static vector<vector<int>> find_all_palindrome_pairs(const vector<string> & words) {
    vector<vector<int>> palin_pairs;
    if (words.empty()) { return palin_pairs; }

    unordered_map<string, int> token_to_idx_map;
    unordered_map<string, int>::iterator entry_itr;
    for (int i = 0; i < words.size(); i++) { token_to_idx_map[words[i]] = i; }

    string curr_token, remaining_part;
    for (int i = 0; i < words.size(); i++) {
      curr_token = words[i];
      for (int j = curr_token.size() - 1; j >= 0; j--) {
        if (!is_str_palin(curr_token, j, curr_token.size() - 1)) { continue; }
        if (j == 0) { remaining_part = ""; } else {
          remaining_part = curr_token.substr(0, j);
          reverse(remaining_part.begin(), remaining_part.end());
        }
        entry_itr = token_to_idx_map.find(remaining_part);
        if (token_to_idx_map.end() != entry_itr && i != entry_itr->second) {
          palin_pairs.push_back({ i, entry_itr->second });
        }
      }
      for (int j = 0; j < curr_token.size(); j++) {
        if (!is_str_palin(curr_token, 0, j)) { continue; }
        if (j == curr_token.size() - 1) {
          remaining_part = "";
        } else {
          remaining_part = curr_token.substr(j + 1);
          reverse(remaining_part.begin(), remaining_part.end());
        }
        entry_itr = token_to_idx_map.find(remaining_part);
        if (token_to_idx_map.end() != entry_itr && i != entry_itr->second) {
          palin_pairs.push_back({ entry_itr->second, i });
        }
      }

      remaining_part = curr_token;
      reverse(remaining_part.begin(), remaining_part.end());
      entry_itr = token_to_idx_map.find(remaining_part);
      if (token_to_idx_map.end() != entry_itr && i != entry_itr->second) {
        palin_pairs.push_back({ i, entry_itr->second });
      }
    }

    return palin_pairs;
  }

  static void test_find_all_palindrome_pairs() {
    cout << "17. test_find_all_palindrome_pairs" << endl;
    vector<vector<int>> result;
    vector<vector<string>> test_input = { { "abcd","dcba","lls","s","sssll" }, {"bat","tab","cat"} };
    vector<vector<vector<int>>> test_output { {{0,1},{1,0},{3,2},{2,4}}, {{0,1},{1,0}} };
    for (int i = 0; i < test_input.size(); i++) {
      result = find_all_palindrome_pairs(test_input[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < test_output[i].size(); j++) { assert(result[j] == test_output[i][j]); }
    }
  }

  /**
   * 892. Alien Dictionary
   * - There is a new alien language which uses the latin alphabet. However,
   *   the order among letters are unknown to you. You receive a list of
   *   non-empty words from the dictionary, where words are sorted
   *   lexicographically by the rules of this new language. Derive the order
   *   of letters in this language.
   *
   * - You may assume all letters are in lowercase.
   * - You may assume that if a is a prefix of b, then a must appear before b
   *   in the given dictionary.
   * - If the order is invalid, return an empty string.
   * - There may be multiple valid order of letters, return the smallest in
   *   lexicographical order
   *
   * Example
   * Given the following words in dictionary,
   * { "wrt", "wrf", "er", "ett", "rftt" }
   * The correct order is: "wertf"
   * Given the following words in dictionary,
   * { "z", "x" }
   * The correct order is: "zx".
   *
   * Intuition:
   * - alphabet are included in dictionary, which also give out order of words
   * - by each pair of words, we can derive order of each char (at least some)
   * - essentially a dag, where each each node is a char, our goal is:
   *   1. derive whole dag with in-out degrees
   *   2. serialize whole dag by topological sorting (if we can).
   * - could be simplified as we only have 26 chars, use array.
   */
  const static int VERTEX_CNT = (int)'z' - (int)'a' + 1;
  class dag_vertex {
  public:
    dag_vertex(int v = -1) : vid(v) {}
    virtual ~dag_vertex() {}
    int vid;
    unordered_set<int> incoming_vids;
    unordered_set<int> outgoing_vids;
  };
  static int get_char_idx(const char c) { return int(c) - int('a'); }
  static string calc_correct_order(vector<string> & words) {
    string char_order_str;
    if (words.empty()) { return char_order_str; }

    unordered_set<char> char_lookup;
    /* 1. try to derive all possible edges, char_dag[i][j] -> 1 means char i < char j */
    vector<dag_vertex> char_dag;
    for (int i = 0; i < VERTEX_CNT; i++) { char_dag.push_back(dag_vertex(i)); }
    int l_token_id = 0, r_token_id = 0, total_link_cnt = 0, l_vid = 0, r_vid = 0;
    for (int i = 0; i < words.size(); i++) {
      for (int j = i + 1; j < words.size(); j++) {
        l_token_id = 0, r_token_id = 0;
        while(l_token_id < words[i].size() && r_token_id < words[j].size() &&
              words[i][l_token_id] == words[j][r_token_id]) {
          l_token_id += 1; r_token_id += 1;
        }
        if (!(l_token_id < words[i].size() && r_token_id < words[j].size())) { continue; }
        char_lookup.insert(words[i][l_token_id]);
        char_lookup.insert(words[j][r_token_id]);
        l_vid = get_char_idx(words[i][l_token_id]);
        r_vid = get_char_idx(words[j][r_token_id]);
        if (char_dag[l_vid].outgoing_vids.count(r_vid) == 0) {
          total_link_cnt += 1;
          char_dag[l_vid].outgoing_vids.insert(r_vid);
          char_dag[r_vid].incoming_vids.insert(l_vid);
        }
      }
    }
    for (int i = 0; i < words.size(); i++) {
      for (int k = 0; k < words[i].size(); k++) {
        l_vid = get_char_idx(words[i][k]);
        if (char_dag[l_vid].outgoing_vids.empty() && char_dag[l_vid].incoming_vids.empty()) {
          for (int j = 0; j < l_vid; j++) {
            if (char_lookup.count(char(j + int('a'))) == 0) { continue; }
            total_link_cnt += 1;
            char_dag[j].outgoing_vids.insert(l_vid);
            char_dag[l_vid].incoming_vids.insert(j);
          }
          for (int j = l_vid + 1; j < VERTEX_CNT; j++) {
            if (char_lookup.count(char(j + int('a'))) == 0) { continue; }
            total_link_cnt += 1;
            char_dag[l_vid].outgoing_vids.insert(j);
            char_dag[j].incoming_vids.insert(l_vid);
          }
        }
      }
    }

    if (char_lookup.size() == 1) {
      char_order_str.append(1, * char_lookup.begin()); return char_order_str;
    }
    /* 2. beyond this, char_dag have all chars & edges, start toplogical sort */
    dag_vertex start_vertex;
    for (auto & v : char_dag) {
      if (v.outgoing_vids.size() > 0 && v.incoming_vids.size() == 0) { start_vertex = v; break; }
    }
    if (start_vertex.vid < 0) { return char_order_str; }

    vector<dag_vertex> vid_buffer = { start_vertex };
    while (!vid_buffer.empty()) {
      start_vertex = vid_buffer.back();
      char_order_str.append(1, (char)(start_vertex.vid + (int)('a')));
      vid_buffer.pop_back();
      for (int vid = 0; vid < VERTEX_CNT; vid++) {
        if (start_vertex.outgoing_vids.count(vid) == 0) { continue; }
        char_dag[vid].incoming_vids.erase(start_vertex.vid);
        total_link_cnt -= 1;
        if (char_dag[vid].incoming_vids.empty()) {
          vid_buffer.push_back(char_dag[vid]);
        }
      }
    }

    if (total_link_cnt != 0) { char_order_str = ""; }

    return char_order_str;
  }

  static void test_calc_correct_order() {
    string result;
    vector<string> test_output = { "wertf", "zx", "yxz" };
    vector<vector<string>> test_input = { { "wrt", "wrf", "er", "ett", "rftt" }, { "z", "x" }, { "zy", "zx" }};
    cout << "18. test_calc_correct_order" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_correct_order(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 829. Word Pattern II
   * Description
   * - Given a pattern and a string str, find if str follows the same pattern.
   * - Here follow means a full match, such that there is a bijection
   *   between a letter in pattern and a non-empty substring in str.
   *   (i.e if a corresponds to s, then b cannot correspond to s. For example,
   *    given pattern = "ab", str = "ss", return false.)
   * - You may assume both pattern and str contains only lowercase letters.
   * Example
   * - Given pattern = "abab", str = "redblueredblue", return true.
   * - Given pattern = "aaaa", str = "asdasdasdasd", return true.
   * - Given pattern = "aabb", str = "xyzabcxzyabc", return false.
   * Intuition:
   * - given a pattern, say pat, judge if str conforms to it.
   * - string problem & pattern related (chr in pat can be replaced with substr)
   * - only true | false required, DP?
   * - bijection -> pat[i] <=> substr[i..j] in text.
   * - let check_pattern_lookup(i, j) denotes substr[0..j] conforms with pat[0..i]
   *   goal is to calc check_pattern_lookup(m, n)
   * - to calc check_pattern_lookup(i, j), we already know check_pattern_lookup(i - 1, 0..j - 1)
   *   for k in (0..j - 1) exist a k, such that
   *     check_pattern_lookup(i - 1, k) is good, 
   *     pattern[i] <=> substr(k + 1, j) does not conflicts with prev mapping.
   * - key is to detect any conflicts in bijection mapping in state transition,
   *   then recur + map? also maintain the curr bijection will be a hard part
   * - base case:
   *   when i = 0, always true, as any substr can be a pattern.
   *   when i < j, always false, as not enough substr can be assigned.
   * - inheriently, DFS like recur & curr bijection would be easier to implement.
   *   due to the complexity to maintain curr bijection.
   *     x y z a b c x z y a b c
   *   a 1 1 1 1 1 1 1 1 1 1 1 1
   *   a 0 0 0
   *   b (a-x), (a-y) | a -> xy, a -> z, or a -> x, a -> yz
   *   b
  static void is_word_follows_pattern_recur(const string & pattern,
                                            const string & str, int i, int j,
                                            vector<vector<bool>> & check_pattern_lookup,
                                            unordered_map<char, string> & curr_bijection) {
    if (i >= pattern.size() || j >= str.size() || check_pattern_lookup.back().back()) { return; }

    int next_i = i, next_j = j + 1;
    if (next_j >= str.size()) { next_j = 0; next_i += 1; };

    if (i <= j) {
      // curr pat k -> pat[i] <--> token -> substr[j], token starts between i & j
      for (int k = i; k <= j; k++) {
        // check any conflicts for mapping of pat[i] -> substr
        if ((0 == i || true == check_pattern_lookup[i - 1][k - 1]) &&
            (curr_bijection.count(pattern[i]) == 0 ||
             curr_bijection[pattern[i]] == str.substr(k, k - i + 1))) {
          curr_bijection[pattern[i]] = str.substr(k, k - i + 1);
          check_pattern_lookup[i][j] = true;
          is_word_follows_pattern_recur(pattern, str, next_i, next_j, check_pattern_lookup, curr_bijection);
          if (check_pattern_lookup.back().back()) { return; }
          check_pattern_lookup[i][j] = false;
          curr_bijection.erase(pattern[i]);
        }
      }
    }
    is_word_follows_pattern_recur(pattern, str, next_i, next_j, check_pattern_lookup, curr_bijection);
  }
   */
  static bool is_word_follows_pattern_recur(const string & pattern, int pid,
                                            const string & str, int sid,
                                            unordered_set<string> & tested_substr_set,
                                            unordered_map<char, string> & curr_bijection) {
    if (str.empty() || pattern.empty() || str.size() < pattern.size()) { return false; }

    /* reach the end at the same time, return true */
    if (pid == pattern.size() && sid == str.size()) { return true; }
    if (pid == pattern.size() || sid == str.size()) { return false; }

    /* curr pattern k already existed in mapping, then just check the substr */
    if (curr_bijection.count(pattern[pid]) > 0) {
      string curr_token = curr_bijection[pattern[pid]];
      string curr_subst = str.substr(sid, curr_token.size());
      if (curr_subst == curr_token) {
        return is_word_follows_pattern_recur(
          pattern, pid + 1, str, sid + curr_token.size(), tested_substr_set, curr_bijection
        );
      }
    } else {
    /* curr pattern k not existed in mapping, then just check all possible substr */
      for (int end_pos = sid; end_pos < str.size(); end_pos++) {
        string curr_subst = str.substr(sid, end_pos - sid + 1);
        if (tested_substr_set.count(curr_subst) > 0) { continue; }
        tested_substr_set.insert(curr_subst);
        curr_bijection[pattern[pid]] = curr_subst;
        if (is_word_follows_pattern_recur(pattern, pid + 1, str, end_pos + 1,
                                          tested_substr_set, curr_bijection)) { return true; }
        curr_bijection.erase(pattern[pid]);
        tested_substr_set.erase(curr_subst);
      }
    }
    return false;
  }

  static bool is_word_follows_pattern(const string & pattern, const string & str) {
    unordered_set<string> tested_substr_set;
    unordered_map<char, string> curr_bijection;
    return is_word_follows_pattern_recur(pattern, 0, str, 0, tested_substr_set, curr_bijection);
  }

  static void test_is_word_follows_pattern() {
    cout << "19. test_is_word_follows_pattern" << endl;
    bool result;
    vector<bool> test_output = { true, true, true, false };
    vector<vector<string>> test_input = { { "abba", "redbluebluered" }, { "abab", "redblueredblue" }, { "aaaa", "asdasdasdasd" }, { "aabb", "xyzabcxzyabc" } };
    for (int i = 0; i < test_input.size(); i++) {
      result = is_word_follows_pattern(test_input[i].front(), test_input[i].back());
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }
};


int main(void) {
  using string_util::test_find_word_in_batch;
  using string_util::test_get_shortest_palindrome;
  using string_util::test_get_longest_palindrome;
  using string_util::test_gen_shortest_palindrome;
  using string_util::test_reverse_words;
  using string_util::test_reverse_words_in_place;
  using string_util::test_is_interleave;
  using string_util::test_fast_is_interleave;
  using string_util::test_get_min_window;
  using string_util::test_fast_min_window;
  using string_util::test_cnt_distinct_subseqs;
  using string_util::test_simplify_path;
  using string_util::test_add_binary;
  using string_util::test_calc_longest_valid_parentheses;
  using string_util::test_is_wild_card_matched;
  using string_util::test_is_regex_matched;
  using string_util::test_is_number_valid;
  using string_util::test_find_all_concat_words;
  using string_util::test_find_all_palindrome_pairs;
  using string_util::test_calc_correct_order;
  using string_util::test_is_word_follows_pattern;

  test_find_word_in_batch();
  test_get_shortest_palindrome();
  test_get_longest_palindrome();
  test_gen_shortest_palindrome();
  test_reverse_words();
  test_reverse_words_in_place();
  test_is_interleave();
  test_fast_is_interleave();
  test_get_min_window();
  test_fast_min_window();
  test_cnt_distinct_subseqs();
  test_simplify_path();
  test_add_binary();
  test_calc_longest_valid_parentheses();
  test_is_wild_card_matched();
  test_is_regex_matched();
  test_is_number_valid();
  test_find_all_concat_words();
  test_find_all_palindrome_pairs();
  test_calc_correct_order();
  test_is_word_follows_pattern();

  return 0;
}
