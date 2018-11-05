#include <list>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <cassert>
#include <utility>

namespace cache {

using std::cout;
using std::endl;
using std::pair;
using std::list;
using std::unordered_map;
using std::unordered_set;

const static int DEF_CACHE_SIZE_CAP = 65536;

template<typename key_t, typename value_t>
class lfu_entry {
public:
  lfu_entry() : freq(1) {}
  lfu_entry(const key_t & k, const value_t & v, int f = 1) : freq(f), key(k), value(v) {}
  virtual ~lfu_entry() {}
  int freq;
  key_t key;
  value_t value;
};

template<typename key_t, typename value_t>
class cache_lfu {
  typedef class lfu_entry<key_t, value_t> lfu_entry_t;
  typedef typename list<unordered_map<key_t, lfu_entry_t>>::iterator freq_list_entry_itr;
  typedef typename unordered_map<key_t, freq_list_entry_itr>::iterator kv_lookup_pair_itr;

public:
  cache_lfu(int max_cap = DEF_CACHE_SIZE_CAP) : max_size_cap(max_cap), total_item_cnt(0) {}
  virtual ~cache_lfu() {}

  bool set(const key_t & key, const value_t & value) {
    bool is_set_op_succes = false;
    /* 1. check to see if curr. entry already exists, if yes, then just update & upd freq. */
    kv_lookup_pair_itr curr_pair_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() != curr_pair_itr) {
      curr_pair_itr->second->find(key)->second.value = value;
      is_set_op_succes = increment_entry_freq(curr_pair_itr);
    } else {
    /* 2. enter here means we need to insert a new entry with freq of 1 */
      /* check & evict entry if needed */
      check_and_evict_for_new_entry();
      lfu_entry_t new_lfu_entry(key, value, 1);
      /* check the head of the list to see if they share same freq,
       * actions to insert new needed iff:
       * 1. freq-list is empty
       * 2. 1st map in freq-list has a diff freq than 1 */
      if (lfu_freq_list.empty() || (!lfu_freq_list.begin()->empty() && 
                                     lfu_freq_list.begin()->begin()->second.freq != 1)) {
        lfu_freq_list.push_front(unordered_map<key_t, lfu_entry_t>());
      }
      lfu_freq_list.front()[key] = new_lfu_entry;
      kv_pair_lookup[key] = lfu_freq_list.begin();
      total_item_cnt += 1;
      is_set_op_succes = true;
    }

    return is_set_op_succes;
  }

  /* basically pass by ref., true if we found a val, false if not */
  bool get(const key_t & key, value_t & value_ret) {
    kv_lookup_pair_itr curr_pair_itr = kv_pair_lookup.find(key);
    /* if no key found, short circuit */
    if (kv_pair_lookup.end() == curr_pair_itr) { return false; }
    /* return the value, list<unordered_map<key_t, lfu_entry_t>> lfu_freq_list */
    value_ret = curr_pair_itr->second->find(key)->second.value;
    /* update freq list & adjusting kv-lookup entry */
    return increment_entry_freq(curr_pair_itr);
  }

  bool del(const key_t & key) {
    bool is_del_op_succes = false;
    /* 1. check to see if curr. entry already exists, if no, then just return false */
    kv_lookup_pair_itr curr_pair_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() == curr_pair_itr) { return is_del_op_succes; }
    /* 2. start to remove from both lists */
    curr_pair_itr->second->erase(key);
    /* check if curr entry in freq is empty, remove if true */
    if (curr_pair_itr->second->empty()) {
      lfu_freq_list.erase(curr_pair_itr->second);
    }
    kv_pair_lookup.erase(curr_pair_itr);
    total_item_cnt -= 1;
    is_del_op_succes = true;
    return is_del_op_succes;
  }

  void show_all_kv_pairs() {
    /* freq_entry_map -> a map if key -> lfu_entry<freq, key, value> */
    cout << "[ ";
    for (auto & freq_entry_map : lfu_freq_list) {
      cout << "{ ";
      for (auto & entry_map_pair : freq_entry_map) {
        cout << "(" << entry_map_pair.second.freq << ":"
                    << entry_map_pair.second.key << ":"
                    << entry_map_pair.second.value << ") ";
      }
      cout << "} ";
    }
    cout << "]" << endl;
  }

