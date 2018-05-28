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

class GreedyUtil{
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
   * 12.1 Jump Game
   * Given an array of non-negative integers, you are initially positioned at
   * the first index of the array. Each element in the array represents your
   * maximum jump length at that position.
   * Determine if you are able to reach the last index.
   * For example:
   * A = [2,3,1,1,4], return true. A = [3,2,1,0,4], return false.
   * To be able to jump to the end, all ladders should be contigious, such
   * that any ladder with 0 max-jump needs to be skipped.
   * +++++ 0 ++++ 0 0 +++++
   * 1 2 3 0 1 4
   * jump_lookup[i] => true | false =>
   * jump_lookup[i] => false if max[i] = 0
   *                => 1 <= j <= max[i], any jump_lookup[i + j] is true.
   * calc. from n - 1 => 0, return jump_lookup[0].
   */
  static bool _is_boundary_reachable(const vector<int> jump_arr) {
    vector<bool> jump_lookup(jump_arr.size(), false);
    int boundary_idx = jump_arr.size() - 1;
    for (int i = boundary_idx; i >= 0; i--) {
      for (int j = 1; j <= jump_arr[i]; j++) {
        if ((i + j > boundary_idx) || (true == jump_lookup[i + j])){
          jump_lookup[i] = true; break;
        }
      }
    }
    return jump_lookup[0];
  }

  static bool is_boundary_reachable(const vector<int> jump_arr) {
    int curr_pos = 0, max_reachable_pos = 0, target_pos = jump_arr.size() - 1;
    for (; ((curr_pos <= max_reachable_pos) && (max_reachable_pos <= target_pos)); curr_pos++) {
      max_reachable_pos = max(max_reachable_pos, curr_pos + jump_arr[curr_pos]);
    }
    return (max_reachable_pos > target_pos);
  }

  /**
   * 12.2 Jump Game II
   * Given an array of non-negative integers, you are initially positioned at
   * the first index of the array. Each element in the array represents your
   * maximum jump length at that position.
   * Your goal is to reach the last index in the minimum number of jumps.
   * For example: Given array A = [2,3,1,1,4]
   * The minimum number of jumps to reach the last index is 2. (Jump 1 step
   * from index 0 to 1, then 3 steps to the last index.)
   */
  static int min_jump_steps(const vector<int> jump_arr) {
    int step = 0, curr_pos = 0, max_span = 0, target_span = jump_arr.size() - 1;
    for(; ((curr_pos <= max_span) && (max_span < target_span)); curr_pos++) {
      if ((curr_pos + jump_arr[curr_pos]) > max_span) { step += 1; }
      max_span = max(max_span, curr_pos + jump_arr[curr_pos]);
    }
    return step;
  }

  /**
   * 12.5 Longest Substring Without Repeating Characters
   * Given a string, find the length of the longest substring without repeating
   * characters. For example, the longest substring without repeating letters
   * for "abcabcbb" is "abc", which the length is 3. For "bbbbb" the longest
   * substring is "b", with the length of 1.
   */
  static int calc_max_size_of_lstr_with_uniq_chars(const string input) {
    int max_lstr_size = 1;
    vector<bool> lookup(int('z') - int('a') + 1, false);
    for (int start_pos = 0, curr_pos = 0; curr_pos < input.size(); curr_pos++) {
      if (true == lookup[int(input[curr_pos]) - int('a')]) {
        max_lstr_size = max(max_lstr_size, curr_pos - start_pos);
        lookup = vector<bool>(int('z') - int('a') + 1, false);
        start_pos = curr_pos;
      }
      lookup[int(input[curr_pos]) - int('a')] = true;
    }
    return max_lstr_size;
  }

  /**
   * 12.6 Container With Most Water
   * Given n non-negative integers a1 a2 ... an where each represents a point at
   * coordinate (i, ai ). n vertical lines are drawn such that the two endpoints
   * of line i is at (i, ai) and (i, 0). Find two lines, which together with
   * x-axis forms a container, such that the container contains the most water.
   * Note: You may not slant the container.
   */
  int calc_max_container_volume(vector<int> lines) {
    int left_pos = 0, right_pos = lines.size() - 1, volume = 0;
    while (left_pos < right_pos) {
      volume = max(volume, (right_pos - left_pos) * min(lines[right_pos], lines[left_pos]));
      if (lines[right_pos] < lines[left_pos]) { right_pos--; } else { left_pos++; }
    }
    return volume;
  }
};

int main(void) {
  GreedyUtil::print_all_elem<int>(vector<int>({ 1, 2, 3, 0, 1, 4 }));
  cout << GreedyUtil::is_boundary_reachable(vector<int>({ 1, 2, 3, 0, 1, 4 })) << endl;
  GreedyUtil::print_all_elem<int>(vector<int>({ 2, 3, 1, 1, 4 }));
  cout << GreedyUtil::is_boundary_reachable(vector<int>({ 2, 3, 1, 1, 4 })) << endl;
  GreedyUtil::print_all_elem<int>(vector<int>({ 3, 2, 1, 0, 4 }));
  cout << GreedyUtil::is_boundary_reachable(vector<int>({ 3, 2, 1, 0, 4 })) << endl;

  GreedyUtil::print_all_elem<int>(vector<int>({ 1, 2, 3, 0, 1, 4 }));
  cout << GreedyUtil::min_jump_steps(vector<int>({ 1, 2, 3, 0, 1, 4 })) << endl;
  GreedyUtil::print_all_elem<int>(vector<int>({ 2, 3, 1, 1, 4 }));
  cout << GreedyUtil::min_jump_steps(vector<int>({ 2, 3, 1, 1, 4 })) << endl;
  GreedyUtil::print_all_elem<int>(vector<int>({ 4, 3, 1, 1, 4 }));
  cout << GreedyUtil::min_jump_steps(vector<int>({ 4, 3, 1, 1, 4 })) << endl;

  // "abcabcbb" => "abc", 3. "bbbbb" => "b", 1.
  cout << "abcabcbb" << endl << GreedyUtil::calc_max_size_of_lstr_with_uniq_chars("abcabcbb") << endl;
  cout << "bbbbbbbb" << endl << GreedyUtil::calc_max_size_of_lstr_with_uniq_chars("bbbbbbbb") << endl;
  cout << "b" << endl << GreedyUtil::calc_max_size_of_lstr_with_uniq_chars("b") << endl;
  return 0;
}
