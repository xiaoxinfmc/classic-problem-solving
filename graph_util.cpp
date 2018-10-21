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
#include <bitset>

using namespace std;

namespace graph_util {
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

  /**
   * 133. Clone Graph
   * Nodes are labeled uniquely.
   * We use # as a separator for each node, and, as a separator for node label and
   * each neighbor of the node. As an example, consider the serialized graph
   * { 0, 1, 2#1, 2#2, 2 }.
   * The graph has a total of three nodes, and therefore contains three parts as
   * separated by #.
   * First node is labeled as 0. Connect node 0 to both nodes 1 and 2.
   * Second node is labeled as 1. Connect node 1 to node 2.
   * Third node is labeled as 2. Connect node 2 to 2 (itself), forming a self-cycle.
   * Visually, the graph looks like the following:
   *        1
   *       / \
   *      /   \
   *     0 --- 2
   *          / \
   *          \_/
   * 1, (0, 2)
   * 1 | [ 0, 2 ]    < 1 >
   */
  class undirected_graph_vertex {
  public:
    int label;
    vector<undirected_graph_vertex *> neighbors;
    undirected_graph_vertex(int value) : label(value) {}
    virtual ~undirected_graph_vertex() {}
  };

  typedef undirected_graph_vertex UndirectedGraphNode;

  static undirected_graph_vertex * clone_undirected_graph(undirected_graph_vertex * udag_start_ptr) {
    undirected_graph_vertex * new_vertex_ptr = NULL, * curr_node_ptr = NULL;
    if (NULL == udag_start_ptr) { return new_vertex_ptr; }

    /* visit lookup + holds label & new vertex ptr */
    unordered_map<undirected_graph_vertex *, undirected_graph_vertex *> existing_udg_ptrs = {
      { udag_start_ptr, new undirected_graph_vertex(udag_start_ptr->label) }
    };
    assert(NULL != existing_udg_ptrs[udag_start_ptr]);
    vector<undirected_graph_vertex *> udg_ptr_buffer = { udag_start_ptr };
    unordered_set<undirected_graph_vertex *> visit_lookup;

    while (!udg_ptr_buffer.empty()) {
      curr_node_ptr = udg_ptr_buffer.back();
      udg_ptr_buffer.pop_back();
      if (visit_lookup.count(curr_node_ptr) > 0) { continue; }
      visit_lookup.insert(curr_node_ptr);
      for (auto & udg_ptr : curr_node_ptr->neighbors) {
        if (existing_udg_ptrs.count(udg_ptr) <= 0) {
          existing_udg_ptrs[udg_ptr] = new undirected_graph_vertex(udg_ptr->label);
          assert(NULL != existing_udg_ptrs[udg_ptr]);
        }
        existing_udg_ptrs[curr_node_ptr]->neighbors.push_back(existing_udg_ptrs[udg_ptr]);
        udg_ptr_buffer.push_back(udg_ptr);
      }
    }

    return existing_udg_ptrs[udag_start_ptr];
  }

  /* 0 |2| 0#1 0#2 1 |2| 1#0 1#2 2 |3| 2#0 2#1 2#2 */
  static void print_undirected_graph(undirected_graph_vertex * graph_ptr) {
    if (NULL == graph_ptr) { return; }

    undirected_graph_vertex * curr_graph_ptr = NULL;
    deque<undirected_graph_vertex *> graph_ptr_buffer;
    unordered_map<undirected_graph_vertex *, bool> visited_flag_map;

    graph_ptr_buffer.push_back(graph_ptr);
    while(false == graph_ptr_buffer.empty()) {
      curr_graph_ptr = graph_ptr_buffer.front();
      graph_ptr_buffer.pop_front();
      if (true == visited_flag_map[curr_graph_ptr]) { continue; }
      cout << curr_graph_ptr->label << " |"
           << curr_graph_ptr->neighbors.size() << "| ";
      for (size_t i = 0; i < curr_graph_ptr->neighbors.size(); i++) {
        cout << curr_graph_ptr->label << "#"
             << curr_graph_ptr->neighbors[i]->label << " ";
        graph_ptr_buffer.push_back(curr_graph_ptr->neighbors[i]);
      }
      visited_flag_map[curr_graph_ptr] = true;
    }
    cout << endl;
  }

