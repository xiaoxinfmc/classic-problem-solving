#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <set>
#include <climits>

namespace scan_line_util {
  using namespace std;

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
   *             v--v
   *           v-----v
   *         v---------v
   *       v------------v
   *   |---------------------|
   *    1 v-----------v
   *    2 v-------------------v
   *    3     v-----v => end range move ahead
   *    4        v----------------------------v
   *    5             X  v---------------------------------v
   *   |-----------------------------------------------------|
   *               v-----v
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
};

int main(void) {
  using scan_line_util::test_count_num_of_planes;
  using scan_line_util::test_calc_closest_pair_dist;

  test_count_num_of_planes();
  test_calc_closest_pair_dist();
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
 */
