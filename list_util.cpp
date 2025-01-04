/**
 * Given a node from a cyclic linked list which has been sorted, write a
 * function to insert a value into the list such that it remains a cyclic
 * sorted list. The given node can be any single node in the list.
 */
#include <iostream>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

using namespace std;

/**
 * 1 -> 3 -> 3 -> 7
 * ^--------------|
 * 3 (1 < 3 < 7)
 *
 * - 1 -> 3 -> 3 -> (3) -> 7
 *   ^---------x-----------|
 *   case 1: the value is in the middle of the list, then loop until find
 *           a node whose next val is larger than 3.
 * - 2 -> 3 -> 3 -> (3) -> 1 2 2
 *   ^---------x---------------|
 *   case 2: the value is the max of the list, then loop until find a node
 *           whose val is not larger than 3 but next val starts to decr.
 * - 3 -> 3 -> 3 -> (0) -> 3 3 3
 *   ^---------x---------------|
 *   case 3: all existing values are same, loop until it back to same point
 * - (0) -> 1 -> 3 -> 3 -> 7
 *    ^--------------------x
 *   case 4: the value is the min of the list, then loop until find a node
 *           whose val is larger then next value and its next value > curr_val
 * - when list has only 1 elem, then it points to itself.
 */
namespace list_util {

