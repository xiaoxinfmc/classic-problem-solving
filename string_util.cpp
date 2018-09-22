#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <unordered_set>

namespace string_util {
  using std::cout;
  using std::endl;
  using std::string;
  using std::vector;
  using std::unordered_set;

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

  static string get_longest_palindrome(string input_str) {
    string text = insert_delim(input_str);
    vector<int> palin_boundary_lookup(text.size(), 0);

    int curr_radius = 0, curr_pivot = 0, mirror_pivot = 0;
    int left_boundary = 0, right_boundary = 0;
    int left_idx = 0, right_idx = 0;
    int max_palin_id = 0, max_palin_len = 0;
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
      if (max_palin_len < palin_boundary_lookup[i]) {
        max_palin_id = i; max_palin_len = palin_boundary_lookup[i];
      }
    }
    string token = text.substr(max_palin_id + 1, max_palin_len);
    string result = token;
    reverse(token.begin(), token.end());
    result = remove_delim(token + string(1, text[max_palin_id]) + result);
    return result;
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
};

int main(void) {
  string_util::test_find_word_in_batch();
  string_util::test_get_shortest_palindrome();
  string_util::test_get_longest_palindrome();
  return 0;
}