private:

  /* true if eviction happens, false if not */
  bool check_and_evict_for_new_entry() {
    bool is_item_evicted = false;
    if (total_item_cnt == max_size_cap) {
      is_item_evicted = del(lfu_freq_list.begin()->begin()->first);
    }
    return is_item_evicted;
  }

  bool increment_entry_freq(kv_lookup_pair_itr curr_kv_pair_itr) {
    /* 1. get new freq value */
    key_t key_to_lookup = curr_kv_pair_itr->first;
    freq_list_entry_itr curr_freq_entry_itr = curr_kv_pair_itr->second;
    int new_freq = curr_freq_entry_itr->begin()->second.freq + 1;
    (* curr_freq_entry_itr)[key_to_lookup].freq = new_freq;

    /* 2. check to see if need to insert a new entry, we add a new entry iff:
     *    - next_freq_entry_itr has a diff freq value than new_freq.
     *    - next_freq_entry_itr reach the end of the list */
    freq_list_entry_itr next_freq_entry_itr = curr_freq_entry_itr;
    next_freq_entry_itr++;
    if (lfu_freq_list.end() == next_freq_entry_itr ||
        next_freq_entry_itr->begin()->second.freq > new_freq) {
      next_freq_entry_itr = lfu_freq_list.insert(next_freq_entry_itr, unordered_map<key_t, lfu_entry_t>());
    }
    (* next_freq_entry_itr)[key_to_lookup] = (* curr_freq_entry_itr)[key_to_lookup];

    /* after prop the new elem, remove its old entry */
    curr_freq_entry_itr->erase(key_to_lookup);
    if (curr_freq_entry_itr->empty()) { lfu_freq_list.erase(curr_freq_entry_itr); }
    kv_pair_lookup[key_to_lookup] = next_freq_entry_itr;

    return true;
  }

  /* main entry for kv lookup, directing to the corresponding map in freq list */
  unordered_map<key_t, freq_list_entry_itr> kv_pair_lookup;
  /* each entry represents a map contains key -> lfu-entry, all pairs has same
   * freq, while whole list is ordered by the freq of each map in asc order.
   * each time if we need a eviction, it will always happens at head of list
   * which shares the min. freq value */
  list<unordered_map<key_t, lfu_entry_t>> lfu_freq_list;
  int max_size_cap, total_item_cnt;
};

};

static void test_cache_lfu() {
  const static std::string delim = " ";
  cache::cache_lfu<std::string, std::string> lfu_cache_obj(3);
  std::string curr_line, curr_op, curr_key, curr_value;
  size_t curr_pos = std::string::npos, prev_pos = curr_pos;
  while(std::getline(std::cin, curr_line)) {
    if ("show" == curr_line) { lfu_cache_obj.show_all_kv_pairs(); continue; }
    curr_pos = curr_line.find(delim);
    if (std::string::npos == curr_pos) { continue; }
    curr_op = curr_line.substr(0, curr_pos);
    if ("set" == curr_op) {
      prev_pos = curr_pos;
      curr_pos = curr_line.find(delim, curr_pos + 1);
      curr_key = curr_line.substr(prev_pos + 1, curr_pos - prev_pos - 1);
      curr_value = curr_line.substr(curr_pos + 1, std::string::npos);
      lfu_cache_obj.set(curr_key, curr_value);
    } else if ("get" == curr_op) {
      curr_key = curr_line.substr(curr_pos + 1, std::string::npos);
      lfu_cache_obj.get(curr_key, curr_value);
      std::cout << curr_key << ":" << curr_value << std::endl;
    } else if ("del" == curr_op) {
      curr_key = curr_line.substr(curr_pos + 1, std::string::npos);
      lfu_cache_obj.del(curr_key);
    }
  }
}

int main(void) {
  test_cache_lfu();
  return 0;
}
