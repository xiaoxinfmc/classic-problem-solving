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
  static void print_all_elem(const vector<Type> & input) {
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
   * 15.5 Minimum Window Substring
   * Given a string S and a string T , find the minimum window in S which will
   * contain all the characters in T in complexity O(n).
   * For example, S = "ADOBECODEBANC", T = "ABC" Minimum window is "BANC".
   * Note:
   * - If there is no such windowin S that covers all characters in T,return the
   *   emtpy string. If there are multiple such windows, you are guaranteed that
   *   there will always be only one unique minimum window in S.
   */
  static string find_min_window(const string text, const string pattern) {
    int char_buffer_size = (int)'z' - (int)'a' + 1;
    int matched_char_cnt = 0, uniq_char_cnt = 0;
    unsigned char char_lookup[char_buffer_size];
    memset(char_lookup, 0, sizeof(char_lookup));
    for (int i = 0; i < pattern.size(); i++) {
      char_lookup[(int)pattern[i] - 'a'] = 1;
    }
    for (int i = 0; i < char_buffer_size; i++) {
      if (1 == char_lookup[i]) { uniq_char_cnt++; }
    }

    int curr_start_pos = 0, curr_end_pos = 0,
        min_start_pos = 0, min_end_pos = INT_MAX;
    for (; curr_end_pos < text.size(); curr_end_pos++) {
      if (1 == char_lookup[text[curr_end_pos] - 'a']) {
        if (0 == matched_char_cnt){ curr_start_pos = curr_end_pos; }
        char_lookup[text[curr_end_pos] - 'a'] = 2;
        matched_char_cnt += 1;
      }
      if (matched_char_cnt == uniq_char_cnt) {
        int uniq_char_matched = 0;
        for (int x = curr_end_pos; x >= curr_start_pos; x--){
          if (2 == char_lookup[text[x] - 'a']) {
            char_lookup[text[x] - 'a'] = 3;
            uniq_char_matched += 1;
          }
          if (uniq_char_matched == uniq_char_cnt) {
            curr_start_pos = x;
            break;
          }
        }
        if (min_end_pos - min_start_pos > curr_end_pos - curr_start_pos) {
          min_start_pos = curr_start_pos; min_end_pos = curr_end_pos;
        }
        for (int i = 0; i < char_buffer_size; i++) {
          if (3 == char_lookup[i]) { char_lookup[i] = 1; }
        }
        curr_start_pos = curr_end_pos + 1;
        matched_char_cnt = 0;
      }
    }
    string window_str = "";
    if (INT_MAX != min_end_pos) {
      window_str = text.substr(min_start_pos, (min_end_pos - min_start_pos + 1));
    }
    return window_str;
  }

  /**
   * 15.6 Multiply Large Numbers represented as Strings
   * Given two numbers as strings. The numbers may be very large (may not fit
   * in long long int), the task is to find sum of these two numbers.
   * Examples:
   * Input : num1 = 4154
   *         num2 = 51454
   * Output : 213779916
   * Input :  num1 =      654154154151454545415415454
   *          num2 = 63516561563156316545145146514654
   * Output : 41549622603955309777243716069997997007620439937711509062916
   *     91
   *    912
   *  ------
   *    182 -> (len-l-num + 1)    |
   *    91          .             |--> len-r-num
   *  819           .             |
   *  ------
   *  82992 -> (len-l-num + len-r-num)
   */
  static string multiply_big_number(const string lnum, const string rnum) {
    bool is_negative = (('-' == lnum[0] && '-' != rnum[0]) ||
                        ('-' != lnum[0] && '-' == rnum[0]));
    string l_num = lnum, r_num = rnum;
    if ('-' == lnum[0]) { l_num = l_num.substr(1); }
    if ('-' == rnum[0]) { r_num = r_num.substr(1); }
    vector<char> digits_arr(l_num.size() + r_num.size(), 0);
    int start_digit_idx = digits_arr.size() - 1;
    int curr_digit_idx = 0, curr_multiple = 0;
    for (int mult_idx = r_num.size() - 1; mult_idx >= 0; mult_idx--) {
      for (int numb_idx = l_num.size() - 1; numb_idx >= 0; numb_idx--) {
        curr_digit_idx = start_digit_idx - (l_num.size() - numb_idx - 1);
        curr_multiple = (get_digit_value(r_num[mult_idx]) *
                         get_digit_value(l_num[numb_idx]));
        check_and_add_up_values(digits_arr, curr_multiple, curr_digit_idx);
      }
      start_digit_idx--;
    }
    for (int i = 0; i < digits_arr.size(); i++) {
      digits_arr[i] = get_char_value(digits_arr[i]);
    }
    string result = ('0' == digits_arr[0]) ? string(digits_arr.begin() + 1, digits_arr.end()) :
                                             string(digits_arr.begin(), digits_arr.end());
    if (true == is_negative) { result = "-" + result; }
    return result;
  }

  static void check_and_add_up_values(vector<char> & existing_digits,
                                      int value, int start_digit_idx) {
    int curr_value = value;
    for (int i = start_digit_idx; i >= 0; i--) {
      curr_value = (existing_digits[i] + curr_value);
      existing_digits[i] = curr_value % 10;
      curr_value = curr_value / 10;
      if (0 == curr_value) { break; }
    }
  }

  static int get_digit_value(char digit) { return ((int)digit - (int)'0'); }

  static char get_char_value(int digit) { return (char)(digit + (int)'0'); }

  /**
   * 15.7 Substring with Concatenation of All Words
   * You are given a string, S, and a list of words, L, that are all of the same
   * length. Find all starting indices of substring(s) in S that is a concat of
   * each word in L exactly once and without any intervening characters.
   * For example, given:
   * S: "barfoothefoobarman"
   * L: ["foo", "bar"]
   * You should return the indices: [0, 9].(order does not matter).
   */
  static vector<int> find_all_tokens(const string text, vector<string> words) {
    vector<int> matching_pos_arr;
    int window_size = words.front().size();
    int appeared_tokens_cnt = 0, total_tokens_cnt = words.size();

    unordered_map<string, int> dict_lookup;
    for (auto & token : words) {
      if (dict_lookup.end() == dict_lookup.find(token)) {
        dict_lookup[token] = 1;
      } else {
        dict_lookup[token] += 1;
      }
    }

    int curr_idx = 0; string curr_token = "";
    for (int i = 0; i < text.size(); i++) {
      if ((text.size() - i) < window_size) { break; }
      curr_token = text.substr(i, window_size);
      if (dict_lookup.end() != dict_lookup.find(curr_token)) {
        appeared_tokens_cnt = 0;
        unordered_map<string, int> curr_dict = dict_lookup;
        for (curr_idx = i; curr_idx < text.size() - window_size + 1; curr_idx += window_size) {
          curr_token = text.substr(curr_idx, window_size);
          if ((curr_dict.end() != curr_dict.find(curr_token)) &&
              (curr_dict[curr_token] > 0)) {
            curr_dict[curr_token] -= 1; appeared_tokens_cnt += 1;
          } else { break; }
        }
        if (appeared_tokens_cnt == total_tokens_cnt) { matching_pos_arr.push_back(i); }
      }
    }

    return matching_pos_arr;
  }

  /**
   * 15.8 Pascal’s Triangle
   * Given numRows, generate the first numRows of Pascal's triangle.
   * For example, given numRows = 5, Return
   * [    [1],           => r from 0 ~ 4, c from 0 ~ r, m[r][c] = m[r-1][c-1] + m[r-1][c]
   *     [1,1],             if c == 0 || c == r - 1 => m[r-1][c]
   *    [1,2,1],
   *   [1,3,3,1],
   *  [1,4,6,4,1] m[4][2] = m[3][1] + m[3][2]
   * ]
   */
  static vector<vector<int>> get_pascal_triangle(int n) {
    vector<vector<int>> triangle;
    for (int r = 0; r < n; r++) {
      triangle.push_back(vector<int>(r + 1, 0));
      for (int c = 0; c <= r; c++) {
        if (0 == c || r == c) { triangle[r][c] = 1; }
        else { triangle[r][c] = triangle[r - 1][c - 1] + triangle[r - 1][c]; }
      }
    }
    return triangle;
  }

  /**
   * 15.10 Spiral Matrix
   * Given a matrix of mxn elements (m rows, n columns), return all elements of
   * the matrix in spiral order.  For example, Given the following matrix:
   * [ [ 1, 2, 3 ],    (0, 0)         -> (0, 1)         ... (0, n - 1)     n #        i => 0
   *   [ 4, 5, 6 ],    (1, n - 1),    -> (2, n - 1)     ... (m - 1, n -1)  m - 1 #
   *   [ 7, 8, 9 ] ]   (m - 1, n - 2) -> (m - 1, n - 3) ... (m - 1, 0)     n - 1 #
   *                   (m - 2, 0),    -> (m - 3, 0)     ... (1, 0)         m - 2 #
   *
   *                   (1, 1),        -> (1, 2)         ... (1, n -2)      n - 2 #    i => 1
   * You should return [1,2,3,6,9,8,7,4,5].
   */
  static vector<int> matrix_spiral_walk(vector<vector<int>> matrix) {
    int max_loop_cnt = (int)(ceil(matrix.size() / 2.0));
    int row_cnt = matrix.size(), column_cnt = matrix.front().size();
    vector<int> spiral_walk(row_cnt * column_cnt, 0);
    for (int curr_idx = 0, loop_cnt = 0; loop_cnt < max_loop_cnt; loop_cnt++) {
      for (int c = loop_cnt ; c < column_cnt - loop_cnt; c++) {
        spiral_walk[curr_idx] = matrix[loop_cnt][c]; curr_idx++;
      }
      if (column_cnt - loop_cnt - 1 >= 0) {
        for (int r = loop_cnt + 1; r < row_cnt - loop_cnt - 1; r++) {
          spiral_walk[curr_idx] = matrix[r][column_cnt - loop_cnt - 1]; curr_idx++;
        }
      }
      if (row_cnt - loop_cnt - 1 > loop_cnt) {
        for (int c = column_cnt - loop_cnt - 1; c >= loop_cnt; c--) {
          spiral_walk[curr_idx] = matrix[row_cnt - loop_cnt - 1][c]; curr_idx++;
        }
      }
      for (int r = row_cnt - loop_cnt - 2; r >= loop_cnt + 1; r--) {
        spiral_walk[curr_idx] = matrix[r][loop_cnt]; curr_idx++;
      }
    }
    return spiral_walk;
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

  /**
   * 2.1.12 Next Permutation
   * Implement next permutation, which rearranges numbers into the
   * lexicographically next greater permutation of numbers. If such arrangement
   * is not possible, it must rearrange it as the lowest possible order (ie,
   * sorted in ascending order). The replacement must be in-place, do not
   * allocate extra memory. Here are some examples. Inputs are in the left-hand
   * column and its corresponding outputs are in the right-hand column.
   * 1,2,3 => 1,3,2
   * 3,2,1 => 1,2,3
   * 1,1,5 => 1,5,1
   *
   * swap digits from LSB, 123 -> 132
   * 6 8 7 4 3 2 => from 2 -> 8, 6 breaks the ascending & 7 is the closest one
   *   ^            6 <-> 7, => 7 8 6 4 3 2 => 7 2 3 4 6 8
   *                              1 2 3 4 5 (size -> 6) => (6 - 1) / 2
   */
  static string get_next_sequence(string input) {
    string digits = input;
    int curr_pos = digits.size() - 1, swap_pos = curr_pos; char temp;
    while (curr_pos >= 0) {
      if (curr_pos > 0 && digits[curr_pos] > digits[curr_pos - 1]){ break; }
      curr_pos--;
    }
    curr_pos--;
    if (curr_pos >= 0) {
      for (swap_pos = curr_pos; swap_pos < digits.size(); swap_pos++) {
        if (digits[curr_pos] > digits[swap_pos]) { break; }
      }
      swap_pos--;
      if (swap_pos < digits.size()) {
        temp = digits[curr_pos];
        digits[curr_pos] = digits[swap_pos];
        digits[swap_pos] = temp;
      }
    }
    if (curr_pos - swap_pos > 1) {
      for (int i = 0; i < (digits.size() - curr_pos - 1) / 2; i++) {
        temp = digits[curr_pos + i];
        digits[curr_pos + i] = digits[digits.size() - i - 1];
        digits[digits.size() - i - 1] = temp;
      }
    }
    return digits;
  }

  /**
   * 2.1.3 Search in Rotated Sorted Array
   * Suppose a sorted array is rotated at some pivot unknown to you beforehand
   * (i.e., 0 1 2 4 5 6 7 might become 4 5 6 7 0 1 2).
   * You are given a target value to search. If found in the array return its
   * index otherwise return -1 You may assume no duplicate exists in the array
   * --> up --> down, if last elem > first elem, then no rotation.
   * 2 4 5 6 7 0 1 => search for max, (0 + 6) / 2 -> 3, (6 > 0 & 2 < 6) => 7 0 1
   * 0 1 2 3 4 5  6  7 8 9 => 7 0 1 => 0, & 7 > 1 & 7 > 0 => (7, 0)
   * 2 3 4 5 6 7 -4 -3 0 1 => search 0, 6  -> pivot, 2 < 6 & 2 > 1 => 7 0 1 -3 -4
   *                       => search 0, -3 -> pivot, 7 > -4 & -4 < 0 => 0 1
   */
  static int search_rotated_sorted_arr(vector<int> nums, int target) {
    int left_pos = 0, right_pos = nums.size() - 1, target_pos = 0;
    while (left_pos <= right_pos) {
      target_pos = (left_pos + right_pos) / 2;
      if (nums[target_pos] == target) { break; }
      if (left_pos < target_pos) {
        if (((nums[left_pos] < nums[target_pos]) && (nums[left_pos] <= target) && (target <= nums[target_pos])) || // no rotation for left part
            ((nums[left_pos] > nums[target_pos]) && (nums[left_pos] >= target) && (target <= nums[target_pos]))) {
          right_pos = target_pos - 1; continue;
        }
      }
      if (right_pos > target_pos) {
        if (((nums[target_pos] < nums[right_pos]) && (nums[target_pos] <= target) && (target <= nums[right_pos])) || // no rotation for left part
            ((nums[target_pos] > nums[right_pos]) && (nums[target_pos] >= target) && (target <= nums[right_pos]))) {
          left_pos = target_pos + 1; continue;
        }
      }
      return -1;
    }
    return target_pos;
  }

  /**
   * 2.1.5 Median of Two Sorted Arrays
   * There are two sorted arrays A and B of size m and n respectively. Find
   * the median of the two sorted arrays. The overall run time complexity
   * should be O(log(m + n)).           v
   * [ 1, 2, 7, 8, 9 ] => [ 1, 2, 3, 4, 5, 6, 7, 8, 9 ] median is 5 (id => 4)
   * [ 3, 4, 5, 6 ]       5 # in total, first half [ 1, 2 ], from A or B
   * A => [ 1, 2 ], B => [ 3, 4 ] => B covers more as B [1] > A[1]
   * All half of A will be included before B[1] merged => not including 5.
   * => A[k / 2 - 1] == B[k / 2 - 1] => either one is good
   *    A[k / 2 - 1] < B[k / 2 - 1] => A[start_pos ... k / 2 - 1] => can skip
   *    A[k / 2 - 1] > B[k / 2 - 1] => B[start_pos ... k / 2 - 1] => can skip
   */
  static double find_median_elem(vector<int> l_sorted_arr,
                                 vector<int> r_sorted_arr) {
    int l_arr_size = l_sorted_arr.size(), r_arr_size = r_sorted_arr.size();
    if (0 == (l_arr_size + r_arr_size) % 2) {
      return (
        (find_kth_elem(l_sorted_arr.begin(), l_arr_size,
                       r_sorted_arr.begin(), r_arr_size,
                       (l_arr_size + r_arr_size) / 2) +
         find_kth_elem(l_sorted_arr.begin(), l_arr_size,
                       r_sorted_arr.begin(), r_arr_size,
                       (l_arr_size + r_arr_size) / 2 + 1)) / 2.0
      );
    } else {
      return find_kth_elem(l_sorted_arr.begin(), l_arr_size,
                           r_sorted_arr.begin(), r_arr_size,
                           (l_arr_size + r_arr_size) / 2 + 1);
    }
  }

  static int find_kth_elem(vector<int>::const_iterator l_arr_itr, int l_elem_remained,
                           vector<int>::const_iterator r_arr_itr, int r_elem_remained,
                           int elem_to_merge) {
    // always make the case that l_arr have lq elem to merge
    if (l_elem_remained > r_elem_remained) {
      return find_kth_elem(r_arr_itr, r_elem_remained, l_arr_itr, l_elem_remained, elem_to_merge);
    }
    // return from r if l over
    if (0 == l_elem_remained) { return * (r_arr_itr + elem_to_merge - 1); }
    // return min
    if (1 == elem_to_merge) { return min(* l_arr_itr, * r_arr_itr); }
    int l_delta = min(elem_to_merge / 2, l_elem_remained),
        r_delta = elem_to_merge - l_delta;
    if (* (l_arr_itr + l_delta - 1) < * (r_arr_itr + r_delta - 1)) {
      return find_kth_elem(l_arr_itr + l_delta, l_elem_remained - l_delta,
                           r_arr_itr, r_elem_remained, elem_to_merge - l_delta);
    } else if (* (l_arr_itr + l_delta - 1) > * (r_arr_itr + r_delta - 1)) {
      return find_kth_elem(l_arr_itr, l_elem_remained, r_arr_itr + r_delta,
                           r_elem_remained - r_delta, elem_to_merge - r_delta);
    }
    return INT_MAX;
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

  cout << "For example, S = ADOBECODEBANC, T = ABC Minimum window is BANC." << endl;
  cout << ChoresUtil::find_min_window("adobecodebanc", "abc") << endl;
  cout << ChoresUtil::find_min_window("adobecbaebanc", "abc") << endl;

  /**
   * Input : num1 = 4154
   *         num2 = 51454
   * Output : 213779916
   * Input :  num1 =      654154154151454545415415454
   *          num2 = 63516561563156316545145146514654
   * Output : 41549622603955309777243716069997997007620439937711509062916
   */
  cout << ChoresUtil::multiply_big_number("0", "1") << endl;
  cout << ChoresUtil::multiply_big_number("1", "0") << endl;
  cout << ChoresUtil::multiply_big_number("1", "1") << endl;
  cout << ChoresUtil::multiply_big_number("912", "1") << endl;
  cout << ChoresUtil::multiply_big_number("1", "912") << endl;
  cout << ChoresUtil::multiply_big_number("91", "912") << endl;
  cout << ChoresUtil::multiply_big_number("912", "91") << endl;
  cout << "4154 * 51454 = 213739916 | " << ChoresUtil::multiply_big_number("4154", "51454") << endl;
  cout << "654154154151454545415415454 * 63516561563156316545145146514654 = 41549622603955309777243716069997997007620439937711509062916" << endl;
  cout << "654154154151454545415415454 * 63516561563156316545145146514654 = "
       << ChoresUtil::multiply_big_number("654154154151454545415415454", "63516561563156316545145146514654") << endl;
  cout << "-654154154151454545415415454 * 63516561563156316545145146514654 = -41549622603955309777243716069997997007620439937711509062916" << endl;
  cout << "-654154154151454545415415454 * 63516561563156316545145146514654 = "
       << ChoresUtil::multiply_big_number("-654154154151454545415415454", "63516561563156316545145146514654") << endl;
  cout << "654154154151454545415415454 * -63516561563156316545145146514654 = -41549622603955309777243716069997997007620439937711509062916" << endl;
  cout << "654154154151454545415415454 * -63516561563156316545145146514654 = "
       << ChoresUtil::multiply_big_number("654154154151454545415415454", "-63516561563156316545145146514654") << endl;
  cout << "-654154154151454545415415454 * -63516561563156316545145146514654 = 41549622603955309777243716069997997007620439937711509062916" << endl;
  cout << "-654154154151454545415415454 * -63516561563156316545145146514654 = "
       << ChoresUtil::multiply_big_number("-654154154151454545415415454", "-63516561563156316545145146514654") << endl;

  cout << "S: barfoothefoobarman | L: [ foo, bar ] | [ 0, 9 ]" << endl;
  ChoresUtil::print_all_elem<int>(ChoresUtil::find_all_tokens("barfoothefoobarman", vector<string>({ "foo", "bar" })));
  cout << "S: catbatatecatatebat | L: [ cat, ate, bat ] | [ 0, 3, 9 ]" << endl;
  ChoresUtil::print_all_elem<int>(ChoresUtil::find_all_tokens("catbatatecatatebat", vector<string>({ "cat", "ate", "bat" })));
  cout << "S: abcdababcd | L: [ ab, ab, cd ] | [ 0 2 4 ]" << endl;
  ChoresUtil::print_all_elem<int>(ChoresUtil::find_all_tokens("abcdababcd", vector<string>({ "ab", "ab", "cd" })));
  cout << "S: abcdababcd | L: [ ab, ab ] | [ 4 ]" << endl;
  ChoresUtil::print_all_elem<int>(ChoresUtil::find_all_tokens("abcdababcd", vector<string>({ "ab", "ab" })));

  ChoresUtil::print_all_elem_vec<int>(ChoresUtil::get_pascal_triangle(5));

  vector<vector<int>> test_0({ vector<int>({ 1, 2, 3 }) });
  ChoresUtil::print_all_elem<int>(ChoresUtil::matrix_spiral_walk(test_0));
  vector<vector<int>> test_1({ vector<int>({ 1 }),
                               vector<int>({ 2 }),
                               vector<int>({ 3 }) });
  ChoresUtil::print_all_elem<int>(ChoresUtil::matrix_spiral_walk(test_1));
  // [1,2,3,6,9,8,7,4,5]
  vector<vector<int>> test_2({ vector<int>({ 1, 2, 3 }),
                               vector<int>({ 4, 5, 6 }),
                               vector<int>({ 7, 8, 9 }) });
  ChoresUtil::print_all_elem<int>(ChoresUtil::matrix_spiral_walk(test_2));
  vector<vector<int>> test_3({ vector<int>({ 1, 2, 3 }),
                               vector<int>({ 4, 5, 6 }),
                               vector<int>({ 7, 8, 9 }),
                               vector<int>({ 10, 11, 12 }) });
  ChoresUtil::print_all_elem<int>(ChoresUtil::matrix_spiral_walk(test_3));
  /**
   * 1,2,3 => 1,3,2
   * 3,2,1 => 1,2,3
   * 1,1,5 => 1,5,1
   * 6 8 7 4 3 2 => 7 2 3 4 6 8
   */
  cout << "1 <=> 1 <=> " << ChoresUtil::get_next_sequence("1") << endl;
  cout << "123 <=> 132 <=> " << ChoresUtil::get_next_sequence("123") << endl;
  cout << "321 <=> 123 <=> " << ChoresUtil::get_next_sequence("321") << endl;
  cout << "115 <=> 151 <=> " << ChoresUtil::get_next_sequence("115") << endl;
  cout << "687432 <=> 723468 <=> " << ChoresUtil::get_next_sequence("687432") << endl;
  /* 2 3 4 5 6 7 -4 -3 0 1 => search 0, 6  -> pivot, 2 < 6 & 2 > 1 => 7 0 1 -3 -4
   *                       => search 0, -3 -> pivot, 7 > -4 & -4 < 0 => 0 1
   */
  cout << "2 3 4 5 6 7 -4 -3 0 1 <=> 0 | 8 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ 2, 3, 4, 5, 6, 7, -4, -3, 0, 1 }), 0
          )
       << endl;
  cout << "2 3 4 5 6 7 -4 -3 0 1 <=> 1 | 9 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ 2, 3, 4, 5, 6, 7, -4, -3, 0, 1 }), 1
          )
       << endl;
  cout << "2 3 4 5 6 7 -4 -3 0 1 <=> 2 | 0 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ 2, 3, 4, 5, 6, 7, -4, -3, 0, 1 }), 2
          )
       << endl;
  cout << "-4 -3 0 1 2 3 4 5 6 7 <=> 2 | 4 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ -4, -3, 0, 1, 2, 3, 4, 5, 6, 7 }), 2
          )
       << endl;

  cout << "2 3 4 5 6 7 -4 -3 0 0 0 0 1 <=> 0 | 8 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ 2, 3, 4, 5, 6, 7, -4, -3, 0, 0, 0, 0, 1 }), 0
          )
       << endl;
  cout << "2 3 4 5 6 7 -4 -3 0 1 1 1 1 <=> 1 | 9 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ 2, 3, 4, 5, 6, 7, -4, -3, 0, 1, 1, 1, 1 }), 1
          )
       << endl;
  cout << "2 2 2 2 3 4 5 6 7 -4 -3 0 1 <=> 2 | 0 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ 2, 2, 2, 2, 3, 4, 5, 6, 7, -4, -3, 0, 1 }), 2
          )
       << endl;
  cout << "-4 -3 0 1 2 2 2 2 3 4 5 6 7 <=> 2 | 4 <=> "
       << ChoresUtil::search_rotated_sorted_arr(
            vector<int>({ -4, -3, 0, 1, 2, 2, 2, 2, 3, 4, 5, 6, 7 }), 2
          )
       << endl;
  cout << "{2, 3, 6, 7, 9} & {1, 4, 8, 10} <=> 6 <=> "
       << ChoresUtil::find_median_elem(vector<int>({ 2, 3, 6, 7, 9 }),
                                       vector<int>({ 1, 4, 8, 10 }))
       << endl;
  cout << "{1, 4, 8, 10} & {2, 3, 6, 7, 9} <=> 6 <=> "
       << ChoresUtil::find_median_elem(vector<int>({ 1, 4, 8, 10 }),
                                       vector<int>({ 2, 3, 6, 7, 9 }))
       << endl;
  cout << "{1} & {2} <=> 1.5 <=> "
       << ChoresUtil::find_median_elem(vector<int>({ 1 }), vector<int>({ 2 }))
       << endl;

  return 0;
}
