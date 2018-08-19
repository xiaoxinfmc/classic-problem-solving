#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * goal: print all combination of well formed parentheses.
 * (()()), (())(), ()()(), ((())), good 
 * ()()(), ()(()),  (()()), ((()))
 * ())((), ()))((, )))(((, bad
 *
 * Observation:
 * - all valid combination has to start with '(', end with ')', # has to match.
 * - at any time, # of '(' is no less than # of ')'
 * - we form a good comb if reach end & conform all valid conditions @ each step
 */
class ParenthesisUtil {
public:

  static vector<string> generate_parenthesis(int pair_cnt) {
    vector<string> paren_comb_ret;
    gen_valid_parenthesis_recur(0, 0, pair_cnt, "", paren_comb_ret);
    return paren_comb_ret;
  }

  static void gen_valid_parenthesis_recur(int left_char_cnt, int right_char_cnt,
                                          int pair_cnt, string curr_parenthesis,
                                          vector<string> & paren_comb_ret) {
    if (left_char_cnt > pair_cnt || right_char_cnt > pair_cnt) { return; }
    if (left_char_cnt == pair_cnt && right_char_cnt == pair_cnt) {
      paren_comb_ret.push_back(curr_parenthesis); return;
    }
    if (left_char_cnt == right_char_cnt) {
      gen_valid_parenthesis_recur(left_char_cnt + 1, right_char_cnt, pair_cnt,
                                  curr_parenthesis + "(", paren_comb_ret);
    } else if (left_char_cnt > right_char_cnt) {
       gen_valid_parenthesis_recur(left_char_cnt + 1, right_char_cnt, pair_cnt,
                                   curr_parenthesis + "(", paren_comb_ret);
       gen_valid_parenthesis_recur(left_char_cnt, right_char_cnt + 1, pair_cnt,
                                   curr_parenthesis + ")", paren_comb_ret);
    }
  }
};

int main(void) {
  for (auto & pair : ParenthesisUtil::generate_parenthesis(10)) {
    cout << pair << endl;
  }
  return 0;
}
