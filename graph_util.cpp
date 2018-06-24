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
      return (lv.priority * -1 < rv.priority * -1);
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
      return (le.weight * -1 < re.weight);
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

  static undirected_graph_vertex * clone_undirected_graph(
    undirected_graph_vertex * udag_start_ptr
  ) {
    undirected_graph_vertex * cpy_graph_ptr = NULL,
                           * tmp_graph_ptr = NULL,
                           * cur_graph_ptr = udag_start_ptr;

    if (NULL == udag_start_ptr) { return cpy_graph_ptr; }

    deque<undirected_graph_vertex *> graph_path_buffer;
    unordered_map<int, undirected_graph_vertex *> visited_node_ptr_map;

    graph_path_buffer.push_back(cur_graph_ptr);
    while (false == graph_path_buffer.empty()) {
      cur_graph_ptr = graph_path_buffer.front();
      graph_path_buffer.pop_front();
      if (visited_node_ptr_map.find(cur_graph_ptr->label) !=
          visited_node_ptr_map.end()) { continue; }

      tmp_graph_ptr = new undirected_graph_vertex(cur_graph_ptr->label);
      if (cur_graph_ptr == udag_start_ptr) { cpy_graph_ptr = tmp_graph_ptr; }

      visited_node_ptr_map[tmp_graph_ptr->label] = tmp_graph_ptr;

      for (undirected_graph_vertex * neighbor_itr : cur_graph_ptr->neighbors) {
        graph_path_buffer.push_back(neighbor_itr);
        /* one note is that for a udag, child also has a link back
         * going back to its parent, such that we can create the node
         * first, then create the link when actually reaching the child */
        if (visited_node_ptr_map.find(neighbor_itr->label) !=
            visited_node_ptr_map.end()) {
          tmp_graph_ptr->neighbors.push_back(
            visited_node_ptr_map[neighbor_itr->label]
          );
          if (neighbor_itr->label != tmp_graph_ptr->label) {
            visited_node_ptr_map[neighbor_itr->label]->neighbors
              .push_back(tmp_graph_ptr);
          }
        }
      }
    }

    return cpy_graph_ptr;
  }

  static vector<graph_vertex> calc_shortest_paths(
    const vector<vector<int>> & sp_graph_metrix, int start_vid
  ) {
    int vertex_cnt = sp_graph_metrix.size();
    vector<graph_vertex> shortest_path_distance_vec, vertex_heap;
    vector<bool> is_vertex_visited_lookup(vertex_cnt, false);

    for (int i = 0; i < vertex_cnt; i++) {
      shortest_path_distance_vec.push_back(graph_vertex(i, INT_MAX));
      if (i == start_vid) { shortest_path_distance_vec.back().priority = 0; }
    }

    vertex_heap.push_back(graph_vertex(start_vid, 0));

    while (false == vertex_heap.empty()) {
      graph_vertex pending_vertex = vertex_heap.front();
      pop_heap(vertex_heap.begin(), vertex_heap.end());
      vertex_heap.pop_back();

      if (true == is_vertex_visited_lookup[pending_vertex.id]) { continue; }
      for (int i = 0; i < vertex_cnt; i++) {
        if (0 == sp_graph_metrix[pending_vertex.id][i]) { continue; }
        int tentative_distance = (
          shortest_path_distance_vec[pending_vertex.id].priority +
          sp_graph_metrix[pending_vertex.id][i]
        );
        if (tentative_distance < shortest_path_distance_vec[i].priority) {
          shortest_path_distance_vec[i].priority = tentative_distance;
          shortest_path_distance_vec[i].id_from  = pending_vertex.id;
          vertex_heap.push_back(graph_vertex(i, tentative_distance));
          push_heap(vertex_heap.begin(), vertex_heap.end());
        }
      }
      is_vertex_visited_lookup[pending_vertex.id] = true;
    }

    return shortest_path_distance_vec;
  }

  static vector<graph_edge> calc_minimum_spanning_tree(
    const vector<vector<int>> & mst_graph_metrix
  ) {
    int total_vertex_cnt = mst_graph_metrix.size();

    vector<graph_edge> mst_edeges;
    vector<graph_vertex> vertex_heap;
    vector<bool> is_vertex_visited_lookup(total_vertex_cnt, false);

    /* by default, we start from picking edges outgoing from vertex 0 */
    vertex_heap.push_back(graph_vertex(0, 0));
    while (false == vertex_heap.empty()) {
      graph_vertex pending_vertex = vertex_heap.front();
      pop_heap(vertex_heap.begin(), vertex_heap.end());
      vertex_heap.pop_back();
      if (true == is_vertex_visited_lookup[pending_vertex.id]) { continue; }
      if (-1 != pending_vertex.id_from) {
        mst_edeges.push_back(graph_edge(
          pending_vertex.id_from, pending_vertex.id, pending_vertex.priority
        ));
      }
      for (int i = 0; i < total_vertex_cnt; i++) {
        if (0 == mst_graph_metrix[pending_vertex.id][i]) { continue; }
        vertex_heap.push_back(graph_vertex(
          i, mst_graph_metrix[pending_vertex.id][i], pending_vertex.id
        ));
      }
      is_vertex_visited_lookup[pending_vertex.id] = true;
    }

    return mst_edeges;
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
  using graph_util::undirected_graph_vertex;
  using graph_util::graph_vertex;
  using graph_util::graph_edge;

  undirected_graph_vertex * a_ptr = new undirected_graph_vertex(0);
  undirected_graph_vertex * b_ptr = new undirected_graph_vertex(1);
  undirected_graph_vertex * c_ptr = new undirected_graph_vertex(2);

  a_ptr->neighbors.push_back(b_ptr); a_ptr->neighbors.push_back(c_ptr);
  b_ptr->neighbors.push_back(a_ptr); b_ptr->neighbors.push_back(c_ptr);
  c_ptr->neighbors.push_back(a_ptr); c_ptr->neighbors.push_back(b_ptr);
  c_ptr->neighbors.push_back(c_ptr);

  cout << "1. clone undirected graph:" << endl;
  graph_util::print_undirected_graph(a_ptr);
  graph_util::print_undirected_graph(b_ptr);
  graph_util::print_undirected_graph(c_ptr);
  cout << "<<==>>" << endl;
  graph_util::print_undirected_graph(graph_util::clone_undirected_graph(a_ptr));
  graph_util::print_undirected_graph(graph_util::clone_undirected_graph(b_ptr));
	graph_util::print_undirected_graph(graph_util::clone_undirected_graph(c_ptr));

  cout << "2. gen shortest paths:" << endl;
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
  graph_util::print_all_elem<graph_vertex>(vector<graph_vertex>({
    graph_vertex(0, 0),  graph_vertex(1, 4), graph_vertex(2, 12),
    graph_vertex(3, 19), graph_vertex(4, 21), graph_vertex(5, 11),
    graph_vertex(6, 9),  graph_vertex(7, 8), graph_vertex(8, 14)
  }));
  graph_util::print_all_elem<graph_vertex>(
    graph_util::calc_shortest_paths(sp_graph_metrix, 0)
  );

  cout << "3. gen minimum spanning tree:" << endl;
  vector<vector<int>> mst_graph_metrix({
    vector<int>({0, 2, 0, 6, 0}), vector<int>({2, 0, 3, 8, 5}),
    vector<int>({0, 3, 0, 0, 7}), vector<int>({6, 8, 0, 0, 9}),
    vector<int>({0, 5, 7, 9, 0}),
  });
  graph_util::print_all_elem<graph_edge>(vector<graph_edge>({
    graph_edge(0, 1, 2), graph_edge(1, 2, 3),
    graph_edge(0, 3, 6), graph_edge(1, 4, 5),
  }));
  graph_util::print_all_elem<graph_edge>(
    graph_util::calc_minimum_spanning_tree(mst_graph_metrix)
  );

  return 0;
}
