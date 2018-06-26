/**
 * Given a node from a cyclic linked list which has been sorted, write a
 * function to insert a value into the list such that it remains a cyclic
 * sorted list. The given node can be any single node in the list.
 */
#include <iostream>
#include <vector>

using namespace std;

/**
 * 1 -> 3 -> 3 -> 7
 * ^--------------|
 * 3 (1 < 3 < 7)
 * 1 -> 3 -> 3 -> (3) -> 7
 * ^---------------------|
 * needs check back to origin
 * case I:  given 3, => loop next until val > 3 or start decr.
 * case II: given 1, => loop next val > 3 or start decr.
 * case IV: given 7, => loop next val > 3 && val < 7 or start decr.
 */
namespace list_util {
  class circular_sorted_list_node {
  public:
    circular_sorted_list_node(int val) { value = val; next_ptr = this; }
    virtual ~circular_sorted_list_node() {}
    circular_sorted_list_node * next_ptr;
    int value;
  };
  static circular_sorted_list_node *
    insert_circular_sorted_list(circular_sorted_list_node * list_head_ptr,
                                int value_to_insert) {
    circular_sorted_list_node * inst_node_ptr = new circular_sorted_list_node(value_to_insert);
    if (NULL == list_head_ptr) { return inst_node_ptr; }

    circular_sorted_list_node * curr_node_ptr = list_head_ptr,
                              * temp_node_ptr = NULL;

    if (curr_node_ptr == curr_node_ptr->next_ptr) {
      list_head_ptr->next_ptr = inst_node_ptr;
      inst_node_ptr->next_ptr = list_head_ptr;
    } else {
      do {
        if ((curr_node_ptr->value <= inst_node_ptr->value &&           // mid
             inst_node_ptr->value <  curr_node_ptr->next_ptr->value) ||
            (curr_node_ptr->value >  curr_node_ptr->next_ptr->value && // min
             curr_node_ptr->next_ptr->value >= inst_node_ptr->value) ||
            (curr_node_ptr->value >  curr_node_ptr->next_ptr->value && // max
             curr_node_ptr->value <= inst_node_ptr->value) ||
            (curr_node_ptr->next_ptr == list_head_ptr)) {              // eql
          temp_node_ptr = curr_node_ptr->next_ptr;
          curr_node_ptr->next_ptr = inst_node_ptr;
          inst_node_ptr->next_ptr = temp_node_ptr;
          break;
        }
        curr_node_ptr = curr_node_ptr->next_ptr;
      } while (curr_node_ptr != list_head_ptr);
    }
    return inst_node_ptr;
  }
  static void print_circular_sorted_list(circular_sorted_list_node * list_head_ptr) {
    if (NULL == list_head_ptr) { return; }
    circular_sorted_list_node * curr_node_ptr = list_head_ptr;
    do {
      cout << curr_node_ptr->value << " ";
      curr_node_ptr = curr_node_ptr->next_ptr;
    } while (curr_node_ptr != list_head_ptr); cout << endl;
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
