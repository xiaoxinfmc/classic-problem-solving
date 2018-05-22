/**
 * Objective: Given a Binary tree (Not binary Search Tree ), find the inorder
 * successor of a node.
 *        a
 *      /   \
 *     b     c
 *    / \   / \
 *   d   e f   g
 *    \       / \
 *     t     i    h
 *      \
 *       k
 *
 * d, t, k, b, e, a, f, c, i, g, h
 * inorderSuccessor('a', 'e') => 'a'
 * inorderSuccessor('a', 't') => 'b'
 * inorderSuccessor('a', 'k') => 'b'
 *
 * zig_zag
 * d -> (b -> t) -> (a -> e -> f -> k) -> (c -> i) -> g -> h
 *
 * connect:
 * a -> b -> c -> d -> e -> f -> g -> t -> i -> h -> k
 */

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <cassert>
#include <unordered_map>

using namespace std;

class BinaryTreeUtil {
public:
  class BinaryTreeNode {
  public:
    BinaryTreeNode(int x) {
      value = x; visited = false;
      left = NULL; right = NULL;
      neighbor_to_right = NULL;

      column_id = 0; level_id = 0;
      is_right_child = false;
    }

    friend bool operator< (const BinaryTreeNode & l_node_to_cmp,
                           const BinaryTreeNode & r_node_to_cmp) {
      if (l_node_to_cmp.column_id == r_node_to_cmp.column_id) {
        if (l_node_to_cmp.level_id == r_node_to_cmp.level_id) {
          return (true == l_node_to_cmp.is_right_child);
        } else {
          return (l_node_to_cmp.level_id > r_node_to_cmp.level_id);
        }
      } else {
        return (l_node_to_cmp.column_id < r_node_to_cmp.column_id);
      }
    }

    int value; bool visited;
    BinaryTreeNode * left;
    BinaryTreeNode * right;
    BinaryTreeNode * neighbor_to_right;
    int column_id; int level_id; bool is_right_child;
  };

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
   * zig_zag
   * 1 -> (4 -> 2) -> (6 -> 5 -> 7 -> 3) -> (8 -> 9) -> 10 -> 11
   */
  static void print_bst_in_column_order(BinaryTreeNode * root_ptr) {
    vector<BinaryTreeNode> tree_node_vec;
    flag_coordinates_for_tree_nodes(root_ptr, 0, 0, false, tree_node_vec);
    sort(tree_node_vec.begin(), tree_node_vec.end());
    for (auto & itr : tree_node_vec) {
      cout << itr.value << " ";
    }
    cout << endl;
  }

  static void flag_coordinates_for_tree_nodes(BinaryTreeNode * root_ptr,
                                              int column_id, int level_id,
                                              bool is_right_child,
                                              vector<BinaryTreeNode> & tree_node_vec){
    if (NULL == root_ptr) { return; }
    flag_coordinates_for_tree_nodes(
      root_ptr->left, column_id - 1, level_id - 1, false, tree_node_vec
    );
    flag_coordinates_for_tree_nodes(
      root_ptr->right, column_id + 1, level_id - 1, true, tree_node_vec
    );
    root_ptr->column_id = column_id;
    root_ptr->level_id = level_id;
    root_ptr->is_right_child = is_right_child;
    tree_node_vec.push_back(* root_ptr);
  }

  static BinaryTreeNode * get_inorder_successor(BinaryTreeNode * root,
                                                BinaryTreeNode * target) {
    int inorder_succ_flag = -1;
    BinaryTreeNode * inorder_succ_ptr = _get_inorder_successor(
      root, target, inorder_succ_flag
    );
    if (NULL == inorder_succ_ptr) { inorder_succ_ptr = target; }
    return inorder_succ_ptr;
  }

  static BinaryTreeNode * _get_inorder_successor(BinaryTreeNode * root,
                                                 BinaryTreeNode * target,
                                                 int & inorder_succ_flag) {
    BinaryTreeNode * inorder_succ_ptr = NULL;

    if (NULL == root || NULL == target) { return inorder_succ_ptr; }

    inorder_succ_ptr = _get_inorder_successor(root->left, target, inorder_succ_flag);
    if (NULL != inorder_succ_ptr) { return inorder_succ_ptr; }

    if (0 == inorder_succ_flag) { inorder_succ_ptr = root; inorder_succ_flag++; }
    if (root->value == target->value) { inorder_succ_flag++; }

    if (NULL != inorder_succ_ptr) { return inorder_succ_ptr; }
    inorder_succ_ptr = _get_inorder_successor(root->right, target, inorder_succ_flag);

    return inorder_succ_ptr;
  }

