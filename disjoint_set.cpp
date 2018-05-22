#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

class DisjointSet {
public:
  DisjointSet(int len) {
    max_slots = len; num_of_comps = len; slots_buffer_ptr = new int[len];
    assert((0 < len) && (NULL != slots_buffer_ptr));
    for (int i = 0; i < len; i++) {
      slots_buffer_ptr[i] = (int)DEF_NOT_BELONG_TO_ANY;
    }
  }
  bool is_all_connected() { return (1 == num_of_comps); }

  /* mark next & prev will be under same parent, larger id merged to smaller id */
  bool union_set(int prev, int next) {
    if (!is_id_within_range(prev) || !is_id_within_range(next)){ return false; }
    int prev_src = union_find(prev);
    int next_src = union_find(next);
    if (prev_src == next_src) { return false; }
    union_by_root(prev_src, next_src);
    num_of_comps--;
    return true;
  }

  /* un-connected means belongs to itself */
  int union_find(int comp_idx) {
    if(!is_id_within_range(comp_idx)) { return (int)DEF_NOT_BELONG_TO_ANY; }
    if (slots_buffer_ptr[comp_idx] < 0) { return comp_idx; }
    slots_buffer_ptr[comp_idx] = union_find(slots_buffer_ptr[comp_idx]);
    return slots_buffer_ptr[comp_idx];
  }

private:
  void union_by_root(int prev_root, int next_root) {
    /* -2 < -3, => merge big to small */
    if (slots_buffer_ptr[prev_root] > slots_buffer_ptr[next_root]) {
      slots_buffer_ptr[prev_root] += slots_buffer_ptr[next_root];
      slots_buffer_ptr[next_root] = prev_root;
    } else {
      slots_buffer_ptr[next_root] += slots_buffer_ptr[prev_root];
      slots_buffer_ptr[prev_root] = next_root;
    }
  }
  bool is_id_within_range(int idx) { return ((idx > 0) && (idx < max_slots)); }
  int max_slots; int num_of_comps; int * slots_buffer_ptr;
  enum { DEF_NOT_BELONG_TO_ANY = -1 };
};
