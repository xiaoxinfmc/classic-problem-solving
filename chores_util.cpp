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
    long long postive_max_abs_limit = (long)INT_MAX;
    long long negtive_max_abs_limit = (long)INT_MAX + 1;
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
   * 15.2 Palindrome Number
   * Determine whether an integer is a palindrome. Do this without extra space.
   * Some hints:
   * Could negative integers be palindromes? (ie, -1)
   * If you are thinking of converting the integer to string, note the
   * restriction of using extra space. You could also try reversing an integer.
   * However, if you have solved the problem Reverse Integer, you know that the
   * reversed integer might overflow. How would you handle such case? There is
   * a more generic way of solving this problem.
   * -121 -> true, 0 -> true, 101 -> true
   */
  static bool is_palin_number(int num) {
    int digits_cnt = 1;
    for (int divisor = 10; num / divisor != 0; divisor = 10 * divisor) { digits_cnt++; }
    for (int i = 0; i < digits_cnt; i++) {
      if ((num / (int)pow(10, i)) % 10 !=
          (num / (int)pow(10, digits_cnt - i - 1)) % 10) { return false; }
    }
    return true;
  }

  /**
   * 15.3 Insert Interval
   * Given a set of non-overlapping intervals, insert a new interval into the
   * intervals (merge if necessary). You may assume that the intervals were
   * initially sorted according to their start times.
   * Ex1: Given intervals [1,3],[6,9], insert and merge [2,5] in as [1,5],[6,9].
   * Ex2: Given [1,2],[3,5],[6,7],[8,10],[12,16], insert and merge [4,9] in as
   *            [1,2],[3,10],[12,16]. => [1,2], [3, 5 > 4, => (3, 9),
   * This is because the new interval [4,9] overlaps with [3,5],[6,7],[8,10].
   */
  class prob_intv {
  public:
    prob_intv(int s, int e) : start(s), end(e) {}
    virtual ~prob_intv() {}
    friend ostream & operator<< (ostream & os, const prob_intv & intv) {
      os << "[ " << intv.start << ", " << intv.end << " ]"; return os;
    }
    int start, end;
  };
  static vector<prob_intv> merge_interval(vector<prob_intv> intv_arr,
                                          prob_intv new_intv) {
    vector<prob_intv> new_intv_arr; bool is_intv_inserted = false;
    for (vector<prob_intv>::const_iterator itr = intv_arr.begin();
         itr != intv_arr.end(); itr++) {
      if (false == is_intv_inserted) {
        if (new_intv.end < itr->start) { // [ns, ne ] [ x, y ]
          new_intv_arr.push_back(new_intv);
          new_intv_arr.push_back(* itr);
          is_intv_inserted = true;
        } else if (new_intv.start > itr->end){ // [ x, y ] [ ns, ne ]
          new_intv_arr.push_back(* itr);
        } else { // [ x, ns, y, ne ] || [ x, ns, ne, y ], [ns, x, ne, y ], [ ns, x, y, ne ]
          new_intv_arr.push_back(prob_intv(min(itr->start, new_intv.start),
                                           max(itr->end, new_intv.end)));
          is_intv_inserted = true;
        }
      } else {
        if (itr->start < new_intv_arr.back().end) {
          new_intv_arr.back().end = max(itr->end, new_intv_arr.back().end);
        } else {
          new_intv_arr.push_back(* itr);
        }
      }
    }
    return new_intv_arr;
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

  cout << "==>> palin: 1 => 1" << ChoresUtil::is_palin_number(1) << endl;
  cout << "==>> palin: 1 => 11" << ChoresUtil::is_palin_number(11) << endl;
  cout << "==>> palin: 1 => 101" << ChoresUtil::is_palin_number(101) << endl;
  cout << "==>> palin: 1 => 11011" << ChoresUtil::is_palin_number(11011) << endl;
  cout << "==>> palin: 1 => -1" << ChoresUtil::is_palin_number(-1) << endl;
  cout << "==>> palin: 1 => -11" << ChoresUtil::is_palin_number(-11) << endl;
  cout << "==>> palin: 1 => -101" << ChoresUtil::is_palin_number(-101) << endl;
  cout << "==>> palin: 0 => -1000" << ChoresUtil::is_palin_number(-1000) << endl;
  cout << "==>> palin: 0 => -10002" << ChoresUtil::is_palin_number(-10002) << endl;

  /**
   * Ex1: Given intervals [1,3],[6,9], insert and merge [2,5] in as [1,5],[6,9].
   * Ex2: Given [1,2],[3,5],[6,7],[8,10],[12,16], insert and merge [4,9] in as
   *            [1,2],[3,10],[12,16]. => [1,2], [3, 5 > 4, => (3, 9),
   */
  typedef ChoresUtil::prob_intv prob_intv;
  ChoresUtil::print_all_elem<prob_intv>(
    ChoresUtil::merge_interval(
      vector<prob_intv>({ prob_intv(1, 3), prob_intv(6, 9) }), prob_intv(2, 5)
    )
  );
  ChoresUtil::print_all_elem<prob_intv>(
    ChoresUtil::merge_interval(
      vector<prob_intv>({ prob_intv(1, 2), prob_intv(3, 5), prob_intv(6, 7), prob_intv(8, 10), prob_intv(12, 16) }), prob_intv(4, 9)
    )
  );
  ChoresUtil::print_all_elem<prob_intv>(
    ChoresUtil::merge_interval(
      vector<prob_intv>({ prob_intv(1, 2), prob_intv(3, 5), prob_intv(8, 10), prob_intv(12, 16) }), prob_intv(6, 7)
    )
  );
  ChoresUtil::print_all_elem<prob_intv>(
    ChoresUtil::merge_interval(
      vector<prob_intv>({ prob_intv(1, 2), prob_intv(3, 5), prob_intv(8, 10), prob_intv(12, 16) }), prob_intv(0, 17)
    )
  );
  ChoresUtil::print_all_elem<prob_intv>(
    ChoresUtil::merge_interval(
      vector<prob_intv>({ prob_intv(1, 2), prob_intv(3, 5), prob_intv(8, 10), prob_intv(12, 16) }), prob_intv(0, 11)
    )
  );
  ChoresUtil::print_all_elem<prob_intv>(
    ChoresUtil::merge_interval(
      vector<prob_intv>({ prob_intv(1, 2), prob_intv(3, 5), prob_intv(8, 10), prob_intv(12, 16) }), prob_intv(9, 17)
    )
  );
  return 0;
}