  static void bfs_bst_connect(BinaryTreeNode * root_ptr) {
    if (NULL == root_ptr) { return; }

    deque<BinaryTreeNode *> node_ptr_buffer;
    node_ptr_buffer.push_back(root_ptr);
    BinaryTreeNode * curr_ptr = NULL, * prev_ptr = NULL;

    while (false == node_ptr_buffer.empty()) {
      curr_ptr = node_ptr_buffer.front();
      if (NULL != prev_ptr) { prev_ptr->neighbor_to_right = curr_ptr; }
      if (NULL != curr_ptr->left) { node_ptr_buffer.push_back(curr_ptr->left); }
      if (NULL != curr_ptr->right) { node_ptr_buffer.push_back(curr_ptr->right); }
      prev_ptr = curr_ptr;
      node_ptr_buffer.pop_front();
    }
  }

  static void bfs_bst_print(BinaryTreeNode * root_ptr) {
    if (NULL == root_ptr) { return; }
    deque<BinaryTreeNode *> node_ptr_buffer;
    node_ptr_buffer.push_back(root_ptr);
    while(false == node_ptr_buffer.empty()) {
      root_ptr = node_ptr_buffer.front();
      cout << root_ptr->value << " ";
      if (NULL != root_ptr->left) { node_ptr_buffer.push_back(root_ptr->left); }
      if (NULL != root_ptr->right) { node_ptr_buffer.push_back(root_ptr->right); }
      node_ptr_buffer.pop_front();
    }; cout << endl;
  }

  static void vlr_bst_print(BinaryTreeNode * root_ptr) {
    if (NULL == root_ptr) { return; }
    cout << root_ptr->value << " ";
    vlr_bst_print(root_ptr->left);
    vlr_bst_print(root_ptr->right);
  }

  static void lrv_bst_print(BinaryTreeNode * root_ptr) {
    if (NULL == root_ptr) { return; }
    lrv_bst_print(root_ptr->left);
    lrv_bst_print(root_ptr->right);
    cout << root_ptr->value << " ";
  }

  static void lvr_bst_print(BinaryTreeNode * root_ptr) {
    if (NULL == root_ptr) { return; }
    lvr_bst_print(root_ptr->left);
    cout << root_ptr->value << " ";
    lvr_bst_print(root_ptr->right);
  }

  static void bfs_bst_print_by_neighbor(BinaryTreeUtil::BinaryTreeNode * root_ptr) {
    for (BinaryTreeUtil::BinaryTreeNode * tmp = root_ptr; NULL != tmp;
                                        tmp = tmp->neighbor_to_right) {
      cout << tmp->value << " ";
    };cout << endl;
  }

  static BinaryTreeNode * inorder_successor(BinaryTreeNode * root_ptr,
                                            BinaryTreeNode * predecessor_ptr) {
    BinaryTreeNode * successor_ptr = NULL;
    if (NULL == root_ptr || NULL == predecessor_ptr) { return successor_ptr; }
    if (root_ptr == predecessor_ptr) {
      // if input root of current subtree is the predecessor, check successor.
      if (NULL != root_ptr->right) {
        // hit target if it has a right child.
        for (root_ptr = root_ptr->right; NULL != root_ptr->left;
             root_ptr = root_ptr->left) {}
        successor_ptr = root_ptr;
      } else {
        // if no right child found, then successor must be in upper level, we
        // assign successor_ptr to be predecessor_ptr, showing we found a node.
        successor_ptr = predecessor_ptr;
      }
    } else {
      // if it is not the predecessor, then keep traversing to find one.
      successor_ptr = inorder_successor(root_ptr->left, predecessor_ptr);
      if (NULL == successor_ptr) {
        successor_ptr = inorder_successor(root_ptr->right, predecessor_ptr);
      } else {
        // enter here means we find either predecessor/successor on left subtree
        if (successor_ptr == predecessor_ptr) {
          // no immediate successor found, but we got the predecessor.
          successor_ptr = root_ptr;
        } else {
          // here means successor_ptr is the one we want, do nothing.
        }
      }
    }
    return successor_ptr;
  }

