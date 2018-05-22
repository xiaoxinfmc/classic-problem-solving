/*
OJ's Undirected graph serialization:
Nodes are labeled uniquely.

We use # as a separator for each node, and, as a separator for node label and
each neighbor of the node. As an example, consider the serialized graph
{ 0, 1, 2#1, 2#2, 2 }.

The graph has a total of three nodes, and therefore contains three parts as
separated by #.

First node is labeled as 0. Connect node 0 to both nodes 1 and 2.
Second node is labeled as 1. Connect node 1 to node 2.
Third node is labeled as 2. Connect node 2 to 2 (itself), forming a self-cycle.

Visually, the graph looks like the following:
       1
      / \
     /   \
    0 --- 2
         / \
         \_/
1, (0, 2)
1 | [ 0, 2 ]    < 1 >


*/

/**
 * Definition for undirected graph.
 * struct UndirectedGraphNode {
 *     int label;
 *     vector<UndirectedGraphNode *> neighbors;
 *     UndirectedGraphNode(int x) : label(x) {};
 * };
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <deque>

using namespace std;

class UndirectedGraphNode {
public:
  int label;
  vector<UndirectedGraphNode *> neighbors;
  UndirectedGraphNode(int x) : label(x) {};
}; 

class GraphUtil{
public:
  /**
   * input:
   * - UndirectedGraphNode * node: ptr points to any node inside a graph
   * output:
   * - UndirectedGraphNode * graph_copy_ptr: ptr points to node in copied one.
   * assumptions:
   * - graph is undirected & connected.
   * - all node within the graph has a uniq int lable.
   * - may have cycle, need a hash for flagging visited.
   * idea:
   * - start with the input, traversing the whole graph & make a copy.
   */
  static UndirectedGraphNode * clone_undirected_graph(UndirectedGraphNode * graph_ptr) {
    UndirectedGraphNode * curr_graph_ptr = NULL,
                        * graph_copy_ptr = NULL,
                        * graph_node_ptr = NULL;

    if (NULL == graph_ptr) { return graph_copy_ptr; }

    deque<UndirectedGraphNode *> graph_ptr_buffer;
    unordered_map<UndirectedGraphNode *, UndirectedGraphNode *> curr_node_to_new_ptr_map;
    unordered_map<UndirectedGraphNode *, UndirectedGraphNode *>::const_iterator new_node_iter;

    graph_ptr_buffer.push_back(graph_ptr);
    while(false == graph_ptr_buffer.empty()) {
      curr_graph_ptr = graph_ptr_buffer.front();
      graph_ptr_buffer.pop_front();

      if (curr_node_to_new_ptr_map.end() !=
          curr_node_to_new_ptr_map.find(curr_graph_ptr)) { continue; }

      graph_node_ptr = new UndirectedGraphNode(curr_graph_ptr->label);
      assert(NULL != graph_node_ptr);
      /* store the starting point of graph to copy */
      if (NULL == graph_copy_ptr) { graph_copy_ptr = graph_node_ptr; }
      /* store the mapping between old ptr and new ptr copied also used as visited flag */
      curr_node_to_new_ptr_map[curr_graph_ptr] = graph_node_ptr;

      for (size_t i = 0; i < curr_graph_ptr->neighbors.size(); i++) {
        graph_ptr_buffer.push_back(curr_graph_ptr->neighbors[i]);
        /* copy neighbor link information */
        new_node_iter = curr_node_to_new_ptr_map.find(curr_graph_ptr->neighbors[i]);
        if (curr_node_to_new_ptr_map.end() != new_node_iter) {
          graph_node_ptr->neighbors.push_back(new_node_iter->second);
          if (graph_node_ptr != new_node_iter->second) {
            new_node_iter->second->neighbors.push_back(graph_node_ptr);
          }
        }
      }
    }
    return graph_copy_ptr;
  }

  /* 0 |2| 0#1 0#2 1 |2| 1#0 1#2 2 |3| 2#0 2#1 2#2 */
  static void print_undirected_graph(UndirectedGraphNode * graph_ptr) {
    if (NULL == graph_ptr) { return; }

    UndirectedGraphNode * curr_graph_ptr = NULL;
    deque<UndirectedGraphNode *> graph_ptr_buffer;
    unordered_map<UndirectedGraphNode *, bool> visited_flag_map;

    graph_ptr_buffer.push_back(graph_ptr);
    while(false == graph_ptr_buffer.empty()) {
      curr_graph_ptr = graph_ptr_buffer.front();
      graph_ptr_buffer.pop_front();
      if (true == visited_flag_map[curr_graph_ptr]) { continue; }
      cout << curr_graph_ptr->label << " |" << curr_graph_ptr->neighbors.size() << "| ";
      for (size_t i = 0; i < curr_graph_ptr->neighbors.size(); i++) {
        cout << curr_graph_ptr->label << "#" << curr_graph_ptr->neighbors[i]->label << " ";
        graph_ptr_buffer.push_back(curr_graph_ptr->neighbors[i]);
      }
      visited_flag_map[curr_graph_ptr] = true;
    }
    cout << endl;
  }

  static UndirectedGraphNode * clone_graph(UndirectedGraphNode * node) {
    UndirectedGraphNode * curr_node_ptr = node,
                        * temp_node_ptr = NULL,
                        * existing_nptr = NULL,
                        * grph_copy_ptr = NULL;
    if (NULL == node) { return curr_node_ptr; }

    deque<UndirectedGraphNode *> graph_path_buffer;
    unordered_map<int, UndirectedGraphNode *> visited_node_ptr_map;
    unordered_map<int, UndirectedGraphNode *> existed_node_ptr_map;
    unordered_map<int, UndirectedGraphNode *>::iterator node_itr;
    // O(e)
    graph_path_buffer.push_back(curr_node_ptr);
    while(false == graph_path_buffer.empty()) {
      // pop the front from in buffer to visit & clone
      curr_node_ptr = graph_path_buffer.front();
      graph_path_buffer.pop_front();

      // skip if nodes been visited already.
      if (visited_node_ptr_map.find(curr_node_ptr->label) !=
          visited_node_ptr_map.end()){ continue; }

      // new node, unvisited, then create new & insert to map.
      node_itr = existed_node_ptr_map.find(curr_node_ptr->label);
      if (node_itr == existed_node_ptr_map.end()) {
        temp_node_ptr = new UndirectedGraphNode(curr_node_ptr->label);
        assert(NULL != temp_node_ptr);
        existed_node_ptr_map[temp_node_ptr->label] = temp_node_ptr;
      } else {
        temp_node_ptr = node_itr->second;
      }
      if (NULL == grph_copy_ptr) { grph_copy_ptr = temp_node_ptr; }

      visited_node_ptr_map[curr_node_ptr->label] = curr_node_ptr;
      for(auto & itr : curr_node_ptr->neighbors) {
        // add to its neighbor links
        node_itr = existed_node_ptr_map.find(itr->label);
        if (node_itr == existed_node_ptr_map.end()) {
          existing_nptr = new UndirectedGraphNode(itr->label);
          assert(NULL != existing_nptr);
          existed_node_ptr_map[existing_nptr->label] = existing_nptr;
          temp_node_ptr->neighbors.push_back(existing_nptr);
        } else {
          temp_node_ptr->neighbors.push_back(node_itr->second);
        }
        // check if already visited or not, skip as needed.
        if (visited_node_ptr_map.find(itr->label) !=
            visited_node_ptr_map.end()){ continue; }
        graph_path_buffer.push_back(itr);
      }
    }
    return grph_copy_ptr;
  }

  static void print(UndirectedGraphNode * curr_node_ptr) {
    deque<UndirectedGraphNode *> graph_path_buffer;
    unordered_map<int, UndirectedGraphNode *> visited_node_ptr_map;
    // O(e)
    graph_path_buffer.push_back(curr_node_ptr);
    while(false == graph_path_buffer.empty()) {
      // pop the front from in buffer to visit & clone
      curr_node_ptr = graph_path_buffer.front();
      graph_path_buffer.pop_front();

      // skip if nodes been visited already.
      if (visited_node_ptr_map.find(curr_node_ptr->label) !=
          visited_node_ptr_map.end()){ continue; }
      // new node, unvisited, then create new & insert to map.
      visited_node_ptr_map[curr_node_ptr->label] = curr_node_ptr;
      cout << curr_node_ptr->label << " |" << curr_node_ptr->neighbors.size() << "| ";
      for(auto & itr : curr_node_ptr->neighbors) {
        cout << curr_node_ptr->label << "#" << itr->label << " ";
        // check if already visited or not, skip as needed.
        if (visited_node_ptr_map.find(itr->label) !=
            visited_node_ptr_map.end()){ continue; }
        graph_path_buffer.push_back(itr);
      }
    }
    cout << endl;
  }
};

