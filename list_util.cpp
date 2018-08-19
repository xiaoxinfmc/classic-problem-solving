/**
 * Given a node from a cyclic linked list which has been sorted, write a
 * function to insert a value into the list such that it remains a cyclic
 * sorted list. The given node can be any single node in the list.
 */
#include <iostream>
#include <cassert>
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
};

int main(void) {
  using list_util::circular_sorted_list_node;
  using list_util::insert_circular_sorted_list;
  using list_util::print_circular_sorted_list;

  cout << "1. insert_circular_sorted_list:" << endl;
  cout << "-1 -1 0 1 1 2 3 5 5" << endl;
  vector<int> elem_value_arr({ -1, -1, 1, 1, 2, 3, 5, 5 });
  circular_sorted_list_node * list_head_ptr = new circular_sorted_list_node(0);
  for (auto & elem_value : elem_value_arr) {
    list_head_ptr = insert_circular_sorted_list(list_head_ptr, elem_value);
    print_circular_sorted_list(list_head_ptr);
  }
  print_circular_sorted_list(list_head_ptr);
  return 0;
}
