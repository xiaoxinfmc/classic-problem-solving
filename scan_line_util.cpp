#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <set>
#include <climits>
#include <cstdint>
#include <list>
#include <utility>

namespace scan_line_util {
  using namespace std;

  template <class Type>
  static void print_all_elem(const vector<Type> & input) {
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
  template <class t1, class t2>
  static void print_all_pairs(const vector<pair<t1, t2>> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr.first << "|" << arr.second << " "; }
    cout << "]" << endl;
  }

  /**
   * 391 Number of Airplanes in the Sky
   * Description
   * - Given an interval list which are flying and landing time of the flight.
   *   How many airplanes are on the sky at most?
   * Notice
   * - If landing and flying happens at the same time, we consider landing should
   *   happen at first.
   * Example
   * - [ [1,10], [2,3], [5,8], [4,7] ]
   * - Return 3
   * Intuition:
   *           v--v
   *          v-----v
   *        v---------v
   *       v------------v
   *   |---------------------|
   *    1 v-----------v
   *    2 v-------------------v
   *    3     v-----v => end range move ahead
   *    4  v----------------------------v
   *    5       X  v---------------------------------v
   *   |-----------------------------------------------------|
   *         v-----v
   *      v------v   v-------v
   *   |-------------------------------------------|
   * - observation: max overlapping must contains >= 1 start time point
   * - sort all interval by start & end asc 1st.
   * - for each time point
   *     greedy search until rule not match (while)
   *     { rule: any subseq interval should cover some portion of the min end of curr. set of intvs }
   *     swith the starting time point
   * - use a map?
   *   key will be any time point (start, end)
   *   values will be any intervals covers the time point
   *   map will be constructed after sorting
   *   for each time point, perform binary search for both end & log the max size;
   * - Actually not that complex at all, after sorting, simply go through the points
   *   if it is a start point, then incr 1, otherwise decr by 1.
   */
  class interval {
  public:
    interval(int s, int e) : start(s), end(e) {}
    virtual ~interval() {}
    int start, end;
  };

  class time_point {
  public:
    time_point(int t, int id, bool b_start) :
      time(t), interval_id(id), is_start(b_start) {}
    virtual ~time_point() {}
    int time, interval_id;
    bool is_start;

    friend bool operator<(const time_point & l, const time_point & r) {
      return ((l.time < r.time) || (l.time == r.time && !l.is_start));
    }
  };

  static int count_num_of_planes(vector<interval> & schedule_arr) {
    if (schedule_arr.empty()) { return 0; }

    vector<time_point> points_vec;
    for (int i = 0; i < schedule_arr.size(); i++) {
      points_vec.push_back(time_point(schedule_arr[i].start, i, true));
      points_vec.push_back(time_point(schedule_arr[i].end, i, false));
    }
    sort(points_vec.begin(), points_vec.end());

    int max_cnt = 0, curr_cnt = 0;
    for (int curr_id = 0; curr_id < points_vec.size(); curr_id++) {
      curr_cnt = (points_vec[curr_id].is_start) ? (curr_cnt + 1) : (curr_cnt - 1);
      max_cnt = max(max_cnt, curr_cnt);
    }
    return max_cnt;
  }

