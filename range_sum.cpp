#include <iostream>
#include <vector>

namespace range_sum {
using std::cout;
using std::endl;
using std::vector;

template <class type>
static void print_all_elem(const vector<type>& input) {
  cout << "[ ";
  for (auto & arr : input) { cout << arr << " "; }
  cout << "]" << endl;
}

template <class type>
static void print_all_elem_vec(const vector<vector<type>> & input) {
  cout << "[" << endl;
  for (auto & arr : input){ cout << "  "; print_all_elem<type>(arr); }
  cout << "]" << endl;
}

/**
 * 303. Range Sum Query - Immutable
 * Given an integer array nums, find the sum of the elements between indices i and j (i ≤ j), inclusive.
 * Example:
 * Given nums = [-2, 0, 3, -5, 2, -1]
 * - sumRange(0, 2) -> 1
 * - sumRange(2, 5) -> -1
 * - sumRange(0, 5) -> -3
 * Note:
 * - You may assume that the array does not change.
 * - There are many calls to sumRange function.
 * Intuition:
 * - sum(2, 5) <=> sum(0, 5) - sum(0, 2)
 */
class range_sum_immutable {
public:
  range_sum_immutable(const vector<int> & nums) : prefix_sum(nums) {
    for (int i = 1; i < prefix_sum.size(); i++) {
      prefix_sum[i] += prefix_sum[i - 1];
    }
  }

  virtual ~range_sum_immutable() {}

  int calc_sum_via_range(int from, int to) {
    if (from < 0 || from > to || to >= prefix_sum.size()) { return INT_MIN; }
    if (0 == from) { return prefix_sum[to]; }
    return prefix_sum[to] - prefix_sum[from - 1];
  }

private:
  vector<int> prefix_sum;
};

/**
 * 304. Range Sum Query 2D - Immutable
 *
 * Given a 2D matrix matrix, find the sum of the elements inside the rectangle
 * defined by its upper left corner (row1, col1) and lower right (row2, col2).
 *
 * Example:
 * - Given matrix = [
 *   [3, 0, 1, 4, 2],
 *      (xy)  (x - 1, w)
 *   [5, 6, 3, 2, 1],
 *   [1, 2, 0, 1, 5],
 *   [4, 1, 0, 1, 7],
 * (v, y-1)   (vw)
 *   [1, 0, 3, 0, 5]
 * ]
 *
 * - sumRegion(2, 1, 4, 3) -> 8
 * - sumRegion(1, 1, 2, 2) -> 11
 * - sumRegion(1, 2, 2, 4) -> 12
 *
 * Note:
 * - You may assume that the matrix does not change.
 * - There are many calls to sumRegion function.
 * - You may assume that row1 ≤ row2 and col1 ≤ col2.
 * Intuition:
 * - pre-calc prefix sum for m[i][j] => total sum for (<0, 0>, <i, j>)
 *   such that sum_region(x, y, v, w) => m[v][w] - m[x - 1][w] - m[v][y - 1] + m[x - 1][y - 1]
 * - m[i][j] = m[i - 1][j] + m[i][j - 1] - m[i - 1][j - 1]
 */
class region_sum_immutable {
public:

  region_sum_immutable(const vector<vector<int>> matrix) : region_sum_lookup(matrix) {
    /* pre-calc 1st column */
    for (int i = 1; i < matrix.size(); i++) {
      region_sum_lookup[i][0] += region_sum_lookup[i - 1][0];
    }
    /* pre-calc 1st row */
    for (int i = 1; i < matrix.front().size(); i++) {
      region_sum_lookup[0][i] += region_sum_lookup[0][i - 1];
    }
    for (int i = 1; i < matrix.size(); i++) {
      for (int j = 1; j < matrix[i].size(); j++) {
        region_sum_lookup[i][j] += region_sum_lookup[i - 1][j] + region_sum_lookup[i][j - 1] - region_sum_lookup[i - 1][j - 1];
      }
    }
  }

  virtual ~region_sum_immutable() {}

  int calc_region_sum(int x, int y, int v, int w) {
    if (!(x <= v && y <= w && x >= 0 && x < region_sum_lookup.size() &&
                              y >= 0 && y < region_sum_lookup.front().size() &&
                              v >= 0 && v < region_sum_lookup.size() &&
                              w >= 0 && w < region_sum_lookup.front().size())) {
      return INT_MIN;
    }
    int region_sum = region_sum_lookup[v][w];
    if (x > 0) { region_sum -= region_sum_lookup[x - 1][w]; }
    if (y > 0) { region_sum -= region_sum_lookup[v][y - 1]; }
    if (x > 0 && y > 0) { region_sum += region_sum_lookup[x - 1][y - 1]; }
    return region_sum;
  }

private:
  vector<vector<int>> region_sum_lookup;
};

};

int main(void) {
  using std::cout;
  using std::endl;
  using std::vector;
  using range_sum::range_sum_immutable;
  using range_sum::region_sum_immutable;

  vector<int> values({ -2, 0, 3, -5, 2, -1 });

  range_sum_immutable range_sum_obj(values);
  cout << "1. range_sum_immutable" << endl;
  cout << "1 <=> " << range_sum_obj.calc_sum_via_range(0, 2) << endl;
  cout << "-1 <=> " << range_sum_obj.calc_sum_via_range(2, 5) << endl;
  cout << "-3 <=> " << range_sum_obj.calc_sum_via_range(0, 5) << endl;

  vector<vector<int>>matrix({{ 3, 0, 1, 4, 2 }, { 5, 6, 3, 2, 1 },
                             { 1, 2, 0, 1, 5 }, { 4, 1, 0, 1, 7 },
                             { 1, 0, 3, 0, 5 }});
  region_sum_immutable region_sum_obj(matrix);
  cout << "8 <=> " << region_sum_obj.calc_region_sum(2, 1, 4, 3) << endl;
  cout << "11 <=> " << region_sum_obj.calc_region_sum(1, 1, 2, 2) << endl;
  cout << "12 <=> " << region_sum_obj.calc_region_sum(1, 2, 2, 4) << endl;

  return 0;
}
