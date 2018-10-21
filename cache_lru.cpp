#include <list>
#include <string>
#include <iostream>
#include <unordered_map>

/**
 * 145. LRU Cache
 * - Design and implement a data structure for Least Recently Used (LRU) cache.
 *   It should support the following operations: get and put.
 * - get(key) - Get the value (will always be positive) of the key if the key
 *   exists in the cache, otherwise return -1.
 * - put(key, value) - Set or insert the value if the key is not already present
 *   When the cache reached its capacity it should invalidate the least recently
 *   used item before inserting a new item.
 * - Follow up:
 *   Could you do both operations in O(1) time complexity?
 * Example:
 *   LRUCache cache = new LRUCache(2)
 *   cache.put(1, 1);
 *   cache.put(2, 2);
 *   cache.get(1);       // returns 1
 *   cache.put(3, 3);    // evicts key 2
 *   cache.get(2);       // returns -1 (not found)
 *   cache.put(4, 4);    // evicts key 1
 *   cache.get(1);       // returns -1 (not found)
 *   cache.get(3);       // returns 3
 *   cache.get(4);       // returns 4
 * Intuition:
 * - use a unordered map to hold a mapping from key -> value.
 * - keep a counter for total count to decide whether to evict.
 * -     a       b
 *       v       v
 *      itr     itr
 *       v       v
 *   [ (a, 1), (b, 2) ]
 * - each time a new pair inserted/fetched, it is the most recent item
 * - the key here is to efficiently maintain the recency order of keys
 * - hold a map of { key -> list-itr }, { list of values }
 * - case i, within size limit, then
 *   append to the head of list when insert new values each time.
 *   re-arrange the item to the head of list when values been retrieved.
 *   erase the item when del issued.
 *   (be very careful when existing key been updated with diff value)
 * - case ii, reach size limit, then
 *   erase the oldest item in the list (tail elem) & remove k-v from map
 *   insert new elem to the head & k-v to map.
 */
namespace cache {

const static int DEF_CACHE_MAX_SIZE = 65536;

template<typename key_type, typename value_type>
class cache_lru {

typedef typename std::pair<key_type, value_type> list_elem_type;
typedef typename std::list<list_elem_type>::iterator list_itr_type;
typedef typename std::unordered_map<key_type, list_itr_type>::iterator map_itr_type;

public:

  bool put(const key_type & k, const value_type & v) {
    bool is_success = true;
    del(k);
    if (get_curr_elem_size() >= max_elem_size_cap) {
      evict_least_recent_elem();
    }
    elem_list_by_recency.push_front(list_elem_type(k, v));
    key_lookup[k] = elem_list_by_recency.begin();
    return is_success;
  }

  bool get(const key_type & k, value_type & v_ret) {
    bool is_elem_found = false;
    if (key_lookup.count(k) <= 0) { return is_elem_found; }
    list_itr_type curr_elem_itr = key_lookup[k];
    v_ret = curr_elem_itr->second;
    promote_elem_in_recency_list(curr_elem_itr);
    return is_elem_found;
  }

  bool del(const key_type & k) {
    bool is_elem_found = false;
    if (key_lookup.count(k) <= 0) { return is_elem_found; }
    elem_list_by_recency.erase(key_lookup[k]);
    key_lookup.erase(k);
    return is_elem_found;
  }

  void show_all_kv_pairs() {
    std::cout << "{ ";
    for (auto & curr_kv_pair : elem_list_by_recency) {
      std::cout << curr_kv_pair.first << ":" << curr_kv_pair.second << " ";
    }
    std::cout << "}" << std::endl;
  }

  int get_curr_elem_size() { return key_lookup.size(); }

  void promote_elem_in_recency_list(list_itr_type curr_elem_itr) {
    /* if curr_elem is not the 1st elem, then move it to the head of the list */
    if (curr_elem_itr == elem_list_by_recency.begin()) { return; }
    elem_list_by_recency.splice(elem_list_by_recency.begin(), elem_list_by_recency, curr_elem_itr);
  }

  void evict_least_recent_elem() {
    if (elem_list_by_recency.empty()) { return; }
    key_lookup.erase(elem_list_by_recency.back().first);
    elem_list_by_recency.pop_back();
  }

  cache_lru(int size_cap = DEF_CACHE_MAX_SIZE) : max_elem_size_cap(size_cap) {}
  virtual ~cache_lru() {}

  std::list<list_elem_type> elem_list_by_recency;
  std::unordered_map<key_type, list_itr_type> key_lookup;
  int max_elem_size_cap;
};
};

static void test_cache_lru() {
  const static std::string delim = " ";
  cache::cache_lru<std::string, std::string> lru_cache_obj(2);
  std::string curr_line, curr_op, curr_key, curr_value;
  size_t curr_pos = std::string::npos, prev_pos = curr_pos;
  while(std::getline(std::cin, curr_line)) {
    curr_key = "", curr_value = "";
    if ("show" == curr_line) { lru_cache_obj.show_all_kv_pairs(); continue; }
    curr_pos = curr_line.find(delim);
    if (std::string::npos == curr_pos) { continue; }
    curr_op = curr_line.substr(0, curr_pos);
    if ("put" == curr_op) {
      prev_pos = curr_pos;
      curr_pos = curr_line.find(delim, curr_pos + 1);
      curr_key = curr_line.substr(prev_pos + 1, curr_pos - prev_pos - 1);
      curr_value = curr_line.substr(curr_pos + 1, std::string::npos);
      lru_cache_obj.put(curr_key, curr_value);
    } else if ("get" == curr_op) {
      curr_key = curr_line.substr(curr_pos + 1, std::string::npos);
      lru_cache_obj.get(curr_key, curr_value);
      std::cout << curr_key << ":" << curr_value << std::endl;
    } else if ("del" == curr_op) {
      curr_key = curr_line.substr(curr_pos + 1, std::string::npos);
      lru_cache_obj.del(curr_key);
    }
  }
}

int main(void) {
  test_cache_lru();
  return 0;
}
