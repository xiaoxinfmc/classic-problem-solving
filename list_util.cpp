/**
 * Given a node from a cyclic linked list which has been sorted, write a
 * function to insert a value into the list such that it remains a cyclic
 * sorted list. The given node can be any single node in the list.
 */
#include <iostream>
#include <cassert>

using namespace std;

class ListUtil {
/**
 * 1 -> 3 -> 3 -> 7
 * ^--------------|
 * 3 (1 < 3 < 7)
 * 1 -> 3 -> 3 -> (3) -> 7
 * ^---------------------|
 * needs check back to origin
 * case I: given 3, => loop next until val > 3 or start <.
 * case II: given 1, => loop next val > 3 or start <.
 * case IV: given 7, => loop next val > 3 && val < 7 or start <.
 */
public:
  class CyclicSortedListNode {
  public:
    CyclicSortedListNode(int val) { value = val; next_ptr = this; }
    virtual ~CyclicSortedListNode() { delete next_ptr; next_ptr = NULL; }
    CyclicSortedListNode * next_ptr;
    int value;
  };

  static CyclicSortedListNode * insert_cyclic_sorted_list(
    CyclicSortedListNode * list_head_ptr, int value_to_insert)
  {
    CyclicSortedListNode * insert_node_ptr =new CyclicSortedListNode(value_to_insert);
    CyclicSortedListNode * curr_node_ptr = list_head_ptr;

    if (NULL == list_head_ptr) { return insert_node_ptr; }

    // (3),  1, 3, 3, 5, 7(given), 9 loop until 1 (curr_ptr -> 9, 1 <= 3)
    // or back to 7, say 6 7 7 (7) (curr_ptr -> last 7)
    // or back to 7, say 6 7 (7) 7 (curr_ptr -> last 7)
    do {
      if ((value_to_insert >= curr_node_ptr->value) &&
          (value_to_insert < curr_node_ptr->next_ptr->value) &&
          (curr_node_ptr->value < curr_node_ptr->next_ptr->value)) {
        break;
      } else if ((curr_node_ptr->value > curr_node_ptr->next_ptr->value) &&
                 (value_to_insert >= curr_node_ptr->value)) {
        break;
      } else {
        curr_node_ptr = curr_node_ptr->next_ptr;
      }
    } while (list_head_ptr != curr_node_ptr);

    insert_node_ptr->next_ptr = curr_node_ptr->next_ptr;
    curr_node_ptr->next_ptr = insert_node_ptr;

    return insert_node_ptr;
  }

  static void print_list(CyclicSortedListNode * root_ptr) {
    if (NULL == root_ptr) { return; }
    CyclicSortedListNode * curr_node_ptr = root_ptr; ;
    do {
      cout << curr_node_ptr->value << " ";
      curr_node_ptr = curr_node_ptr->next_ptr;
    } while (curr_node_ptr != root_ptr); cout << endl;
  }
};

int main(void) {
  ListUtil::CyclicSortedListNode * list_one =
    ListUtil::insert_cyclic_sorted_list(NULL, 1);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, 1);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, 1);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, 3);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, -1);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, -1);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, 5);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, 5);
  ListUtil::print_list(list_one);
  list_one = ListUtil::insert_cyclic_sorted_list(list_one, 2);
  ListUtil::print_list(list_one);
  return 0;
}
