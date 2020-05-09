/*
Given an array of strings, group anagrams together.
For example, given: ["eat", "tea", "tan", "ate", "nat", "bat"], 
Return:
[
  ["ate", "eat","tea"],
  ["nat","tan"],
  ["bat"]
]
assume: only lower case chars
*/

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <deque>
#include <string>

using namespace std;

class Anagram{
public:
  const static char DE_CODE_MIN_CHAR = 'A';
  const static char DE_CODE_MAX_CHAR = 'Z';

  const static char ANAGRAM_MIN_CHAR = 'a';
  const static char ANAGRAM_MAX_CHAR = 'z';

  // abcc => [ 'a' => 1, 'b' => 1, 'c' => 2 ] => buffer of int[26] : signature
  // int std::memcmp(const void *s1, const void *s2, size_t n(bytes));
  // void * memset(void *b, int c, size_t len);
  // string& append (size_t n, char c);
  static vector<vector<string> > build_anagram_group(const vector<string> & strs) {
    vector<vector<string> > anagram_group_arr;
    if (true == strs.empty()) { return anagram_group_arr; }

    unordered_map<string, int> anagram_signature_to_group_id_map;
    unordered_map<string, int>::const_iterator signatrue_to_group_id_itr;
    string signature;
    for (auto & str : strs) {
      signature = get_input_signature(str);
      signatrue_to_group_id_itr = anagram_signature_to_group_id_map.find(signature);
      if (anagram_signature_to_group_id_map.end() == signatrue_to_group_id_itr) {
        anagram_group_arr.push_back(vector<string>());
        anagram_group_arr.back().push_back(str);
        anagram_signature_to_group_id_map[signature] = anagram_group_arr.size() - 1;
      } else {
        anagram_group_arr[signatrue_to_group_id_itr->second].push_back(str);
      }
    }
    return anagram_group_arr;
  }

  static string get_input_signature(const string & input) {
    char char_cnt_buf[int(ANAGRAM_MAX_CHAR) - int(ANAGRAM_MIN_CHAR) + 1];
    std::memset((void *) char_cnt_buf, 0, sizeof(char_cnt_buf));
    for (int i = 0; i < int(input.size()); i++) {
      char_cnt_buf[int(input[i]) - int(ANAGRAM_MIN_CHAR)] += 1;
    }
    string signature = "";
    for (int i = 0; i < sizeof(char_cnt_buf); i++) {
      if (0 >= char_cnt_buf[i]) { continue; }
      signature.append(char_cnt_buf[i], char(i + int(ANAGRAM_MIN_CHAR)));
    }
    return signature;
  }

  static vector<vector<string> > fetch_anagram_group(vector<string>& strs) {
    vector<vector<string> > anagram_group_arr;
    unordered_map<string, int> anagram_key_to_group_id_map;
    unordered_map<string, int>::iterator group_itr;
    if (true == strs.empty()) { return anagram_group_arr; }
    string anagram_signature = "";
    for (auto & str : strs) {
      anagram_signature = get_anagram_signature(str);
      group_itr = anagram_key_to_group_id_map.find(anagram_signature);
      if (anagram_key_to_group_id_map.end() == group_itr) {
        anagram_group_arr.push_back(vector<string>());
        anagram_group_arr.back().push_back(str);
        anagram_key_to_group_id_map[anagram_signature] = anagram_group_arr.size() - 1;
      } else {
        anagram_group_arr[group_itr->second].push_back(str);
      }
    }
    return anagram_group_arr;
  }

  static string get_anagram_signature(const string & str) {
    int char_cnt_buffer[int(ANAGRAM_MAX_CHAR) - int(ANAGRAM_MIN_CHAR) + 1];
    memset(char_cnt_buffer, 0, sizeof(char_cnt_buffer));
    for (int i = 0; i < int(str.size()); i++) {
      char_cnt_buffer[int(str[i]) - int(ANAGRAM_MIN_CHAR)] += 1;
    }
    string signature = "";
    for (int i = 0; i < sizeof(char_cnt_buffer)/sizeof(int); i++) {
      if (0 < char_cnt_buffer[i]) {
        signature.append(char_cnt_buffer[i], char(int(ANAGRAM_MIN_CHAR) + i));
      }
    }
    return signature;
  }

