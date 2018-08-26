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

  const static int GRAPH_VERTEX_CNT = 9;

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

  class undirected_graph_vertex {
  public:
    int label;
    vector<undirected_graph_vertex *> neighbors;
    undirected_graph_vertex(int value) : label(value) {}
    virtual ~undirected_graph_vertex() {}
  };

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

  /**
   * OJ's Undirected graph serialization:
   * Nodes are labeled uniquely.
   * 
   * We use # as a separator for each node, and, as a separator for node label and
   * each neighbor of the node. As an example, consider the serialized graph
   * { 0, 1, 2#1, 2#2, 2 }.
   * 
   * The graph has a total of three nodes, and therefore contains three parts as
   * separated by #.
   * 
   * First node is labeled as 0. Connect node 0 to both nodes 1 and 2.
   * Second node is labeled as 1. Connect node 1 to node 2.
   * Third node is labeled as 2. Connect node 2 to 2 (itself), forming a self-cycle.
   * 
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

  static undirected_graph_vertex * clone_undirected_graph(undirected_graph_vertex * udag_start_ptr) {
    undirected_graph_vertex * udag_copy_ptr = NULL,
                            * curr_udag_ptr = NULL,
                            * temp_udag_ptr = NULL;
    if (NULL == udag_start_ptr) { return udag_copy_ptr; }

    vector<undirected_graph_vertex *> udag_ptr_buffer;
    unordered_set<undirected_graph_vertex *> udag_visited_lookup;
    unordered_map<int, undirected_graph_vertex *> udag_vid_to_ptr_map;

    udag_ptr_buffer.push_back(udag_start_ptr);
    while (false == udag_ptr_buffer.empty()) {
      curr_udag_ptr = udag_ptr_buffer.back();
      udag_ptr_buffer.pop_back();
      if (udag_visited_lookup.end() != udag_visited_lookup.find(curr_udag_ptr)) { continue; }
      temp_udag_ptr = new undirected_graph_vertex(curr_udag_ptr->label);
      if (NULL == udag_copy_ptr) { udag_copy_ptr = temp_udag_ptr; }
      udag_vid_to_ptr_map[temp_udag_ptr->label] = temp_udag_ptr;
      udag_visited_lookup.insert(curr_udag_ptr);
      for (undirected_graph_vertex * vptr : curr_udag_ptr->neighbors) {
        udag_ptr_buffer.push_back(vptr);
        if (udag_visited_lookup.end() != udag_visited_lookup.find(vptr)) {
          temp_udag_ptr->neighbors.push_back(udag_vid_to_ptr_map[vptr->label]);
          if (temp_udag_ptr->label != vptr->label) {
            udag_vid_to_ptr_map[vptr->label]->neighbors.push_back(temp_udag_ptr);
          }
        }
      }
    }
    return udag_copy_ptr;
  }

  /* graph_vertex(int vid, int vpriority, int vfrom = -1) */
  static vector<graph_vertex> calc_shortest_paths(
    const vector<vector<int>> & sp_graph_matrix, int start_vid
  ) {
    /* store all shortest path info, store parent of each v. */
    vector<graph_vertex> shortest_path_vec;
    for (int i = 0; i < sp_graph_matrix.size(); i++) {
      shortest_path_vec.push_back(graph_vertex(i, INT_MAX));
    }
    shortest_path_vec[start_vid].priority = 0;

    /* dist vector store all dist from start_v to curr_node */
    vector<graph_vertex> vertex_min_heap;
    unordered_set<int> visited_lookup;

    vertex_min_heap.push_back(shortest_path_vec[start_vid]);
    while (false == vertex_min_heap.empty()) {
      /* each time adding the closest node to the buffer */
      graph_vertex curr_vertex = vertex_min_heap.front();
      pop_heap(vertex_min_heap.begin(), vertex_min_heap.end());
      vertex_min_heap.pop_back();
      if (visited_lookup.end() != visited_lookup.find(curr_vertex.id)) { continue; }
      visited_lookup.insert(curr_vertex.id);
      for (int i = 0; i < sp_graph_matrix[curr_vertex.id].size(); i++) {
        if ((curr_vertex.id == i) || (0 == sp_graph_matrix[curr_vertex.id][i])) { continue; }
        /* update distance vector, push to heap if we see a shorter dist */
        int tentative_dist_from_src = curr_vertex.priority + sp_graph_matrix[curr_vertex.id][i];
        if (tentative_dist_from_src < shortest_path_vec[i].priority) {
          shortest_path_vec[i].priority = tentative_dist_from_src;
          shortest_path_vec[i].id_from = curr_vertex.id;
          vertex_min_heap.push_back(shortest_path_vec[i]);
          push_heap(vertex_min_heap.begin(), vertex_min_heap.end());
        }
      }
    }
    return shortest_path_vec;
  }

  /* graph_edge(int f, int t, int w) : from(f), to(t), weight(w) {} */
  /* graph_vertex(int vid, int vpriority, int vfrom = -1) */
  static vector<graph_edge> calc_minimum_spanning_tree(const vector<vector<int>> & mst_graph_metrix) {
    vector<graph_edge> mst_edges;

    vector<graph_vertex> mst_vertices_min_heap;
    unordered_set<int> visited_lookup;
    mst_vertices_min_heap.push_back(graph_vertex(0, 0, -1));
    while (false == mst_vertices_min_heap.empty()) {
      graph_vertex curr_vertex = mst_vertices_min_heap.front();
      pop_heap(mst_vertices_min_heap.begin(), mst_vertices_min_heap.end());
      mst_vertices_min_heap.pop_back();
      if (visited_lookup.end() != visited_lookup.find(curr_vertex.id)) { continue; }
      visited_lookup.insert(curr_vertex.id);
      if (-1 != curr_vertex.id_from) {
        mst_edges.push_back(graph_edge(curr_vertex.id_from, curr_vertex.id, curr_vertex.priority));
      }
      for (int i = 0; i < mst_graph_metrix[curr_vertex.id].size(); i++) {
        if (0 == mst_graph_metrix[curr_vertex.id][i] || curr_vertex.id == i) { continue; }
        mst_vertices_min_heap.push_back(
          graph_vertex(i, mst_graph_metrix[curr_vertex.id][i], curr_vertex.id)
        );
        push_heap(mst_vertices_min_heap.begin(), mst_vertices_min_heap.end());
      }
    }

    return mst_edges;
  }

  /*
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
  static bool is_sequence_unique(vector<int>& origin_seq, vector<vector<int>>& seqs) {
    unordered_map<int, int> links_to_confirm_map, value_to_index_map;
    for (int i = 0; i < origin_seq.size(); i++) {
      if (i < origin_seq.size() - 1) {
        links_to_confirm_map[origin_seq[i]] = origin_seq[i + 1];
      }
      value_to_index_map[origin_seq[i]] = i;
    }

    for (auto & seq_to_chk : seqs) {
      for (int i = 0; i < seq_to_chk.size() - 1; i++) {
        if (value_to_index_map[seq_to_chk[i]] >
            value_to_index_map[seq_to_chk[i + 1]]) { return false; }
        if (links_to_confirm_map.find(seq_to_chk[i]) != links_to_confirm_map.end() &&
            links_to_confirm_map[seq_to_chk[i]] == seq_to_chk[i + 1]) {
          links_to_confirm_map.erase(seq_to_chk[i]);
        }
      }
    }

    return links_to_confirm_map.empty();
  }

  static bool _is_sequence_unique(vector<int>& origin_seq, vector<vector<int>>& seqs) {
    unordered_map<int, int> links_to_confirm_map, value_to_index_map;

    for (int i = 0; i < origin_seq.size(); i++) {
      if (i < origin_seq.size() - 1) {
        links_to_confirm_map[origin_seq[i]] = origin_seq[i + 1];
      }
      value_to_index_map[origin_seq[i]] = i;
    }

    for (auto & seq_to_chk : seqs) {
      for (int i = 0; i < seq_to_chk.size(); i++) {
        if (i == seq_to_chk.size() - 1) {
          if (value_to_index_map.find(seq_to_chk[i]) ==
              value_to_index_map.end()) { return false; }
          continue;
        }
        if (value_to_index_map[seq_to_chk[i]] >=
            value_to_index_map[seq_to_chk[i + 1]]) { return false; }
        if (links_to_confirm_map.find(seq_to_chk[i]) !=
            links_to_confirm_map.end()) {
          if (links_to_confirm_map[seq_to_chk[i]] == seq_to_chk[i + 1]) {
            links_to_confirm_map.erase(seq_to_chk[i]);
          }
        }
      }
    }
    return links_to_confirm_map.empty();
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
  class dag_vertex {
  public:
    dag_vertex(int id = 0) : vid(id) {}
    virtual ~dag_vertex() {}

    int vid;
    unordered_set<int> incoming_vids;
    unordered_set<int> outgoing_vids;
  };

  static bool can_all_courses_be_taken(int n, vector<pair<int, int>> prerequisites) {
    /* build the graph based on input */
    int total_edges_cnt = 0;
    vector<dag_vertex> graph_vertices(n, dag_vertex());
    for (int i = 0; i < n; i++) { graph_vertices[i].vid = i; }
    for (auto & pair_ref : prerequisites) {
      if (pair_ref.first == pair_ref.second) { return false; }
      graph_vertices[pair_ref.second].outgoing_vids.insert(pair_ref.first);
      graph_vertices[pair_ref.first].incoming_vids.insert(pair_ref.second);
      total_edges_cnt += 1;
    }

    /* start topological sort */
    vector<int> vertex_id_buffer;  // used here as a stack
    vector<bool> visited_vid_lookup(n, false);

    for (auto & vertex_ref : graph_vertices) {
      if (true == vertex_ref.incoming_vids.empty()) {
        vertex_id_buffer.push_back(vertex_ref.vid);
      }
    }
    int curr_vid = -1;
    while (false == vertex_id_buffer.empty()) {
      curr_vid = vertex_id_buffer.back();
      dag_vertex & curr_vertex = graph_vertices[curr_vid];
      vertex_id_buffer.pop_back();

      if (true == visited_vid_lookup[curr_vid]) { continue; }

      visited_vid_lookup[curr_vid] = true;

      for (auto & outgoing_vid : curr_vertex.outgoing_vids) {
        graph_vertices[outgoing_vid].incoming_vids.erase(curr_vid);
        if (true == graph_vertices[outgoing_vid].incoming_vids.empty()) {
          vertex_id_buffer.push_back(outgoing_vid);
        }
        total_edges_cnt -= 1;
      }
    }

    return (total_edges_cnt == 0);
  }

  static vector<int> plan_courses_to_take(int n, vector<pair<int, int>> prerequisites) {
    /* build the graph based on input */
    vector<int> course_planing;
    vector<dag_vertex> graph_vertices(n, dag_vertex());
    for (int i = 0; i < n; i++) { graph_vertices[i].vid = i; }
    for (auto & pair_ref : prerequisites) {
      if (pair_ref.first == pair_ref.second) { return course_planing; }
      graph_vertices[pair_ref.second].outgoing_vids.insert(pair_ref.first);
      graph_vertices[pair_ref.first].incoming_vids.insert(pair_ref.second);
    }

    /* start topological sort */
    vector<int> vertex_id_buffer;  // used here as a stack
    vector<bool> visited_vid_lookup(n, false);
    for (auto & vertex_ref : graph_vertices) {
      if (true == vertex_ref.incoming_vids.empty()) {
        vertex_id_buffer.push_back(vertex_ref.vid);
      }
    }
    int curr_vid = -1;
    while (false == vertex_id_buffer.empty()) {
      curr_vid = vertex_id_buffer.back();
      dag_vertex & curr_vertex = graph_vertices[curr_vid];
      vertex_id_buffer.pop_back();
      if (true == visited_vid_lookup[curr_vid]) { continue; }
      course_planing.push_back(curr_vid);
      visited_vid_lookup[curr_vid] = true;
      for (auto & outgoing_vid : curr_vertex.outgoing_vids) {
        graph_vertices[outgoing_vid].incoming_vids.erase(curr_vid);
        if (true == graph_vertices[outgoing_vid].incoming_vids.empty()) {
          vertex_id_buffer.push_back(outgoing_vid);
        }
      }
    }
    if (course_planing.size() != n){ course_planing.clear(); }
    return course_planing;
  }


};

