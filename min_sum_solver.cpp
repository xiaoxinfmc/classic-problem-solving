/**
 * Given a triangle, find the minimum path sum from top to bottom. Each step you
 * may move to adjacent numbers on the row below.
 * For example, given the following triangle
 * [
 *     [2], ---- 0 i
 *    [3,4], --- 1
 *   [6,5,7], -- 2, i => (k, k || k + 1)
 *  [4,1,8,3]  - 3 (0, 1, 2, 3, j)
 * ]
 * The minimum path sum from top to bottom is 11 (i.e., 2 + 3 + 5 + 1 = 11).
 * Note: Bonus point if you are able to do this using only O(n) extra space,
 *       where n is the total number of rows in the triangle.
 * min_sum(i, k) = { m[0, 0], i == 0;
 *                   min{ min_sum(i - 1, k - 1) + m[i, k],
 *                        min_sum(i - 1, k)     + m[i, k] }
 * - i => current row id of input of vectors.
 * - k => current index @ end of the path on row i.
 */
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

class MinSumSolver {
public:
  static int calc_min_sum_dp(const vector<vector<int>> & input_triangle_matrix){
    if (input_triangle_matrix.empty()) { return INT_MAX; }

    vector<int> prev_sum_buf(input_triangle_matrix.back().size(), INT_MAX);
    vector<int> curr_sum_buf(input_triangle_matrix.back().size(), INT_MAX);
    vector<int> * curr_sum_buf_ref = & curr_sum_buf,
                * prev_sum_buf_ref = & prev_sum_buf,
                * temp_sum_buf_ref = NULL;
    prev_sum_buf[0] = input_triangle_matrix[0][0];
    size_t row_id = 0, col_id = 0; int alt_min = 0, prev_min = 0;
    for (row_id = 1; row_id < input_triangle_matrix.size(); row_id++) {
      for (col_id = 0; col_id < input_triangle_matrix[row_id].size(); col_id++){
        if ((input_triangle_matrix[row_id].size() - 1) > col_id){
          curr_sum_buf_ref->at(col_id) = (
            prev_sum_buf_ref->at(col_id) + input_triangle_matrix[row_id][col_id]
          );
        }
        if (0 == col_id) { continue; }
        alt_min = (
          prev_sum_buf_ref->at(col_id - 1) + input_triangle_matrix[row_id][col_id]
        );
        if (curr_sum_buf_ref->at(col_id) > alt_min) {
          curr_sum_buf_ref->at(col_id) = alt_min;
        }
      }
      temp_sum_buf_ref = curr_sum_buf_ref;
      curr_sum_buf_ref = prev_sum_buf_ref;
      prev_sum_buf_ref = temp_sum_buf_ref;
    }
    return find_min(* prev_sum_buf_ref);
  }

  static int calc_min_sum(const vector<vector<int>> & input_triangle_matrix) {
    int min_sum = 0;
    if (input_triangle_matrix.empty()) { return min_sum; }
    for (auto int_vec : input_triangle_matrix) { min_sum += find_min(int_vec); }
    return min_sum;
  }
  static int find_min(const vector<int> & int_vec) {
    int min_val = int_vec.front();
    for (auto & val : int_vec) { if (val < min_val) { min_val = val; } }
    return min_val;
  }
};

int main(void) {
  int a[] = { 2 }, b[] = { 3, 4 }, c[] = { 6, 5, 7 }, d[] = { 4, 1, 8, 3 };
  vector<vector<int>> input_triangle_matrix;
  input_triangle_matrix.push_back(vector<int>(a, a + sizeof(a) / sizeof(a[0])));
  input_triangle_matrix.push_back(vector<int>(b, b + sizeof(b) / sizeof(b[0])));
  input_triangle_matrix.push_back(vector<int>(c, c + sizeof(c) / sizeof(c[0])));
  input_triangle_matrix.push_back(vector<int>(d, d + sizeof(d) / sizeof(d[0])));
  cout << MinSumSolver::calc_min_sum(input_triangle_matrix) << endl;
  cout << MinSumSolver::calc_min_sum_dp(input_triangle_matrix) << endl;
  return 0;
}
