#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

class StringUtil{
public:
  /*
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
   *    |--------------|--?-----------|
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
  static string get_longest_palindrome(string input_str) {
    string input_str_copy = "$", result = "";
    if (true == input_str.empty()) { return result; }
    for (int i = 0; i < input_str.size(); i++) {
      input_str_copy += input_str[i]; input_str_copy += "$";
    }

    // center point will not be included in radius for a boundary.
    int left_boundary_id = 0, right_boundary_id = 0, boundary_radius = 0;
    int curr_left_boundary_id = 0, curr_right_boundary_id = 0;
    int * boundary_radius_buffer_ptr = new int[input_str_copy.size()];
    assert(NULL != boundary_radius_buffer_ptr);

    for (int i = 0; i < input_str_copy.size(); i++) { boundary_radius_buffer_ptr[i] = 0; }
// cout << "radiu: "; for (int i = 0; i < input_str_copy.size(); i++) { cout << boundary_radius_buffer_ptr[i] << "\t"; } cout << endl;

    boundary_radius_buffer_ptr[0] = 0; boundary_radius_buffer_ptr[1] = 1;
    for (int i = 2; i < input_str_copy.size(); i++) {
      if ((input_str_copy.size() - 1) == i) {
        boundary_radius_buffer_ptr[i] = 0; continue;
      }
      left_boundary_id = i - 1 - boundary_radius_buffer_ptr[i - 1];
      right_boundary_id = i - 1 + boundary_radius_buffer_ptr[i - 1];
      if (i < right_boundary_id) {
        boundary_radius_buffer_ptr[i] = boundary_radius_buffer_ptr[i - 2];
        curr_right_boundary_id = i + boundary_radius_buffer_ptr[i - 2];
        curr_right_boundary_id = (
          curr_right_boundary_id <= right_boundary_id ? curr_right_boundary_id : right_boundary_id
        );
        curr_left_boundary_id = 2 * i - curr_right_boundary_id;
        curr_left_boundary_id--; curr_right_boundary_id++;
      } else {
        curr_left_boundary_id = i - 1;
        curr_right_boundary_id = i + 1;
      }
// cout << "i: " << i << " l: " << curr_left_boundary_id << " r: " << curr_right_boundary_id << endl;
      while ((curr_left_boundary_id >= 0) && (curr_right_boundary_id < input_str_copy.size()) &&
             (input_str_copy[curr_left_boundary_id] == input_str_copy[curr_right_boundary_id])) {
        boundary_radius_buffer_ptr[i]++;
        curr_left_boundary_id--; curr_right_boundary_id++;
      }
		}

    int max_boundary_len = 0, max_pivot_idx = 0;
		for (int i = 0; i < input_str_copy.size(); i += 1) {
      if (boundary_radius_buffer_ptr[i] <= max_boundary_len) { continue; }
      max_boundary_len = boundary_radius_buffer_ptr[i]; max_pivot_idx = i;
    }
    for (int i = (max_pivot_idx - max_boundary_len + 1);
             i < (max_pivot_idx + max_boundary_len); i += 2) {
      result += input_str_copy[i];
    }

    delete [] boundary_radius_buffer_ptr;
// cout << "input: " << input_str << endl;
// cout << "copys: "; for (int i = 0; i < input_str_copy.size(); i++) { cout << input_str_copy[i] << "\t"; } cout << endl;
// cout << "index: "; for (int i = 0; i < input_str_copy.size(); i++) { cout << i << "\t"; } cout << endl;
// cout << "radiu: "; for (int i = 0; i < input_str_copy.size(); i++) { cout << boundary_radius_buffer_ptr[i] << "\t"; } cout << endl;
// cout << "expss: "; int x[] = { 0, 1, 0, 3, 0, 1, 0, 7, 0, 1, 0, 9, 0, 1, 0, 5, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0 }; for (int i = 0; i < sizeof(x)/sizeof(int); i++) { cout << x[i] << "\t"; } cout << endl;
// cout << "max_p: " << max_pivot_idx << endl;
// cout << "max_b: " << max_boundary_len << endl;
// cout << "maxpl: "; cout << result << endl;
    return result;
  }
};

int main(void) {
// babcbabcbaccba
  string test_input[] = {
    "babcbabcbaccba", "abaaba", "abababa", "abcbabcbabcba", "forgeeksskeegfor",
    "caba", "abacdfgdcaba", "abacdfgdcabba", "abacdedcaba", "a", "aa", "",
  };
  string test_output[] = {
    "abcbabcba", "abaaba", "abababa", "abcbabcbabcba", "geeksskeeg",
    "aba", "aba", "abba", "abacdedcaba", "a", "aa", "",
  };
  for (int i = 0; i < sizeof(test_output) / sizeof(string); i++) {
// cout << "expect: " << test_output[i] << endl;
    assert(StringUtil::get_longest_palindrome(test_input[i]) == test_output[i]);
  }
  return 0;
}
