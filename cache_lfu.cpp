#include <list>
#include <string>
#include <iostream>
#include <unordered_map>

namespace cache {

using std::cout;
using std::endl;
using std::pair;
using std::list;
using std::unordered_map;

const static int DEF_CACHE_SIZE_CAP = 65536;

template<typename key_t, typename value_t>
class cache_lfu {
public:
  typedef typename unordered_map<key_t, pair<value_t, int>>::iterator kv_pair_itr;
  typedef typename list<kv_pair_itr>::iterator freq_list_itr;

  cache_lfu(int max_cap = DEF_CACHE_SIZE_CAP) : max_item_cnt(max_cap),
                                                min_freq_cnt(INT_MAX) {}
  virtual ~cache_lfu() {}

  bool set(const key_t & key, const value_t & value) {
    int curr_entry_cnt = 1;
    kv_pair_itr curr_entry_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() == curr_entry_itr) {
      /* completely new, not exist, then check size 1st */
      if (kv_pair_lookup.size() >= max_item_cnt) {
        /* execeed size limit, evict first */
        kv_pair_lookup.erase(freq_lookup[min_freq_cnt].back());
        freq_list_itr_lookup.erase(freq_lookup[min_freq_cnt].back()->first);
        freq_lookup[min_freq_cnt].pop_back();
        if (freq_lookup[min_freq_cnt].empty()) { freq_lookup.erase(min_freq_cnt); }
      }
      /* after evict if needed, start insert */
      kv_pair_lookup[key] = pair<value_t, int>(value, curr_entry_cnt);
      if (freq_lookup.end() == freq_lookup.find(curr_entry_cnt)) {
        freq_lookup[curr_entry_cnt] = list<kv_pair_itr>();
      }
      freq_lookup[curr_entry_cnt].push_front(kv_pair_lookup.find(key));
      freq_list_itr_lookup[key] = freq_lookup[curr_entry_cnt].begin();
      min_freq_cnt = curr_entry_cnt;
    } else {
      /* exist already, then update value & incr. freq */
      curr_entry_cnt = curr_entry_itr->second.second;
      freq_lookup[curr_entry_cnt].erase(freq_list_itr_lookup[key]);
      if (freq_lookup[curr_entry_cnt].empty()) { 
        freq_lookup.erase(curr_entry_cnt);
        if (min_freq_cnt == curr_entry_cnt) { min_freq_cnt += 1; }
      }
      curr_entry_cnt += 1;
      curr_entry_itr->second = pair<value_t, int>(value, curr_entry_cnt);
      if (freq_lookup.end() == freq_lookup.find(curr_entry_cnt)) {
        freq_lookup[curr_entry_cnt] = list<kv_pair_itr>();
      }
      freq_lookup[curr_entry_cnt].push_front(curr_entry_itr);
      freq_list_itr_lookup[key] = freq_lookup[curr_entry_cnt].begin();
    }
    return true;
  }

  bool get(const key_t & key, value_t & value_ret) {
    bool ret = false;
    kv_pair_itr curr_entry_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() != curr_entry_itr) {
      int curr_freq_cnt = curr_entry_itr->second.second;
      value_ret = curr_entry_itr->second.first;
      freq_lookup[curr_freq_cnt].erase(freq_list_itr_lookup[key]);
      if (freq_lookup[curr_freq_cnt].empty()) { 
        freq_lookup.erase(curr_freq_cnt);
        if (min_freq_cnt == curr_freq_cnt) { min_freq_cnt += 1; }
      }
      curr_freq_cnt += 1;
      curr_entry_itr->second.second = curr_freq_cnt;
      if (freq_lookup[curr_freq_cnt].empty()) {
        freq_lookup[curr_freq_cnt] = list<kv_pair_itr>();
      }
      freq_lookup[curr_freq_cnt].push_front(curr_entry_itr);
      freq_list_itr_lookup[key] = freq_lookup[curr_freq_cnt].begin();
      ret = true;
    }
    return ret;
  }

  bool del(const key_t & key) {
    bool ret = false;
    kv_pair_itr curr_entry_itr = kv_pair_lookup.find(key);
    if (kv_pair_lookup.end() != curr_entry_itr) {
      int curr_freq_cnt = curr_entry_itr->second.second;
      freq_lookup[curr_freq_cnt].erase(freq_list_itr_lookup[key]);
      if (freq_lookup[curr_freq_cnt].empty()) { freq_lookup.erase(curr_freq_cnt); }
      kv_pair_lookup.erase(curr_entry_itr);
      freq_list_itr_lookup.erase(key);
      ret = true;
    }
    return ret;
  }

  void show_all_kv_pairs() {
    for (auto & entry : kv_pair_lookup) {
      cout << "min-freq: " << min_freq_cnt << " " << entry.first << ":"
           << entry.second.first << ":" << entry.second.second
           << ":" << freq_lookup.find(entry.second.second)->first << " ";
    } cout << endl;
  }

private:
  /* used to store the mapping from key -> <value, freq_cnt> */
  unordered_map<key_t, pair<value_t, int>> kv_pair_lookup;
  /* used to store the mapping from freq -> list of itr. in kv_pair_lookup */
  unordered_map<int, list<kv_pair_itr>> freq_lookup;
  /* used to store the mapping from key -> iterator in freq list in freq_lookup */
  unordered_map<key_t, freq_list_itr> freq_list_itr_lookup;

  /* minimum freq cnt for all cached items */
  int min_freq_cnt, max_item_cnt;
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