  /*
   *                  4
   *                /   \
   *               3     6
   *              / \   / \
   *             1   2 5   7
   *                 { pivot | left-subtree | right-subtree }
   * int[] peorder = { 4, 3, 1, 2, 6, 5, 7 }
   *                 { left-subtree | pivot | right-subtree }
   * int[] inorder = { 1, 2, 3, 4, 5, 6, 7 }
   * build_bst_via_pre_in(n) = n + build_bst_via_pre_in(n/2) +
   *                               build_bst_via_pre_in(n/2)
   * - assume all values are uniq & input orders are valid.
   * - find pivot & left-subt & right-subt took O(n)
   * - T(n) -> nlogn
   */
  static BinaryTreeNode * build_bst_via_pre_and_in(const vector<int> & preorder,
                                                   int pre_min, int pre_max,
                                                   const vector<int> & inorder,
                                                   int in_min, int in_max){
    BinaryTreeNode * root_ptr = NULL;
    if (false == is_input_valid(preorder, pre_min, pre_max,
                                inorder, in_min, in_max)) {
      return root_ptr;
    }

    root_ptr = new BinaryTreeNode(preorder[pre_min]); assert(NULL != root_ptr);
    if (in_max == in_min) { return root_ptr; }

    int inorder_pivot_idx = in_min; int preorder_pivot_idx = pre_min;
    while (preorder[pre_min] != inorder[inorder_pivot_idx]) {
      inorder_pivot_idx++; preorder_pivot_idx++;
    }
    root_ptr->left  = build_bst_via_pre_and_in(preorder, pre_min + 1, preorder_pivot_idx,
                                               inorder, in_min, inorder_pivot_idx - 1);
    root_ptr->right = build_bst_via_pre_and_in(preorder, preorder_pivot_idx + 1, pre_max,
                                               inorder, inorder_pivot_idx + 1, in_max);
    return root_ptr;
  }