  static int get_decoding_count(const string & number_str) {
cout << "==>> input: " << number_str << endl;
    int * cnt_buf = new int[number_str.size()];
    for (int i = 0; i < number_str.size(); i++) { cnt_buf[i] = -1; }
    return get_translate_cnt_recur(number_str, 0, cnt_buf);
  }

  static void get_decoding_count_recur(const string & number_str, int * total_cnt_ptr,
                                       int curr_idx = 0, bool is_combined_before = false) {
    if (curr_idx > int(number_str.size() - 1)) {
      * total_cnt_ptr += 1;
      return;
    } else {
      if (true == is_non_zero(number_str[curr_idx])) {
        get_decoding_count_recur(number_str, total_cnt_ptr, curr_idx + 1, false);
        if ((curr_idx < int(number_str.size() - 1)) &&
            (true == is_pair_valid(number_str[curr_idx], number_str[curr_idx + 1]))) {
          get_decoding_count_recur(number_str, total_cnt_ptr, curr_idx + 2, true);
        }
      }
    }
  }

  static bool is_pair_valid(char num_l, char num_r) {
    return (
      (num_l == '2' && num_r <= '6') || (num_l == '1') ||
      ((num_l == '2' || num_l == '1') && (num_r == '0'))
    );
  }

  static bool is_non_zero(char num){ return ('0' != num); }

  // return 1 when successfully reaching the end of str, also means valid decode.
  // # of valid combination from curr_idx to the end of str.
  static int get_translate_cnt_recur(const string & str, int curr_idx, int * cnt_buf) {
    int count = 0;
    // if reaching the end of str, means valid decoding, then return 1
    if (curr_idx > int(str.size() - 1)) {
      count = 1;
    } else {
      if (true == is_non_zero(str[curr_idx])) {
        if (-1 != cnt_buf[curr_idx]) {
          count = cnt_buf[curr_idx];
cout << "==>> hit: " << curr_idx << " | " << count << endl;
        } else {
          // if move 1 char, then count will not incr.
          count = get_translate_cnt_recur(str, curr_idx + 1, cnt_buf);
          if ((curr_idx + 1 < str.size()) && (true == is_pair_valid(str[curr_idx], str[curr_idx + 1]))) {
            // if move 2 char, then count will incr.
            count += get_translate_cnt_recur(str, curr_idx + 2, cnt_buf);
          }
          cnt_buf[curr_idx] = count;
        }
      }
    }
    return count;
  }
};

int main(void) {
  string input[] = { "eat", "tea", "tan", "ate", "nat", "bat" };
  vector<string> strs(input, input + sizeof(input)/sizeof(string));
  // vector<vector<string> > ret = Anagram::fetch_anagram_group(strs);
  vector<vector<string> > ret = Anagram::build_anagram_group(strs);
  for (auto & group : ret) {
    for (auto & str : group){ cout << str << " "; }; cout << endl;
  }
  cout << Anagram::get_decoding_count(string("1923")) << endl;
  cout << Anagram::get_decoding_count(string("1")) << endl;
  cout << Anagram::get_decoding_count(string("12")) << endl;
  cout << Anagram::get_decoding_count(string("1072512")) << endl;
  cout << Anagram::get_decoding_count(string("10")) << endl;
  cout << Anagram::get_decoding_count(string("100")) << endl;
  cout << Anagram::get_decoding_count(string("0")) << endl;
  cout << Anagram::get_decoding_count(string("010")) << endl;
  cout << Anagram::get_decoding_count(string("110")) << endl;
  cout << Anagram::get_decoding_count(string("111")) << endl;
  cout << Anagram::get_decoding_count(string("107251107251107251107251107251107251107251107251107251107251107251107251107251107251222222222222221072512107251107251107251107251107251107251107251107251107251107251107251107251107251107251222222222222221072512")) << endl;
  return 0;
}
