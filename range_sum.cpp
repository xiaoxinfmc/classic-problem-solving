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

class binary_index_tree {
public:
  binary_index_tree(const vector<int> & values) : raw(values), bit(values) {
    initialize_bit();
  }
  virtual ~binary_index_tree() {}

  /* 5 -> 0 0 1 0 1 -> 0 0 1 1 0 -> 0 1 0 0 0 -> 1 0 0 0 0 */
  bool update(int id, int value) {
    if (!is_id_within_range(id)) { return false; }
    int delta = value - raw[id];
    int remainder = id + 1;
    while (remainder <= bit.size()) {
      bit[remainder - 1] += delta;
      remainder += (remainder & -remainder);
    }
    raw[id] = value;
    return true;
  }

  int get_segment_sum(int id_from, int id_to){
    int segment_sum = INT_MIN;
    if (!is_id_within_range(id_from) ||
        !is_id_within_range(id_to) ||
        id_from > id_to) { return segment_sum; }
    if (id_from == 0) {
      segment_sum = get_prefix_sum(id_to);
    } else {
      segment_sum = get_prefix_sum(id_to) - get_prefix_sum(id_from - 1);
    }
    return segment_sum;
  }

  vector<int> get_bits() { return bit; }

private:

  bool is_id_within_range(int i) { return (i >= 0 && i < bit.size()); }

  int get_prefix_sum(int i) {
    if (!is_id_within_range(i)) { return INT_MIN; }
    int prefix_sum = 0;
    int remainder = (i + 1);
    while (remainder > 0) {
      prefix_sum += bit[remainder - 1];
      remainder -= (remainder & -remainder);
    }
    return prefix_sum;
  }

  void initialize_bit() {
    int remainder = 0; int base_idx = 0;
    for (int i = 0; i < bit.size(); i++) {
      remainder = i + 1; base_idx = i;
      while (0 == remainder % 2) {
        bit[i] += bit[base_idx - 1];
        remainder = (remainder >> 1);
        base_idx -= (base_idx & -base_idx);
      }
    }
  }

  vector<int> bit;
  vector<int> raw;
};
};

int main(void) {
  using std::cout;
  using std::endl;
  using std::vector;
  using range_sum::print_all_elem;
  using range_sum::print_all_elem_vec;
  using range_sum::range_sum_immutable;
  using range_sum::region_sum_immutable;
  using range_sum::binary_index_tree;

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
  cout << "2. region_sum_immutable" << endl;
  cout << "8 <=> " << region_sum_obj.calc_region_sum(2, 1, 4, 3) << endl;
  cout << "11 <=> " << region_sum_obj.calc_region_sum(1, 1, 2, 2) << endl;
  cout << "12 <=> " << region_sum_obj.calc_region_sum(1, 2, 2, 4) << endl;

  cout << "3. binary-index-tree" << endl;
  cout << "[ 1 1 2 4 1 4 0 12 2 7 2 11 3 4 0 29 ] <=> " << endl;
  binary_index_tree bit0({ 1, 0, 2, 1, 1, 3, 0, 4, 2, 5, 2, 2, 3, 1, 0, 2 });
  print_all_elem<int>(bit0.get_bits());
  vector<int> ps0;
  cout << "[ 1 1 3 4 5 8 8 12 14 19 21 23 26 27 27 29 ] <=> " << endl;
  for (int i = 0; i < bit0.get_bits().size(); i++) {
    ps0.push_back(bit0.get_segment_sum(0, i));
  }
  print_all_elem<int>(ps0);
  ps0.clear();
  cout << "[ 1 0 2 1 1 3 0 4 2 5 2 2 3 1 0 2 ] <=> " << endl;
  for (int i = 0; i < bit0.get_bits().size(); i++) {
    ps0.push_back(bit0.get_segment_sum(i, i));
  }
  print_all_elem<int>(ps0);
  cout << endl;

  cout << "[ 1 1 2 4 1 4 0 12 2 7 2 11 3 4 0 29 1 ] <=> " << endl;
  binary_index_tree bit1({ 1, 0, 2, 1, 1, 3, 0, 4, 2, 5, 2, 2, 3, 1, 0, 2, 1 });
  print_all_elem<int>(bit1.get_bits());
  vector<int> ps1;
  cout << "[ 1 1 3 4 5 8 8 12 14 19 21 23 26 27 27 29 30 ] <=> " << endl;
  for (int i = 0; i < bit1.get_bits().size(); i++) {
    ps1.push_back(bit1.get_segment_sum(0, i));
  }
  print_all_elem<int>(ps1);

  return 0;
}
