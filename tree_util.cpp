#include <iostream>
#include <utility>
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
      prev_ptr = NULL; next_ptr = NULL;
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
    binary_tree_node * left_ptr, * right_ptr, * sibling, * prev_ptr, * next_ptr;
  };

  /**
   * Inorder Tree Traversal without Recursion
   * - Using Stack is the obvious way to traverse tree without recursion. Below
   *   is an algorithm for traversing binary tree using stack. See this for step
   *   wise step execution of the algorithm.
   * 1) Create an empty stack S.
   * 2) Initialize current node as root
   * 3) Push the current node to S and set current = current->left until current is NULL
   * 4) If current is NULL and stack is not empty then 
   *    a) Pop the top item from stack.
   *    b) Print the popped item, set current = popped_item->right 
   *    c) Go to step 3.
   * 5) If current is NULL and stack is empty then we are done.
   */
  static void lvr_bst_print_non_recur(binary_tree_node * root_ptr) {
    binary_tree_node * curr_ptr = root_ptr;
    vector<binary_tree_node *> visit_buffer;

    while (NULL != curr_ptr || false == visit_buffer.empty()) {
      while (NULL != curr_ptr) {
        visit_buffer.push_back(curr_ptr);
        curr_ptr = curr_ptr->left_ptr;
      }

      curr_ptr = visit_buffer.back();
      visit_buffer.pop_back();
      cout << curr_ptr->value << " ";

      curr_ptr = curr_ptr->right_ptr;
    }
  }

  static void _lvr_bst_print_non_recur(binary_tree_node * root_ptr) {
    binary_tree_node * curr_ptr;
    vector<binary_tree_node *> visit_buffer(1, root_ptr);

    while (false == visit_buffer.empty()) {
      curr_ptr = visit_buffer.back();
      if (NULL == curr_ptr) { visit_buffer.pop_back(); continue; }

      while (NULL !=  curr_ptr->left_ptr &&
             false == curr_ptr->left_ptr->is_visited) {
        visit_buffer.push_back(curr_ptr->left_ptr);
        curr_ptr = curr_ptr->left_ptr;
      }

      curr_ptr = visit_buffer.back();
      visit_buffer.pop_back();
      cout << curr_ptr->value << " ";
      curr_ptr->is_visited = true;

      visit_buffer.push_back(curr_ptr->right_ptr);
    }
  }

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

  /**
   * Convert a Binary Tree to a Circular Doubly Link List
   * Given a Binary Tree, convert it to a Circular Doubly Linked List(In-Place)
   * - The left and right pointers in nodes are to be used as previous and next
   *   pointers respectively in converted Circular Linked List.
   * - The order of nodes in List must be same as Inorder of given Binary Tree.
   * - The first node of Inorder traversal must be head node of Circular List.
  class binary_tree_node {
  public:
    binary_tree_node(int val) {
      value = val; column_id = 0; level_id = 0;
      is_visited = false; is_right_child = false;
      left_ptr = NULL; right_ptr = NULL; sibling = NULL;
    }
  }
   *       6a
   *      /   \
   *    4b     c8
   *    / \   / \
   *  1d  5e f7  g10
   *    \       / \
   *   2.5t    i9   h11
   *   /  \
   *  2   3k
   *  1(4) - 2(2.5) - 2.5(1) - 3(2) - 4(6) - 5(4)
   * in-order traversal => give us the order of the list, but its parent is not its next-ptr/prev.
   *                    => we can return the 
   * - each internal node, prev-ptr -> the right-most node from its left-subtree
   *                       next-ptr -> the left-most node from its right-subtree
   * - post-order, for each subtree, return pair of (left most (min) & right most (max) ptr)
   */
  static pair<binary_tree_node *, binary_tree_node *> inorder_linked_list_from_bst_recur(binary_tree_node * curr_ptr)
  {
    if (NULL == curr_ptr) { return pair<binary_tree_node *, binary_tree_node *>(NULL, NULL); }

    pair<binary_tree_node *, binary_tree_node *>left_tree_pair = inorder_linked_list_from_bst_recur(curr_ptr->left_ptr);
    pair<binary_tree_node *, binary_tree_node *>right_tree_pair = inorder_linked_list_from_bst_recur(curr_ptr->right_ptr);

    if (NULL != left_tree_pair.second) { curr_ptr->prev_ptr = left_tree_pair.second; left_tree_pair.second->next_ptr = curr_ptr; }
    if (NULL != right_tree_pair.first) { curr_ptr->next_ptr = right_tree_pair.first; right_tree_pair.first->prev_ptr = curr_ptr; }

    pair<binary_tree_node *, binary_tree_node *>curr_pair(curr_ptr, curr_ptr);
    if (NULL != left_tree_pair.first) { curr_pair.first = left_tree_pair.first; }
    if (NULL != right_tree_pair.second) { curr_pair.second = right_tree_pair.second; }

    return curr_pair;
  }

  static binary_tree_node * inorder_linked_list_from_bst(binary_tree_node * root) {
    pair<binary_tree_node *, binary_tree_node *> ptr_pair = inorder_linked_list_from_bst_recur(root);
    ptr_pair.first->prev_ptr = ptr_pair.second;
    ptr_pair.second->next_ptr = ptr_pair.first;
    return ptr_pair.first;
  }

  /**
   * 236. Lowest Common Ancestor of a Binary Tree
   * - Given a binary tree, find lowest common ancestor of two nodes in tree.
   *
   * - According to the definition of LCA on Wikipedia: "The lowest common
   *   ancestor is defined between two nodes p and q as the lowest node in T
   *   that has both p and q as descendants (where we allow a node to be a
   *   descendant of itself).”
   *
   * - Given the following binary tree:  root = [3,5,1,6,2,0,8,null,null,7,4]
   *
   *          _______3______
   *         /              \
   *     ___5__           ___1__
   *    /      \         /      \
   *   6       _2       0        8
   *          /  \
   *         7    4
   * Example 1:
   *
   * - Input: root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 1
   * - Output: 3
   * - Explanation: The LCA of of nodes 5 and 1 is 3.
   *
   * Example 2:
   *
   * - Input: root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 4
   * - Output: 5
   * - Explanation: The LCA of nodes 5 and 4 is 5, since a node can be a
   #                descendant of itself according to the LCA definition.
   * Note:
   * - All of the nodes values will be unique.
   * - p and q are different and both values will exist in the binary tree.
   * Observation:
   * - DFS for 2 nodes then merge paths from 2, lca will be 1st node diverted
   * - Post-order?
  class binary_tree_node {
  public:
    binary_tree_node(int val) {
      value = val; column_id = 0; level_id = 0;
      is_visited = false; is_right_child = false;
      left_ptr = NULL; right_ptr = NULL; sibling = NULL;
      prev_ptr = NULL; next_ptr = NULL;
    }
  };
   */
  static void _fast_lca(binary_tree_node * root_ptr,
                        binary_tree_node * left_ptr,
                        binary_tree_node * right_ptr,
                        bool * is_left_found_ptr,
                        bool * is_right_found_ptr,
                        binary_tree_node ** lca_ptr_ptr){
    if (NULL == root_ptr) { return; }
    _fast_lca(root_ptr->left_ptr, left_ptr, right_ptr, is_left_found_ptr,
              is_right_found_ptr, lca_ptr_ptr);
    _fast_lca(root_ptr->right_ptr, left_ptr, right_ptr, is_left_found_ptr,
              is_right_found_ptr, lca_ptr_ptr);
    if (false == * is_left_found_ptr) { * is_left_found_ptr = (root_ptr == left_ptr); }
    if (false == * is_right_found_ptr) { * is_right_found_ptr = (root_ptr == right_ptr); }
    if (true == * is_left_found_ptr && true == * is_right_found_ptr && NULL == * lca_ptr_ptr) {
      * lca_ptr_ptr = root_ptr;
    }
  }

  static pair<binary_tree_node *, binary_tree_node *> fast_lca_recur(binary_tree_node * root_ptr,
                                                                     binary_tree_node * left_ptr,
                                                                     binary_tree_node * right_ptr,
                                                                     binary_tree_node ** lca_ptr_ptr){
    pair<binary_tree_node *, binary_tree_node *> ptr_pair(NULL, NULL), tmp_pair(NULL, NULL);

    if (NULL == root_ptr) { return ptr_pair; }

    if (NULL == ptr_pair.first && root_ptr == left_ptr) { ptr_pair.first = root_ptr; }
    if (NULL == ptr_pair.second && root_ptr == right_ptr) { ptr_pair.second = root_ptr; }

    if (NULL == ptr_pair.first || NULL == ptr_pair.second) {
      tmp_pair = fast_lca_recur(root_ptr->left_ptr, left_ptr, right_ptr, lca_ptr_ptr);
      if (NULL == ptr_pair.first && NULL != tmp_pair.first) { ptr_pair.first = tmp_pair.first; }
      if (NULL == ptr_pair.second && NULL != tmp_pair.second) { ptr_pair.second = tmp_pair.second; }
    }
    if (NULL == ptr_pair.first || NULL == ptr_pair.second) {
      tmp_pair = fast_lca_recur(root_ptr->right_ptr, left_ptr, right_ptr, lca_ptr_ptr);
      if (NULL == ptr_pair.first && NULL != tmp_pair.first) { ptr_pair.first = tmp_pair.first; }
      if (NULL == ptr_pair.second && NULL != tmp_pair.second) { ptr_pair.second = tmp_pair.second; }
    }

    if (!(NULL == ptr_pair.first) && !(NULL == ptr_pair.second) && (NULL == * lca_ptr_ptr)) {
      * lca_ptr_ptr = root_ptr;
    }

    return ptr_pair;
  }

  static binary_tree_node * fast_lca(binary_tree_node * root_ptr,
                                     binary_tree_node * left_ptr,
                                     binary_tree_node * right_ptr) {
    binary_tree_node * lca_ptr = NULL;
    fast_lca_recur(root_ptr, left_ptr, right_ptr, &lca_ptr);
    return lca_ptr;
  }

  static void dfs_search_path(binary_tree_node * root_ptr, binary_tree_node * target_ptr,
                              vector<pair<binary_tree_node *, int>> & dfs_path) {
    pair<binary_tree_node *, int> curr_pair;
    vector<pair<binary_tree_node *, int>> dfs_buffer;

    dfs_buffer.push_back(pair<binary_tree_node *, int>(root_ptr, 0));
    while (false == dfs_buffer.empty()) {
      curr_pair = dfs_buffer.back();
      dfs_buffer.pop_back();

      if (NULL == curr_pair.first) { continue; }

      while (false == dfs_path.empty() &&
             dfs_path.back().second != curr_pair.second - 1) { dfs_path.pop_back(); }
      dfs_path.push_back(curr_pair);
      if (curr_pair.first == target_ptr) { break; }

      dfs_buffer.push_back(pair<binary_tree_node *, int>(curr_pair.first->right_ptr, curr_pair.second + 1));
      dfs_buffer.push_back(pair<binary_tree_node *, int>(curr_pair.first->left_ptr, curr_pair.second + 1));
    }
  }

  static binary_tree_node * get_lca(binary_tree_node * root_ptr,
                                    binary_tree_node * left_ptr,
                                    binary_tree_node * right_ptr) {
    binary_tree_node * lca_ptr = NULL;
    if (NULL == root_ptr || NULL == left_ptr || NULL == right_ptr) { return lca_ptr; }
    if (left_ptr == right_ptr) { return left_ptr; }

    vector<pair<binary_tree_node *, int>> left_dfs_path, right_dfs_path;
    dfs_search_path(root_ptr, left_ptr, left_dfs_path);
    dfs_search_path(root_ptr, right_ptr, right_dfs_path);

    int max_path_len = min(left_dfs_path.size(), left_dfs_path.size());
    for (int i = 0; i < max_path_len; i++) {
      if (left_dfs_path[i].first != right_dfs_path[i].first) { break; }
      lca_ptr = left_dfs_path[i].first;
    }
    return lca_ptr;
  }

  /**
   * 823. Binary Trees With Factors
   * - Given an array of unique integers, each integer is strictly greater than
   *   1. We make a binary tree using these integers and each number may be
   *   used for any number of times.
   * - Each non-leaf node's value should be equal to the product of the values
   *   of it's children.
   * - How many binary trees can we make? Return the answer modulo 10**9 + 7.
   * Example 1:
   * - Input: A = [2, 4]
   * - Output: 3
   * - Explanation: We can make these trees: [2], [4], [4, 2, 2]
   * Example 2:
   * - Input: A = [2, 4, 5, 10]
   * - Output: 7
   * - Explanation: We can make these trees:
   *   [2], [4], [5], [10], [4, 2, 2], [10, 2, 5], [10, 5, 2].
   * Note:
   * - 1 <= A.length <= 1000.
   * - 2 <= A[i] <= 10^9.
   * Observation:
   * - each subcomponents contains 3 nodes, parent value > children's
   * - Only need to return count & subproblems may interleave, DP?
   * - probably sort the input first, then find all possible components?
   * - every single elem is a diff. bt.
   * - [2, 4, 5, 10] ==>> { (2, nil, nil), (4, nil, nil), (5, nil, nil), (10, nil, nil) }
   *                              bt0            bt1           bt2              bt3
   *   then                           (4, 2, 2) <--> (10, 2, 5) <--> (10, 5, 2)
   *                                     bt4             bt5             bt6
   *                                                   NO-NEW bt
   * - pre-compute: 10 -> 10 * 5 => 50 ? all possible pairs available.
   * - recursion until no new binary tree found.
   * - T(n) => T(n/3) + n^2 ?
   *
   * Intuition
   * - The largest value v used must be the root node in the tree.
   *   Say dp(v) is the number of ways to make a tree with root node v.
   * - If the root node of the tree (with value v) has children with
   *   values x and y (and x * y == v), then there are dp(x) * dp(y)
   *   ways to make this tree.
   * - In total, there are ∑𝑥∗𝑦=𝑣dp(𝑥)∗dp(𝑦) ways to make a tree with root node v.
   *
   * Algorithm
   * - Let dp[i] be the number of ways to have a root node with value A[i].
   *   Since in the above example we always have x < v and y < v, we can calc
   *   the values of dp[i] in increasing order using dynamic programming.
   * - For some root value A[i], let's try to find candidates for the children
   *   with values A[j] and A[i] / A[j] so that evidently
   * - A[j] * (A[i] / A[j]) = A[i]
   * - To do this quickly, we will need index which looks up this value:
   *   if A[k] = A[i] / A[j], then index[A[i] / A[j]] = k'.
   * - After, we'll add all possible dp[j] * dp[k] (with j < i, k < i) to our
   *   answer dp[i]. In our Java implementation, we carefully used long so avoid
   *   overflow issues.
   */
  static int count_diff_bts(vector<int> values) {
    long mod_base = 10^9 + 7;
    long total_bt_cnt = 0;

    if (true == values.empty()) { return total_bt_cnt; }

    vector<long> bt_cnt_lookup(values.size(), 1);
    sort(values.begin(), values.end());

    unordered_map<int, int> value_to_idx_map;
    for (int i = 0; i < values.size(); i++) { value_to_idx_map[values[i]] = i; }

    int target_token_val = 0;
    for (int i = 0; i < values.size(); i++) {
      for (int j = i - 1; j >= 0; j--) {
        target_token_val = values[i] / values[j];
        if (target_token_val * values[j] != values[i]) { continue; }
        if (value_to_idx_map.end() != value_to_idx_map.find(target_token_val)) {
          bt_cnt_lookup[i] += (bt_cnt_lookup[j] * bt_cnt_lookup[value_to_idx_map[target_token_val]]);
        }
      }
    }

    for (auto & val : bt_cnt_lookup) { total_bt_cnt += val; }
    return total_bt_cnt % mod_base;
  }

  /**
   * Print Edge Nodes (Boundary) of a Binary Tree
   * - Print all edge nodes of a complete binary tree anti-clockwise.
   * - That is all the left most nodes starting at root, then the leaves left
   *   to right and finally all the rightmost nodes. In other words, print the
   *   boundary of the tree.
   * - Variant: Print the same for a tree that is not complete.
   *         ______30______
   *        /              \
   *     __20__          __40__
   *    /      \        /      \
   *   10      25      35      50
   *  /  \       \            /
   *  5  15      28          41
   * The correct solution should print 30, 20, 10, 5, 15, 28, 35, 41, 50, 40.
   */
  static vector<int> boundary_traverse_bt(binary_tree_node * root_ptr) {
    vector<int> boundary;
    if (NULL == root_ptr) { return boundary; }

    boundary.push_back(root_ptr->value);

    binary_tree_node * curr_ptr = root_ptr->left_ptr;
    while(NULL != curr_ptr) {
      boundary.push_back(curr_ptr->value);
      if (NULL != curr_ptr->left_ptr) {
        curr_ptr = curr_ptr->left_ptr;
      } else if (NULL != curr_ptr->right_ptr) {
        curr_ptr = curr_ptr->right_ptr;
      } else {
        boundary.pop_back();
        curr_ptr = NULL;
      }
    }

    curr_ptr = root_ptr;
    vector<binary_tree_node * > visit_buffer(1, root_ptr);
    while (false == visit_buffer.empty()) {
      curr_ptr = visit_buffer.back();
      visit_buffer.pop_back();
      if (NULL == curr_ptr){ continue; }
      if (NULL == curr_ptr->right_ptr && NULL == curr_ptr->left_ptr &&
          root_ptr != curr_ptr) {
        boundary.push_back(curr_ptr->value);
      }
      visit_buffer.push_back(curr_ptr->right_ptr);
      visit_buffer.push_back(curr_ptr->left_ptr);
    }

    curr_ptr = root_ptr->right_ptr;
    vector<int> right_boundary;
    while(NULL != curr_ptr) {
      right_boundary.push_back(curr_ptr->value);
      if (NULL != curr_ptr->right_ptr) {
        curr_ptr = curr_ptr->right_ptr;
      } else if (NULL != curr_ptr->left_ptr) {
        curr_ptr = curr_ptr->left_ptr;
      } else {
        right_boundary.pop_back();
        curr_ptr = NULL;
      }
    }

    for (int i = right_boundary.size() - 1; i >= 0; i--) {
      boundary.push_back(right_boundary[i]);
    }

    return boundary;
  }

  /**
   * 98. Validate Binary Search Tree
   * - Given a binary tree, determine if it is a valid binary search tree (BST).
   * Assume a BST is defined as follows:
   * - The left subtree of a node contains nodes with keys less than node's key.
   * - The right subtree of a node contains only nodes with keys greater.
   * - Both the left and right subtrees must also be binary search trees.
   * Example 1:
   * Input:
   *     2
   *    / \
   *   1   3
   * Output: true
   * Example 2:
   *
   *     5
   *    / \
   *   1   4
   *      / \
   *     3   6
   * Output: false
   * Explanation: The input is: [5,1,4,null,null,3,6]. The root node's value
   *              is 5 but its right child's value is 4.
   * return: <min-node-for-curr-subtree, max-node-for-curr-subtree>
   */
  static pair<binary_tree_node *, binary_tree_node *> is_bst_valid_recur(
    binary_tree_node * root_ptr, bool * is_bst_valid_ptr)
  {
    if (false == * is_bst_valid_ptr || NULL == root_ptr) {
      return pair<binary_tree_node *, binary_tree_node *>(NULL, NULL);
    }
    pair<binary_tree_node *, binary_tree_node *> node_ptr_pair(root_ptr, root_ptr),
                                                 lsub_ptr_pair(NULL, NULL),
                                                 rsub_ptr_pair(NULL, NULL);
    /* min-from-lsub, max-from-lsub */
    lsub_ptr_pair = is_bst_valid_recur(root_ptr->left_ptr, is_bst_valid_ptr);
    /* min-from-rsub, max-from-rsub */
    rsub_ptr_pair = is_bst_valid_recur(root_ptr->right_ptr, is_bst_valid_ptr);

    if ((NULL != lsub_ptr_pair.second) &&
        (root_ptr->value <= lsub_ptr_pair.second->value)) { * is_bst_valid_ptr = false; }
    if ((NULL != rsub_ptr_pair.first) &&
        (root_ptr->value >= rsub_ptr_pair.first->value)) { * is_bst_valid_ptr = false; }

    if (NULL != lsub_ptr_pair.first) { node_ptr_pair.first = lsub_ptr_pair.first; }
    if (NULL != rsub_ptr_pair.second) { node_ptr_pair.second= rsub_ptr_pair.second; }

    return node_ptr_pair;
  }

  static bool is_bst_valid(binary_tree_node * root_ptr) {
    bool is_bst_valid = true;
    is_bst_valid_recur(root_ptr, &is_bst_valid);
    return is_bst_valid;
  }

  struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
  };

  static int calc_subtree_depth(TreeNode * root, bool & is_bst_balanced) {
    if (NULL == root) { return 0; }
    int left_depth = calc_subtree_depth(root->left, is_bst_balanced);
    int right_depth = calc_subtree_depth(root->right, is_bst_balanced);
    is_bst_balanced = is_bst_balanced && (abs(left_depth - right_depth) <= 1);
    return max(left_depth, right_depth) + 1;
  }

  static int calc_max_subtree_depth(TreeNode * root) {
    if (NULL == root) { return 0; }
    int left_subtree_depth = calc_max_subtree_depth(root->left);
    int right_subtree_depth = calc_max_subtree_depth(root->right);
    return max(left_subtree_depth, right_subtree_depth) + 1;
  }

  static int calc_min_subtree_depth(TreeNode * root) {
    if (NULL == root) { return 0; }
    int left_subtree_depth = calc_min_subtree_depth(root->left);
    int right_subtree_depth = calc_min_subtree_depth(root->right);
    int depth = min(left_subtree_depth, right_subtree_depth) + 1;
    if (NULL == root->left && NULL != root->right) { depth = right_subtree_depth + 1; }
    if (NULL != root->left && NULL == root->right) { depth = left_subtree_depth + 1; }
    return depth;
  }

  class max_subtree_ret {
  public:
    max_subtree_ret(binary_tree_node * fi = NULL,
                    binary_tree_node * se = NULL,
                    int a = 0) {
      is_curr_bst_valid = true;
      first = fi; second = se; area = a;
    }
    virtual ~max_subtree_ret() {}
    binary_tree_node * first, * second;
    bool is_curr_bst_valid;
    int area;
  };
  /**
   * Largest Subtree Which is a Binary Search Tree (BST)
   * - Given a binary tree, find the largest subtree which is a Binary Search
   *   Tree (BST), where largest means subtree with largest num of nodes in it.
   * - the subtree must include all of its descendents.
   *       6a
   *      /   \
   *    4b     c8
   *    / \   / \
   *  1d  5e f7  g10
   *    \       / \
   *    2t     i9   h11
   *      \
   *      3k
   * [ 6, 4, 1, 2, 3, 5, 7, 9, 11, 10, 8 ]
   */
  static max_subtree_ret find_largest_sub_bst_recur(binary_tree_node * root_ptr,
                                                    int * max_area_ptr,
                                                    binary_tree_node ** subtree_ptr_ptr)
  {
    if (NULL == root_ptr) { return max_subtree_ret(); }

    max_subtree_ret node_ptr_pair(root_ptr, root_ptr, 1),
                    lsub_ptr_pair(NULL, NULL, 0),
                    rsub_ptr_pair(NULL, NULL, 0);
    lsub_ptr_pair = find_largest_sub_bst_recur(root_ptr->left_ptr, max_area_ptr, subtree_ptr_ptr);
    rsub_ptr_pair = find_largest_sub_bst_recur(root_ptr->right_ptr, max_area_ptr, subtree_ptr_ptr);

    node_ptr_pair.is_curr_bst_valid = (lsub_ptr_pair.is_curr_bst_valid && rsub_ptr_pair.is_curr_bst_valid);
    if ((NULL != lsub_ptr_pair.second) &&
        (root_ptr->value <= lsub_ptr_pair.second->value)) { node_ptr_pair.is_curr_bst_valid = false; }
    if ((NULL != rsub_ptr_pair.first) &&
        (root_ptr->value >= rsub_ptr_pair.first->value)) { node_ptr_pair.is_curr_bst_valid = false; }

    if (NULL != lsub_ptr_pair.first) { node_ptr_pair.first = lsub_ptr_pair.first; }
    if (NULL != rsub_ptr_pair.second) { node_ptr_pair.second = rsub_ptr_pair.second; }

    if (true == node_ptr_pair.is_curr_bst_valid) {
      node_ptr_pair.area += (lsub_ptr_pair.area + rsub_ptr_pair.area);
      if (node_ptr_pair.area > * max_area_ptr) { * max_area_ptr = node_ptr_pair.area;
                                                 * subtree_ptr_ptr = root_ptr; }
    }
    return node_ptr_pair;
  }

  static int find_largest_sub_bst(binary_tree_node * root_ptr) {
    int max_area = 0; binary_tree_node * subtree_ptr;
    find_largest_sub_bst_recur(root_ptr, &max_area, &subtree_ptr);
    return max_area;
  }
  /**
   * 558. Quad Tree Intersection
   * A quadtree is a tree data in which each internal node has exactly four
   * children: topLeft, topRight, bottomLeft and bottomRight. Quad trees are
   * often used to partition a two-dimensional space by recursively subdividing
   * it into four quadrants or regions.
   *
   * We want to store True/False information in our quad tree. The quad tree is
   * used to represent a N * N boolean grid. For each node, it is subdivided
   * into four children nodes until the values in the region it represents are
   * all the same. Each node has another two boolean attributes : isLeaf & val.
   * isLeaf is true if and only if the node is a leaf node. The val attribute
   * for a leaf node contains the value of the region it represents.
   *
   * For example, below are two quad trees A and B:
   * A:
   * +-------+-------+   T: true
   * |       |       |   F: false
   * |   T   |   T   |
   * |       |       |
   * +-------+-------+
   * |       |       |
   * |   F   |   F   |
   * |       |       |
   * +-------+-------+
   * topLeft: T
   * topRight: T
   * bottomLeft: F
   * bottomRight: F
   *
   * B:
   * +-------+---+---+
   * |       | F | F |
   * |   T   +---+---+
   * |       | T | T |
   * +-------+---+---+
   * |       |       |
   * |   T   |   F   |
   * |       |       |
   * +-------+-------+
   * topLeft: T
   * topRight:
   *      topLeft: F
   *      topRight: F
   *      bottomLeft: T
   *      bottomRight: T
   * bottomLeft: T
   * bottomRight: F
   *
   * Your task is to implement a function that will take two quadtrees and
   * return a quadtree that represents the logical OR (or union) of two trees.
   * A:                 B:                 C (A or B):
   * +-------+-------+  +-------+---+---+  +-------+-------+
   * |       |       |  |       | F | F |  |       |       |
   * |   T   |   T   |  |   T   +---+---+  |   T   |   T   |
   * |       |       |  |       | T | T |  |       |       |
   * +-------+-------+  +-------+---+---+  +-------+-------+
   * |       |       |  |       |       |  |       |       |
   * |   F   |   F   |  |   T   |   F   |  |   T   |   F   |
   * |       |       |  |       |       |  |       |       |
   * +-------+-------+  +-------+-------+  +-------+-------+
   * Note:
   *
   * Both A and B represent grids of size N * N.
   * N is guaranteed to be a power of 2.
   * If you want to know more about the quad tree, you can refer to its wiki.
   * The logic OR operation is defined as this: "A or B" is true if A is true,
   * or if B is true, or if both A and B are true
   */
