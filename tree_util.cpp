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
  class binary_tree_node {
  public:
    binary_tree_node(int val) {
      value = val; column_id = 0; level_id = 0;
      is_visited = false; is_right_child = false;
      left_ptr = NULL; right_ptr = NULL; sibling = NULL;
    }
  }
   */
  static vector<binary_tree_node> bst_in_column_order(binary_tree_node * root){
    vector<binary_tree_node> tree_node_arr;
    unordered_map<int, vector<binary_tree_node *>> column_id_to_node_ptr_arr;
    int min_column_id = 0, max_column_id = 0;

    if (NULL == root) { return tree_node_arr; }

    deque<pair<binary_tree_node *, int>> treverse_buffer;
    treverse_buffer.push_back(pair<binary_tree_node *, int>(root, 0));

    while (false == treverse_buffer.empty()) {
      pair<binary_tree_node *, int> ptr_id_pair = treverse_buffer.front();
      min_column_id = min(min_column_id, ptr_id_pair.second);
      max_column_id = max(max_column_id, ptr_id_pair.second);
      treverse_buffer.pop_front();
      if (column_id_to_node_ptr_arr.end() == column_id_to_node_ptr_arr.find(ptr_id_pair.second)) {
        column_id_to_node_ptr_arr[ptr_id_pair.second] = vector<binary_tree_node *>();
      }
      column_id_to_node_ptr_arr[ptr_id_pair.second].push_back(ptr_id_pair.first);
      if (NULL != ptr_id_pair.first->left_ptr) {
        treverse_buffer.push_back(
          pair<binary_tree_node *, int>(ptr_id_pair.first->left_ptr, ptr_id_pair.second - 1)
        );
      }
      if (NULL != ptr_id_pair.first->right_ptr) {
        treverse_buffer.push_back(
          pair<binary_tree_node *, int>(ptr_id_pair.first->right_ptr, ptr_id_pair.second + 1)
        );
      }
    }
    for (int i = min_column_id; i <= max_column_id; i++) {
      if (column_id_to_node_ptr_arr.end() == column_id_to_node_ptr_arr.find(i)) { continue; }
      for (auto ptr : column_id_to_node_ptr_arr[i]) { tree_node_arr.push_back(* ptr); }
    }
    return tree_node_arr;
  }

  static void _flag_coord_scan_bst(binary_tree_node * root,
                                   int col, int lvl, bool is_right,
                                   vector<binary_tree_node> & bst_node_arr) {
    if (NULL == root) { return; }
    root->column_id = col; root->level_id = lvl; root->is_right_child =is_right;
    bst_node_arr.push_back(* root);
    _flag_coord_scan_bst(root->left_ptr, col - 1, lvl + 1, false, bst_node_arr);
    _flag_coord_scan_bst(root->right_ptr, col + 1, lvl + 1, true, bst_node_arr);
  }
  static vector<binary_tree_node> _bst_in_column_order(binary_tree_node * root){
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

  static void _find_bst_inorder_succ(binary_tree_node * root_ptr,
                                     binary_tree_node * dest_ptr,
                                     binary_tree_node * succ_ptr,
                                     binary_tree_node * prev_ptr) {
    if (NULL == root_ptr) { return; }
    _find_bst_inorder_succ(root_ptr->left_ptr, dest_ptr, succ_ptr, prev_ptr);
    if (root_ptr == dest_ptr) { * prev_ptr = * root_ptr; }
    if (-1 != prev_ptr->value && -1 == succ_ptr->value && root_ptr != dest_ptr){
      * succ_ptr = * root_ptr; return;
    }
    _find_bst_inorder_succ(root_ptr->right_ptr, dest_ptr, succ_ptr, prev_ptr);
  }

  static binary_tree_node bst_inorder_succ(binary_tree_node * root_ptr,
                                           binary_tree_node * dest_ptr) {
    binary_tree_node succ_node(-1), prev_node(-1);
    _find_bst_inorder_succ(root_ptr, dest_ptr, & succ_node, & prev_node);
    return succ_node;
  }

  static bool _is_input_valid(const vector<int> & preorder,
                              int pre_min, int pre_max,
                              const vector<int> & inorder,
                              int in_min, int in_max) {
    return (
      (pre_max >= pre_min) && (pre_min >= 0) && (pre_max < preorder.size()) &&
      (in_max >= in_min) && (in_min >= 0) && (in_max < inorder.size()) &&
      ((pre_max - pre_min) == (in_max - in_min))
    );
  }
  /*
   *                  4
   *                /   \
   *               3     6
   *              / \   / \
   *             1   2 5   7
   *                 { pivot | left-subtree | right-subtree }
   * int[] peorder = { 4, 3, 1, 2, | 6, 5, 7 }
   *                 { left-subtree | pivot | right-subtree }
   * int[] inorder = { 1, 2, 3, 4, | 5, 6, 7 }
   * build_bst_via_pre_in(n) = n + build_bst_via_pre_in(n/2) +
   *                               build_bst_via_pre_in(n/2)
   * - assume all values are uniq & input orders are valid.
   * - find pivot & left-subt & right-subt took O(n)
   * - T(n) -> nlogn
   */
  static binary_tree_node * build_bst_via_pre_and_in(const vector<int> & preorder,
                                                     int pre_min, int pre_max,
                                                     const vector<int> & inorder,
                                                     int in_min, int in_max){
    binary_tree_node * root_ptr = NULL;
    if (false == _is_input_valid(preorder, pre_min, pre_max,
                                 inorder, in_min, in_max)) { return root_ptr; }
    root_ptr = new binary_tree_node(preorder[pre_min]);
    if (in_max == in_min) { return root_ptr; }
    int preorder_pivot = pre_min, inorder_pivot = in_min;
    while (inorder[inorder_pivot] != preorder[pre_min]) {
      preorder_pivot++; inorder_pivot++;
    }
    root_ptr->left_ptr = build_bst_via_pre_and_in(
      preorder, pre_min + 1, preorder_pivot, inorder, in_min, inorder_pivot - 1
    );
    root_ptr->right_ptr = build_bst_via_pre_and_in(
      preorder, preorder_pivot + 1, pre_max, inorder, inorder_pivot + 1, in_max
    );
    return root_ptr;
  }
  /*
   *                  4
   *                /   \
   *               3     6
   *              / \   / \
   *             1   2 5   7
   *                 { left-subtree | right-subtree | pivot }
   * int[] ptorder = { 1, 2, 3, 5, 7, 6, 4 }
   *                 { left-subtree | pivot | right-subtree }
   * int[] inorder = { 1, 2, 3, 4, 5, 6, 7 }
   */
  static binary_tree_node * build_bst_via_post_and_in(const vector<int> & postorder,
                                                      int post_min, int post_max,
                                                      const vector<int> & inorder,
                                                      int in_min, int in_max){
    binary_tree_node * root_ptr = NULL;
    if (false == _is_input_valid(postorder, post_min, post_max,
                                 inorder, in_min, in_max)) { return root_ptr; }
    root_ptr = new binary_tree_node(postorder[post_max]);
    if (in_max == in_min) { return root_ptr; }

    int postorder_pivot = post_min, inorder_pivot = in_min;
    while (inorder[inorder_pivot] != postorder[post_max]) {
      postorder_pivot++; inorder_pivot++;
    }
    root_ptr->left_ptr = build_bst_via_post_and_in(
      postorder, post_min, postorder_pivot - 1, inorder, in_min, inorder_pivot - 1
    );
    root_ptr->right_ptr = build_bst_via_post_and_in(
      postorder, postorder_pivot, post_max - 1, inorder, inorder_pivot + 1, in_max
    );
    return root_ptr;
  }
};