int main(void) {

/*
 * 0, 1, 2
 * 0 | [ 1, 2 ] | <0> { 0#1, 0#2 }
 * 1 | [ 2, 2 ] | <0, 1> { 1#0, 1#2 }
 * 2 | [ 2, 2, 2 ] <0, 1, 2> { 2#0, 2#1, 2#2 }
 */
/*
       1
      / \
     /   \
    0 --- 2
         / \
         \_/
*/
  UndirectedGraphNode * a_ptr = new UndirectedGraphNode(0);
  UndirectedGraphNode * b_ptr = new UndirectedGraphNode(1);
  UndirectedGraphNode * c_ptr = new UndirectedGraphNode(2);
  a_ptr->neighbors.push_back(b_ptr);
  a_ptr->neighbors.push_back(c_ptr);
  b_ptr->neighbors.push_back(a_ptr);
  b_ptr->neighbors.push_back(c_ptr);
  c_ptr->neighbors.push_back(a_ptr);
  c_ptr->neighbors.push_back(b_ptr);
  c_ptr->neighbors.push_back(c_ptr);

  GraphUtil::print(a_ptr);
  GraphUtil::print(b_ptr);
  GraphUtil::print(c_ptr);

  GraphUtil::print(GraphUtil::clone_graph(a_ptr));
  GraphUtil::print(GraphUtil::clone_graph(b_ptr));
	GraphUtil::print(GraphUtil::clone_graph(c_ptr));

  cout << endl;

  GraphUtil::print_undirected_graph(a_ptr);
  GraphUtil::print_undirected_graph(b_ptr);
  GraphUtil::print_undirected_graph(c_ptr);

  GraphUtil::print(GraphUtil::clone_undirected_graph(a_ptr));
  GraphUtil::print(GraphUtil::clone_undirected_graph(b_ptr));
	GraphUtil::print(GraphUtil::clone_undirected_graph(c_ptr));

  return 0;
}