int main(void) {
  /*
     0, 1, 2
     0 | [ 1, 2 ] | <0> { 0#1, 0#2 }
     1 | [ 2, 2 ] | <0, 1> { 1#0, 1#2 }
     2 | [ 2, 2, 2 ] <0, 1, 2> { 2#0, 2#1, 2#2 }
         1
        / \
       /   \
      0 --- 2
           / \
           \_/
  */
  using graph_util::print_all_elem;
  using graph_util::print_all_elem_vec;

  using graph_util::undirected_graph_vertex;
  using graph_util::graph_vertex;
  using graph_util::graph_edge;

  using graph_util::clone_undirected_graph;
  using graph_util::calc_shortest_paths;
  using graph_util::calc_minimum_spanning_tree;
  using graph_util::is_sequence_unique;
  using graph_util::can_all_courses_be_taken;
  using graph_util::plan_courses_to_take;

  undirected_graph_vertex * a_ptr = new undirected_graph_vertex(0);
  undirected_graph_vertex * b_ptr = new undirected_graph_vertex(1);
  undirected_graph_vertex * c_ptr = new undirected_graph_vertex(2);

  a_ptr->neighbors.push_back(b_ptr); a_ptr->neighbors.push_back(c_ptr);
  b_ptr->neighbors.push_back(a_ptr); b_ptr->neighbors.push_back(c_ptr);
  c_ptr->neighbors.push_back(a_ptr); c_ptr->neighbors.push_back(b_ptr);
  c_ptr->neighbors.push_back(c_ptr);

  cout << "1. clone_undirected_graph:" << endl;
  print_undirected_graph(a_ptr);
  print_undirected_graph(b_ptr);
  print_undirected_graph(c_ptr);
  cout << "<<==>>" << endl;
  print_undirected_graph(clone_undirected_graph(a_ptr));
  print_undirected_graph(clone_undirected_graph(b_ptr));
	print_undirected_graph(clone_undirected_graph(c_ptr));

  cout << "2. calc_shortest_paths:" << endl;
  /* V D 0 0 1 4 2 12 3 19 4 21 5 11 6 9 7 8 8 14 */
  vector<vector<int>> sp_graph_metrix({
    vector<int>({0, 4, 0, 0, 0, 0, 0, 8, 0}),
    vector<int>({4, 0, 8, 0, 0, 0, 0, 11, 0}),
    vector<int>({0, 8, 0, 7, 0, 4, 0, 0, 2}),
    vector<int>({0, 0, 7, 0, 9, 14, 0, 0, 0}),
    vector<int>({0, 0, 0, 9, 0, 10, 0, 0, 0}),
    vector<int>({0, 0, 4, 14, 10, 0, 2, 0, 0}),
    vector<int>({0, 0, 0, 0, 0, 2, 0, 1, 6}),
    vector<int>({8, 11, 0, 0, 0, 0, 1, 0, 7}),
    vector<int>({0, 0, 2, 0, 0, 0, 6, 7, 0}),
  });
  print_all_elem<graph_vertex>(vector<graph_vertex>({
    graph_vertex(0, 0),  graph_vertex(1, 4), graph_vertex(2, 12),
    graph_vertex(3, 19), graph_vertex(4, 21), graph_vertex(5, 11),
    graph_vertex(6, 9),  graph_vertex(7, 8), graph_vertex(8, 14)
  }));
  print_all_elem<graph_vertex>(calc_shortest_paths(sp_graph_metrix, 0));

  cout << "3. calc_minimum_spanning_tree:" << endl;
  vector<vector<int>> mst_graph_metrix({
    vector<int>({0, 2, 0, 6, 0}), vector<int>({2, 0, 3, 8, 5}),
    vector<int>({0, 3, 0, 0, 7}), vector<int>({6, 8, 0, 0, 9}),
    vector<int>({0, 5, 7, 9, 0}),
  });
  print_all_elem<graph_edge>(vector<graph_edge>({
    graph_edge(0, 1, 2), graph_edge(1, 2, 3),
    graph_edge(0, 3, 6), graph_edge(1, 4, 5),
  }));
  print_all_elem<graph_edge>(
    calc_minimum_spanning_tree(mst_graph_metrix)
  );

  cout << "4. is_sequence_unique:" << endl;
  vector<int> av({ 1, 2, 3 }); vector<int> av1({ 1, 2 });
  vector<int> av2({ 1, 3 });   vector<int> av3({ 2, 3 });
  vector<vector<int>> avv1; avv1.push_back(av1);
                            avv1.push_back(av2);
  vector<vector<int>> avv2; avv2.push_back(av1);
  vector<vector<int>> avv3; avv3.push_back(av1);
                            avv3.push_back(av2);
                            avv3.push_back(av3);
  vector<int> bv({ 4, 1, 5, 2, 6, 3 });
  vector<int> bv1({ 5, 2, 6, 3 });
  vector<int> bv2({ 4, 1, 5, 2 });
  vector<vector<int>> bvv; bvv.push_back(bv1);
                           bvv.push_back(bv2);

  print_all_elem<int>(av);
  print_all_elem_vec<int>(avv1);
  cout << false << endl;
  assert(false == is_sequence_unique(av, avv1));
  print_all_elem<int>(av);
  print_all_elem_vec<int>(avv2);
  cout << false << endl;
  assert(false == is_sequence_unique(av, avv2));
  print_all_elem<int>(av);
  print_all_elem_vec<int>(avv3);
  cout << true << endl;
  assert(true == is_sequence_unique(av, avv3));
  print_all_elem<int>(bv);
  print_all_elem_vec<int>(bvv);
  cout << true << endl;
  assert(true == is_sequence_unique(bv, bvv));

  cout << "5. can_all_courses_be_taken:" << endl;
  cout << "1 <=> " << can_all_courses_be_taken(2, vector<pair<int, int>>({ pair<int, int>(1, 0) })) << endl;
  cout << "0 <=> " << can_all_courses_be_taken(2, vector<pair<int, int>>({ pair<int, int>(1, 0), pair<int, int>(0, 1) })) << endl;
  cout << "1 <=> " << can_all_courses_be_taken(6, vector<pair<int, int>>({ pair<int, int>(5, 2), pair<int, int>(5, 0),
                                                                           pair<int, int>(4, 0), pair<int, int>(4, 1),
                                                                           pair<int, int>(2, 3), pair<int, int>(3, 1) })) << endl;

  cout << "6. plan_courses_to_take:" << endl;
  cout << "[ 0 1 ] <=> "; print_all_elem<int>(plan_courses_to_take(2, vector<pair<int, int>>({ pair<int, int>(1, 0) })));
  cout << "[ ] <=> "; print_all_elem<int>(plan_courses_to_take(2, vector<pair<int, int>>({ pair<int, int>(1, 0), pair<int, int>(0, 1) })));
  cout << "[ 1 3 2 0 5 4 ] <=> "; print_all_elem<int>(plan_courses_to_take(6, vector<pair<int, int>>({ pair<int, int>(5, 2), pair<int, int>(5, 0),
                                                                                                       pair<int, int>(4, 0), pair<int, int>(4, 1),
                                                                                                       pair<int, int>(2, 3), pair<int, int>(3, 1) })));
  cout << "[ 0 1 2 3 ] <=> "; print_all_elem<int>(plan_courses_to_take(4, vector<pair<int, int>>({ pair<int, int>(1, 0), pair<int, int>(2, 0),
                                                                                                   pair<int, int>(3, 1), pair<int, int>(3, 2) })));
  return 0;
}
