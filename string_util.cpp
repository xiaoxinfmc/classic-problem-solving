#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <cctype>
#include <list>
#include <cmath>

namespace string_util {
  using std::cout;
  using std::endl;
  using std::string;
  using std::vector;
  using std::unordered_set;
  using std::list;
  using std::min;

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

  static void test_get_min_window() {
    string test_input[] = { "ADOBECODEBANC", "ABC", "ABOBECODEBABC", "ABBC", "ABOBCBABX", "ABBC" };
    string test_output[] = { "BANC", "BABC", "BCBA" };
    cout << "9. test_get_min_window" << endl;
    for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
      string resu = get_min_window(test_input[i * 2], test_input[i * 2 + 1]);
      cout << resu << " <=> " << test_output[i] << " <=> " << test_input[i * 2] << " | " << test_input[i * 2 + 1] << endl;
      assert(resu == test_output[i]);
    }
  }
};

int main(void) {
  string_util::test_find_word_in_batch();
  string_util::test_get_shortest_palindrome();
  string_util::test_get_longest_palindrome();
  string_util::test_gen_shortest_palindrome();
  string_util::test_reverse_words();
  string_util::test_reverse_words_in_place();
  string_util::test_is_interleave();
  string_util::test_fast_is_interleave();
  string_util::test_get_min_window();
  return 0;
}
