#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <cassert>
#include <unordered_map>

using namespace std;

namespace tree_util {

  template <class type>
  static void print_all_elem(const vector<type> & input) {
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

  class binary_tree_node {
  public:
    binary_tree_node(int val) {
      value = val; column_id = 0; level_id = 0;
      is_visited = false; is_right_child = false;
      left_ptr = NULL; right_ptr = NULL; sibling = NULL;
    }
    virtual ~binary_tree_node() {}
    friend bool operator< (const binary_tree_node & lnode,
                           const binary_tree_node & rnode) {
      bool is_lnode_rank_higher = false;
      if (lnode.column_id == rnode.column_id) {
        if (lnode.level_id == rnode.level_id) {
          is_lnode_rank_higher = (
            (true == lnode.is_right_child) && (false == rnode.is_right_child)
          );
        } else {
          is_lnode_rank_higher = (lnode.level_id < rnode.level_id);
        }
      } else {
        is_lnode_rank_higher = (lnode.column_id < rnode.column_id);
      }
      return is_lnode_rank_higher;
    }
    friend bool operator== (const binary_tree_node & lnode,
                            const binary_tree_node & rnode) {
      return (lnode.column_id      == rnode.column_id &&
              lnode.level_id       == rnode.level_id &&
              lnode.is_right_child == rnode.is_right_child);
    }
    friend ostream & operator<< (ostream & os, const binary_tree_node & node) {
      os << "(" << node.value << ", " << node.column_id << ", "
         << node.level_id << ", " << node.is_right_child << ")"; return os;
    }
    int value, column_id, level_id;
    bool is_visited, is_right_child;
    binary_tree_node * left_ptr, * right_ptr, * sibling;
  };

  /**
   *
   *            1 (0, 0)
   *          /   \
   * (-1, 1) 2     3 (1, 1)
   *       /  \   /  \
   *      4    5 6    7
   * (-2, 2)      \    \
   *               8    9
   * The output of print this tree vertically will be:
   * 4 -> 2 -> 1 5 6 -> 3 8 -> 7 -> 9
   * smallest column & level goes first
   *       6a
   *      /   \
   *    4b     c8
   *    / \   / \
   *  1d  5e f7  g10
   *    \       / \
   *    2t     i9   h11
   *      \
   *      3k
   * zig_zag
   * [ 1 -> (4 -> 2) -> (6 -> 5 -> 7 -> 3) -> (8 -> 9) -> 10 -> 11
   * [ (1, -2, 2, 0) (4, -1, 1, 0) (2, -1, 3, 1) (6, 0, 0, 0) (5, 0, 2, 1)
   *   (7, 0, 2, 0) (3, 0, 4, 1) (8, 1, 1, 1) (9, 1, 3, 0) (10, 2, 2, 1)
   *   (11, 3, 3, 1) ]
   */

  static void _flag_coord_scan_bst(binary_tree_node * root,
                                   int col, int lvl, bool is_right,
                                   vector<binary_tree_node> & bst_node_arr) {
    if (NULL == root) { return; }
    root->column_id = col; root->level_id = lvl; root->is_right_child =is_right;
    bst_node_arr.push_back(* root);
    _flag_coord_scan_bst(root->left_ptr, col - 1, lvl + 1, false, bst_node_arr);
    _flag_coord_scan_bst(root->right_ptr, col + 1, lvl + 1, true, bst_node_arr);
  }
  static vector<binary_tree_node> bst_in_column_order(binary_tree_node * root){
    vector<binary_tree_node> bst_node_arr;
    _flag_coord_scan_bst(root, 0, 0, false, bst_node_arr);
    sort(bst_node_arr.begin(), bst_node_arr.end());
    return bst_node_arr;
  }

  static void vlr_bst_print(binary_tree_node * root_ptr) {
    if (NULL == root_ptr) { return; }
    cout << root_ptr->value << " ";
    vlr_bst_print(root_ptr->left_ptr);
    vlr_bst_print(root_ptr->right_ptr);
  }

  static void lrv_bst_print(binary_tree_node * root_ptr) {
    if (NULL == root_ptr) { return; }
    lrv_bst_print(root_ptr->left_ptr);
    lrv_bst_print(root_ptr->right_ptr);
    cout << root_ptr->value << " ";
  }

  static void lvr_bst_print(binary_tree_node * root_ptr) {
    if (NULL == root_ptr) { return; }
    lvr_bst_print(root_ptr->left_ptr);
    cout << root_ptr->value << " ";
    lvr_bst_print(root_ptr->right_ptr);
  }

  static void bfs_bst_print_by_neighbor(binary_tree_node * root_ptr) {
    for (binary_tree_node * tmp = root_ptr; NULL != tmp; tmp = tmp->sibling) {
      cout << tmp->value << " ";
    };cout << endl;
  }

  static void bfs_bst_print(binary_tree_node * root_ptr) {
    if (NULL == root_ptr) { return; }
    deque<binary_tree_node *> node_ptr_buffer;
    node_ptr_buffer.push_back(root_ptr);
    while(false == node_ptr_buffer.empty()) {
      root_ptr = node_ptr_buffer.front();
      cout << root_ptr->value << " ";
      if (NULL != root_ptr->left_ptr) {
        node_ptr_buffer.push_back(root_ptr->left_ptr);
      }
      if (NULL != root_ptr->right_ptr) {
        node_ptr_buffer.push_back(root_ptr->right_ptr);
      }
      node_ptr_buffer.pop_front();
    }; cout << endl;
  }

  static void bfs_bst_connect(binary_tree_node * root_ptr) {
    binary_tree_node * prev_node_ptr = NULL;
    deque<binary_tree_node *> node_ptr_buffer;
    node_ptr_buffer.push_back(root_ptr);
    while (false == node_ptr_buffer.empty()) {
      if (NULL != prev_node_ptr) {
        prev_node_ptr->sibling = node_ptr_buffer.front();
      }
      if (NULL != node_ptr_buffer.front()->left_ptr) {
        node_ptr_buffer.push_back(node_ptr_buffer.front()->left_ptr);
      }
      if (NULL != node_ptr_buffer.front()->right_ptr) {
        node_ptr_buffer.push_back(node_ptr_buffer.front()->right_ptr);
      }
      prev_node_ptr = node_ptr_buffer.front();
      node_ptr_buffer.pop_front();
    }
  }
};

int main(void) {
  using tree_util::binary_tree_node;
  using tree_util::print_all_elem;
  using tree_util::bfs_bst_print;
  using tree_util::bfs_bst_connect;
  using tree_util::bfs_bst_print_by_neighbor;

  binary_tree_node a(6);  binary_tree_node b(4);  binary_tree_node c(8);
  binary_tree_node d(1);  binary_tree_node e(5);  binary_tree_node f(7);
  binary_tree_node g(10); binary_tree_node h(11); binary_tree_node i(9);
  binary_tree_node t(2);  binary_tree_node k(3);

  a.left_ptr = &b;  a.right_ptr = &c; b.left_ptr = &d; b.right_ptr = &e;
  d.right_ptr = &t; t.right_ptr = &k; c.left_ptr = &f; c.right_ptr = &g;
  g.left_ptr = &i;  g.right_ptr = &h;

  cout << "1. bst_in_column_order:" << endl;
  cout << "[ (1, -2, 2, 0) (4, -1, 1, 0) (2, -1, 3, 1) (6, 0, 0, 0) "
       << "(5, 0, 2, 1) (7, 0, 2, 0) (3, 0, 4, 1) (8, 1, 1, 1) (9, 1, 3, 0) "
       << "(10, 2, 2, 1) (11, 3, 3, 1) ]" << endl;
  print_all_elem<binary_tree_node>(bst_in_column_order(&a));

  cout << "2. bfs_bst_connect:" << endl;
  bfs_bst_print(&a);
  bfs_bst_connect(&a);
  bfs_bst_print_by_neighbor(&a);

  return 0;
}
