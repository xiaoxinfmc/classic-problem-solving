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
#include <bitset>

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
   * 15.1 Reverse Integer
   * Reverse digits of an integer.
   * - Example1: x = 123, return 321
   *   Example2: x = -123, return -321
   * Have you thought about this?
   * - Here are some good questions to ask before coding. Bonus points for you
   *   if you have already thought through this! If the integer’s last digit is
   *   0, what should the output be? ie, cases such as 10, 100. Did you notice
   *   that the reversed integer might overflow? Assume the input is a 32-bit
   *   integer, then the reverse of 1000000003 overflows. How should you handle
   *   such cases? Throw an exception? Good, but what if throwing an exception
   *   is not an option? You would then have to re-design the function
   *   (ie, add an extra parameter).
   */
  static int reverse_integer(long input) {
    long long flipped_num = 0;
    long long postive_max_abs_limit = INT_MAX;
    long long negtive_max_abs_limit = INT_MAX + 1;
    // 1003 -> 3, 3 * 10 + 0, 30 * 10 + 0, 300 * 10 + 1
    for (int digit = 0; (input / (int)pow(10, digit)) != 0; digit++) {
      flipped_num = flipped_num * 10 + ((input / (int)pow(10, digit)) % 10);
    }
    flipped_num = abs(flipped_num);
    bool is_negative = (input < 0);
    if (((true == is_negative) && (flipped_num > negtive_max_abs_limit)) ||
        ((false == is_negative) && (flipped_num > postive_max_abs_limit))) {
      flipped_num = 0;
    } else {
      flipped_num = (true == is_negative) ? -1 * flipped_num : flipped_num;
    }
    // positive-int: 0111 1111 1111 1111 1111 1111 1111 1111 -> 2^31 - 1
    // negative-int: 1000 0000 0000 0000 0000 0000 0000 0000 -> -2......
    // negative-int: 1111 1111 1111 1111 1111 1111 1111 1111 -> -1
    // negative-int: 1111 1111 1111 1111 1111 1111 1111 1110 -> -2
    return flipped_num;
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
  /**
   * 15.15 Max Points on a Line
   * Given n points on a 2D plane, find the maximum number of points that lie on
   * the same straight line.
   */
  class Point{ };
  static int max_points_per_line(const vector<Point> & points_arr) {
    return 0;
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
  int x = pow(2, 30);
  // 0111 1111 1111 1111 1111 1111 1111 1111
  cout << INT_MAX << endl;
  // 0111 1111 1111 1111 1111 1111 1111 1111
  cout << x - 1 << endl;
  // 1000 0000 0000 0000 0000 0000 0000 0000
  cout << x << endl;
  // 1000 0000 0000 0000 0000 0000 0000 0001
  cout << x + 1 << endl;
  // 1000 0000 0000 0000 0000 0000 0000 0010
  cout << x + 2 << endl;
  // 0000 0000 0000 0000 0000 0000 0000 0000
  cout << x * 2 << endl;
  // 0000 0000 0000 0000 0000 0000 0000 0000
  cout << (x << 1) << endl;
  // 0100 0000 0000 0000 0000 0000 0000 0000
  cout << (x >> 1) << endl;
  cout << int(pow(2, 30)) << endl;
  bitset<sizeof(int) * CHAR_BIT> y(x);
  cout << "int-byte-size: " << sizeof(int) * CHAR_BIT << " : raw : " << y << endl;
  y = (x << 1);
  cout << "int-byte-size: " << sizeof(int) * CHAR_BIT << " : l-s : " << y << endl;
  y = (x >> 1);
  cout << "int-byte-size: " << sizeof(int) * CHAR_BIT << " : r-s : " << y << endl;
  y = (int)-1;
  cout << "int-byte-size: " << sizeof(int) * CHAR_BIT << " : n-1 : " << y << endl;
  y = (int)-2;
  cout << "int-byte-size: " << sizeof(int) * CHAR_BIT << " : n-2 : " << y << endl;
  y = (int)0;
  cout << "int-byte-size: " << sizeof(int) * CHAR_BIT << " : n-0 : " << y << endl;
  cout << sizeof(int) * CHAR_BIT << endl;
  cout << sizeof(long) * CHAR_BIT << endl;

  cout << (-1 % 10) << endl;
  cout << "==>> 1003: " << ChoresUtil::reverse_integer(1003) << endl;
  cout << "==>> -1003: " << ChoresUtil::reverse_integer(-1003) << endl;
  cout << "==>> 10: " << ChoresUtil::reverse_integer(10) << endl;
  cout << "==>> 0: " << ChoresUtil::reverse_integer(0) << endl;
  cout << "==>> 1: " << ChoresUtil::reverse_integer(1) << endl;
  cout << "==>> -10: " << ChoresUtil::reverse_integer(-10) << endl;
  cout << "==>> 1000000003: " << ChoresUtil::reverse_integer(1000000003) << endl;
  cout << "==>> -1000000003: " << ChoresUtil::reverse_integer(-1000000003) << endl;
  cout << "==>> int-max: " << ChoresUtil::reverse_integer(INT_MAX) << endl;
  cout << "==>> int-min: " << ChoresUtil::reverse_integer((-1 * long(INT_MAX) + 1)) << endl;
  return 0;
}
