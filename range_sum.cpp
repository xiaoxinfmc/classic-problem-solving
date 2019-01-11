#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

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
  binary_index_tree() {}
  binary_index_tree(const vector<int> & values) : raw(values), bit(values) {
    initialize_bit();
  }
  virtual ~binary_index_tree() {}

  void initialize_via_input(const vector<int> & values) {
    raw = values; bit = values;
    initialize_bit();
  }

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

class segment_sum_tree {
public:
  segment_sum_tree(const vector<int> & values) { initialize_st(values); }
  virtual ~segment_sum_tree() {}

  /* 0 1 2 3 4 5 6 7 8 9 -> 19 (0 -> 18) 10 -> ceil(log2(10)) -> 4
   9 * 10 / 2 => 45
     [ 45 10 35 3 7 18 17 1 2 3 4 11 7 8 9 0 1 0 0 0 0 0 0 5 6 0 0 0 0 0 0 ]
            0-9 (9 / 2 -> 4) 0
            / \
     1 0-4      2 5-9 ((5+9)/2 -> 7
       / \        / \
     0-2  3-4    5-7 8-9 ((8+9)/2 -> 8    6 * 2 + 1 -> 13
     / \  | |    |  \ \ \                 6 * 2 + 2 -> 14
 7 0-1  2 3 4   5-6  7 8 9
   | |          | |
   0 1          5 6     11 * 2 + 1 => 23 -> 24
left -> 15 -> 7 * 2 + 1 => 15
right-> 16 -> 7 * 2 + 2 => 16
   */
  void initialize_st(const vector<int> & values) {
    raw_values = values;
    segment_sum_arr = vector<int>(pow(2, 1 + int(ceil(log2(values.size())))), 0);
    initialize_segment_sum_recur(raw_values, segment_sum_arr, 0, 0, raw_values.size() - 1);
  }

  bool update(int id, int value) {
    if (!is_id_within_range(id)) { return false; }
    update_segment_sum_recur(value - raw_values[id], id, 0, 0, raw_values.size() - 1);
    raw_values[id] = value;
    return true;
  }

  int calc_segment_sum(int id_from, int id_to){
    if (id_from > id_to || !is_id_within_range(id_from) ||
                           !is_id_within_range(id_to)) { return INT_MIN; }
    return get_segment_sum_recur(id_from, id_to, 0, 0, raw_values.size() - 1);
  }

  vector<int> get_segment_sum_arr() { return segment_sum_arr; }
  vector<int> get_raw() { return raw_values; }

private:
  void update_segment_sum_recur(int delta, int updated_id,
                                int curr_segment_sum_id,
                                int segment_from, int segment_to) {
    if (segment_from <= updated_id && updated_id <= segment_to) {
// cout << "update: " << segment_from << " : " << segment_to << " : " << segment_sum_arr[curr_segment_sum_id] << " : " << segment_sum_arr[curr_segment_sum_id] + delta << endl;
      segment_sum_arr[curr_segment_sum_id] += delta;
    } else { return; }
    if (segment_from >= segment_to) { return; }
    update_segment_sum_recur(delta, updated_id, curr_segment_sum_id * 2 + 1,
                             segment_from, (segment_from + segment_to) / 2);
    update_segment_sum_recur(delta, updated_id, curr_segment_sum_id * 2 + 2,
                             (segment_from + segment_to) / 2 + 1, segment_to);
  }

  /* return sum for any segment within searching range & add up all */
  int get_segment_sum_recur(int id_from, int id_to,
                            int curr_segment_sum_id,
                            int segment_from,
                            int segment_to) {
    if (segment_from > segment_to) { return 0; }
    /* segment_from ... segment_to ... id_from ... id_to */
    if (segment_to < id_from || id_to < segment_from) { return 0; }
    /* id_from ... segment_from ... segment_to ... id_to */
    if (segment_from >= id_from && segment_to <= id_to) {
// cout << "hit: " << segment_from << " : " << segment_to << " : " << segment_sum_arr[curr_segment_sum_id] << endl;
      return segment_sum_arr[curr_segment_sum_id];
    }
    int left_segment_sum  = get_segment_sum_recur(id_from, id_to, curr_segment_sum_id * 2 + 1,
                                                  segment_from, (segment_from + segment_to) / 2);
    int right_segment_sum = get_segment_sum_recur(id_from, id_to, curr_segment_sum_id * 2 + 2,
                                                  (segment_from + segment_to) / 2 + 1, segment_to);
    return (left_segment_sum + right_segment_sum);
  }

  int initialize_segment_sum_recur(vector<int> & raw_values,
                                   vector<int> & segment_sum_arr,
                                   int curr_segment_sum_id,
                                   int segment_from,
                                   int segment_to) {
    if (segment_from > segment_to) { return 0; }
    if (segment_from == segment_to) {
      segment_sum_arr[curr_segment_sum_id] = raw_values[segment_from];
    } else {
      int left_seg_sum = initialize_segment_sum_recur(
        raw_values, segment_sum_arr, curr_segment_sum_id * 2 + 1,
        segment_from, (segment_from + segment_to) / 2
      );
      int right_seg_sum = initialize_segment_sum_recur(
        raw_values, segment_sum_arr, curr_segment_sum_id * 2 + 2,
        (segment_from + segment_to) / 2 + 1, segment_to
      );
      segment_sum_arr[curr_segment_sum_id] += (left_seg_sum + right_seg_sum);
    }
    return segment_sum_arr[curr_segment_sum_id];
  }