  static void test_count_num_of_planes() {
    int result = 0;
    cout << "1. test_count_num_of_planes" << endl;
    vector<int> test_output = { 3 };
    vector<vector<interval>> test_input = { { interval(1,10), interval(2,3), interval(5,8), interval(4,7) } };
    for (int i = 0; i < test_input.size(); i++) {
      result = count_num_of_planes(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * Problem: Find closest pair of points in given array of n distinct points
   * - This problem can be solved by comparing all pairs of points, but then n^2.
   *   So, we need a better algorithm for this. Here, we'll discuss it using
   *   line sweep technique.
   *
   * - For this problem, we can consider the points in the array as our events.
   *   And in a set, we store the already visited points sorted by y coordinate.
   *   So, first we sort the points in x direction as we want our line to move
   *   towards right.
   *
   * - Now, suppose we have processed the points from 1 to N-1, and let h be
   *   the shortest distance we have got so far. For Nth point, we want to find
   *   points whose distance from Nth point is less than or equal to h.
   *
   * - Now, we know we can only go till h distance from Xn to find such point,
   *   and in the y direction we can go in h distance upwards and h distance
   *   downwards. So, all such points whose x coordinate lie in [Yn-h, Yn+h]
   *   and y coordinates lie in [Xn-h, Xn] are what we are concerned with and
   *   these form the active events of the set. All points in the set with x
   *   coordinates less than Xn-h are to be deleted. After this processing,
   *   we'll add the Nth point to the set.
   *
   * - One thing to note is that at any instance, the number of points which
   *   are active events is O(1)(there can be at most 5 points around a point
   *   which are active excluding the point itself).
   *
   * - {{2, 3}, {12, 30}, {40, 50}, {5, 1}, {12, 10}, {3, 4}}, 1.414214
   *
   * Intuition:
   *    x          - sort points via x & y incr. direction (moving right up)
   *     x  x    x - loop the points set left->right, bottom->top, each round
   *  x   x   x      keep a curr-pos (1st node), then find the closest points
   *     x  x        that ordered before the curr point.
   *               - the key is to check distance smartly, avoid redundants.
   *               - for node i, we already know the closest dist to node i
   *   +----+        is k, then for node i + i, we only need to check points
   *   | x  |        within the radius of k, if we see smaller one, then upd
   *   |h   |        the distance limit and move forward.
   *   +----x      - radius could be tricky, we can use bounding box x & y
   *   |x   |        instead, say searching range is curr x - h & y +/- h
   *   |h   |      - keep a balanced tree for search (kd tree? set to start)
   *   +----+        add curr x to bounding box & move to right & filter left
   */
  class point {
  public:
    point(long long xv, long long yv) : x(xv), y(yv) {}
    virtual ~point() {}
    long long x, y;
    friend bool operator< (const point & l, const point & r) {
      if ((l.x < r.x) || (l.x == r.x && l.y < r.y)) { return true; } else { return false; }
    }
    friend bool operator== (const point & l, const point & r) {
      return (l.x == r.x && l.y == r.y);
    }
  };

  static double calc_closest_pair_dist(vector<point> & points) {
    double curr_dist_limit = numeric_limits<double>::max();

    sort(points.begin(), points.end());

    set<point> points_buffer = { points.front() };
    int left_most_point_id = 0;

    for (int i = 1; i < points.size(); i++) {
      while (left_most_point_id < i && ((points[i].x - points[left_most_point_id].x) > curr_dist_limit)) {
        points_buffer.erase(points[left_most_point_id]); left_most_point_id += 1;
      }
      point bottom_left(points[i].x - curr_dist_limit, points[i].y - curr_dist_limit);
      for (set<point>::iterator itr  = points_buffer.lower_bound(bottom_left);
                                itr != points_buffer.end() && (itr->y <= points[i].y + 2 * curr_dist_limit);
                                itr++) {
        curr_dist_limit = min(curr_dist_limit, sqrt(pow(points[i].x - itr->x, 2.0) + pow(points[i].y - itr->y, 2.0)));
      }
      points_buffer.insert(points[i]);
    }

    return curr_dist_limit;
  }

  static void test_calc_closest_pair_dist() {
    cout << "2. test_calc_closest_pair_dist" << endl;
    double result = 0.0;
    vector<vector<point>> test_input = {
      { point(2, 3), point(12, 30), point(40, 50), point(5, 1), point(12, 10), point(3, 4) },
      { point(2, 3), point(12, 30), point(2, 2), point(40, 50), point(5, 1), point(12, 10), point(3, 4) }
    };
    vector<double> test_output = { 1.414214, 1 };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_closest_pair_dist(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(abs(result - test_output[i]) < 0.000001);
    }
  }

  /**
   * 850. Rectangle Area II
   * - We are given a list of (axis-aligned) rectangles.
   *   Each rectangle[i] = [x1, y1, x2, y2], where (x1, y1) are the coordinates
   *   of the bottom-left corner, and (x2, y2) are the coordinates of the
   *   top-right corner of the ith rectangle.
   * - Find the total area covered by all rectangles in the plane. Since the
   *   answer may be too large, return it modulo 10^9 + 7.
   * Example 1:
   * - Input: {{0,0,2,2},{1,0,2,3},{1,0,3,1}}
   *   Output: 6
   *   Explanation: As illustrated in the picture.
   * Example 2:
   * - Input: {{0,0,1000000000,1000000000}}
   *   Output: 49
   *   Explanation: The answer is 10^18 modulo (10^9 + 7), which is (10^9)^2 = (-7)^2 = 49.
   * Note:
   * - 1 <= rectangles.length <= 200
   * - rectanges[i].length = 4
   * - 0 <= rectangles[i][j] <= 10^9
   * - The total area covered by all rectangles will never exceed 2^63 - 1 and
   *   thus will fit in a 64-bit signed integer.
   */
  class rec_point {
  public:
    rec_point(int x, int y, int rid, bool is_bl, bool is_srt_by_x) :
      x_pos(x), y_pos(y), rect_id(rid), is_bottom_left(is_bl), is_sort_by_x_pos(is_srt_by_x) {}
    virtual ~rec_point(){}

    int x_pos, y_pos, rect_id;
    bool is_bottom_left, is_sort_by_x_pos;

    static bool sort_by_x_pos(const rec_point & l, const rec_point & r) {
      if ((l.x_pos < r.x_pos) || (l.x_pos == r.x_pos && l.y_pos < r.y_pos)){ return true; }
      return false;
    }

    static bool sort_by_y_pos(const rec_point & l, const rec_point & r) {
      if ((l.y_pos < r.y_pos) || (l.y_pos == r.y_pos && l.x_pos < r.x_pos)){ return true; }
      return false;
    }

    friend bool operator< (const rec_point & l, const rec_point & r) {
      return ((true == l.is_sort_by_x_pos) ? sort_by_x_pos(l, r) : sort_by_y_pos(l, r));
    }

    friend bool operator== (const rec_point & l, const rec_point & r) {
      return (l.x_pos == r.x_pos && l.y_pos == r.y_pos);
    }

    friend ostream & operator<< (ostream & os, const rec_point & pt) {
      os << pt.x_pos << ":" << pt.y_pos << ":" << pt.rect_id; return os;
    }
  };

  const static int64_t DEF_AREA_MODULO = 1000000007;

  static int calc_union_of_rectangles(vector<vector<int>>& rectangles) {
    int64_t union_of_area = 0;
    if (rectangles.empty()) { return union_of_area; }

    /* 1. scramble all critical points & sort in x & y */
    vector<bool> rect_active_lookup(rectangles.size(), false);

    vector<rec_point> points_arr_by_x;
    for (int i = 0; i < rectangles.size(); i++) {
      assert(4 == rectangles[i].size());
      points_arr_by_x.push_back(rec_point(rectangles[i][0], rectangles[i][1], i, true, true));
      points_arr_by_x.push_back(rec_point(rectangles[i][2], rectangles[i][3], i, false, true));
    }
    vector<rec_point> points_arr_by_y(points_arr_by_x.begin(), points_arr_by_x.end());
    for (auto & point : points_arr_by_y) { point.is_sort_by_x_pos = false; }
    sort(points_arr_by_x.begin(), points_arr_by_x.end());
    sort(points_arr_by_y.begin(), points_arr_by_y.end());

    /* 2. start scan left -> right, & bottom -> top */
    rect_active_lookup[points_arr_by_x.front().rect_id] = true;
    for (int i = 1; i < points_arr_by_x.size(); i++) {

      /* check if curr point actually moving to the right of x-axis, & log active rect */
      rec_point & curr_point = points_arr_by_x[i];
      int x_pos_dist = curr_point.x_pos - points_arr_by_x[i - 1].x_pos;
      if (0 == x_pos_dist) {
        /* de-activate the rect if we encounter a top-right point */
        rect_active_lookup[curr_point.rect_id] = curr_point.is_bottom_left; continue;
      }

      /* 1st point with larger x-pos, we found a region, then start to check overlap in y */
      int active_rect_cnt = 0, start_y_pos = 0;
      for (int j = 0; j < points_arr_by_y.size(); j++) {
        if (false == rect_active_lookup[points_arr_by_y[j].rect_id]) { continue; }
        if (true == points_arr_by_y[j].is_bottom_left) {
          if (0 == active_rect_cnt) { start_y_pos = points_arr_by_y[j].y_pos; };
          active_rect_cnt += 1;
        } else {
          active_rect_cnt -= 1;
          if (0 == active_rect_cnt) {
            union_of_area += (int64_t)(points_arr_by_y[j].y_pos - start_y_pos) * x_pos_dist;
          }
        }
      }
      /* de-activate the rect if we encounter a top-right point */
      rect_active_lookup[curr_point.rect_id] = curr_point.is_bottom_left;
    }

    return union_of_area % DEF_AREA_MODULO;
  }

  static void test_calc_union_of_rectangles() {
    cout << "3. test_calc_union_of_rectangles" << endl;
    int result = 0;
    vector<vector<vector<int>>> test_input = {
      {{0,0,2,2},{1,0,2,3},{1,0,3,1}}, {{0,0,1000000000,1000000000}}
    };
    vector<int> test_output = { 6, 49 };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_union_of_rectangles(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 218. The Skyline Problem
   * - A city's skyline is the outer contour of the silhouette formed by all the
   *   buildings in that city when viewed from a distance. Now suppose you are
   *   given the locations and height of all the buildings as shown on a cityscape
   *   photo (Figure A), write a program to output the skyline formed by these
   *   buildings collectively (Figure B).
   * - The geometric information of each building is represented by a triplet of
   *   integers [Li, Ri, Hi], where Li and Ri are the x coordinates of the left
   *   and right edge of the ith building, respectively, and Hi is its height.
   *   It is guaranteed 0 ≤ Li, Ri ≤ INT_MAX, 0 < Hi ≤ INT_MAX, Ri - Li > 0
   *   You may assume all buildings are perfect rectangles grounded on an
   *   absolutely flat surface at height 0.
   * - For instance, the dimensions of all buildings in Figure A are recorded
   *   as: [ [2 9 10], [3 7 15], [5 12 12], [15 20 10], [19 24 8] ] .
   * - The output is a list of "key points" (red dots in Figure B) in the format
   *   of [ [x1,y1], [x2, y2], [x3, y3], ... ] that uniquely defines a skyline.
   *   A key point is the left endpoint of a horizontal line segment. Note that
   *   the last key point, where the rightmost building ends, is merely used to
   *   mark the termination of the skyline, and always has zero height. Also,
   *   the ground in between any two adjacent buildings should be considered
   *   part of the skyline contour.
   * - For instance, the skyline in Figure B should be represented as:
   *   [ [2 10], [3 15], [7 12], [12 0], [15 10], [20 8], [24, 0] ].
   * Notes:
   * - The number of buildings in any input list is guaranteed to be in the range [0, 10000].
   * - The input list is already sorted in ascending order by the left x position Li.
   * - The output list must be sorted by the x position.
   * - There must be no consecutive horizontal lines of equal height in the output skyline.
   *   For instance, [...[2 3], [4 5], [7 5], [11 5], [12 7]...] is not acceptable; the
   *   three lines of height 5 should be merged into one in the final output as such:
   *   [...[2 3], [4 5], [12 7], ...]
   * Intuition:
   * - input will be an array of rectangles by bl & tr points, goal is to calc its area.
   *     *---* (2,3)     - break down inputs by rec_point with its start pos & max y
   *     |   |     - sort by its x pos in asc order (with y val as tie breaker)
   * *---@---* (2,2)     - scan from min x -> right, keep upd the max y available.
   * |   |   |     - add up the union of their area.
   * |   *---@---* (3,1) - and its actually more efficient to use the points as star.
   * |   |   |   |   which means we will be using the tl & tr points(not bl & tr)
   * x---x---+---+       - [ 0:2:0 1:1:2 1:3:1 2:2:0 2:3:1 3:1:2 ]
   *(0,0)(1,0)     - again, we actually needs all external intersection points @
   *         - to insert those intersections, we scan the sorted list of points
   *     x---*     - if curr point has a higher y_pos & its x_pos is < prev point end-x_pos
   *     |   |       or curr point has a lower y_pos & its x_pos is > prev point end-x_pos
   * x---+---*     - ideally we only want to keep x & @ (exactly as skyline problem)
   * |   |   |
   * *---|---* (2,2)
   * |   |   |
   * |   *---x---x (3,1)
   * |   |   |   |
   * x---x---+---x
   */
  vector<pair<int, int>> get_boundary_points(vector<vector<int>> & rectangles) {
    vector<pair<int, int>> boundary_points;
    if (rectangles.empty()) { return boundary_points; }

    return boundary_points;
  }

  static void test_get_boundary_points() {
    cout << "4. test_get_boundary_points" << endl;
    vector<pair<int, int>> result;
    vector<vector<vector<int>>> test_input = {
      { {2, 9, 10}, {3, 7, 15}, {5, 12, 12}, {15, 20, 10}, {19, 24, 8} }
    };
    vector<vector<pair<int, int>>> test_output = {
      { {2, 10}, {3, 15}, {7, 12}, {12, 0}, {15, 10}, {20, 8}, {24, 0} }
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = get_boundary_points(test_input[i]);
      print_all_pairs(result);
      cout << "<=>" << endl;
      print_all_pairs(test_output[i]);
    }
  }
};

int main(void) {
  using scan_line_util::test_count_num_of_planes;
  using scan_line_util::test_calc_closest_pair_dist;
  using scan_line_util::test_calc_union_of_rectangles;
  using scan_line_util::test_get_boundary_points;

  test_count_num_of_planes();
  test_calc_closest_pair_dist();
  test_calc_union_of_rectangles();
  test_get_boundary_points();
}
