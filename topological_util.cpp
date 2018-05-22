#include <iostream>
#include <vector>
#include <cassert>
#include <unordered_map>

using namespace std;

class TopologicalUtil{
public:
/*
 * Check whether the original sequence org can be uniquely reconstructed from
 * the sequences in seqs. The org sequence is a permutation of the integers
 * from 1 to n, with 1 ≤ n ≤ 10^4. Reconstruction means building a shortest
 * common supersequence of the sequences in seqs (i.e., a shortest sequence
 * so that all sequences in seqs are subsequences of it). Determine whether
 * there is only one sequence that can be reconstructed from seqs and it is
 * the org sequence.
 *
 * Example 1:
 * Input: * org: [1,2,3], seqs: [[1,2],[1,3]]
 * Output: false
 * Explanation: [1,2,3] is not the only one sequence that can be reconstructed,
 *              because [1,3,2] is a valid sequence that can be reconstructed.
 * Example 2:
 * Input: org: [1,2,3], seqs: [[1,2]]
 * Output: false
 * Explanation: The reconstructed sequence can only be [1,2].
 *
 * Example 3:
 * Input: org: [1,2,3], seqs: [[1,2],[1,3],[2,3]]
 * Output: true
 * Explanation: The sequences [1,2], [1,3], and [2,3] can uniquely reconstruct
 *              the original sequence [1,2,3].
 * 1 > 2
 * v   v
 * 3 < +
 * Example 4:
 * Input: org: [4,1,5,2,6,3], seqs: [[5,2,3,4],[4,1,5,2,6,3]]
 * Output: true
 *
 * 4 - 1
 *     |
 *     5 - 2 - 6 - 3
 * [ 10,000, 10,000 ] => 10,000-10,000
 * [      4,      3 ] =>      4-00,003
 * only 1 sequence can be constructed => seqs covers all links & no conflicts.
 */
  static bool is_sequence_unique(vector<int>& origin_seq, vector<vector<int>>& seqs) {
    unordered_map<int, int> value_to_pos_map, value_to_chk_map;
    if (1 == origin_seq.size()) {
      for (auto & sub_seq : seqs) {
        if ((1 != sub_seq.size()) || (origin_seq[0] != sub_seq[0])) { return false; }
      }
      return true;
    }
    for (int i = 0; i < (int)origin_seq.size(); i++) {
      value_to_pos_map[origin_seq[i]] = i;
      if (i ==  origin_seq.size() - 1) { continue; }
      value_to_chk_map[origin_seq[i]] = i;
    }
    int curr_num_idx = 0;
    unordered_map<int, int>::const_iterator curr_link_pair, next_link_pair;
    for (auto & sub_seq : seqs) {
      for (int i = 0; i < (int)(sub_seq.size() - 1); i++) {
        curr_link_pair = value_to_pos_map.find(sub_seq[i]);
        next_link_pair = value_to_pos_map.find(sub_seq[i + 1]);
        /* if subseq contains value not presented before, then fail */
        if ((value_to_pos_map.end() == curr_link_pair) ||
            (value_to_pos_map.end() == next_link_pair)) { return false; }
        /* check conflicts */
        if (curr_link_pair->second >= next_link_pair->second) { return false; }
        /* actual link confirmed, remove from existing set to check */
        if ((sub_seq[i] == origin_seq[curr_link_pair->second]) &&
            (sub_seq[i + 1] == origin_seq[curr_link_pair->second + 1])) {
          value_to_chk_map.erase(sub_seq[i]);
        }
      }
    }
    return value_to_chk_map.empty();
  }
};

int main(void) {
  int a[] = { 1, 2, 3 }; vector<int> av(a, a + sizeof(a) / sizeof(a[0]));
  int a1[] = { 1, 2 }; vector<int> av1(a1, a1 + sizeof(a1) / sizeof(a1[0]));
  int a2[] = { 1, 3 }; vector<int> av2(a2, a2 + sizeof(a2) / sizeof(a2[0]));
  int a3[] = { 2, 3 }; vector<int> av3(a3, a3 + sizeof(a3) / sizeof(a3[0]));
  vector<vector<int>> avv1; avv1.push_back(av1); avv1.push_back(av2);
  vector<vector<int>> avv2; avv2.push_back(av1);
  vector<vector<int>> avv3; avv3.push_back(av1); avv3.push_back(av2); avv3.push_back(av3);

  int b[] = { 4, 1, 5, 2, 6, 3 }; vector<int> bv(b, b + sizeof(b)/sizeof(b[0]));
  int b1[] = { 5, 2, 6, 3 }; vector<int> bv1(b1, b1 + sizeof(b1)/sizeof(b1[0]));
  int b2[] = { 4, 1, 5, 2 }; vector<int> bv2(b2, b2 + sizeof(b2)/sizeof(b2[0]));
  vector<vector<int>> bvv; bvv.push_back(bv1); bvv.push_back(bv2);

  assert(false == TopologicalUtil::is_sequence_unique(av, avv1));
  assert(false == TopologicalUtil::is_sequence_unique(av, avv2));
  assert(true == TopologicalUtil::is_sequence_unique(av, avv3));
  assert(true == TopologicalUtil::is_sequence_unique(bv, bvv));
  return 0;
}