  bool is_id_within_range(int i) { return (i >= 0 && i < raw_values.size()); }

  vector<int> raw_values;
  vector<int> segment_sum_arr;
};

  /**
   * 327. Count of Range Sum
   * - Given an integer array nums, return the number of range sums that lie in
   *   [lower, upper] inclusive. Range sum S(i, j) is defined as the sum of the
   *   elements in nums between indices i and j (i ≤ j), inclusive.
   * Note:
   * - A naive algorithm of O(n2) is trivial. You MUST do better than that.
   * Example:
   * - Input: nums = [-2,5,-1], lower = -2, upper = 2,
   * - Output: 3 
   * - Explanation: The three ranges are : [0,0], [2,2], [0,2] and their
   *   respective sums are: -2, -1, 2.
   * Intuition:
   * - via BIT/SEG-TREE, we can easily buildup(n) for range-sum query of any
   *   range in log(n) time, but here we are not dealing with query, but to
   *   count the # of ranges that meets the sum requirements. (a tree-walk?)
   * - however, we may find sth simpler which may use idea of divide-conqure
   *   a range is a subset of data that been looked at, they are consequtive.
   * - for [0...i] & [i+1...n], assume we already know left-cnt & right-cnt,
   *   then obviously any new valid ranges(same sum) must contains items from
   *   both side, and it is a simple O(n) check (because they are a range!)
   * - O(nlogn), S(n)
   * - acutally combining phase is hard, needs additional process, as # of
   *   ranges still being n2 instead of n
   * - Another big hint is that we only care about the count instead of acutal
   *   plan, which leads us to think we could benefit via sorting intermediate
   *   results.
   * - assume we have a prefix sum arr, which looks like:
   *   [ 0, 0.1, 0.2, 0.3 ... 0.n ]
   *   then each elem is a valid sum for range starts from beginning, the key
   *   depends how to utilize it efficiently, for any 2 prefix sum, we know
   *   range(i,j) = pfs(0.j) - pfs(0.i-1)
   * - if we divide the prefix sum by 2, then we have:
   *   [ 0, 0.1, 0.2, 0.3 ... 0.i ] [ 0.i+1, 0.i+2, 0.i+3, ... 0.n ]
   *         ^---------^       p
   *   ALL pfs on right side includes all elems from elems in left side, any
   *   combination of r-pfs - l-pfs represents a valid range!
   * - EVEN better, if 2 parts are sorted in asc order already, then all we
   *   need to is to figure out a portion of pfs on the right side 
   */
  static int calc_ranges_qualified_recur(vector<int> & pfs,
                                         int lower, int upper,
                                         int start_pos, int end_pos) {
    if (lower > upper) { return 0; }
    int curr_cnt = 0, pivot = (lower + upper) / 2;
    int left_cnt = calc_ranges_qualified_recur(pfs, lower, upper, start_pos, pivot);
    int right_cnt = calc_ranges_qualified_recur(pfs, lower, upper, pivot + 1, end_pos);

    int left_idx = pivot, right_idx = pivot, total_cnt = 0;
    for (int i = lower; i < pivot; i++) {
      while(left_idx < end_pos && pfs[left_idx] - pfs[i] < lower) { left_idx++; }
      while(right_idx < end_pos && pfs[right_idx] - pfs[i] <= upper) { right_idx++; }
      total_cnt += (right_idx - left_idx);
    }
    std::inplace_merge(pfs.begin() + start_pos, pfs.begin() + pivot + 1, pfs.end());
    return total_cnt + left_cnt + right_cnt;
  }

  static int calc_ranges_qualified(vector<int> & nums, int lower, int upper) {
    vector<long> prefix_sum_arr(nums.begin(), nums.end());
    for (int i = 1; i < prefix_sum_arr.size(); i++) {
      prefix_sum_arr[i] -= prefix_sum_arr[i - 1];
    }
    return calc_ranges_qualified_recur(
      prefix_sum_arr, lower, upper, 0, prefix_sum_arr.size() - 1
    );
  }

  static void test_calc_ranges_qualified() {
    cout << "5. test_calc_ranges_qualified" << endl;
    int result = 0;
    vector<int> test_output = { 3 };
    vector<vector<int>> test_input = { { -2, 5, -1 } };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_ranges_qualified(test_input[i]);
      assert(result == test_output[i]);
    }
  }
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
  using range_sum::segment_sum_tree;

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

  cout << "3. binary_index_tree" << endl;
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

  cout << "4. segment_sum_tree" << endl;
  segment_sum_tree sst({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
  cout << "[ 45 10 35 3 7 18 17 1 2 3 4 11 7 8 9 0 1 0 0 0 0 0 0 5 6 0 0 0 0 0 0 0 ] <=>" << endl;
  print_all_elem<int>(sst.get_segment_sum_arr());
  for (int i = 0; i < sst.get_raw().size(); i++) {
    cout << sst.calc_segment_sum(0, i) << " ";
  } cout << endl;
  for (int i = 0; i < sst.get_raw().size(); i++) {
    sst.update(i, sst.get_raw()[i] + 1);
  }
  print_all_elem<int>(sst.get_segment_sum_arr());
  return 0;
}