/*
  class Node {
  public:
    bool val;
    bool isLeaf;
    Node* topLeft;
    Node* topRight;
    Node* bottomLeft;
    Node* bottomRight;

    Node() {}

    Node(bool _val, bool _isLeaf, Node* _topLeft, Node* _topRight, Node* _bottomLeft, Node* _bottomRight) {
      val = _val;
      isLeaf = _isLeaf;
      topLeft = _topLeft;
      topRight = _topRight;
      bottomLeft = _bottomLeft;
      bottomRight = _bottomRight;
    }
  };

  static Node * intersect_recur(Node * root, Node* quadTree1, Node* quadTree2, bool qt1_val, bool qt2_val) {
    if (NULL == quadTree1 && NULL == quadTree2) { return NULL; }

    bool _val = false;
    if (NULL != quadTree1 && quadTree1->isLeaf) { _val = _val || quadTree1->val; } else { _val = _val || qt1_val; }
    if (NULL != quadTree2 && quadTree2->isLeaf) { _val = _val || quadTree2->val; } else { _val = _val || qt2_val; }

    Node * qt1_tl = (quadTree1 == NULL ? NULL : quadTree1->topLeft);
    Node * qt2_tl = (quadTree2 == NULL ? NULL : quadTree2->topLeft);

    Node * qt1_tr = (quadTree1 == NULL ? NULL : quadTree1->topRight);
    Node * qt2_tr = (quadTree2 == NULL ? NULL : quadTree2->topRight);

    Node * qt1_bl = (quadTree1 == NULL ? NULL : quadTree1->bottomLeft);
    Node * qt2_bl = (quadTree2 == NULL ? NULL : quadTree2->bottomLeft);

    Node * qt1_br = (quadTree1 == NULL ? NULL : quadTree1->bottomRight);
    Node * qt2_br = (quadTree2 == NULL ? NULL : quadTree2->bottomRight);

    bool _qt1_val = (quadTree1 == NULL ? false : quadTree1->isLeaf && quadTree1->val);
    bool _qt2_val = (quadTree2 == NULL ? false : quadTree2->isLeaf && quadTree2->val);

    return new Node(_val, true, intersect_recur(qt1_tl, qt2_tl, _qt1_val, _qt2_val),
                                intersect_recur(qt1_tr, qt2_tr, _qt1_val, _qt2_val),
                                intersect_recur(qt1_bl, qt2_bl, _qt1_val, _qt2_val),
                                intersect_recur(qt1_br, qt2_br, _qt1_val, _qt2_val));
  }

  static Node * intersect(Node* quadTree1, Node* quadTree2) {
    return intersect_recur(quadTree1, quadTree2, quadTree1->val, quadTree2->val);
  }
*/

};