  static void test_clone_undirected_graph() {
    undirected_graph_vertex * a_ptr = new undirected_graph_vertex(0);
    undirected_graph_vertex * b_ptr = new undirected_graph_vertex(1);
    undirected_graph_vertex * c_ptr = new undirected_graph_vertex(2);

    a_ptr->neighbors.push_back(b_ptr); a_ptr->neighbors.push_back(c_ptr);
    b_ptr->neighbors.push_back(a_ptr); b_ptr->neighbors.push_back(c_ptr);
    c_ptr->neighbors.push_back(a_ptr); c_ptr->neighbors.push_back(b_ptr);
    c_ptr->neighbors.push_back(c_ptr);

    cout << "1. clone_undirected_graph:" << endl;
    print_undirected_graph(a_ptr);
    cout << "<=>" << endl;
    print_undirected_graph(clone_undirected_graph(a_ptr));
    print_undirected_graph(b_ptr);
    cout << "<=>" << endl;
    print_undirected_graph(clone_undirected_graph(b_ptr));
    print_undirected_graph(c_ptr);
    cout << "<=>" << endl;
  	print_undirected_graph(clone_undirected_graph(c_ptr));

    undirected_graph_vertex * d_ptr = new undirected_graph_vertex(-1);
    undirected_graph_vertex * e_ptr = new undirected_graph_vertex(1);

    d_ptr->neighbors.push_back(e_ptr); e_ptr->neighbors.push_back(d_ptr);
    print_undirected_graph(d_ptr);
    cout << "<=>" << endl;
  	print_undirected_graph(clone_undirected_graph(d_ptr));
  }

  class graph_vertex {
  public:
    graph_vertex(int vid, int vpriority, int vfrom = -1) {
      id = vid; priority = vpriority; id_from = vfrom;
    }
    virtual ~graph_vertex(){}
    // default stl heap is max heap, by inverting the < we got min heap.
    friend bool operator< (const graph_vertex & lv, const graph_vertex & rv) {
      return (lv.priority > rv.priority);
    }
    friend bool operator== (const graph_vertex & lv, const graph_vertex & rv) {
      return (lv.priority == rv.priority);
    }
    friend ostream & operator<< (ostream & os, const graph_vertex & v) {
      os << "< " << v.id << ", " << v.priority << ", " << v.id_from << " >";
      return os;
    }
    int id, priority, id_from;
  };

  /* graph_vertex(int vid, int vpriority, int vfrom = -1) */
  static vector<graph_vertex> calc_shortest_paths(const vector<vector<int>> & sp_graph_matrix, int start_vid = 0) {
    vector<graph_vertex> all_src_dist_vec;
    vector<graph_vertex> vertex_dist_min_heap = { graph_vertex(start_vid, 0, -1) };
    vector<bool> is_visited_lookup(sp_graph_matrix.size(), false);

    for (int i = 0; i < sp_graph_matrix.size(); i++) {
      all_src_dist_vec.push_back(graph_vertex(i, INT_MAX, -1));
    }
    all_src_dist_vec[0].priority = 0;

    while (!vertex_dist_min_heap.empty()) {
      graph_vertex curr_vertex = vertex_dist_min_heap.front();
      pop_heap(vertex_dist_min_heap.begin(), vertex_dist_min_heap.end());
      vertex_dist_min_heap.pop_back();
      if (is_visited_lookup[curr_vertex.id]) { continue; }
      is_visited_lookup[curr_vertex.id] = true;
      for (int vid = 0; vid < sp_graph_matrix[curr_vertex.id].size(); vid++) {
        if (vid == curr_vertex.id || is_visited_lookup[vid] ||
            0 >= sp_graph_matrix[curr_vertex.id][vid]) { continue; }
        int tentative_dist = all_src_dist_vec[curr_vertex.id].priority +
                             sp_graph_matrix[curr_vertex.id][vid];
        if (tentative_dist < all_src_dist_vec[vid].priority) {
          all_src_dist_vec[vid].priority = tentative_dist;
          all_src_dist_vec[vid].id_from = curr_vertex.id;
          vertex_dist_min_heap.push_back(all_src_dist_vec[vid]);
          push_heap(vertex_dist_min_heap.begin(), vertex_dist_min_heap.end());
        }
      }
    }

    return all_src_dist_vec;
  }

