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
};

int main(void) {
  using slide_window_util::test_get_max_via_sliding_window;

  test_get_max_via_sliding_window();

  return 0;;
}