int main(void) {
  using tree_util::binary_tree_node;
  using tree_util::print_all_elem;
  using tree_util::bfs_bst_print;
  using tree_util::bfs_bst_print_by_neighbor;
  using tree_util::lvr_bst_print;
  using tree_util::lrv_bst_print;
  using tree_util::vlr_bst_print;

  using tree_util::bfs_bst_connect;
  using tree_util::bst_in_column_order;
  using tree_util::build_bst_via_pre_and_in;
  using tree_util::build_bst_via_post_and_in;

  binary_tree_node a(6);  binary_tree_node b(4);  binary_tree_node c(8);
  binary_tree_node d(1);  binary_tree_node e(5);  binary_tree_node f(7);
  binary_tree_node g(10); binary_tree_node h(11); binary_tree_node i(9);
  binary_tree_node t(2);  binary_tree_node k(3);

  a.left_ptr = &b;  a.right_ptr = &c; b.left_ptr = &d; b.right_ptr = &e;
  d.right_ptr = &t; t.right_ptr = &k; c.left_ptr = &f; c.right_ptr = &g;
  g.left_ptr = &i;  g.right_ptr = &h;

  cout << endl << "1. bst_in_column_order:" << endl;
  cout << "[ (1, -2, 2, 0) (4, -1, 1, 0) (2, -1, 3, 1) (6, 0, 0, 0) "
       << "(5, 0, 2, 1) (7, 0, 2, 0) (3, 0, 4, 1) (8, 1, 1, 1) (9, 1, 3, 0) "
       << "(10, 2, 2, 1) (11, 3, 3, 1) ]" << endl;
  print_all_elem<binary_tree_node>(bst_in_column_order(&a));

  cout << endl << "2. bfs_bst_connect:" << endl;
  bfs_bst_print(&a);
  bfs_bst_connect(&a);
  bfs_bst_print_by_neighbor(&a);

  /**
   *       6a
   *      /   \
   *    4b     c8
   *    / \   / \
   *  1d  5e f7  g10
   *    \       / \
   *    2t     i9   h11
   *      \
   *      3k
   */
  cout << endl << "3. bst_inorder_succ:" << endl;
  lvr_bst_print(&a); cout << endl;
  cout << k << " : " << bst_inorder_succ(&a, &k) << endl;
  cout << b << " : " << bst_inorder_succ(&a, &b) << endl;
  cout << c << " : " << bst_inorder_succ(&a, &c) << endl;
  cout << i << " : " << bst_inorder_succ(&a, &i) << endl;
  cout << h << " : " << bst_inorder_succ(&a, &h) << endl;


  vector<int> inorder({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
  vector<int> preorder({ 6, 4, 1, 2, 3, 5, 8, 7, 10, 9, 11 });
  vector<int> postorder({ 3, 2, 1, 5, 4, 7, 9, 11, 10, 8, 6 });

  cout << endl << "4. build_bst_via_pre_and_in" << endl;
  cout << "in-order: 1 2 3 4 5 6 7 8 9 10 11" << endl
       << "pe-order: 6 4 1 2 3 5 8 7 10 9 11" << endl;
  cout << "in-order: "; lvr_bst_print(&a); cout << endl;
  cout << "pe-order: "; vlr_bst_print(&a); cout << endl;
  binary_tree_node * ei_root = build_bst_via_pre_and_in(
    preorder, 0, preorder.size() - 1, inorder, 0, inorder.size() - 1
  );
  cout << "in-order: "; lvr_bst_print(ei_root); cout << endl;
  cout << "pe-order: "; vlr_bst_print(ei_root); cout << endl;

  cout << endl << "5. build_bst_via_post_and_in" << endl;
  cout << "in-order: 1 2 3 4 5 6 7 8 9 10 11" << endl
       << "pt-order: 3 2 1 5 4 7 9 11 10 8 6" << endl;
  cout << "in-order: "; lvr_bst_print(&a); cout << endl;
  cout << "pt-order: "; lrv_bst_print(&a); cout << endl;
  binary_tree_node * pi_root = build_bst_via_post_and_in(
    postorder, 0, postorder.size() - 1, inorder, 0, inorder.size() - 1
  );
  cout << "in-order: "; lvr_bst_print(pi_root); cout << endl;
  cout << "pt-order: "; lrv_bst_print(pi_root); cout << endl;

  return 0;
}
