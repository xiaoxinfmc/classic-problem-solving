#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <list>
#include <deque>

namespace slide_window_util {
  using std::deque;
  using std::string;
  using std::cout;
  using std::endl;
  using std::vector;
  using std::list;
  using std::pair;
  using std::unordered_map;
  using std::ostream;
  using std::unordered_set;

  template <class type>
  static void print_all_elem(const vector<type> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }

  template <class t1, class t2>
  static void print_all_pairs(const vector<pair<t1, t2>> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr.first << "|" << arr.second << " "; }
    cout << "]" << endl;
  }

  template <class type>
  static void print_all_elem_vec(const vector<vector<type>> & input) {
    cout << "[" << endl;
    for (auto & arr : input){ cout << "  "; print_all_elem<type>(arr); }
    cout << "]" << endl;
  }

  /**
   * 239. Sliding Window Maximum
   * - Given an array nums, there is a sliding window of size k which is moving
   *   from the very left of the array to the very right. You can only see the
   *   k numbers in the window. Each time the sliding window moves right by one
   *   position. Return the max sliding window.
   * Example:
   * - Input: nums = [1,3,-1,-3,5,3,6,7], and k = 3
   *   Output: [3,3,5,5,6,7]
   * Explanation:
   * - Window position                Max
   *   ---------------               -----
   *   [1  3  -1] -3  5  3  6  7       3
   *    1 [3  -1  -3] 5  3  6  7       3
   *    1  3 [-1  -3  5] 3  6  7       5
   *    1  3  -1 [-3  5  3] 6  7       5
   *    1  3  -1  -3 [5  3  6] 7       6
   *    1  3  -1  -3  5 [3  6  7]      7
   * Note:
   * - You may assume k is always valid, 1 <= k <= input array's size for non
   *   empty array.
   * Follow up:
   * - Could you solve it in linear time?
   * Intuition:
   * - Input contains an array of numbers, while we can only see k consequtive
   *   values max at a time, the goal is to find the window contains the max.
   * - obviously we want to keep a record of max value & its position  at any
   *   time. The hard part would be how do we keep track of max in curr window
   *    1  3  -1  -3  5 [3  6  7]      7
   *     (0 + 7 - 3 + 1) 5     7
   * - Most important note is that it took O(n) to keep a queue of size n sorted
   *   iff each op only happens @ head/end of the queue.
       { 5, 5, 5, 5, 5, 9, 10 },
   * - { 0, 5, 5, 1, 5, 0, 3, 4, 9, 10 },
   *   { 0 }
   *   { 5 } 0
   *   { 5 } 0 5
   *   { 5 1 } 0 5 -> 5
   *   { 5 } 0 5 1 5 -> 5
   *   { 5 0 } 0 5 1 5 -> 5
   *   { 5 3 } 0 5 1 5 0 -> 5
   *   { 5 4 } 0 5 1 5 0 3 -> 5
   *   { 9 } 0 5 1 5 0 3 4 5 -> 9
   */
  static vector<int> get_max_via_sliding_window(const vector<int> & nums, int k) {
    vector<int> max_elem_arr;
    deque<int> elem_pos_buffer;
    if (1 == k) { return nums; }
    /* slide the window & check the max */
    for (int i = 0; i < nums.size(); i++) {
      if (!elem_pos_buffer.empty() && elem_pos_buffer.front() <= i - k) {
        elem_pos_buffer.pop_front();
      }
      while (!elem_pos_buffer.empty() && nums[elem_pos_buffer.back()] <= nums[i]) {
        elem_pos_buffer.pop_back();
      }
      elem_pos_buffer.push_back(i);
      if (i >= k - 1 && max_elem_arr.size() <= nums.size() - k) {
        max_elem_arr.push_back(nums[elem_pos_buffer.front()]);
      }
    }
    return max_elem_arr;
  }

  static void test_get_max_via_sliding_window() {
    cout << "1. test_get_max_via_sliding_window" << endl;
    vector<int> result;
    vector<int> test_input_k = { 5, 2, 1, 3, 3, 4, 4, 4 };
    vector<vector<int>> test_output = {
      { 10, 10, 9, 2 },
      { 3, 3, -1, 5, 5, 6, 7 },
      { 1, 3, -1, -3, 5, 3, 6, 7 },
      {3,3,5,5,6,7}, {9,9,5,5,6,7},
      { 5, 5, 5, 5, 5, 9, 10 },
      { 5, 5, 6, 6, 6, 9, 10 },
      {7,7,7,7,7},
    };
    vector<vector<int>> test_input = {
      { 9,10,9,-7,-4,-8,2,-6 },
      { 1, 3, -1, -3, 5, 3, 6, 7 },
      { 1, 3, -1, -3, 5, 3, 6, 7 },
      { 1, 3, -1, -3, 5, 3, 6, 7 },
      { 1, 9, -1, -3, 5, 3, 6, 7 },
      { 0, 5, 5, 1, 5, 0, 3, 4, 9, 10 },
      { 0, 5, 5, 1, 5, 6, 3, 4, 9, 10 },
      {-7,-8,7,5,7,1,6,0},
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = get_max_via_sliding_window(test_input[i], test_input_k[i]);
      print_all_elem<int>(result); cout << " <=>" << endl;
      print_all_elem<int>(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) { assert(test_output[i][j] == result[j]); }
    }
  }

  /**
   * 386. Longest Substring with At Most K Distinct Characters
   * Description
   * - Given a string s, find the length of the longest substring T that
   *   contains at most k distinct characters.
   * Example
   * - For example, Given s = "eceba", k = 3,
   *   T is "eceb" which its length is 4.
   * Challenge
   * - O(n), n is the size of the string s.
   * Intuition:
   * - assume all chars are within a-z
   * - bf way of thinking is that to check the size of each substr from n -> k,
   *   until we found the 1st substr with k diff chars, => O(n3) as we have
   *   O(n2) substrs to check, and checking took O(n).
   * - a improved version would be DP, essentially let char_lookup(i, j) store
   *   all diff chars used for substr[i..j], then goal is to calc all and find
   *   the cell with most chars used, char_lookup(i, j) = char_lookup(i, j-1)+1
   *                                                      char_lookup(i, j-1)
   *   we have O(n2) cell to fill, compute each cell took O(alpha) => alpha O(n2)
   * - sliding window of k diff chars based on unordered_map & max-substr-len
   *   for O(n), obviously we need a drastic diff. approach, a natural way to
   *   think is to use sliding window tech.
   * - maintain a window from start_pos -> end_pos, with corresponding char cnt
   *   map, then the size of the map is the k, while the cnt of certain char
   *   shows the cnt of char within curr. window.
   * - at the same time, we maintain a max count for total sum of all values,
   *   the window should always contain k diff. chars, and each time we move it
   *   from left -> right, & update the map & max-cnt accordingly.
   */
  static void decr_char_cnt_lookup(unordered_map<char, int> & lookup, char val) { lookup[val] -= 1; }

  static void incr_char_cnt_lookup(unordered_map<char, int> & lookup, char val) { lookup[val] += 1; }

  static int fetch_char_cnt(unordered_map<char, int> & lookup, char val) { return lookup[val]; }

  static int get_lsubstr_len_with_at_most_k_diff_chars(const string & str, int k) {
    int lsubstr_len = 0, diff_char_cnt = 0;
    unordered_map<char, int> char_cnt_lookup;
    /* we will always maintain the diff_char_cnt to be exactly k */
    for (int start_pos = 0, end_pos = 0; end_pos < str.size() && start_pos < str.size(); ) {
      if (end_pos < start_pos) { end_pos = start_pos; }
      if (diff_char_cnt <= k) {
        /* we have less than k diff chars, then expanding window with end_pos */
        if (0 == fetch_char_cnt(char_cnt_lookup, str[end_pos])) { diff_char_cnt += 1; }
        incr_char_cnt_lookup(char_cnt_lookup, str[end_pos]);
        if (diff_char_cnt <= k) { lsubstr_len = std::max(lsubstr_len, (end_pos - start_pos + 1)); }
        end_pos += 1;
      } else {
        /* we have k + 1 diff chars, then log curr_len if needed, shrink window with start_pos */
        decr_char_cnt_lookup(char_cnt_lookup, str[start_pos]);
        if (0 == fetch_char_cnt(char_cnt_lookup, str[start_pos])) { diff_char_cnt -= 1; }
        start_pos += 1;
      }
    }
    return lsubstr_len;
  }

  static void test_get_lsubstr_len_with_at_most_k_diff_chars() {
    cout << "2. test_get_lsubstr_len_with_at_most_k_diff_chars" << endl;
    int result = 0;
    vector<int> test_output = { 4, 1, 8, 1, 3, 6, 0, 2 };
    vector<int> test_input_k = { 3, 1, 1, 1, 1, 2, 0, 10 };
    vector<string> test_input_str = { "eceba", "eceba", "aaaaaaaa", "a", "aaak", "aaakaas", "", "kb" };
    for (int i = 0; i < test_input_str.size(); i++) {
      result = get_lsubstr_len_with_at_most_k_diff_chars(test_input_str[i], test_input_k[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 660. Read N Characters Given Read4 II - Call multiple times
   * Description
   * - The API: int read4(char *buf) reads 4 characters at a time from a file.
   * - The return value is the actual number of characters read. For example,
   *   it returns 3 if there is only 3 characters left in the file.
   * - By using the read4 API, implement the function int read(char *buf, int n)
   *   that reads n characters from the file.
   * - The read function may be called multiple times.
   * Intuition:
   * - implement read base on read4 is easy, while we may want to account for
   *   the case of api been called multiple times for same file but diff. chars.
   * - the goal could be caching the previous read as much as possible ?
   * - apparently we are looking for a neat solution to keep track of curr ptr.
   */
  static int read4(char * buf) { return 0; }
  const static int READ_BUFFER_SIZE = 4;
  class algor_read_obj {
  public:
    algor_read_obj() : start_pos(-1), end_pos(-1) { memset(read_buffer, 0, sizeof(read_buffer)); }
    virtual ~algor_read_obj() {}

    int algor_read(char * buf, int n) {
      int total_char_read = 0, curr_char_read = 0;
      if (NULL == buf || n <= 0) { return total_char_read; }

      /* 1. check and fetch from existing buffer */
      curr_char_read = check_and_fetch_from_existing_buffer(
        buf, 0, min(n - total_char_read, READ_BUFFER_SIZE)
      );
      total_char_read += curr_char_read;

      /* 2. if curr buffer does not cover all needs, move to read4 */
      while (total_char_read < n) {
        /* usually curr_char_read will be 4, it will be less either:
         * - reach the end of file or reach the max chars to read */
        curr_char_read = check_and_fetch_from_sys_buffer(
          buf, total_char_read, min(n - total_char_read, READ_BUFFER_SIZE)
        );
        total_char_read += curr_char_read;
        /* check if we reach the end of the file */
        if (curr_char_read < READ_BUFFER_SIZE) { break; }
      }

      return total_char_read;
    }

  private:
    /* this method will 1st call read4 to buffer 4 chars to internal buffer then
     * passing back the value as needed, and return chars been copied */
    int check_and_fetch_from_sys_buffer(char * dest_buf, int dest_buf_idx = 0,
                                        int fetch_cnt = READ_BUFFER_SIZE){
      memset(read_buffer, 0, sizeof(read_buffer));
      start_pos = 0; end_pos = read4(read_buffer); end_pos -= 1;
      int char_to_ret = min(end_pos - start_pos + 1, fetch_cnt);
      for (int i = 0; i < char_to_ret; i++) {
        dest_buf[dest_buf_idx + i] = read_buffer[start_pos];
        start_pos += 1;
      }
      return curr_char_read;
    }
    /* this method is supposed to be called everytime before calling read4
     * such that we can avoid over reaching the pointer in read4, if fetch_cnt
     * is 2, while we have 3 in curr_buf, then there is no need to call read4
     * at all, then curr_buf of [ a b c 0 ] will be [ a b c 0 ], and return 2.
     *                            ^   ^                   ^^ */
    int check_and_fetch_from_existing_buffer(char * dest_buf, int dest_buf_idx = 0,
                                             int fetch_cnt = READ_BUFFER_SIZE){
      if (false == is_curr_buffer_has_data()) { return 0; }
      int char_to_ret = min(end_pos - start_pos + 1, fetch_cnt);
      for (int i = 0; i < char_to_ret; i++) {
        dest_buf[dest_buf_idx + i] = read_buffer[start_pos];
        start_pos += 1;
      }
      return char_to_ret;
    }
    bool is_curr_buffer_has_data() {
      return (end_pos >= start_pos && start_pos >= 0 && end_pos < READ_BUFFER_SIZE);
    }
    char read_buffer[READ_BUFFER_SIZE];
    int start_pos, end_pos;
  };
};

int main(void) {
  using slide_window_util::test_get_max_via_sliding_window;
  using slide_window_util::test_get_lsubstr_len_with_at_most_k_diff_chars;

  test_get_max_via_sliding_window();
  test_get_lsubstr_len_with_at_most_k_diff_chars();

  return 0;;
}
