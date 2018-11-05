#include <list>
#include <string>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <utility>

namespace cache {

using std::cout;
using std::endl;
using std::pair;
using std::list;
using std::unordered_map;

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
  typedef typename list<lfu_entry_t>::iterator lfu_entry_list_itr;
  typedef typename list<list<lfu_entry_t>>::iterator freq_list_entry_itr;
  typedef pair<freq_list_entry_itr, lfu_entry_list_itr> freq_list_entry_itr_pair;
  typedef typename unordered_map<key_t, freq_list_entry_itr_pair>::iterator kv_lookup_pair_itr;

public:
  cache_lfu(int max_cap = DEF_CACHE_SIZE_CAP) : max_size_cap(max_cap), total_item_cnt(0) {}
  virtual ~cache_lfu() {}

  bool set(const key_t & key, const value_t & value) {
    bool is_set_op_succes = false;
    if (0 >= max_size_cap) { return is_set_op_succes; }
    kv_lookup_pair_itr curr_pair_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() != curr_pair_itr) {
      /* 1. check to see if curr. entry already exists, if yes, then just update & upd freq. */
      /* pair<freq_list_entry_itr, lfu_entry_list_itr> freq_list_entry_itr_pair; */
      curr_pair_itr->second.second->value = value;
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
                                     lfu_freq_list.begin()->begin()->freq != 1)) {
        lfu_freq_list.push_front(list<lfu_entry_t>());
      }
      lfu_freq_list.front().push_front(new_lfu_entry);
      kv_pair_lookup[key] = freq_list_entry_itr_pair(
        lfu_freq_list.begin(), lfu_freq_list.begin()->begin()
      );
      total_item_cnt += 1;
      is_set_op_succes = true;
    }

    return is_set_op_succes;
  }

  /* basically pass by ref., true if we found a val, false if not */
  bool get(const key_t & key, value_t & value_ret) {
    if (0 >= max_size_cap) { return false; }
    kv_lookup_pair_itr curr_pair_itr = kv_pair_lookup.find(key);
    /* if no key found, short circuit */
    if (kv_pair_lookup.end() == curr_pair_itr) { return false; }
    /* return the value, pair<freq_list_entry_itr, lfu_entry_list_itr> freq_list_entry_itr_pair; */
    value_ret = curr_pair_itr->second.second->value;
    /* update freq list & adjusting kv-lookup entry */
    return increment_entry_freq(curr_pair_itr);
  }

  bool del(const key_t & key) {
    bool is_del_op_succes = false;
    if (0 >= max_size_cap) { return is_del_op_succes; }
    /* 1. check to see if curr. entry already exists, if no, then just return false */
    kv_lookup_pair_itr curr_pair_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() == curr_pair_itr) { return is_del_op_succes; }
    /* 2. start to remove from both lists */
    /* pair<freq_list_entry_itr, lfu_entry_list_itr> freq_list_entry_itr_pair; */
    curr_pair_itr->second.first->erase(curr_pair_itr->second.second);
    /* check if curr entry in freq is empty, remove if true */
    if (curr_pair_itr->second.first->empty()) {
      lfu_freq_list.erase(curr_pair_itr->second.first);
    }
    kv_pair_lookup.erase(curr_pair_itr);
    total_item_cnt -= 1;
    is_del_op_succes = true;
    return is_del_op_succes;
  }

  void show_all_kv_pairs() {
    /* freq_entry_map -> a map if key -> lfu_entry<freq, key, value> */
    cout << "[ ";
    for (auto & freq_entry_list : lfu_freq_list) {
      cout << "{ ";
      for (auto & lfu_entry : freq_entry_list) {
        cout << "(" << lfu_entry.freq << ":" << lfu_entry.key << ":" << lfu_entry.value << ") ";
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
      is_item_evicted = del(lfu_freq_list.begin()->back().key);
    }
    return is_item_evicted;
  }

  bool increment_entry_freq(kv_lookup_pair_itr curr_kv_pair_itr) {
    /* 1. get new freq value */
    key_t key_to_lookup = curr_kv_pair_itr->first;
    freq_list_entry_itr curr_freq_entry_itr = curr_kv_pair_itr->second.first;
    lfu_entry_list_itr curr_lfu_entry_itr = curr_kv_pair_itr->second.second;
    int new_freq = curr_lfu_entry_itr->freq + 1;
    curr_lfu_entry_itr->freq = new_freq;

    /* 2. check to see if need to insert a new entry, we add a new entry iff:
     *    - next_freq_entry_itr has a diff freq value than new_freq.
     *    - next_freq_entry_itr reach the end of the list */
    freq_list_entry_itr next_freq_entry_itr = curr_freq_entry_itr;
    next_freq_entry_itr++;
    if (lfu_freq_list.end() == next_freq_entry_itr ||
        next_freq_entry_itr->begin()->freq > new_freq) {
      next_freq_entry_itr = lfu_freq_list.insert(next_freq_entry_itr, list<lfu_entry_t>());
    }
    next_freq_entry_itr->splice(next_freq_entry_itr->begin(), * curr_freq_entry_itr, curr_lfu_entry_itr);

    /* 3. after moving the elem to new list, remove its old entry */
    if (curr_freq_entry_itr->empty()) { lfu_freq_list.erase(curr_freq_entry_itr); }
    kv_pair_lookup[key_to_lookup] = freq_list_entry_itr_pair(next_freq_entry_itr, next_freq_entry_itr->begin());

    return true;
  }

  /* main entry for kv lookup, directing to the corresponding map in freq list */
  unordered_map<key_t, freq_list_entry_itr_pair> kv_pair_lookup;
  /* each entry represents a list contains lfu-entry, all pairs has same
   * freq, while whole list is ordered by the freq of each entry in asc order.
   * each time if we need a eviction, it will always happens at head of list
   * which shares the min. freq value */
  list<list<lfu_entry_t>> lfu_freq_list;
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
