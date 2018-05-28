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

class ChoresUtil{
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
   * 15.14 Text Justification
   * Given an array of words and a length L, format the text such that each
   * line has exactly L characters and is fully (left and right) justified.
   * You should pack your words in a greedy approach; that is, pack as many
   * words as you can in each line. Pad extra spaces ' ' when necessary so that
   * each line has exactly L characters.
   * - Extra spaces between words should be distributed as evenly as possible.
   *   If the number of spaces on a line do not divide evenly between words, the
   *   empty slots on the left will be assigned more spaces than the slots on
   *   the right.
   * - For the last line of text, it should be left justified and no extra space
   *   is inserted between words. For example, words:
   *   ["This", "is", "an", "example", "of", "text", "justification."] L: 16.
   *   Return the formatted lines as:
   *   [ "This    is    an", "example  of text", "justification.  " ]
   * Note:
   * - Each word is guaranteed not to exceed L in length. Corner Cases:
   *   A line other than the last line might contain only one word. What should
   *   you do in this case? In this case, that line should be left
   * - Min space between each word is 1
   */
  static vector<string> justify_text(const vector<string> text, int max_len) {
    vector<string> justified_text;
    int curr_pos = 0, curr_line_len = 0,
        curr_words_cnt = 0, curr_words_len = 0,
        start_pos = 0, words_cnt = text.size();
    for (; curr_pos <= words_cnt; curr_pos++) {

      if (curr_pos < words_cnt) {
        curr_line_len = curr_words_cnt + curr_words_len + text[curr_pos].size();
      }
      if ((curr_line_len > max_len) || (curr_pos == words_cnt)) {
        string line = "";
        if (curr_words_cnt > 1) {
          // calc padding for words from start_pos ~ curr_pos - 1
          int space_left = max_len - curr_words_len - (curr_words_cnt - 1);
          // 3 / 8 => 0, 13 / 8 => 1, 16 / 8 => 2
          int extra_pads = space_left / (curr_words_cnt - 1);
          int extra_fill = (space_left - extra_pads * (curr_words_cnt - 1));
          for (int i = start_pos; i < curr_pos; i++) {
            line += text[i];
            if (i == curr_pos - 1) { continue; }
            for (int c = 0; c <= extra_pads; c++){ line += " "; }
            if (extra_fill > 0) { line += " "; extra_fill--; }
          }
        } else {
          line = text[start_pos];
          for (int i = 0; i <= max_len - line.size(); i++) { line += " "; }
        }
        justified_text.push_back(line);

        if (curr_pos < words_cnt) {
          start_pos = curr_pos;
          curr_words_cnt = 1;
          curr_words_len = text[curr_pos].size();
        }
      } else {
        curr_words_cnt += 1;
        curr_words_len += text[curr_pos].size();
      }
    }
    return justified_text;
  }
};

int main(void) {
  vector<string> input_text = vector<string>(
    { "This", "is", "an", "example", "of", "text",
      "justification.", "justi", "fication." }
  );
  vector<string> output_txt = vector<string>({ "This    is    an",
                                               "example  of text",
                                               "justification.  ",
                                               "justi  fication." });
  int max_len = 16;
  ChoresUtil::print_all_elem<string>(input_text);
  cout << "max-len-for-adjustment: " << max_len << endl;
  ChoresUtil::print_all_elem<string>(output_txt);
  ChoresUtil::print_all_elem<string>(
    ChoresUtil::justify_text(input_text, max_len)
  );
  return 0;
}