  static bool is_input_valid(const vector<int> & preorder, int pre_min, int pre_max,
                             const vector<int> & inorder, int in_min, int in_max) {
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
   *                 { left-subtree | right-subtree | pivot }
   * int[] ptorder = { 1, 2, 3, 5, 7, 6, 4 }
   *                 { left-subtree | pivot | right-subtree }
   * int[] inorder = { 1, 2, 3, 4, 5, 6, 7 }
   */
  static BinaryTreeNode * build_bst_via_post_and_in(const vector<int> & postorder,
                                                    int post_min, int post_max,
                                                    const vector<int> & inorder,
                                                    int in_min, int in_max){
    BinaryTreeNode * root_ptr = NULL;
    if (false == is_input_valid(postorder, post_min, post_max,
                                inorder, in_min, in_max)) {
      return root_ptr;
    }

    root_ptr = new BinaryTreeNode(postorder[post_max]); assert(NULL != root_ptr);
    if (in_max == in_min) { return root_ptr; }

    int inorder_pivot_idx = in_max; int postorder_pivot_idx = post_max;
    while (postorder[post_max] != inorder[inorder_pivot_idx]) {
      inorder_pivot_idx--; postorder_pivot_idx--;
    }

    root_ptr->left  = build_bst_via_post_and_in(postorder, post_min, postorder_pivot_idx - 1,
                                                inorder, in_min, inorder_pivot_idx - 1);
    root_ptr->right = build_bst_via_post_and_in(postorder, postorder_pivot_idx, post_max - 1,
                                                inorder, inorder_pivot_idx + 1, in_max);
    return root_ptr;
  }
};

int main(void) {
/*
  BinaryTreeUtil::BinaryTreeNode a(int('a')); BinaryTreeUtil::BinaryTreeNode b(int('b'));
  BinaryTreeUtil::BinaryTreeNode c(int('c')); BinaryTreeUtil::BinaryTreeNode d(int('d'));
  BinaryTreeUtil::BinaryTreeNode e(int('e')); BinaryTreeUtil::BinaryTreeNode f(int('f'));
  BinaryTreeUtil::BinaryTreeNode g(int('g')); BinaryTreeUtil::BinaryTreeNode h(int('h'));
  BinaryTreeUtil::BinaryTreeNode i(int('i')); BinaryTreeUtil::BinaryTreeNode t(int('t'));
  BinaryTreeUtil::BinaryTreeNode k(int('k'));
*/
  BinaryTreeUtil::BinaryTreeNode a(6);  BinaryTreeUtil::BinaryTreeNode b(4);
  BinaryTreeUtil::BinaryTreeNode c(8);  BinaryTreeUtil::BinaryTreeNode d(1);
  BinaryTreeUtil::BinaryTreeNode e(5);  BinaryTreeUtil::BinaryTreeNode f(7);
  BinaryTreeUtil::BinaryTreeNode g(10); BinaryTreeUtil::BinaryTreeNode h(11);
  BinaryTreeUtil::BinaryTreeNode i(9);  BinaryTreeUtil::BinaryTreeNode t(2);
  BinaryTreeUtil::BinaryTreeNode k(3);

  a.left = &b; a.right = &c; b.left = &d; b.right = &e; d.right = &t;
  t.right = &k; c.left = &f; c.right = &g; g.left = &i; g.right = &h;
/*
  cout << char(BinaryTreeUtil::inorder_successor(&a, &k)->value) << endl;
  cout << char(BinaryTreeUtil::inorder_successor(&a, &b)->value) << endl;
  cout << char(BinaryTreeUtil::inorder_successor(&a, &c)->value) << endl;
  cout << char(BinaryTreeUtil::inorder_successor(&a, &i)->value) << endl;
  cout << char(BinaryTreeUtil::inorder_successor(&a, &h)->value) << endl;

  cout << int(BinaryTreeUtil::inorder_successor(&a, &k)->value) << endl;
  cout << int(BinaryTreeUtil::inorder_successor(&a, &b)->value) << endl;
  cout << int(BinaryTreeUtil::inorder_successor(&a, &c)->value) << endl;
  cout << int(BinaryTreeUtil::inorder_successor(&a, &i)->value) << endl;
  cout << int(BinaryTreeUtil::inorder_successor(&a, &h)->value) << endl;
*/

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
 * zig_zag
 * 1 -> (4 -> 2) -> (6 -> 5 -> 7 -> 3) -> (8 -> 9) -> 10 -> 11
 */
  cout << int(BinaryTreeUtil::get_inorder_successor(&a, &k)->value) << endl;
  cout << int(BinaryTreeUtil::get_inorder_successor(&a, &b)->value) << endl;
  cout << int(BinaryTreeUtil::get_inorder_successor(&a, &c)->value) << endl;
  cout << int(BinaryTreeUtil::get_inorder_successor(&a, &i)->value) << endl;
  cout << int(BinaryTreeUtil::get_inorder_successor(&a, &h)->value) << endl;

  BinaryTreeUtil::bfs_bst_print(&a);
  BinaryTreeUtil::bfs_bst_connect(&a);
  BinaryTreeUtil::bfs_bst_print_by_neighbor(&a);

  int preorder_arr[]  = { 6, 4, 1, 2, 3, 5, 8, 7, 10, 9, 11 };
  int postorder_arr[] = { 3, 2, 1, 5, 4, 7, 9, 11, 10, 8, 6 };
  int inorder_arr[]   = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

  vector<int> inorder(inorder_arr, inorder_arr + sizeof(inorder_arr) / sizeof(inorder_arr[0]));
  vector<int> preorder(preorder_arr, preorder_arr + sizeof(preorder_arr) / sizeof(preorder_arr[0]));
  vector<int> postorder(postorder_arr, postorder_arr + sizeof(postorder_arr) / sizeof(postorder_arr[0]));


  cout << "==>> BinaryTreeUtil::build_bst_via_pre_and_in" << endl;
  cout << "in-order: 1 2 3 4 5 6 7 8 9 10 11" << endl
       << "pe-order: 6 4 1 2 3 5 8 7 10 9 11" << endl;
  cout << "in-order: "; BinaryTreeUtil::lvr_bst_print(&a); cout << endl;
  cout << "pe-order: "; BinaryTreeUtil::vlr_bst_print(&a); cout << endl;
  BinaryTreeUtil::BinaryTreeNode * ei_root = BinaryTreeUtil::build_bst_via_pre_and_in(
    preorder, 0, preorder.size() - 1, inorder, 0, inorder.size() - 1
  );
  cout << "in-order: "; BinaryTreeUtil::lvr_bst_print(ei_root); cout << endl;
  cout << "pe-order: "; BinaryTreeUtil::vlr_bst_print(ei_root); cout << endl;

  cout << "==>> BinaryTreeUtil::build_bst_via_post_and_in" << endl;
  cout << "in-order: 1 2 3 4 5 6 7 8 9 10 11" << endl
       << "pt-order: 3 2 1 5 4 7 9 11 10 8 6" << endl;
  cout << "in-order: "; BinaryTreeUtil::lvr_bst_print(&a); cout << endl;
  cout << "pt-order: "; BinaryTreeUtil::lrv_bst_print(&a); cout << endl;
  BinaryTreeUtil::BinaryTreeNode * pi_root = BinaryTreeUtil::build_bst_via_post_and_in(
    postorder, 0, postorder.size() - 1, inorder, 0, inorder.size() - 1
  );
  cout << "in-order: "; BinaryTreeUtil::lvr_bst_print(pi_root); cout << endl;
  cout << "pt-order: "; BinaryTreeUtil::lrv_bst_print(pi_root); cout << endl;

  BinaryTreeUtil::print_bst_in_column_order(&a);

  return 0;
}
