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

    bool is_ending_char() {
      for (int i = 0; i < sizeof(adjacent_ptrs)/sizeof(adjacent_ptrs[0]); i++) {
        if (adjacent_ptrs[i] != NULL) { return false; }
      }
      return true;
    }

    void mark_as_visited() { is_visited_flag = true; }

    bool is_visited() { return is_visited_flag; }

  private:
    char char_val;
    bool is_visited_flag;
    prefix_trie_node * parent_ptr;
    prefix_trie_node * adjacent_ptrs[int('z') - int('a') + 1];
  };

  static prefix_trie_node * build_prefix_trie(const vector<string> & words) {
    prefix_trie_node * trie_root = NULL;
    if (true == words.empty()) { return trie_root; }
    trie_root = new prefix_trie_node('$');
    assert(NULL != trie_root);
    for (auto & word : words) {
      prefix_trie_node * curr_trie_ptr = trie_root;
      for (auto & chr : word) {
        if (false == curr_trie_ptr->is_adjacent_char_exists(chr)) {
          curr_trie_ptr->insert_adjacent_char(new prefix_trie_node(chr));
        }
        curr_trie_ptr = curr_trie_ptr->get_adjacent_char_ptr(chr);
      }
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
    if (curr_row >= board.size() || curr_col >= board.front().size()) { return; }

    int curr_position_key = gen_position_key(curr_row, curr_col, board);

    if (position_taken.end() != position_taken.find(curr_position_key)) { return; }

    if (trie_root_ptr->get_char_val() != board[curr_row][curr_col]) { return; }

    position_taken.insert(curr_position_key);

    if (true == trie_root_ptr->is_ending_char() && false == trie_root_ptr->is_visited()) {
      string curr_word;
      trie_root_ptr->mark_as_visited();
      for (; NULL != trie_root_ptr->get_parent_ptr();
             trie_root_ptr = trie_root_ptr->get_parent_ptr()) {
        curr_word.push_back(trie_root_ptr->get_char_val());
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
  }
};

int main(void) {
  string_util::test_find_word_in_batch();
  return 0;
}
