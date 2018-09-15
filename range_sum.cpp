#include <iostream>
#include <vector>

namespace range_sum {
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

};

int main(void) {
  using std::cout;
  using std::endl;
  using std::vector;
  using range_sum::range_sum_immutable;

  vector<int> values({ -2, 0, 3, -5, 2, -1 });

  range_sum_immutable range_sum_obj(values);
  cout << "1. range_sum_immutable" << endl;
  cout << "1 <=> " << range_sum_obj.calc_sum_via_range(0, 2) << endl;
  cout << "-1 <=> " << range_sum_obj.calc_sum_via_range(2, 5) << endl;
  cout << "-3 <=> " << range_sum_obj.calc_sum_via_range(0, 5) << endl;

  return 0;
}