int main(void) {
  using tree_util::binary_tree_node;
  using tree_util::print_all_elem;
  using tree_util::bfs_bst_print;
  using tree_util::bfs_bst_print_by_neighbor;
  using tree_util::lvr_bst_print;
  using tree_util::lrv_bst_print;
  using tree_util::vlr_bst_print;
  using tree_util::lvr_bst_print_non_recur;

  using tree_util::bfs_bst_connect;
  using tree_util::bst_in_column_order;
  using tree_util::build_bst_via_pre_and_in;
  using tree_util::build_bst_via_post_and_in;
  using tree_util::inorder_linked_list_from_bst_recur;
  using tree_util::get_lca;
  using tree_util::fast_lca;
  using tree_util::count_diff_bts;
  using tree_util::boundary_traverse_bt;
  using tree_util::is_bst_valid;
  using tree_util::find_largest_sub_bst;

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

  cout << endl << "6. inorder_linked_list_from_bst_recur" << endl;
  binary_tree_node * list_ptr = inorder_linked_list_from_bst(pi_root);
  binary_tree_node * curr_ptr = list_ptr;
  do {
    cout << curr_ptr->prev_ptr->value << ":" << curr_ptr->value << " ";
    curr_ptr = curr_ptr->next_ptr;
  } while (curr_ptr != list_ptr); cout << endl;

  list_ptr = inorder_linked_list_from_bst(&a);
  curr_ptr = list_ptr;
  do {
    cout << curr_ptr->prev_ptr->value << ":" << curr_ptr->value << " ";
    curr_ptr = curr_ptr->next_ptr;
  } while (curr_ptr != list_ptr); cout << endl;

  binary_tree_node aa(6);
  list_ptr = inorder_linked_list_from_bst(&aa);
  curr_ptr = list_ptr;
  do {
    cout << curr_ptr->prev_ptr->value << ":" << curr_ptr->value << " ";
    curr_ptr = curr_ptr->next_ptr;
  } while (curr_ptr != list_ptr); cout << endl;

  cout << endl << "7. get_lca" << endl;
  cout << "6 <=> " << get_lca(&a, &k, &i)->value << endl;
  cout << "1 <=> " << get_lca(&a, &d, &k)->value << endl;
  cout << "1 <=> " << get_lca(&a, &k, &d)->value << endl;
  cout << "1 <=> " << get_lca(&a, &d, &d)->value << endl;
  cout << "6 <=> " << get_lca(&a, &a, &a)->value << endl;
  cout << "4 <=> " << get_lca(&a, &k, &e)->value << endl;
  cout << "4 <=> " << get_lca(&a, &e, &k)->value << endl;
  cout << "8 <=> " << get_lca(&a, &f, &i)->value << endl;
  cout << "8 <=> " << get_lca(&a, &i, &f)->value << endl;
  cout << "0 <=> " << get_lca(&a, &i, NULL) << endl;
  cout << "0 <=> " << get_lca(&a, NULL, NULL) << endl;
  cout << "0 <=> " << get_lca(NULL, NULL, NULL) << endl;

  cout << endl << "8. fast_lca" << endl;
  cout << "6 <=> " << fast_lca(&a, &k, &i)->value << endl;
  cout << "1 <=> " << fast_lca(&a, &d, &k)->value << endl;
  cout << "1 <=> " << fast_lca(&a, &k, &d)->value << endl;
  cout << "1 <=> " << fast_lca(&a, &d, &d)->value << endl;
  cout << "6 <=> " << fast_lca(&a, &a, &a)->value << endl;
  cout << "4 <=> " << fast_lca(&a, &k, &e)->value << endl;
  cout << "4 <=> " << fast_lca(&a, &e, &k)->value << endl;
  cout << "8 <=> " << fast_lca(&a, &f, &i)->value << endl;
  cout << "8 <=> " << fast_lca(&a, &i, &f)->value << endl;
  cout << "0 <=> " << fast_lca(&a, &i, NULL) << endl;
  cout << "0 <=> " << fast_lca(&a, NULL, NULL) << endl;
  cout << "0 <=> " << fast_lca(NULL, NULL, NULL) << endl;

  cout << endl << "9. lvr_bst_print_non_recur" << endl;
  cout << "in-order: 1 2 3 4 5 6 7 8 9 10 11" << endl;
  cout << "in-order: "; lvr_bst_print_non_recur(&a); cout << endl;

  cout << endl << "10. count_diff_bts" << endl;
  cout << "0 <=> " << count_diff_bts(vector<int>({ })) << endl;
  cout << "3 <=> " << count_diff_bts(vector<int>({ 2, 4 })) << endl;
  cout << "7 <=> " << count_diff_bts(vector<int>({ 2, 4, 5, 10 })) << endl;

  cout << endl << "11. boundary_traverse_bt" << endl;
  cout << "[ 6 4 1 2 3 5 7 9 11 10 8 ] <=> " << endl;
  print_all_elem<int>(boundary_traverse_bt(&a));
  // a.left_ptr = &b;  a.right_ptr = &c;
  a.left_ptr = NULL; a.right_ptr = &c;
  cout << "[ 6 7 9 11 10 8 ] <=> " << endl;
  print_all_elem<int>(boundary_traverse_bt(&a));
  a.left_ptr = &b; a.right_ptr = NULL;
  cout << "[ 6 4 1 2 3 5 ] <=> " << endl;
  print_all_elem<int>(boundary_traverse_bt(&a));
  a.left_ptr = NULL; a.right_ptr = NULL;
  cout << "[ 6 ] <=> " << endl;
  print_all_elem<int>(boundary_traverse_bt(&a));
  cout << "[ ] <=> " << endl;
  print_all_elem<int>(boundary_traverse_bt(NULL));
  a.left_ptr = &b;  a.right_ptr = &c;

  cout << endl << "12. is_bst_valid" << endl;
  cout << "1 <=> " << is_bst_valid(&a) << endl;
  c.value = 10;
  cout << "0 <=> " << is_bst_valid(&a) << endl;
  c.value = 8;

  cout << endl << "13. find_largest_sub_bst" << endl;
  cout << "11 <=> " << find_largest_sub_bst(&a) << endl;
  c.value = 10;
  cout << "5 <=> " << find_largest_sub_bst(&a) << endl;
  c.value = 8;
  b.value = 10;
  cout << "5 <=> " << find_largest_sub_bst(&a) << endl;
  b.value = 4;
  b.value = 10; c.value = 11;
  cout << "3 <=> " << find_largest_sub_bst(&a) << endl;
  b.value = 4; c.value = 8;

  return 0;
}