  static void test_calc_shortest_paths() {
    cout << "2. test_calc_shortest_paths:" << endl;
    vector<graph_vertex> result;
    /* V D 0 0 1 4 2 12 3 19 4 21 5 11 6 9 7 8 8 14 */
    vector<vector<vector<int>>> test_input = {
      { {0, 4, 0, 0, 0, 0, 0, 8, 0}, {4, 0, 8, 0, 0, 0, 0, 11, 0}, {0, 8, 0, 7, 0, 4, 0, 0, 2},
        {0, 0, 7, 0, 9, 14, 0, 0, 0}, {0, 0, 0, 9, 0, 10, 0, 0, 0}, {0, 0, 4, 14, 10, 0, 2, 0, 0},
        {0, 0, 0, 0, 0, 2, 0, 1, 6}, {8, 11, 0, 0, 0, 0, 1, 0, 7}, {0, 0, 2, 0, 0, 0, 6, 7, 0} } };
    vector<vector<graph_vertex>> test_output = {
      { graph_vertex(0, 0, -1),  graph_vertex(1, 4, 0), graph_vertex(2, 12, 1),
        graph_vertex(3, 19, 2), graph_vertex(4, 21, 5), graph_vertex(5, 11, 6),
        graph_vertex(6, 9, 7),  graph_vertex(7, 8, 0), graph_vertex(8, 14, 2) } };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_shortest_paths(test_input[i]);
      print_all_elem<graph_vertex>(result);
      cout << "<=>" << endl;
      print_all_elem<graph_vertex>(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) {
        assert((result[j].id == test_output[i][j].id) &&
               (result[j].priority == test_output[i][j].priority) &&
               (result[j].id_from == test_output[i][j].id_from));
      }
    }
  }

  class graph_edge {
  public:
    graph_edge(int f, int t, int w) : from(f), to(t), weight(w) {}
    virtual ~graph_edge() {}
    friend ostream & operator<< (ostream & os, const graph_edge & e) {
      os << "(" << e.from << " -> " << e.to << " : " << e.weight << ")"; return os;
    }
    friend bool operator< (const graph_edge & le, const graph_edge & re) {
      return (le.weight > re.weight);
    }
    friend bool operator== (const graph_edge & le, const graph_edge & re) {
      return (le.weight == re.weight);
    }
    int from, to, weight;
  };

  /* graph_edge(int f, int t, int w) : from(f), to(t), weight(w) {} */
  /* graph_vertex(int vid, int vpriority, int vfrom = -1) int id, priority, id_from; */
  static vector<graph_edge> calc_minimum_spanning_tree(const vector<vector<int>> & mst_graph_metrix) {
    vector<graph_edge> mst_edges;
    if (mst_graph_metrix.empty()) { return mst_edges; }

    vector<bool> visit_lookup(mst_graph_metrix.size(), false);
    vector<graph_vertex> vertex_heap = { graph_vertex(0, 0, -1) };
    make_heap(vertex_heap.begin(), vertex_heap.end());

    while (!vertex_heap.empty()) {
      graph_vertex curr_vertex = vertex_heap.front();
      pop_heap(vertex_heap.begin(), vertex_heap.end());
      vertex_heap.pop_back();
      if (true == visit_lookup[curr_vertex.id]) { continue; }
      visit_lookup[curr_vertex.id] = true;
      if (curr_vertex.id_from != -1) {
        mst_edges.push_back(graph_edge(curr_vertex.id_from, curr_vertex.id, curr_vertex.priority));
      }
      for (int i = 0; i < mst_graph_metrix[curr_vertex.id].size(); i++) {
        if (true == visit_lookup[i] || i == mst_graph_metrix[curr_vertex.id][i] ||
                                       0 >= mst_graph_metrix[curr_vertex.id][i]) { continue; }
        vertex_heap.push_back(graph_vertex(i, mst_graph_metrix[curr_vertex.id][i], curr_vertex.id));
        push_heap(vertex_heap.begin(), vertex_heap.end());
      }
    }
    return mst_edges;
  }

  static void test_calc_minimum_spanning_tree() {
    cout << "3. test_calc_minimum_spanning_tree:" << endl;
    vector<graph_edge> result;
    vector<vector<vector<int>>> test_input = {
      { {0, 2, 0, 6, 0}, {2, 0, 3, 8, 5}, {0, 3, 0, 0, 7}, {6, 8, 0, 0, 9}, {0, 5, 7, 9, 0} }
    };
    vector<vector<graph_edge>> test_output = {
      { graph_edge(0, 1, 2), graph_edge(1, 2, 3), graph_edge(1, 4, 5), graph_edge(0, 3, 6) }
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_minimum_spanning_tree(test_input[i]);
      print_all_elem<graph_edge>(result);
      cout << "<=>" << endl;
      print_all_elem<graph_edge>(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) {
        assert((result[j].from == test_output[i][j].from) &&
               (result[j].to == test_output[i][j].to) &&
               (result[j].weight == test_output[i][j].weight));
      }
    }
  }

  /**
   * Check whether the original sequence org can be uniquely reconstructed from
   * the sequences in seqs. The org sequence is a permutation of the integers
   * from 1 to n, with 1 ≤ n ≤ 10^4. Reconstruction means building a shortest
   * common supersequence of the sequences in seqs (i.e., a shortest sequence
   * so that all sequences in seqs are subsequences of it). Determine whether
   * there is only one sequence that can be reconstructed from seqs and it is
   * the org sequence.
   *
   * Example 1:
   * Input: * org: [1,2,3], seqs: [[1,2],[1,3]]
   * Output: false
   * Explanation: [1,2,3] is not the only one sequence that can be reconstructed,
   *              because [1,3,2] is a valid sequence that can be reconstructed.
   * Example 2:
   * Input: org: [1,2,3], seqs: [[1,2]]
   * Output: false
   * Explanation: The reconstructed sequence can only be [1,2].
   *
   * Example 3:
   * Input: org: [1,2,3], seqs: [[1,2],[1,3],[2,3]]
   * Output: true
   * Explanation: The sequences [1,2], [1,3], and [2,3] can uniquely reconstruct
   *              the original sequence [1,2,3].
   * 1 > 2
   * v   v
   * 3 < +
   * Example 4:
   * Input: org: [4,1,5,2,6,3], seqs: [[5,2,6,3],[4,1,5,2]]
   * Output: true
   * map a => all links need to be confirmed
   *     b => all value => curr-index to check conflicts. 5 & 2 => 2 < 3 => good
   *
   * 4 - 1
   *     |
   *     5 - 2 - 6 - 3
   * [ 10,000, 10,000 ] => 10,000-10,000
   * [      4,      3 ] =>      4-00,003
   * only 1 sequence can be constructed => seqs covers all links & no conflicts.
   */
  class dag_vertex {
  public:
    dag_vertex(int id = -1) : vid(id) {}
    virtual ~dag_vertex() {}
    int vid;
    unordered_set<int> incoming_vids; /* all vids must come before */
    unordered_set<int> outgoing_vids; /* all vids can come after */
    friend ostream & operator<<(ostream & os, const dag_vertex & dv) {
      os << dv.vid; return os;
    }
  };

  static bool is_sequence_unique(vector<int>& origin_seq, vector<vector<int>>& seqs) {
    bool is_seq_uniq = true;

    int total_dep_cnt = 0;
    unordered_map<int, dag_vertex> dag_map;
    for (auto & seq : seqs) {
      for (auto & vid : seq) { dag_map[vid] = dag_vertex(vid); }
    }
    for (auto & seq : seqs) {
      for (int i = 0; i < seq.size(); i++) {
        if (i + 1 < seq.size()) {
          dag_map[seq[i]].outgoing_vids.insert(seq[i + 1]);
          dag_map[seq[i + 1]].incoming_vids.insert(seq[i]);
        }
      }
    }
    for (auto & dag_pair : dag_map) { total_dep_cnt += dag_pair.second.outgoing_vids.size(); }

    vector<int> sorted_vids;
    vector<int> start_pos_vec;
    for (auto & dag_pair : dag_map) {
      if (dag_pair.second.incoming_vids.empty()) { start_pos_vec.push_back(dag_pair.first); }
    }
    dag_vertex curr_vertex;
    /* at any given time, we should have only 1 entry to follow, otherwise seq is not uniq */
    while(1 == start_pos_vec.size()) {
      curr_vertex = dag_map[start_pos_vec.back()];
      sorted_vids.push_back(curr_vertex.vid);
      start_pos_vec.pop_back();
      for (auto & out_vid : curr_vertex.outgoing_vids) {
        dag_map[out_vid].incoming_vids.erase(curr_vertex.vid); total_dep_cnt -= 1;
        if (dag_map[out_vid].incoming_vids.empty()) { start_pos_vec.push_back(out_vid); }
      }
    }

    if (sorted_vids.size() != origin_seq.size() || 0 != total_dep_cnt) { is_seq_uniq = false; }
    else {
      for (int i = 0; i < sorted_vids.size(); i++) {
        if (sorted_vids[i] != origin_seq[i]) { is_seq_uniq = false; break; }
      }
    }

    return is_seq_uniq;
  }

  static void test_is_sequence_unique() {
    cout << "4. test_is_sequence_unique:" << endl;
    bool result = false;
    vector<vector<int>> test_input_origin = { { 1 }, { 1, 2, 3 }, { 1, 2, 3 }, { 1, 2, 3 }, { 4, 1, 5, 2, 6, 3 } };
    vector<vector<vector<int>>> test_input_seqs = { { {}, {} }, { { 1, 2 }, { 1, 3 } },
                                                    { { 1, 2 } },
                                                    { { 1, 2 }, { 1, 3 }, { 2, 3 } },
                                                    { { 5, 2, 6, 3 }, { 4, 1, 5, 2 } } };
    vector<bool> test_output = { false, false, false, true, true };
    for (int i = 0; i < test_output.size(); i++) {
      result = is_sequence_unique(test_input_origin[i], test_input_seqs[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 207 Course Schedule
   * There are a total of n courses you have to take, labeled from 0 to n-1.
   * Some courses may have prerequisites, for example to take course 0 you have
   * to first take course 1, which is expressed as a pair: [0,1]
   * Given the total number of courses and a list of prerequisite pairs, is it
   * possible for you to finish all courses?
   * Example 1:
   * - Input: 2, [[1,0]] 
   *   Output: true
   *   Explanation: There are a total of 2 courses to take. 
   *   To take course 1 you should have finished course 0. So it is possible.
   * Example 2:
   * - Input: 2, [[1,0],[0,1]]
   *   Output: false
   *   Explanation: There are a total of 2 courses to take. 
   *   To take course 1 you should have finished course 0, and to take course 0
   *   you should also have finished course 1. So it is impossible.
   * Observation:
   * - each course may have its own requirement of coures needs to be taken 1st
   *   essentailly viewed as an vertex within a DAG with incoming edges.
   * - in the meantime, it can also be a pre-requisite for other courses, then
   *   for this same vertex, we have a bunch of outgoing edges.
   * - to be able to take all courses, we want to see if we can have topological
   *   ordering of the whole graph, such that there may be more than 1 courses
   *   does not require any courses (vertex with no incoming edges), and rest of
   *   them can be scheduled on some certain order.
   * - if we can sort whole DAG, then true, otherwise(not DAG, with cycle) false
   */
  static bool can_all_courses_be_taken(int n, vector<pair<int, int>> prerequisites) {
    bool is_dag_valid = false;
    /* 1. build out the dag before topological sorting */
    int total_dependency_cnt = 0;
    vector<dag_vertex> dag;
    for (int i = 0; i < n; i++) { dag.push_back(dag_vertex(i)); }
    int curr_vid = 0, required_vid = 0;
    for (auto & dep_pair : prerequisites) {
      curr_vid = dep_pair.first; required_vid = dep_pair.second;
      dag[curr_vid].incoming_vids.insert(required_vid);
      dag[required_vid].outgoing_vids.insert(curr_vid);
      total_dependency_cnt += 1;
    }

    /* 2. start topological sorting */
    vector<int> start_vids_vec;
    for (auto & vertex : dag) {
      if (vertex.incoming_vids.empty()) { start_vids_vec.push_back(vertex.vid); }
    }
    while (!start_vids_vec.empty()) {
      /* 2-1. start by pick v without any dependency each time */
      curr_vid = start_vids_vec.back();
      start_vids_vec.pop_back();
      /* 2-2. remove its dependencies from all its outgoing ones */
      for (auto & next_vid : dag[curr_vid].outgoing_vids) {
        dag[next_vid].incoming_vids.erase(curr_vid); total_dependency_cnt -= 1;
        if (dag[next_vid].incoming_vids.empty()) { start_vids_vec.push_back(next_vid); }
      }
    }

    is_dag_valid = (0 == total_dependency_cnt);
    return is_dag_valid;
  }

  static void test_can_all_courses_be_taken() {
    cout << "5. test_can_all_courses_be_taken:" << endl;
    vector<vector<pair<int, int>>> test_input_pairs = {
      { pair<int, int>(1, 0) },
      { pair<int, int>(1, 0), pair<int, int>(0, 1) },
      { pair<int, int>(5, 2), pair<int, int>(5, 0),
        pair<int, int>(4, 0), pair<int, int>(4, 1),
        pair<int, int>(2, 3), pair<int, int>(3, 1) } };
    vector<int> test_input_cnt = { 2, 2, 6 };
    vector<bool> test_output = { true, false, true };
    bool result = false;
    for (int i = 0; i < test_output.size(); i++) {
      result = can_all_courses_be_taken(test_input_cnt[i], test_input_pairs[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 210. Course Schedule II
   */
  static vector<int> plan_courses_to_take(int n, vector<pair<int, int>> prerequisites) {
    vector<int> sorted_dag_vds;

    vector<dag_vertex> dag;
    int total_dep_cnt = 0;
    for (int i = 0; i < n; i++) { dag.push_back(dag_vertex(i)); }
    for (auto & dep_pair : prerequisites) {
      dag[dep_pair.first].incoming_vids.insert(dep_pair.second);
      dag[dep_pair.second].outgoing_vids.insert(dep_pair.first);
      total_dep_cnt += 1;
    }

    vector<int> start_vids_vec;
    for (auto & vertex : dag) {
      if (vertex.incoming_vids.empty()) { start_vids_vec.push_back(vertex.vid); }
    }
    while (!start_vids_vec.empty()) {
      sorted_dag_vds.push_back(start_vids_vec.back());
      start_vids_vec.pop_back();
      auto & curr_vertex = dag[sorted_dag_vds.back()];
      for (auto & next_vid : curr_vertex.outgoing_vids) {
        dag[next_vid].incoming_vids.erase(curr_vertex.vid); total_dep_cnt -= 1;
        if (dag[next_vid].incoming_vids.empty()) { start_vids_vec.push_back(next_vid); }
      }
    }

    if (0 != total_dep_cnt) { sorted_dag_vds = {}; }

    return sorted_dag_vds;
  }

  static void test_plan_courses_to_take() {
    cout << "6. test_plan_courses_to_take:" << endl;
    vector<int> result;
    vector<vector<pair<int, int>>> test_input_pairs = {
      { pair<int, int>(1, 0) },
      { pair<int, int>(1, 0), pair<int, int>(0, 1) },
      { pair<int, int>(5, 2), pair<int, int>(5, 0),
        pair<int, int>(4, 0), pair<int, int>(4, 1),
        pair<int, int>(2, 3), pair<int, int>(3, 1) },
      { pair<int, int>(1, 0), pair<int, int>(2, 0),
        pair<int, int>(3, 1), pair<int, int>(3, 2) }
    };
    vector<int> test_input_cnt = { 2, 2, 6, 4 };
    vector<vector<int>> test_output = {
      { 0, 1 }, {}, { 1, 3, 2, 0, 5, 4 }, { 0, 1, 2, 3 }
    };
    for (int i = 0; i < test_output.size(); i++) {
      result = plan_courses_to_take(test_input_cnt[i], test_input_pairs[i]);
      print_all_elem<int>(result);
      cout << "<=>" << endl;
      print_all_elem<int>(test_output[i]);
      assert(result.size() == test_output[i].size());
      for (int j = 0; j < result.size(); j++) { assert(result[j] == test_output[i][j]); }
    }
  }

  /**
   * 785. Is Graph Bipartite?
   *
   * Given an undirected graph, return true if and only if it is bipartite.
   * Recall that a graph is bipartite if we can split it's set of nodes into two
   * independent subsets A and B such that every edge in the graph has one node
   * in A and another node in B.
   *
   * The graph is given in the following form: graph[i] is a list of indexes j
   * for which the edge between nodes i and j exists. Each node is an integer
   * between 0 and graph.length - 1.  There are no self edges or parallel edges:
   * graph[i] does not contain i, and it doesn't contain any element twice.
   *
   * Example 1:
   * - Input: [[1,3], [0,2], [1,3], [0,2]]
   *   Output: true
   * - Explanation: 
   *   The graph looks like this:
   *   0----1
   *   |    |
   *   |    |
   *   3----2
   *   We can divide the vertices into two groups: {0, 2} and {1, 3}.
   * Example 2:
   * - Input: [[1,2,3], [0,2], [0,1,3], [0,2]]
   *   Output: false
   * - Explanation: 
   *   The graph looks like this:
   *   0----1
   *   | \  |
   *   |  \ |
   *   3----2
   * - We cannot find a way to divide nodes into two independent subsets.
   * Note:
   * - graph will have length in range [1, 100].
   * - graph[i] will contain integers in range [0, graph.length - 1].
   * - graph[i] will not contain i or duplicate values.
   * - The graph is udg: if element j is in graph[i], then i in graph[j].
   * Observation:
   * - bipartite basically means we can color the graph in 2 such that any
   *   two nodes connected has different color
   * - then bfs/dfs & check color of existing connected nodes.
   */
  static bool is_forest_bipartite(vector<vector<int>> graph) {
    enum { COLOR_WHITE = -1, COLOR_NULL = 0, COLOR_BLACK = 1 };
    bool is_bipartite = true;
    /* when traverse the graph, store the vid & its color */
    vector<pair<int,int>> colored_vertex_buffer;
    /* could be a forest, so use a set to store all vid & visit buffer */
    unordered_set<int> vid_set;
    for (int i = 0; i < graph.size(); i++) { vid_set.insert(i); }
    /* used to store all coloring decision to check conflicts */
    vector<int> vid_to_color_lookup(graph.size(), COLOR_NULL);

    unordered_set<int>::iterator curr_start_itr;
    pair<int, int> curr_vid_color_pair;
    while (!vid_set.empty()) {
      curr_start_itr = vid_set.begin();
      colored_vertex_buffer.push_back(pair<int, int>(* curr_start_itr, COLOR_WHITE));

      while(!colored_vertex_buffer.empty()) {
        curr_vid_color_pair = colored_vertex_buffer.back();
        colored_vertex_buffer.pop_back();
        vid_set.erase(curr_vid_color_pair.first);
        /* for each node we going to color, it should use a diff color vs its neighbor */
        for (int i = 0; i < graph[curr_vid_color_pair.first].size(); i++) {
          if (COLOR_NULL == vid_to_color_lookup[graph[curr_vid_color_pair.first][i]]) {
            /* if a node not been colored before, then color it & flag it */
            colored_vertex_buffer.push_back(
              pair<int, int>(graph[curr_vid_color_pair.first][i], curr_vid_color_pair.second * -1)
            );
            vid_to_color_lookup[graph[curr_vid_color_pair.first][i]] = curr_vid_color_pair.second * -1;
          } else {
            /* if a node already been colored, then check conflicts */
            if (curr_vid_color_pair.second == vid_to_color_lookup[graph[curr_vid_color_pair.first][i]]) {
              is_bipartite = false; break;
            }
          }
        }

        if (!is_bipartite) { break; }
      }
      if (!is_bipartite) { break; }
    }

    return is_bipartite;
  }

  static void test_is_forest_bipartite() {
    cout << "7. test_is_forest_bipartite:" << endl;
    bool result = false;
    vector<bool> test_output = { true, false, false, true, true };
    vector<vector<vector<int>>> test_input = {
      { { 1, 3 }, { 0, 2 }, { 1, 3 }, { 0, 2 } },
      { { 1, 2, 3 }, { 0, 2 }, { 0, 1, 3 }, { 0, 2 } },
      { {}, { 2, 4, 6 }, { 1, 4, 8, 9 }, { 7, 8 },
        { 1, 2, 8, 9 }, { 6, 9 }, { 1, 5, 7, 8, 9 }, { 3, 6, 9 },
        { 2, 3, 4, 6, 9 }, { 2, 4, 5, 6, 7, 8 } },
      { { 4 }, {}, { 4 }, { 4 }, { 0, 2, 3 } },
      { {} }
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = is_forest_bipartite(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  static bool is_udg_bipartite(vector<vector<int>> graph) {
    enum { COLOR_NONE = 0, COLOR_BLACK = -1, COLOR_WHITE = 1 };
    bool is_udg_bipartite = true;
    if (true == graph.empty()) { return is_udg_bipartite; }

    /* used as stack for graph traversing, pair -> <vid, parent_vid> */
    vector<pair<int, int>> vertex_id_buffer;
    /* used as color lookup for visited nodes, default to color-none */
    vector<int> vertex_color_lookup(graph.size(), COLOR_NONE);

    int curr_vertex_id = -1, curr_vertex_pid = -1, vertex_id_to_chk = -1;
    vertex_id_buffer.push_back(pair<int, int>(0, 0));

    while(false == vertex_id_buffer.empty()) {
      curr_vertex_id = vertex_id_buffer.back().first;
      curr_vertex_pid = vertex_id_buffer.back().second;
      vertex_id_buffer.pop_back();
      if (COLOR_NONE != vertex_color_lookup[curr_vertex_id]) { continue; }

      if (COLOR_NONE == vertex_color_lookup[curr_vertex_pid]) {
        vertex_color_lookup[curr_vertex_id] = COLOR_BLACK;
      } else {
        vertex_color_lookup[curr_vertex_id] = vertex_color_lookup[curr_vertex_pid] * -1;
      }

      for (int i = 0; i < graph[curr_vertex_id].size(); i++) {
        vertex_id_to_chk = graph[curr_vertex_id][i];
        if (vertex_color_lookup[vertex_id_to_chk] == vertex_color_lookup[curr_vertex_id]) {
          is_udg_bipartite = false; break;
        }
        vertex_id_buffer.push_back(pair<int, int>(vertex_id_to_chk, curr_vertex_id));
      }
    }
    return is_udg_bipartite;
  }

  /**
   * Ford-Fulkerson Algorithm
   * The following is simple idea of Ford-Fulkerson algorithm:
   * 1) Start with initial flow as 0.
   * 2) While there is a augmenting path from source to sink.
   *      Add this path-flow to flow.
   * 3) Return flow.
   * Time Complexity:
   * - Time complexity of the above algorithm is O(max_flow * E). We run a loop
   *   while there is an augmenting path. In worst case, we may add 1 unit flow
   *   in every iteration. Therefore time complexity becomes O(max_flow * E).
   */
  static bool best_effort_dfs_path(vector<vector<int>> & residual_graph,
                                   int src_vid, int sink_vid,
                                   vector<int> & dfs_path_vec) {
    dfs_path_vec[sink_vid] = -1;

    int curr_vid = 0;
    unordered_set<int> vid_lookup;
    vector<int> curr_dfs_buffer = { src_vid };
    while (!curr_dfs_buffer.empty()) {
      curr_vid = curr_dfs_buffer.back();
      curr_dfs_buffer.pop_back();
      vid_lookup.insert(curr_vid);
      /* break after found the path */
      if (curr_vid == sink_vid) { break; }
      for (int next_vid = 0; next_vid < residual_graph[curr_vid].size(); next_vid++) {
        /* usually for traversal, no need to check unless when tracking path */
        if (residual_graph[curr_vid][next_vid] > 0 &&
            vid_lookup.end() == vid_lookup.find(next_vid)) {
          curr_dfs_buffer.push_back(next_vid); dfs_path_vec[next_vid] = curr_vid;
        }
      }
    }
    return (dfs_path_vec[sink_vid] != -1);
  }

  static int calc_max_flow(vector<vector<int>> & input_graph, int src_vid, int sink_vid) {
    int max_flow_to_push = 0;
    /* residual graph for flows in reverse */
    vector<vector<int>> residual_graph(input_graph.begin(), input_graph.end());
    /* stores the parent vid of each path vertex */
    vector<int> dfs_path_vec(input_graph.size(), -1);
    int curr_flow_to_push = 0, curr_vid = 0;
    while (best_effort_dfs_path(residual_graph, src_vid, sink_vid, dfs_path_vec)) {
      curr_flow_to_push = INT_MAX;
      /* calc. the max flow we could push in this round */
      for (curr_vid = sink_vid; -1 != dfs_path_vec[curr_vid];
           curr_vid = dfs_path_vec[curr_vid]) {
        curr_flow_to_push = min(
          curr_flow_to_push, residual_graph[dfs_path_vec[curr_vid]][curr_vid]
        );
      }
      /* based on the max flow of this single path, update the graph & residual */
      for (curr_vid = sink_vid; -1 != dfs_path_vec[curr_vid];
           curr_vid = dfs_path_vec[curr_vid]) {
        residual_graph[dfs_path_vec[curr_vid]][curr_vid] -= curr_flow_to_push;
        residual_graph[curr_vid][dfs_path_vec[curr_vid]] += curr_flow_to_push;
      }
      if (INT_MAX == curr_flow_to_push) { break; }
      /* reset the parent of target vertex for next round of flow push */
      max_flow_to_push += curr_flow_to_push;
    }
    return max_flow_to_push;
  }

  static void test_calc_max_flow() {
    cout << "8. test_calc_max_flow:" << endl;
    int result = 0;
    vector<int> test_output = { 23 };
    vector<int> test_src_input = { 0 };
    vector<int> test_sink_input = { 5 };
    vector<vector<vector<int>>> test_dag_input = {
      {{0, 16, 13, 0, 0, 0}, {0, 0, 10, 12, 0, 0}, {0, 4, 0, 0, 14, 0},
       {0, 0, 9, 0, 0, 20}, {0, 0, 0, 7, 0, 4}, {0, 0, 0, 0, 0, 0}}
    };
    for (int i = 0; i < test_dag_input.size(); i++) {
      result = calc_max_flow(test_dag_input[i], test_src_input[i], test_sink_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }
};

int main(void) {

  using graph_util::test_clone_undirected_graph;
  using graph_util::test_calc_shortest_paths;
  using graph_util::test_calc_minimum_spanning_tree;
  using graph_util::test_is_sequence_unique;
  using graph_util::test_can_all_courses_be_taken;
  using graph_util::test_plan_courses_to_take;
  using graph_util::test_is_forest_bipartite;
  using graph_util::test_calc_max_flow;

  test_clone_undirected_graph();
  test_calc_shortest_paths();
  test_calc_minimum_spanning_tree();
  test_is_sequence_unique();
  test_can_all_courses_be_taken();
  test_plan_courses_to_take();
  test_is_forest_bipartite();
  test_calc_max_flow();

  return 0;
}
