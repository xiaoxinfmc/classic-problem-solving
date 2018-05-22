/**
 * 13.2 Maximum Subarray
 * Find the contiguous subarray within an array (containing at least one number)
 * which has the largest sum. For example, given the array
 * [ -2, 1, -3, 4, -1, 2, 1, -5, 4 ], the contiguous subarray [ 4, -1, 2, 1 ]
 * has the largest sum = 6.
 * max_sub_sum(i, j) : max_sub_sum(i, j - 1) + m[j], max_sub_sum(i, j - 1) >= 0
 *                     m[j], max_sub_sum(i, j - 1) < 0
 *                     m[i], i == j
 */
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

class MaxSubSumSolver{
public:
  static int calc_max_sub_sum_dp(const vector<int> & input_arr){
    int max_sub_sum = INT_MIN;
    if (input_arr.empty()) { return INT_MIN; }

    size_t total_elem_cnt = input_arr.size();

    int curr_sub_sum = input_arr[0];
    for (size_t i = 1; i < total_elem_cnt; i++) {
      if (curr_sub_sum < 0) {
        curr_sub_sum = input_arr[i];
      } else {
        curr_sub_sum += input_arr[i];
      }
      if (max_sub_sum < curr_sub_sum) { max_sub_sum = curr_sub_sum; }
    }
    return max_sub_sum;
  }

  static int find_max(const vector<int> & int_vec) {
    int max_val = int_vec.front();
    for (auto & val : int_vec) { if (val > max_val) { max_val = val; } }
    return max_val;
  }

  static int calc_max_sub_sum(const vector<int> & input_arr){
    int max_sub_sum = INT_MIN;
    if (input_arr.empty()) { return INT_MIN; }
    vector<vector<int>> max_sub_sum_buffer;
    size_t total_elem_cnt = input_arr.size();
    for (size_t i = 0; i < total_elem_cnt; i++){
      max_sub_sum_buffer.push_back(vector<int>(total_elem_cnt, INT_MIN));
    }
    for (size_t i = 0; i < total_elem_cnt; i++){
      max_sub_sum_buffer[i][i] = input_arr[i];
    }
    for (size_t i = 0; i < total_elem_cnt; i++) {
      for (size_t j = i; j < total_elem_cnt; j++) {
        if (j > i) {
          if (max_sub_sum_buffer[i][j-1] > 0) {
            max_sub_sum_buffer[i][j] = max_sub_sum_buffer[i][j-1]+input_arr[j];
          } else {
            max_sub_sum_buffer[i][j] = input_arr[j];
            i = j;
          }
        }
        if (max_sub_sum < max_sub_sum_buffer[i][j]) {
          max_sub_sum = max_sub_sum_buffer[i][j];
        }
      }
    }
    return max_sub_sum;
  }
};

int main(void) {
  int a[] = { -2, 1, -3, 4, -1, 2, 1, -5, 4 };
  vector<int> input_arr(a, a + sizeof(a) / sizeof(a[0]));
  cout << MaxSubSumSolver::calc_max_sub_sum(input_arr) << endl;
  cout << MaxSubSumSolver::calc_max_sub_sum_dp(input_arr) << endl;
  return 0;
}
