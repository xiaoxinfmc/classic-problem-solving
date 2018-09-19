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
    cout << "aaacecaaa <=>" << endl;
    cout << get_shortest_palindrome("aacecaaa") << endl;
    cout << "dcbabcd <=>" << endl;
    cout << get_shortest_palindrome("abcd") << endl;
    cout << "a <=>" << endl;
    cout << get_shortest_palindrome("a") << endl;
    cout << "<=>" << endl;
    cout << get_shortest_palindrome("") << endl;
    cout << "aaacecaaa <=>" << endl;
    cout << get_shortest_palindrome("aaacecaaa") << endl;
  }
};

int main(void) {
  string_util::test_find_word_in_batch();
  string_util::test_get_shortest_palindrome();
  return 0;
}
