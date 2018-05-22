#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * goal: print all combination of well formed parentheses.
 * (()()), (())(), ()()(), ((())), good 
 * ()()(), ()(()),  (()()), ((()))
 * ())((), ()))((, )))(((, bad
 * can be generalized to choose whether or not to enclose the pair each time
 * n pairs of parentheses, 2n tokens, 2n!, needs pruning.
 * number of '(' and ')' must match in the end.
 * always start with '(', when started or all previous () enclosed with each other
 * for any valid pairs(enclosed), '(' and ')' match the count
 * anytime during the logic, left parts must be >= right parts
 */
class ParenthesisUtil {
public:
  static vector<string> generate_parenthesis(int pair_cnt) {
    vector<string> pairs;
    generate_parenthesis_comb(0, 0, pair_cnt, "", pairs);
    return pairs;
  }
  static void generate_parenthesis_comb(int left_cnt, int right_cnt,
                                        int max_pair_cnt, string curr_prefix,
                                        vector<string> & pairs_arr_ret) {
    /* if we just use up all parts, then just return */
    if ((left_cnt > max_pair_cnt) || (right_cnt > max_pair_cnt)) { return; }
    if (curr_prefix.size() == max_pair_cnt * 2) {
      pairs_arr_ret.push_back(curr_prefix); return;
    }
    if (left_cnt == right_cnt) {
      /* either we just start or previous parts already enclosed */
      generate_parenthesis_comb(
        left_cnt + 1, right_cnt, max_pair_cnt, curr_prefix + "(", pairs_arr_ret
      );
    } else if (left_cnt > right_cnt) {
      generate_parenthesis_comb(
        left_cnt + 1, right_cnt, max_pair_cnt, curr_prefix + "(", pairs_arr_ret
      );
      generate_parenthesis_comb(
        left_cnt, right_cnt + 1, max_pair_cnt, curr_prefix + ")", pairs_arr_ret
      );
    }
  }
};

int main(void) {
  for (auto & pair : ParenthesisUtil::generate_parenthesis(10)) {
    cout << pair << endl;
  }
  return 0;
}
