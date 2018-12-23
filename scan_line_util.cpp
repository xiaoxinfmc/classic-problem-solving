#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <set>
#include <climits>
#include <cstdint>
#include <list>
#include <utility>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>

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
  template <class t1, class t2>
  static void print_all_pair_list(const list<pair<t1, t2>> & input) {
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
    point(long long xv = -1, long long yv = -1) : x(xv), y(yv) {}
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
      if ((l.x_pos < r.x_pos) || (l.x_pos == r.x_pos && l.y_pos < r.y_pos) ||
          (l.x_pos == r.x_pos && l.y_pos == r.y_pos && l.rect_id < r.rect_id)){ return true; }
      return false;
    }

    static bool sort_by_y_pos(const rec_point & l, const rec_point & r) {
      if ((l.y_pos < r.y_pos) || (l.y_pos == r.y_pos && l.x_pos < r.x_pos) ||
          (l.x_pos == r.x_pos && l.y_pos == r.y_pos && l.rect_id < r.rect_id)){ return true; }
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
   * - input will be an array of rectangles by bl & tr points, goal is to calc key points
   *     @---+ (2,3)     - break down inputs by rec_point with its start pos & max y
   *     |   |           - sort by its x pos in asc order (with y val as tie breaker)
   * @-------+ (2,2)     - scan from min x -> right, keep upd the max y available.
   * |   |   |           - add up the union of their area.
   * |   +---@---+ (3,1) - and its actually more efficient to use the points as star.
   * |   |   |   |         which means we will be using the tl & tr points(not bl & tr)
   * +---+---+---@       - [ 0:2:0 1:1:2 1:3:1 2:2:0 2:3:1 3:1:2 ]
   *(0,0)(1,0)           - again, we actually needs all external intersection points @
   *   (curr-x-pos)      - to insert those intersections, we scan the sorted list of points
   *     @---x           - if curr point has a higher y_pos & its x_pos is < prev point end-x_pos
   *     | 2 |             or curr point has a lower y_pos & its x_pos is > prev point end-x_pos
   * @-------+           - ideally we only want to keep x & @ (exactly as skyline problem)
   * | 1 | 2 |           - all key points are top-left point of each segment rectangle!!!!
   * +---|---+ (active)
   * | 1 | 2 |   |
   * |   +---@---+       @---------+
   * | 1 | 2 | 3 |       |    4    |
   * +---+---+---@-------+---------@ (@ -> all top-left point of any union of rectangle)
   */

  static void dedup_x_pos(list<pair<int, int>> & boundary_points) {
    for (auto pt_itr = boundary_points.begin(); pt_itr != boundary_points.end();) {
      auto next_itr = pt_itr; next_itr++;
      if (next_itr != boundary_points.end() && pt_itr->first == next_itr->first) {
        pt_itr = boundary_points.erase(pt_itr);
      } else {  pt_itr++; }
    }
  }

  static void dedup_y_pos(list<pair<int, int>> & boundary_points) {
    for (auto pt_itr = boundary_points.begin(); pt_itr != boundary_points.end();) {
      auto next_itr = pt_itr; next_itr++;
      if (next_itr != boundary_points.end() && pt_itr->second == next_itr->second) {
        pt_itr = boundary_points.erase(next_itr);
      } else {  pt_itr++; }
    }
  }

  vector<pair<int, int>> get_boundary_points(vector<vector<int>> & rects) {
    list<pair<int, int>> boundary_points;
    if (rects.empty()) { return vector<pair<int, int>>(); }

    /* rec_point(int x, int y, int rid, bool is_bl, bool is_srt_by_x) */
    vector<rec_point> points_sort_by_x_pos;
    for (int i = 0; i < rects.size(); i++) {
      points_sort_by_x_pos.push_back(rec_point(rects[i][0], rects[i][2], i, true, true));
      points_sort_by_x_pos.push_back(rec_point(rects[i][1], rects[i][2], i, false, true));
    }
    sort(points_sort_by_x_pos.begin(), points_sort_by_x_pos.end());
    multiset<int> y_pos_set = {0};
    int prev_y_pos = 0;
    vector<rec_point> key_points;
    for (auto & curr_point : points_sort_by_x_pos) {
      if (curr_point.is_bottom_left) {
        /* enter here we see a begin of a new rect, we insert its y_pos */
        y_pos_set.insert(curr_point.y_pos);
      } else {
        /* we see an end of a rect, obviously only 1 y_pos needs to rm */
        y_pos_set.erase(y_pos_set.find(curr_point.y_pos));
      }
      if (* y_pos_set.rbegin() != prev_y_pos) {
        prev_y_pos = * y_pos_set.rbegin();
        boundary_points.push_back(pair<int, int>(curr_point.x_pos, prev_y_pos));
      }
    }
    dedup_x_pos(boundary_points);
    dedup_y_pos(boundary_points);
    dedup_y_pos(boundary_points);
    return vector<pair<int, int>>(boundary_points.begin(), boundary_points.end());
  }

  static void test_get_boundary_points() {
    cout << "4. test_get_boundary_points" << endl;
    vector<pair<int, int>> result;
    vector<vector<vector<int>>> test_input = {
      { {2, 9, 10}, {3, 7, 15}, {5, 12, 12}, {15, 20, 10}, {19, 24, 8} },
      { {0,2147483647,2147483647} },
      { {0,2,3},{2,5,3} },
      { {2,9,10},{9,12,15} }, { {0,2,3},{2,4,3},{4,6,3},{6,8,3}},
      { {0,2,3},{2,4,3},{5,6,3},{6,8,3} }
    };
    vector<vector<pair<int, int>>> test_output = {
      { {2, 10}, {3, 15}, {7, 12}, {12, 0}, {15, 10}, {20, 8}, {24, 0} },
      { {0,2147483647}, {2147483647, 0} },
      { { 0, 3 }, { 5, 0 } },
      { {2,10},{9,15},{12,0} }, {{0, 3}, {8, 0}}, {{0, 3}, {4, 0}, { 5, 3 }, { 8, 0 }}
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = get_boundary_points(test_input[i]);
      print_all_pairs(result);
      cout << "<=>" << endl;
      print_all_pairs(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) {
        assert(result[j] == test_output[i][j]);
      }
    }
  }

  /**
   * 803. Shortest Distance from All Buildings
   * - You want to build a house on an empty land which reaches all buildings
   *   in the shortest amount of distance. You can only move up, down, left
   *   and right. You are given a 2D grid of values 0, 1 or 2, where:
   * - Each **0** marks an empty land which you can pass by freely.
   *   Each **1** marks a building which you cannot pass through.
   *   Each **2** marks an obstacle which you cannot pass through.
   * Example
   * - For example, given three buildings at (0,0), (0,4), (2,2), and an
   *   obstacle at (0,2) (row, col):
   *   1 - 0 - 2 - 0 - 1
   *   |   |   |   |   |
   *   0 - 0 - x - 0 - 0
   *   |   |   |   |   |
   *   0 - 0 - 1 - 0 - 0
   * - The point (1,2) is an ideal empty land to build a house, as the total
   *   travel distance of 3+3+1=7 is minimal. So return 7.
   * Intuition:
   * - need to find a free point where it can reach all buildings with min cost
   * - all source shortest path? -> O(n^3), again, may not be that bad, because
   *   diff edges share the same weight, bfs on every free point -> O(n^2) ?
   * - optimization would be avoid redudant calc, as the relation of shortest
   *   path of (point-a -> point-b) can be cached efficiently, "i$j$k$l" -> dist
   */
  static int gen_point_key(int i, int j, const vector<vector<int>> & grid) {
    return i * grid.front().size() + j;
  }

  static bool is_point_valid(int x, int y, int max_row_id, int max_col_id) {
    return (x >= 0 && x <= max_row_id && y >= 0 && y <= max_col_id);
  }

  static void search_and_update_dist_map(const vector<vector<int>> & grid,
                                         point & start_pnt,
                                         unordered_map<int, int> & shortest_dist_map) {
    vector<bool> visit_lookup(grid.size() * grid.front().size(), false);
    deque<pair<int, point>> bfs_buffer = { pair<int, point>(0, start_pnt) };
    int max_row_id = grid.size() - 1, max_col_id = grid.front().size() - 1;

    /* start bfs search for all buildings */
    while (!bfs_buffer.empty()) {
      /* read out curr pos info */
      pair<int, point> entry_pair = bfs_buffer.front();
      int curr_dist = entry_pair.first;
      point curr_point = entry_pair.second;

      /* skip out of bound cases & mark flag as needed */
      bfs_buffer.pop_front();
      if (!is_point_valid(curr_point.x, curr_point.y, max_row_id, max_col_id) ||
          2 == grid[curr_point.x][curr_point.y]) { continue; }
      int curr_point_key = gen_point_key(curr_point.x, curr_point.y, grid);
      if (true == visit_lookup[curr_point_key]) { continue; }
      visit_lookup[curr_point_key] = true;

      /* check to see if we already reach a building & cannot pass a building */
      if (1 == grid[curr_point.x][curr_point.y]) {
        shortest_dist_map[curr_point_key] = curr_dist; continue;
      }

      /* appending new nodes on the path */
      bfs_buffer.push_back(pair<int, point>(curr_dist + 1, point(curr_point.x + 1, curr_point.y)));
      bfs_buffer.push_back(pair<int, point>(curr_dist + 1, point(curr_point.x - 1, curr_point.y)));
      bfs_buffer.push_back(pair<int, point>(curr_dist + 1, point(curr_point.x, curr_point.y + 1)));
      bfs_buffer.push_back(pair<int, point>(curr_dist + 1, point(curr_point.x, curr_point.y - 1)));
    }
  }

  static int calc_shortest_dist_to_all(const vector<vector<int>> & grid) {
    int min_dist_sum = INT_MAX, curr_dist_sum = 0, total_cnt = 0;
    vector<point> free_points;
    for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid[i].size(); j++) {
        if (1 == grid[i][j]) { total_cnt += 1; }
        if (0 == grid[i][j]) { free_points.push_back(point(i, j)); }
      }
    }
    /* holds all pos-key -> dist from a certain pt to all building */
    unordered_map<int, int> shortest_dist_map;
    for (auto & start_pnt : free_points) {
      search_and_update_dist_map(grid, start_pnt, shortest_dist_map);
      if (total_cnt == shortest_dist_map.size()) {
        for (auto & dist_pair : shortest_dist_map) { curr_dist_sum += dist_pair.second; }
        min_dist_sum = min(min_dist_sum, curr_dist_sum);
      }
      shortest_dist_map.clear();
      curr_dist_sum = 0;
    }
    return min_dist_sum;
  }

  static void test_calc_shortest_dist_to_all() {
    cout << "5. test_calc_shortest_dist_to_all" << endl;
    int result = 0;
    vector<int> test_output = { 7, 88 };
    vector<vector<vector<int>>> test_input = {
      { { 1, 0, 2, 0, 1 }, { 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0 } },
      {{1,1,1,1,1,0},{0,0,0,0,0,1},{0,1,1,0,0,1},{1,0,0,1,0,1},{1,0,1,0,0,1},{1,0,0,0,0,1},{0,1,1,1,1,0}}
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_shortest_dist_to_all(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 391. Perfect Rectangle
   * - Given N axis-aligned rectangles where N > 0, determine if they all
   *   together form an exact cover of a rectangular region.
   * - Each rectangle is represented as a bottom-left point and a top-right
   *   point. For example, a unit square is represented as [1,1,2,2].
   *   (coordinate of bottom-left point is (1, 1) and top-right point is (2, 2))
   * Example 1:
   *   rectangles = { {1,1,3,3}, {3,1,4,2}, {3,2,4,4}, {1,3,2,4}, {2,3,3,4} }
   *   Return true. All 5 rectangles together form an exact cover of a rectangular region.
   * Example 2:
   *   rectangles = { {1,1,2,3}, {1,3,2,4}, {3,1,4,2}, {3,2,4,4} }
   *   Return false. Because there is a gap between the two rectangular regions.
   * Example 3:
   *   rectangles = { {1,1,3,3}, {3,1,4,2}, {1,3,2,4}, {3,2,4,4} }
   *   Return false. Because there is a gap in the top center.
   * Example 4:
   *   rectangles = { {1,1,3,3}, {3,1,4,2}, {1,3,2,4}, {2,2,4,4} }
   *   Return false. Because two of the rectangles overlap with each other.
   * Intuition:
   * - a perfect rect? - area should match, no shadowing, boundary smooth.
   * +--- r-- r-rx     - points should not cover each other or by rect.
   * | 1 | 2 | 3 |     - all max y-pos of top-right point should be same
   *  l-r l-- l-r      - all min x-pos of bottom-left point should be same
   * | 1 |   4   |     - sum of all rect area should be same compare to whole
   * xl-- l------@     - when all rect adjacent each other, see 4 points end
   */
  class rpoint {
  public:
    rpoint(int x, int y) : x_pos(x), y_pos(y) {
      hash_key = std::to_string(x_pos) + "$" + std::to_string(y_pos);
    }
    virtual ~rpoint() {}
    friend bool operator< (const rpoint & a, const rpoint & b) {
      return ((a.x_pos < b.x_pos) || (a.x_pos == b.x_pos && a.y_pos < b.y_pos));
    }
    friend bool operator== (const rpoint & a, const rpoint & b) {
      return (a.x_pos == b.x_pos && a.y_pos == b.y_pos);
    }
    friend ostream & operator<< (ostream & os, const rpoint x) {
      os << "(" << x.x_pos << ", " << x.y_pos << ")"; return os;
    }
    struct rpoint_hash_func {
      size_t operator()(const rpoint & pt) const { return std::hash<string>()(pt.hash_key); }
    };
    int x_pos, y_pos;
    string hash_key;
  };

  static void check_and_update_points_set(const rpoint & lb_point, const rpoint & lt_point,
                                          const rpoint & rb_point, const rpoint & rt_point,
                                          unordered_set<rpoint, rpoint::rpoint_hash_func> & final_points_set) {
    vector<rpoint> points_to_chk = { lb_point, lt_point, rb_point, rt_point };
    for (auto & pt : points_to_chk) {
      if (final_points_set.count(pt) > 0) { final_points_set.erase(pt); } else { final_points_set.insert(pt); }
    }
  }

  static bool is_rects_perfect(vector<vector<int>>& rects) {
    /* gen all points, sort by x/y pos */
    vector<rpoint> points;
    rpoint bt_point(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
           tr_point(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    rpoint lb_point(0, 0), lt_point(0, 0), rb_point(0, 0), rt_point(0, 0);
    unordered_set<rpoint, rpoint::rpoint_hash_func> final_points_set;
    int min_x_pos = std::numeric_limits<int>::max(),
        min_y_pos = std::numeric_limits<int>::max(),
        max_x_pos = std::numeric_limits<int>::min(),
        max_y_pos = std::numeric_limits<int>::min(),
        area_sum = 0;
    for (int i = 0; i < rects.size(); i++) {
      points.push_back(rpoint(rects[i][0], rects[i][1]));
      bt_point = min(bt_point, points.back());
      min_x_pos = min(min_x_pos, points.back().x_pos);
      min_y_pos = min(min_y_pos, points.back().y_pos);

      points.push_back(rpoint(rects[i][2], rects[i][3]));
      tr_point = max(tr_point, points.back());
      max_x_pos = max(max_x_pos, points.back().x_pos);
      max_y_pos = max(max_y_pos, points.back().y_pos);

      area_sum += ((rects[i][2] - rects[i][0]) * (rects[i][3] - rects[i][1]));

      lb_point = rpoint(rects[i][0], rects[i][1]);
      lt_point = rpoint(rects[i][0], rects[i][3]);
      rb_point = rpoint(rects[i][2], rects[i][1]);
      rt_point = rpoint(rects[i][2], rects[i][3]);

      check_and_update_points_set(lb_point, lt_point, rb_point, rt_point, final_points_set);
    }
    if (bt_point.x_pos != min_x_pos || bt_point.y_pos != min_y_pos) { return false; }
    if (tr_point.x_pos != max_x_pos || tr_point.y_pos != max_y_pos) { return false; }
    if (area_sum != ((max_x_pos - min_x_pos) * (max_y_pos - min_y_pos))) { return false; }
    return (4 == final_points_set.size() && final_points_set.count(bt_point) > 0 &&
                                            final_points_set.count(tr_point) > 0 &&
                                            final_points_set.count(rpoint(bt_point.x_pos, tr_point.y_pos)) > 0 &&
                                            final_points_set.count(rpoint(tr_point.x_pos, bt_point.y_pos)) > 0);
  }

  static void test_is_rects_perfect() {
    cout << "6. test_is_rects_perfect" << endl;
    bool result = false;
    vector<bool> test_output = { true, false, false, false, true, false };
    vector<vector<vector<int>>> test_input = {
      { {1,1,3,3}, {3,1,4,2}, {3,2,4,4}, {1,3,2,4}, {2,3,3,4} },
      { {1,1,2,3}, {1,3,2,4}, {3,1,4,2}, {3,2,4,4} },
      { {1,1,3,3}, {3,1,4,2}, {1,3,2,4}, {3,2,4,4} },
      { {1,1,3,3}, {3,1,4,2}, {1,3,2,4}, {2,2,4,4} },
      {{0,0,4,1},{7,0,8,2},{6,2,8,3},{5,1,6,3},{4,0,5,1},{6,0,7,2},{4,2,5,3},{2,1,4,3},{0,1,2,2},{0,2,2,3},{4,1,5,2},{5,0,6,1}},
      {{0,0,1,1},{0,0,2,1},{1,0,2,1},{0,2,2,3}}
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = is_rects_perfect(test_input[i]);
      cout << test_output[i] << " <=> " << result << endl;
      assert(test_output[i] == result);
    }
  }

  /**
   * 354. Russian Doll Envelopes
   * - You have a number of envelopes with widths and heights given as a pair
   *   of integers (w, h). One envelope can fit into another if and only if
   *   both the width and height of one envelope is greater than the width
   *   and height of the other envelope.
   * - What is the maximum number of envelopes can you Russian doll?
   * Note:
   * - Rotation is not allowed.
   * Example:
   * - Input: [[5,4],[6,4],[6,7],[2,3]]
   * - Output: 3
   * Explanation:
   * - The maximum number of envelopes you can Russian doll is 3
   *   [2,3] => [5,4] => [6,7]
   * Intuition:
   * - bf way is to run for loop against all item, gives you O(n2)
   * - and the lower bound won't be less than find the max, obviously > O(n)
   * - realistically looking for solution of O(nlogn)?
   * - strategy wise, not clear about the pattern yet, more like sort/hash
   *   related thing.
   * - sort all envs by w 1st, then by h 2nd, we have 2 order of things:
   *   [ 0 2 1 3 5 4 ]
   *   [ 2 1 5 4 3 0 ] -> 2 1 5 4
   *   the goal is to figure out the longest seq that satisfy both constraints
   *   same as build out graph based on 2 sorted sequences, then try to figure
   *   out the longest seq without conflicts(topological order).
   * - topological sorting & find the path with max length based on 2?
   *   or as simple as build out graph then do bfs?
+-----------+
v           v-------+
0 - 2 - 1 - 3 - 5 - 4
        +-------^
0 - 1 x 2 x 3 - 4
   * - sorting => nlogn, build out graph => n, toplogical-sort => n
   *   ==>> nlogn
   */
  class envelop {
  public:
    envelop(int i, int w, int h) : id(i), width(w), height(h) {}
    virtual ~envelop() {}
    int id, width, height;
    friend ostream & operator<<(ostream & os, const envelop & ep) {
      os << ep.id << ":" << ep.width << ":" << ep.height; return os;
    }
    static bool sort_pairs_by_width(const envelop & lenv, const envelop & renv) { return lenv.width < renv.width; }
    static bool sort_pairs_by_height(const envelop & lenv, const envelop & renv) { return lenv.height < renv.height; }
  };

  class ep_vertex {
  public:
    ep_vertex(int i) : vid(i), is_visited(false) {}
    virtual ~ep_vertex() {}
    unordered_set<int> outgoing_vids;
    int vid; bool is_visited;
    friend ostream & operator<<(ostream & os, const ep_vertex & ep) {
      os << ep.vid << "|";
      for (auto x : ep.outgoing_vids) { os << x << ":"; }
      return os;
    }
  };

  static void add_links_for_envelop_graph(vector<ep_vertex> & envelop_graph,
                                          vector<envelop> & envelop_arr) {
    /* only add links if they fits, avoid equal case */
    for (int i = 0; i < envelop_arr.size(); i++) {
      if (i < envelop_arr.size() - 1) {
        envelop_graph[envelop_arr[i].id].outgoing_vids.insert(envelop_arr[i + 1].id);
      }
    }
  }

  static bool is_link_valid(const vector<pair<int, int>> & eps,
                            int l_vid, int r_vid) {
    return (eps[l_vid].first < eps[r_vid].first && eps[l_vid].second < eps[r_vid].second);
  }

  static int find_max_valid_path_len(vector<ep_vertex> & eps_graph, int svid,
                                     const vector<pair<int, int>> & eps) {
    int max_len = 1;
    vector<int> ep_stack = { svid }, ep_path = { svid };
    while (!ep_stack.empty()) {
      ep_vertex & curr_v = eps_graph[ep_stack.back()];
      ep_stack.pop_back();
      if (false == curr_v.is_visited) {
        curr_v.is_visited = true;
        if (!ep_path.empty() && ep_path.back() != curr_v.vid) {
print_all_elem<int>(ep_path);
cout << ep_path.back() << " " << curr_v.vid << endl;
          if (!ep_path.empty()) {
            if (true == is_link_valid(eps, ep_path.back(), curr_v.vid)) {
              break;
            } else {
              eps_graph[ep_stack.back()].outgoing_vids.erase(curr_v.vid);
              if (eps_graph[ep_stack.back()].outgoing_vids.empty()) { ep_path.pop_back(); }
            }
          }
          ep_path.push_back(curr_v.vid);
          max_len = max(ep_path.size(), (size_t)max_len);
        }
        for (auto & vid : curr_v.outgoing_vids) { ep_stack.push_back(vid); }
      }
    }
    return max_len;
  }

  static int calc_max_envelopes(vector<pair<int, int>>& envelopes) {
    if (envelopes.size() <= 0) { return 0; }

    /* initialize all data & graph */
    vector<ep_vertex> envelop_graph(envelopes.size(), ep_vertex(0));
    vector<envelop> envelop_arr;
    for (int i = 0; i < envelopes.size(); i++) {
      envelop_arr.push_back(envelop(i, envelopes[i].first, envelopes[i].second));
      envelop_graph[i].vid = i;
    }

    /* build links based on width */
    sort(envelop_arr.begin(), envelop_arr.end(), envelop::sort_pairs_by_width);
    add_links_for_envelop_graph(envelop_graph, envelop_arr);
print_all_elem<envelop>(envelop_arr);
//cout << "[ 0 2 1 3 5 4 ]" << endl;

    /* build links based on height */
    sort(envelop_arr.begin(), envelop_arr.end(), envelop::sort_pairs_by_height);
    add_links_for_envelop_graph(envelop_graph, envelop_arr);
print_all_elem<envelop>(envelop_arr);
//cout << "[ 2 1 5 4 3 0 ]" << endl;

//cout << "[ 2 1 5 4 ]" << endl;
    /* start calc longest toplogical order */
    return find_max_valid_path_len(envelop_graph, envelop_arr[0].id, envelopes);
  }

/*
+-----------+
v           v-------+
0 - 2 - 1 - 3 - 5 - 4
        +-------^
*/
  static void test_calc_max_envelopes() {
    int result = -1;
    vector<int> test_output = { 3, 4, 6 };
    vector<vector<pair<int, int>>> test_input = {
      {{5,4},{6,4},{6,7},{2,3}},
      {{0,5},{2,1},{1,0},{3,4},{5,3},{4,2}},
      {{2,2},{6,4},{5,3},{7,8},{9,10},{8,9}},
    };
    cout << "7. test_calc_max_envelopes" << endl;
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_max_envelopes(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(test_output[i] == result);
    }
  }
};

/*
print_all_elem<ep_vertex>(envelop_graph);
*/
int main(void) {
  using scan_line_util::test_count_num_of_planes;
  using scan_line_util::test_calc_closest_pair_dist;
  using scan_line_util::test_calc_union_of_rectangles;
  using scan_line_util::test_get_boundary_points;
  using scan_line_util::test_calc_shortest_dist_to_all;
  using scan_line_util::test_is_rects_perfect;
  using scan_line_util::test_calc_max_envelopes;

  test_count_num_of_planes();
  test_calc_closest_pair_dist();
  test_calc_union_of_rectangles();
  test_get_boundary_points();
  test_calc_shortest_dist_to_all();
  test_is_rects_perfect();
  test_calc_max_envelopes();
}
