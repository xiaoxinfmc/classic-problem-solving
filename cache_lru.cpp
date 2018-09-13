#include <list>
#include <string>
#include <iostream>
#include <unordered_map>

namespace cache {

const static int DEF_CACHE_MAX_SIZE = 65536;

template<typename key_t, typename value_t>
class cache_lru {
public:
  typedef typename std::list<std::pair<key_t, value_t>>::iterator kv_pair_itr_t;
  typedef typename std::pair<key_t, value_t> kv_pair_t;
  typedef typename std::unordered_map<key_t, kv_pair_itr_t>::iterator kv_map_itr_t;

  cache_lru(int max_cap = DEF_CACHE_MAX_SIZE) : max_item_cnt(max_cap) {}
  virtual ~cache_lru() {}

  bool set(const key_t & k, const value_t & v) {
    bool ret = true;
    kv_map_itr_t curr_item_pair_itr = item_key_lookup.find(k);
    if (item_key_lookup.end() == curr_item_pair_itr) {
      /* insert if it is completely new & insert to the front */
      if (item_key_lookup.size() >= max_item_cnt) {
        item_key_lookup.erase(items_list.back().first);
        items_list.pop_back();
      }
      items_list.push_front(kv_pair_t(k, v));
      item_key_lookup[k] = items_list.begin();
    } else {
      /* update value if it exists & move it to the front */
      * (curr_item_pair_itr->second) = kv_pair_t(k, v);
      items_list.splice(items_list.begin(), items_list, curr_item_pair_itr->second);
    }
    return ret;
  }

  bool get(const key_t & k, value_t & v_ret) {
    bool ret = false;
    kv_map_itr_t curr_item_pair_itr = item_key_lookup.find(k);
    if (item_key_lookup.end() != curr_item_pair_itr) {
      v_ret = curr_item_pair_itr->second->second;
      /* move item to the head of the list */
      items_list.splice(items_list.begin(), items_list, curr_item_pair_itr->second);
      ret = true;
    }
    return ret;
  }

  bool del(const key_t & k) {
    bool ret = false;
    kv_map_itr_t curr_item_pair_itr = item_key_lookup.find(k);
    if (item_key_lookup.end() != curr_item_pair_itr) {
      items_list.erase(curr_item_pair_itr->second);
      item_key_lookup.erase(curr_item_pair_itr);
      ret = true;
    }
    return ret;
  }

  void show_all_kv_pairs() {
    for (auto & kv_pair : items_list) {
      std::cout << kv_pair.first << ":" << kv_pair.second << ":"
                << item_key_lookup[kv_pair.first]->second << ":"
                << item_key_lookup.find(kv_pair.first)->first << " ";
    } std::cout << std::endl;
  }

private:
  std::unordered_map<key_t, kv_pair_itr_t> item_key_lookup;
  std::list<kv_pair_t> items_list;
  int max_item_cnt;
};
};

static void test_cache_lru() {
  const static std::string delim = " ";
  cache::cache_lru<std::string, std::string> lru_cache_obj(3);
  std::string curr_line, curr_op, curr_key, curr_value;
  size_t curr_pos = std::string::npos, prev_pos = curr_pos;
  while(std::getline(std::cin, curr_line)) {
    if ("show" == curr_line) { lru_cache_obj.show_all_kv_pairs(); continue; }
    curr_pos = curr_line.find(delim);
    if (std::string::npos == curr_pos) { continue; }
    curr_op = curr_line.substr(0, curr_pos);
    if ("set" == curr_op) {
      prev_pos = curr_pos;
      curr_pos = curr_line.find(delim, curr_pos + 1);
      curr_key = curr_line.substr(prev_pos + 1, curr_pos - prev_pos - 1);
      curr_value = curr_line.substr(curr_pos + 1, std::string::npos);
      lru_cache_obj.set(curr_key, curr_value);
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