  template <class Type>
  static void print_all_elem(const vector<Type> input) {
    cout << "[ ";
    for (auto arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }

  template <class Type>
  static void check_all_elem_same(const vector<Type> & l, const vector<Type> & r) {
    print_all_elem<Type>(l);
    print_all_elem<Type>(r);
    assert(l.size() == r.size());
    for (int i = 0; i < l.size(); i++) {
      assert(l[i] == r[i]);
    }
  }

  class csl_node {
  public:
    void insert_after(csl_node * next_csl_node) {
      assert(NULL != next_csl_node);
      cout << "==>> insert " << next_csl_node->_val << " after " << _val << endl;
      next_csl_node->_next_ptr = _next_ptr;
      _next_ptr = next_csl_node;
    }

    vector<int> traverse_all_vals() {
      vector<int> all_vals;
      csl_node * curr_ptr = this, * next_ptr = _next_ptr;
      do {
        next_ptr = curr_ptr->_next_ptr;
        all_vals.push_back(curr_ptr->_val);
        curr_ptr = next_ptr;
      } while (curr_ptr != this && NULL != curr_ptr);
      return all_vals;
    }

    csl_node(int val) : _val(val), _next_ptr(this) {}
    virtual ~csl_node(){ cout << "==>> node : " << _val << " removed" << endl;}

    // upon single node list, _next_ptr points to itself.
    csl_node * _next_ptr;
    int _val;
  };

  static csl_node * csl_initialize_list(const vector<int> & vals) {
    if (true == vals.empty()) {
      return NULL;
    }

    vector<int>::const_iterator curr_itr = vals.begin();
    csl_node *  head_ptr = new csl_node(*curr_itr);
    csl_node *  curr_ptr = head_ptr;

    for (curr_itr++; curr_itr != vals.end(); curr_itr++) {
      curr_ptr->insert_after(new csl_node(*curr_itr));
      curr_ptr = curr_ptr->_next_ptr;
    }

    return head_ptr;
  }

  static void csl_delete_list(csl_node * head_ptr) {
    if (NULL == head_ptr) { return; }

    csl_node * curr_ptr = head_ptr,
             * next_ptr = head_ptr->_next_ptr;
    do {
      cout << ">>>> to rm: " << curr_ptr << endl;
      next_ptr = curr_ptr->_next_ptr;
      delete curr_ptr;
      curr_ptr = next_ptr;
    } while (curr_ptr != head_ptr && NULL != curr_ptr);
  }

  /**
   * 23. Merge k Sorted Lists
   * Merge k sorted linked lists and return it as one sorted list. Analyze
   * and describe its complexity.
   * Example:
   * Input:
   * [ 1->4->5, 1->3->4, 2->6 ]
   * Output: 1->1->2->3->4->4->5->6
   * Example 2:
   * Input: lists = []
   * Output: []
   * Example 3:
   * Input: lists = [[]]
   * Output: []
   */
  static bool greater_than(pair<csl_node *, int> l_csl_ptr, pair<csl_node *, int> r_csl_ptr) {
    return l_csl_ptr.first->_val > r_csl_ptr.first->_val;
  }

  static csl_node * merge_sorted_lists(const vector<csl_node *> & lists) {
    csl_node * head_ptr = NULL;
    if (true == lists.empty() || lists.front() == NULL) { return head_ptr; }

    // each heap elem is a pair of csl_node * & index -> its current list
    vector<pair<csl_node *, int>> min_heap;
    for (int i = 0; i < lists.size(); i++) {
      min_heap.push_back({lists[i], i});
    }
    make_heap(min_heap.begin(), min_heap.end(), greater_than);

    csl_node * prev_node_ptr = NULL, * curr_node_ptr = NULL;

    do {
      pair<csl_node *, int> ptr_index_pair = min_heap.front();
      curr_node_ptr = new csl_node(ptr_index_pair.first->_val);
      if (NULL == head_ptr) {
        head_ptr = curr_node_ptr;
        prev_node_ptr = curr_node_ptr;
      } else {
        prev_node_ptr->insert_after(curr_node_ptr);
        prev_node_ptr = curr_node_ptr;
      }
      pop_heap(min_heap.begin(), min_heap.end(), greater_than);
      min_heap.pop_back();

      if (ptr_index_pair.first->_next_ptr != lists[ptr_index_pair.second]) {
        min_heap.push_back({ptr_index_pair.first->_next_ptr, ptr_index_pair.second});
        push_heap(min_heap.begin(), min_heap.end(), greater_than);
      }
    } while (!min_heap.empty());

    return head_ptr;
  }

  static void test_merge_sorted_lists() {
    cout << "==>> test_merge_sorted_lists" << endl;
    vector<vector<vector<int>>> test_input = {
      {{1, 4, 5}, {1, 3, 4}, {2, 6}},
      {{1}},
      {{1, 2}},
    };
    vector<vector<int>> exp_output {
      {1, 1, 2, 3, 4, 4, 5, 6},
      {1},
      {1, 2},
    };
    for (int i = 0; i < test_input.size(); i++) {
      vector<csl_node *> lists;
      for (auto & sublist : test_input[i]) {
        csl_node * head_ptr = csl_initialize_list(sublist);
        lists.push_back(head_ptr);
      }
      for (int j = 0; j < test_input[i].size(); j++) {
        check_all_elem_same(test_input[i][j], lists[j]->traverse_all_vals());
      }
      csl_node * merged_list = merge_sorted_lists(lists);
      check_all_elem_same(exp_output[i], merged_list->traverse_all_vals());
      for (auto & sublist : lists) {
        csl_delete_list(sublist);
      }
      csl_delete_list(merged_list);
    }
    cout << "<<== test_merge_sorted_lists" << endl;
  }

  /**
   * Given a Circular Linked List node, which is sorted in non-descending order,
   * write a function to insert a value insertVal into the list such that it
   * remains a sorted circular list. The given node can be a reference to any
   * single node in the list and may not necessarily be the smallest value in
   * the circular list.
   *
   * If there are multiple suitable places for insertion, you may choose any
   * place to insert the new value. After the insertion, the circular list
   * should remain sorted.
   *
   * If the list is empty (i.e., the given node is null), you should create a
   * new single circular list and return the reference to that single node.
   * Otherwise, you should return the originally given node.
   *
   * Input: head = [3,4,1], insertVal = 2
   * Output: [3,4,1,2]
   * Explanation: In the figure above, there is a sorted circular list of three
   * elements. You are given a reference to the node with value 3, and we need
   * to insert 2 into the list. The new node should be inserted between node 1
   * and node 3. After the insertion, the list should look like this, and we
   * should still return node 3.
   *
   * Input: head = [], insertVal = 1
   * Output: [1]
   * Explanation: The list is empty (given head is null). We create a new
   * single circular list and return the reference to that single node.
   *
   * Input: head = [1], insertVal = 0
   * Output: [1,0]
   * The number of nodes in the list is in the range [0, 5 * 104].
   * -10^6 <= Node.val, insertVal <= 10^6
   *
   * 5, 6, 9, 1, 2, 3, 4  <- 7
   * ^----|^ insert-before (mono-up, 5 ~ 9)
   *       ^-----------+
   * +-----------------+
   * +----|^ insert-before (down -> mono-up, 9~9)
   * mono-up util 1st larger or down (insert before)
   * - in-btw (7 -> 6, 9)
   * - max (11 -> 9, 1)
   * - min (0 -> 9, 1)
   * - min/max are the same situation, 1st decrease -> insert before
   * - in-btw are common, 1st mono up & within range -> insert before
   *
   * 5, 6, 9, 1, 2, 3, 4  <- 11
   *
   * 5, 6, 9, 1, 2, 3, 4  <- -1
   */
  static bool is_ready_to_insert_in_btw(csl_node * curr_node_ptr,
                                        csl_node * next_node_ptr,
                                        csl_node * to_insert_ptr) {
    cout << "==>> " << curr_node_ptr->_val << " " << next_node_ptr->_val << " " << to_insert_ptr->_val << endl;
    return (
      // in-btw (7 -> 6, 9)
      (curr_node_ptr->_val <= to_insert_ptr->_val && to_insert_ptr->_val <= next_node_ptr->_val) ||
      // max (11 -> 9, 1)
      (curr_node_ptr->_val <= to_insert_ptr->_val && curr_node_ptr->_val >= next_node_ptr->_val) ||
      // min (0 -> 9, 1)
      (next_node_ptr->_val >= to_insert_ptr->_val && curr_node_ptr->_val >= next_node_ptr->_val)
    );
  }

  static csl_node * insert_to_circular_linked_list(csl_node * list_ptr, int val) {
    // empty list then return new head
    if (NULL == list_ptr) {
      return new csl_node(val);
    }

    csl_node * curr_node_ptr = list_ptr,
             * next_node_ptr = list_ptr->_next_ptr,
             * to_insert_ptr = new csl_node(val);

    // single node list
    if (curr_node_ptr == next_node_ptr) {
       cout << ">>>> " << curr_node_ptr->_val << " " << next_node_ptr->_val << " " << to_insert_ptr->_val << endl;
       curr_node_ptr->insert_after(to_insert_ptr);
       return curr_node_ptr;
    }

    do {
      if (true == is_ready_to_insert_in_btw(curr_node_ptr, next_node_ptr, to_insert_ptr)) {
        curr_node_ptr->insert_after(to_insert_ptr);
        break;
      }
      curr_node_ptr = next_node_ptr;
      next_node_ptr = curr_node_ptr->_next_ptr;
    } while (list_ptr != curr_node_ptr);

    return list_ptr;
  }

  static void test_insert_to_circular_linked_list() {
    cout << "==>> test_insert_to_circular_linked_list" << endl;
    vector<pair<pair<vector<int>, int>, vector<int>>> test_cases = {
      {{{3, 4, 1}, 2}, {3, 4, 1, 2}},
      {{{}, 1}, {1}},
      {{{1}, 0}, {1, 0}},
      {{{5, 6, 9, 1, 2, 3, 4}, 7}, {5, 6, 7, 9, 1, 2, 3, 4}},
      {{{5, 6, 9, 1, 2, 3, 4}, 10}, {5, 6, 9, 10, 1, 2, 3, 4}},
      {{{5, 6, 9, 1, 2, 3, 4}, 0}, {5, 6, 9, 0, 1, 2, 3, 4}},
    };
    for (auto & test_case : test_cases) {
      csl_node * head_ptr = csl_initialize_list(test_case.first.first);
      vector<int> raw_elems = head_ptr == NULL ? vector<int>() : head_ptr->traverse_all_vals();
      check_all_elem_same(raw_elems, test_case.first.first);

      csl_node * curr_ptr = insert_to_circular_linked_list(head_ptr, test_case.first.second);
      vector<int> exp_elems = test_case.second;
      vector<int> out_elems = curr_ptr->traverse_all_vals();
      check_all_elem_same(exp_elems, out_elems);

      csl_delete_list(head_ptr);
    }
    cout << "<<== test_insert_to_circular_linked_list" << endl;
  }

  class circular_sorted_list_node {
  public:
    circular_sorted_list_node(int val) : value(val), next_ptr(this) {}
    virtual ~circular_sorted_list_node() {}
    circular_sorted_list_node * next_ptr;
    int value;
  };

  static void print_circular_sorted_list(circular_sorted_list_node * list_head_ptr) {
    if (NULL == list_head_ptr) { return; }
    circular_sorted_list_node * curr_node_ptr = list_head_ptr;
    do {
      cout << curr_node_ptr->value << " ";
      curr_node_ptr = curr_node_ptr->next_ptr;
    } while (curr_node_ptr != list_head_ptr); cout << endl;
  }

  static circular_sorted_list_node * insert_circular_sorted_list(
    circular_sorted_list_node * head_ptr, int new_value) {
    circular_sorted_list_node * curr_node_ptr = new circular_sorted_list_node(new_value),
                              * temp_node_ptr = head_ptr;
    assert(NULL != curr_node_ptr);
    if (NULL == head_ptr) { return curr_node_ptr; }
    // case 3 if we reach the end of the loop.
    do {
      if ((temp_node_ptr->value <= new_value &&          // case 1 mid
           temp_node_ptr->next_ptr->value > new_value)            ||
          (temp_node_ptr->value <= new_value &&          // case 2 max
           temp_node_ptr->next_ptr->value < temp_node_ptr->value) ||
          (new_value < temp_node_ptr->next_ptr->value && // case 4 min
           temp_node_ptr->next_ptr->value < temp_node_ptr->value)) { break; }
      temp_node_ptr = temp_node_ptr->next_ptr;
    } while (temp_node_ptr != head_ptr);
    curr_node_ptr->next_ptr = temp_node_ptr->next_ptr;
    temp_node_ptr->next_ptr = curr_node_ptr;
    return curr_node_ptr;
  }

  static void test_insert_circular_sorted_list() {
    cout << "1. insert_circular_sorted_list:" << endl;
    cout << "-1 -1 0 1 1 2 3 5 5" << endl;
    vector<int> elem_value_arr({ -1, -1, 1, 1, 2, 3, 5, 5 });
    circular_sorted_list_node * list_head_ptr = new circular_sorted_list_node(0);
    for (auto & elem_value : elem_value_arr) {
      list_head_ptr = insert_circular_sorted_list(list_head_ptr, elem_value);
      print_circular_sorted_list(list_head_ptr);
    }
    print_circular_sorted_list(list_head_ptr);
  }

  /**
   * Intuition:
   * - for k lists avg size of n, -> lower bound would be O(kn)
   * - all lists are sorted, each time we need to pick the min ptr from k
   *   lists to append and move foward(until certain list is empty).
   * - pick min from ordered k list head took O(logk)
   * - overall ~ O(knlogk)
   * - tool to use, min heap of size k?
   */
  class list_node {
  public:
    list_node(int v) : val(v), next(NULL), random(NULL) {}
    virtual ~list_node() {}
    int val;
    list_node * next, * random;
  };

  static bool list_ptr_compare_for_min_heap(list_node * l_ptr, list_node * r_ptr) {
    return (r_ptr->val < l_ptr->val);
  }

  static list_node * merge_k_lists(vector<list_node *> & lists) {
    list_node * new_root = NULL, * curr_ptr = NULL, * prev_ptr = NULL;
    if (lists.empty()) { return new_root; }

    /* 1. init curr. heap */
    vector<list_node *> k_list_min_heap;
    for (auto & curr_ptr : lists) {
      if (NULL != curr_ptr) { k_list_min_heap.push_back(curr_ptr); }
    }
    make_heap(k_list_min_heap.begin(), k_list_min_heap.end(), list_ptr_compare_for_min_heap);

    /* 2. start to merge until all elem combined */
    while (!k_list_min_heap.empty()) {
      curr_ptr = k_list_min_heap.front();
      if (NULL == new_root){ new_root = curr_ptr; }
      if (NULL != prev_ptr) { prev_ptr->next = curr_ptr; }
      pop_heap(k_list_min_heap.begin(), k_list_min_heap.end(), list_ptr_compare_for_min_heap);
      k_list_min_heap.pop_back();
      if (NULL != curr_ptr->next) {
        k_list_min_heap.push_back(curr_ptr->next);
        push_heap(k_list_min_heap.begin(), k_list_min_heap.end(), list_ptr_compare_for_min_heap);
      }
      prev_ptr = curr_ptr;
    }
    return new_root;
  }

  static void test_merge_k_lists() {
    cout << "2. test_merge_k_lists" << endl;
  }

  /**
   * 138. Copy List with Random Pointer
   * - Definition for singly-linked list with a random pointer.
   * struct RandomListNode {
   *     int label;
   *     RandomListNode *next, *random;
   *     RandomListNode(int x) : label(x), next(NULL), random(NULL) {}
   * };
   * Intuitioin:
   * - essentially copy a graph(dag), treverse & copy
   * - random pointer is random, but still points to an existing elem in list.
   * - bf way is to copy the list 1st, then 2nd pass to copy all random link
   * - any good idea for 1 pass? each time check next & random at the same time
   *   keep the mapping between old-ptr -> new-ptr & build link as needed.
   *         v-----+
   * - 1 --- 2 --- 3 --- 4 --- 5
   *         +-----^     +-----^
   */
  //typedef RandomListNode list_node;
  static list_node * copy_random_list(list_node * head) {
    list_node * elem_ptr = NULL, * curr_ptr = NULL;
    if (NULL == head) { return NULL; }

    unordered_map<list_node *, list_node *> visit_lookup;

    for (curr_ptr = head; NULL != curr_ptr; curr_ptr = curr_ptr->next) {
      visit_lookup[curr_ptr] = new list_node(curr_ptr->val);
    }

    for (curr_ptr = head; NULL != curr_ptr; curr_ptr = curr_ptr->next) {
      if (NULL != curr_ptr->next) { visit_lookup[curr_ptr]->next = visit_lookup[curr_ptr->next]; }
      if (NULL != curr_ptr->random) { visit_lookup[curr_ptr]->random = visit_lookup[curr_ptr->random]; }
    }

    return visit_lookup[head];
  }

  static list_node * lean_copy_random_list(list_node * head) {
    list_node * head_ptr = NULL, * temp_ptr = NULL, * curr_ptr = NULL;
    if (NULL == head) { return NULL; }

    /* insert new elem after each node */
    for (curr_ptr = head; NULL != curr_ptr; curr_ptr = curr_ptr->next->next) {
      temp_ptr = curr_ptr->next;
      curr_ptr->next = new list_node(curr_ptr->val);
      curr_ptr->next->next = temp_ptr;
    }

    /* stitch all random links */
    for (curr_ptr = head; NULL != curr_ptr; curr_ptr = curr_ptr->next->next) {
      temp_ptr = curr_ptr->random;
      if (NULL != temp_ptr) { curr_ptr->next->random = temp_ptr->next; }
    }

    /* remove list from input */
    head_ptr = head->next;
    for (curr_ptr = head; NULL != curr_ptr; curr_ptr = curr_ptr->next) {
      temp_ptr = curr_ptr->next;
      curr_ptr->next = curr_ptr->next->next;
      if (NULL != curr_ptr->next) { temp_ptr->next = curr_ptr->next->next; }
    }

    return head_ptr;
  }

  static void test_lean_copy_random_list() {
    cout << "3. test_lean_copy_random_list" << endl;
  }

  /**
   * 25. Reverse Nodes in k-Group
   * - Given a linked list, reverse the nodes of a linked list k at a time and
   *   return its modified list.
   * - k is a positive integer and is less than or equal to the length of the
   *   linked list. If the number of nodes is not a multiple of k then left-out
   *   nodes in the end should remain as it is.
   * Example:
   * - Given this linked list: 1->2->3->4->5
2 -> 1
3 -> 2
1 -> 4
   *   For k = 2, you should return: 2->1->4->3->5
   *                                    ^        ^
   *   For k = 3, you should return: 3->2  1->4->5
   *   +-----------v                 ^<-_  ^
   * - 1  2  3  4  5
   *   ^<-^<-^  ^<-^
   *   f     c  n
5 -> 4
4 -> NULL
1 -> 5
   * Note:
   * - Only constant extra memory is allowed.
   * - You may not alter the values in the list's nodes, only nodes itself may
   *   be changed.
   * Intuition:
   * - rather straightforward, simply implement a reverse method take in stream
   */
  // typedef ListNode list_node;
  static bool is_segment_in_full_size(list_node * curr_ptr, int k) {
    int group_size = 0;
    for (list_node * node_ptr = curr_ptr; node_ptr != NULL && group_size < k;
         node_ptr = node_ptr->next, group_size++) { }
    return (group_size == k);
  }

  static list_node * reverse_list_in_k_group(list_node * head, int k) {
    if (k <= 1 || NULL == head) { return head; }
    list_node * curr_ptr = head,  * next_ptr = NULL, * first_ptr = NULL,
              * temp_ptr = NULL, * segs_ptr = NULL, * new_head = NULL;
    while (NULL != curr_ptr) {
      if (!is_segment_in_full_size(curr_ptr, k)) {
        if (NULL == new_head) { new_head = curr_ptr; }; break;
      }
      next_ptr = curr_ptr->next; first_ptr = curr_ptr;
      for (int curr_group_cnt = 1; curr_group_cnt < k &&
                                   NULL != next_ptr; curr_group_cnt++) {
        temp_ptr = next_ptr->next;
        next_ptr->next = curr_ptr;
        curr_ptr = next_ptr;
        next_ptr = temp_ptr;
      }
      /* after the for, curr_ptr points to the last elem of curr segment,
       * next_ptr points to the beginning of next segment */
      if (NULL == new_head) { new_head = curr_ptr; }
      first_ptr->next = next_ptr;
      if (NULL != segs_ptr) { segs_ptr->next = curr_ptr; }
      segs_ptr = first_ptr;
      curr_ptr = next_ptr;
    }
    return new_head;
  }

  static void test_reverse_list_in_k_group() {
    cout << "4. test_reverse_list_in_k_group" << endl;
    vector<list_node> test_input;
    for (int i = 1; i <= 7; i++) { test_input.push_back(list_node(i)); }
    for (int i = 0; i < 6; i++) { test_input[i].next = & test_input[i + 1]; }
    list_node * head = & test_input[0]; int k = 4;
    head = reverse_list_in_k_group(head, k);
    for (; NULL != head; head = head->next) { cout << head->val << " "; } cout << endl;
  }
};

int main(void) {
  using list_util::test_insert_circular_sorted_list;
  using list_util::test_reverse_list_in_k_group;
  using list_util::test_lean_copy_random_list;
  using list_util::test_merge_k_lists;

  test_insert_circular_sorted_list();
  test_lean_copy_random_list();
  test_merge_k_lists();
  test_reverse_list_in_k_group();

  list_util::test_insert_to_circular_linked_list();
  list_util::test_merge_sorted_lists();

  return 0;
}
