#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
#include <unordered_map>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <list>

namespace tool_util {
  using std::string;
  using std::cout;
  using std::endl;
  using std::vector;
  using std::list;
  using std::pair;
  using std::unordered_map;
  using std::ostream;

  template <class type>
  static void print_all_elem(const vector<type> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr << " "; }
    cout << "]" << endl;
  }

  template <class t1, class t2>
  static void print_all_pairs(const vector<pair<t1, t2>> & input) {
    cout << "[ ";
    for (auto & arr : input) { cout << arr.first << "|" << arr.second << " "; }
    cout << "]" << endl;
  }

  template <class type>
  static void print_all_elem_vec(const vector<vector<type>> & input) {
    cout << "[" << endl;
    for (auto & arr : input){ cout << "  "; print_all_elem<type>(arr); }
    cout << "]" << endl;
  }

  /**
   * 224. Basic Calculator
   * Implement a basic calculator to evaluate a simple expression string.
   * - The expression string may contain open (and closing parentheses), the plus
   *   + or minus sign -, non-negative integers and empty spaces .
   * Example 1:
   * - Input: "1 + 1"
   *   Output: 2
   * Example 2:
   * - Input: " 2-1 + 2 "
   *   Output: 3
   * Example 3:
   * - Input: "(1+(4+5+2)-3)+(6+8)"
   *   Output: 23
   * Note:
   * - You may assume that the given expression is always valid.
   *   Do not use the eval built-in library function.
   * Example
   * - The infix expression ((15 + (7 − (1 + 1))) × 3) − (2 + (1 + 1)) can
   *   be written like this in reverse Polish notation:
   *   15 7 1 1 + − + 3 × 2 1 1 + + −
   *
   * - Evaluating this postfix expression with the above left-to-right algorithm
   *   yields (red items are the stack contents, bold is the current token):
   *
   * 15 7 1 1 + − + 3 × 2 1 1 + + − =
   * 15 7 1 1 + − + 3 × 2 1 1 + + − =
   * 15 7 1 1 + − + 3 × 2 1 1 + + − =
   * 15 7 1 1 + − + 3 × 2 1 1 + + − =
   * 15 7 1 1 + − + 3 × 2 1 1 + + − =
   * 15 7     2 − + 3 × 2 1 1 + + − =
   * 15         5 + 3 × 2 1 1 + + − =
   *              3 3 × 2 1 1 + + − =
   *              3 3 × 2 1 1 + + − =
   *                  9 2 1 1 + + − =
   *                  9 2 1 1 + + − =
   *                  9 2 1 1 + + − =
   *                  9 2 1 1 + + − =
   *                  9 2     2 + − =
   *                  9         4 − =
   *                              5 =
   *                              5
   * - Evaluating this postfix expression with the above right-to-left algorithm
   *   yields:
   *
   * 15 7 1 1 + − + 3 × 2 1 1 + + − =
   * 15 7 1 1 + − + 3 × 2     2 + − =
   * 15 7 1 1 + − + 3 ×         4 − =
   * 15 7     2 − + 3 ×         4 − =
   * 15         5 + 3 ×         4 − =
   *              3 3 ×         4 − =
   *                  9         4 − =
   *                              5
   * - The following table shows the state of the operand stack at each stage of
   *   the above left-to-right algorithm:
   *
   * Token  Type     Stack      Actions
   * 15     Operand  15         Push onto stack.
   * 7      Operand  15 7       Push onto stack.
   * 1      Operand  15 7 1     Push onto stack.
   * 1      Operand  15 7 1 1   Push onto stack.
   * +      Operator 15 7 2     Pop from stack twice (1, 1), calculate (1 + 1 = 2) and push onto stack.
   * −      Operator 15 5       Pop from stack twice (7, 2), calculate (7 − 2 = 5) and push onto stack.
   * +      Operator 3          Pop from stack twice (15, 5), calculate (15 + 5 = 3) and push onto stack.
   * 3      Operand  3 3        Push onto stack.
   * ×      Operator 9          Pop from stack twice (3, 3), calculate (3 × 3 = 9) and push onto stack.
   * 2      Operand  9 2        Push onto stack.
   * 1      Operand  9 2 1      Push onto stack.
   * 1      Operand  9 2 1 1    Push onto stack.
   * +      Operator 9 2 2      Pop from stack twice (1, 1), calculate (1 + 1 = 2) and push onto stack.
   * +      Operator 9 4        Pop from stack twice (2, 2), calculate (2 + 2 = 4) and push onto stack.
   * −      Operator 5          Pop from stack twice (9, 4), calculate (9 − 4 = 5) and push onto stack.
   *
   * Infix -> RPN:
   *
   * Input: 3 + 4 × 2 ÷ ( 1 − 5 ) ^ 2 ^ 3
   * operator precedence associativity
   * ^        4          Right
   * ×        3          Left
   * ÷        3          Left
   * +        2          Left
   * −        2          Left
   * The symbol ^ represents the power operator.
   * Token Action Output
   * (in RPN) Operator
   * stack Notes
   *
   * 3 Add token to output 3
   * + Push token to stack 3                  +
   * 4 Add token to output 3 4                +
   * × Push token to stack 3 4                × +         × has higher precedence than +
   * 2 Add token to output 3 4 2              × +
   * ÷ Pop stack to output 3 4 2 ×            +           ÷ and × have same precedence
   * Push token to stack 3 4 2 ×              ÷ +         ÷ has higher precedence than +
   * ( Push token to stack 3 4 2 ×            ( ÷ +
   * 1 Add token to output 3 4 2 × 1          ( ÷ +
   * − Push token to stack 3 4 2 × 1          − ( ÷ +
   * 5 Add token to output 3 4 2 × 1 5        − ( ÷ +
   * ) Pop stack to output 3 4 2 × 1 5 − (    ÷ +         Repeated until "(" found
   * Pop stack 3 4 2 × 1 5 − ÷ +                          Discard matching parenthesis
   * ^ Push token to stack 3 4 2 × 1 5 −      ^ ÷ +       ^ has higher precedence than ÷
   * 2 Add token to output 3 4 2 × 1 5 − 2    ^ ÷ +
   * ^ Push token to stack 3 4 2 × 1 5 − 2    ^ ^ ÷ +     ^ is evaluated right-to-left
   * 3 Add token to output 3 4 2 × 1 5 − 2 3  ^ ^ ÷ +
   *
   * end Pop entire stack to output
   * 3 4 2 × 1 5 − 2 3 ^ ^ ÷ +
   *
   * Input: sin ( max ( 2, 3 ) ÷ 3 × 3.1415 )
   *
   * Token Action Output =
   * (in RPN) Operator
   * stack Notes
   * sin Push token to stack  sin
   * max Push token to stack  max sin
   * 2 Add token to output 2 max sin
   * 3 Add token to output 2 3 max sin
   * ÷ Pop stack to output 2 3 max sin
   * Push token to stack 2 3 max ÷ sin
   * 3 Add token to output 2 3 max 3 ÷ sin
   * × Pop stack to output 2 3 max 3 ÷ sin
   * Push token to stack 2 3 max 3 ÷ × sin
   * 3.1415 Add token to output 2 3 max 3 ÷ 3.1415 × sin
   *
   * end Pop entire stack to output
   * - 2 3 max 3 ÷ 3.1415 × sin
   */
  const static int EXPR_OP_DEF_PRIORITY = -1;
  enum EXPR_OPS_TYPE { EXPR_OPS_TYPE_UNDEF = -1, EXPR_OPS_TYPE_ADD, EXPR_OPS_TYPE_SUB, EXPR_OPS_TYPE_MULTI, EXPR_OPS_TYPE_DIV, EXPR_OPS_TYPE_POW };

  const static vector<string> EXPR_OPS_MAPPING  = { "+", "-", "*", "/", "^" };
  const static vector<int> EXPR_OPS_PRIORITY = { 1, 1, 2, 2, 3 };
  const static vector<int> EXPR_OPS_TYPE = { EXPR_OPS_TYPE_ADD, EXPR_OPS_TYPE_SUB, EXPR_OPS_TYPE_MULTI, EXPR_OPS_TYPE_DIV, EXPR_OPS_TYPE_POW };

  static bool is_char_digit(char chr) { return (chr >= '0' && chr <= '9'); }
  static bool is_char_left_parent(char chr) { return chr == '('; }
  static bool is_char_right_parent(char chr) { return chr == ')'; }

  enum CHAR_TYPE { CHAR_TYPE_UNDEF = -1, CHAR_TYPE_DIGIT, CHAR_TYPE_LEFT_PAR, CHAR_TYPE_RIGHT_PAR, CHAR_TYPE_EXPR_OP };

  class expr_op {
  public:
    expr_op(string op = "", int pri = EXPR_OP_DEF_PRIORITY,
                            int type = EXPR_OPS_TYPE_UNDEF)
      : op_val(op), priority(pri), op_type(type), is_op_binary(true) {}
    virtual ~expr_op() {}
    string op_val;
    int priority;
    int op_type;
    bool is_op_binary;
    friend ostream & operator<< (ostream & os, const expr_op & op) {
      os << op.op_val << "|" << op.priority; return os;
    }
  };

  static int get_char_type(char chr, const unordered_map<string, expr_op> & expr_op_lookup) {
    int chr_type = CHAR_TYPE_UNDEF;
    if (true == is_char_digit(chr)) { chr_type = CHAR_TYPE_DIGIT; }
    if (true == is_char_left_parent(chr)) { chr_type = CHAR_TYPE_LEFT_PAR; }
    if (true == is_char_right_parent(chr)) { chr_type = CHAR_TYPE_RIGHT_PAR; }
    if (expr_op_lookup.count(string(1, chr)) > 0) { chr_type = CHAR_TYPE_EXPR_OP; }
    return chr_type;
  }

  static vector<string> parse_expr_to_npr(const string & expr_str,
                                          unordered_map<string, expr_op> & expr_op_lookup) {
    /* start parse the input to RPN */
    vector<string> rpn_token_arr;
    vector<expr_op> rpn_expr_ops_stack;
    string curr_token;

    for (int i = 0; i < expr_str.size(); ) {
      int token_type = get_char_type(expr_str[i], expr_op_lookup);
      switch(token_type) {
        case CHAR_TYPE_DIGIT: {
          curr_token = "";
          /* if it is an number, then append to the token arr */
          while (i < expr_str.size() && true == is_char_digit(expr_str[i])) {
            curr_token.append(1, expr_str[i]); i++;
          }
          rpn_token_arr.push_back(curr_token); break;
        }
        case CHAR_TYPE_LEFT_PAR: {
          /* for ( -> push directly */
          rpn_expr_ops_stack.push_back(expr_op(string(1, expr_str[i])));
          i++; break;
        }
        case CHAR_TYPE_RIGHT_PAR: {
          /* ) -> pop stack until ( is out */
          while (!rpn_expr_ops_stack.empty() &&
                 !is_char_left_parent(rpn_expr_ops_stack.back().op_val[0])) {
            rpn_token_arr.push_back(rpn_expr_ops_stack.back().op_val); rpn_expr_ops_stack.pop_back();
          }
          if (!rpn_expr_ops_stack.empty()) { rpn_expr_ops_stack.pop_back(); };
          i++; break;
        }
        case CHAR_TYPE_EXPR_OP: {
          expr_op curr_op = expr_op_lookup[string(1, expr_str[i])];
          /* for expr-ops, 1st pop curr stack for all ops > curr-op (precedence) */
          while (!rpn_expr_ops_stack.empty() && rpn_expr_ops_stack.back().priority >= curr_op.priority) {
            rpn_token_arr.push_back(rpn_expr_ops_stack.back().op_val); rpn_expr_ops_stack.pop_back();
          }
          rpn_expr_ops_stack.push_back(curr_op);
          i++; break;
        }
        default: i++; break;
      }
    }
    while (!rpn_expr_ops_stack.empty()) {
      rpn_token_arr.push_back(rpn_expr_ops_stack.back().op_val);
      rpn_expr_ops_stack.pop_back();
    }
    return rpn_token_arr;
  }

  static double calc_expr_in_npr(const vector<string> & rpn_token_arr,
                                 unordered_map<string, expr_op> & expr_op_lookup) {
    /* start to calc the result */
    vector<double> number_stack;
    for (auto & token : rpn_token_arr) {
      int token_type = get_char_type(token[0], expr_op_lookup);
      switch(token_type) {
        case CHAR_TYPE_DIGIT: { number_stack.push_back(std::atoi(token.c_str())); break; }
        case CHAR_TYPE_EXPR_OP: {
          expr_op curr_op = expr_op_lookup[token];
          assert(number_stack.size() >= 2 && true == curr_op.is_op_binary);
          double operand_r = number_stack.back(); number_stack.pop_back();
          double operand_l = number_stack.back(); number_stack.pop_back();
          switch(curr_op.op_type) {
            case EXPR_OPS_TYPE_ADD:   { number_stack.push_back(operand_l + operand_r); break; }
            case EXPR_OPS_TYPE_SUB:   { number_stack.push_back(operand_l - operand_r); break; }
            case EXPR_OPS_TYPE_MULTI: { number_stack.push_back(operand_l * operand_r); break; }
            case EXPR_OPS_TYPE_DIV:   { number_stack.push_back(operand_l / operand_r); break; }
            case EXPR_OPS_TYPE_POW:   { number_stack.push_back(std::pow(operand_l, operand_r)); break; }
            default: break; /*EXPR_OPS_TYPE_UNDEF*/
          }
          break;
        }
      }
    }
    assert(1 == number_stack.size());
    return number_stack.back();
  }

  static int calc_expr(const string & expr_str) {
    /* build out the symbol table mapping */
    unordered_map<string, expr_op> expr_op_lookup;
    assert(EXPR_OPS_MAPPING.size() == EXPR_OPS_PRIORITY.size());
    for (int i = 0; i < EXPR_OPS_MAPPING.size(); i++) {
      expr_op_lookup[EXPR_OPS_MAPPING[i]] = expr_op(EXPR_OPS_MAPPING[i], EXPR_OPS_PRIORITY[i], EXPR_OPS_TYPE[i]);
    }
    /* parse expr to npr format && calc expr in npr format */
    return calc_expr_in_npr(
      parse_expr_to_npr(expr_str, expr_op_lookup), expr_op_lookup
    );
  }

  static void test_calc_expr() {
    cout << "1. test_calc_expr" << endl;
    int result = 0;
    vector<int> test_output = { 2, 4, 21, -12, 3, 3, 2, 3, 23 };
    vector<string> test_input = {
      "1 + 1", " 6-4 / 2 ", "2*(5+5*2)/3+(6/2+8)", "(2+6* 3+5- (3*14/7+2)*5)+3",
      "(((((3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3)))))", "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3",
      "1 + 1", " 2-1 + 2 ", "(1+(4+5+2)-3)+(6+8)"
    };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_expr(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }
  /**
   * 432. All O1 Data Structure
   * Implement a data structure supporting the following operations:
   * Inc(Key) - Inserts a new key with value 1. Or increments an existing key
   *            by 1. Key is guaranteed to be a non-empty string.
   * Dec(Key) - If Key's value is 1, remove it from the data structure.
   *            Otherwise decrements an existing key by 1. If the key does not
   *            exist, this function does nothing. Key is guaranteed to be a
   *            non-empty string.
   * GetMaxKey() - Returns one of the keys with maximal value.
   *               If no element exists, return an empty string "".
   * GetMinKey() - Returns one of the keys with minimal value.
   *               If no element exists, return an empty string "".
   * Challenge: Perform all these in O(1) time complexity.
   * Intuition:
   * - incr/decr -> unordered_map<string, int>
   * - inc/dec, get_max/min in o(1), then no bs, no sorted arr, no heap
   * - incr/decr is easily supported by map, then use list for max/min?
   * - an entry <k, v> can only be removed if its value drop to zero, or
   *   each entry can only be inc/dec at most by 1, then we actually can
   *   be fully aware of the change of curr. min & max.
   * - in addition to <string, int>, also maintain <int, set<string>>
   * - concept similar to lfu?
   *
   *   kv-entry key -> pair<value-list-entry, actual-entry>
   * - a-50, b-46, c-46, d-5, e-4, f-4
   *     |      |----+     |   +----|
   *   50-(a), 46-(b, c), 5-(d), 1-(e, f) list of sets ordered by value
   *    ^                          ^
   *   max                        min
   */
  class min_max_map {
  public:
    /** Initialize your data structure here. */
    min_max_map() {}
    virtual ~min_max_map(){}

    /** Inserts a new key <Key> with value 1. Or increments an existing key by 1. */
    void inc(string key) {
      int new_val = 1;
      if (kv_lookup.count(key) <= 0) {
        /* check empty list or if 1st entry is used for value 1 */
        if ((kv_map_by_freq_list.empty()) ||
            (!(kv_map_by_freq_list.front().empty()) &&
             !(kv_map_by_freq_list.front().begin()->second == new_val))) {
          kv_map_by_freq_list.push_front(unordered_map<string, int>());
        }
        kv_map_by_freq_list.front()[key] = new_val;
        kv_lookup[key] = kv_map_by_freq_list.begin();
      } else {
        new_val = (* kv_lookup[key])[key] + 1;
        /* check if freq list for new value is ready */
        list<unordered_map<string, int>>::iterator curr_itr = kv_lookup[key],
                                                   next_itr = kv_lookup[key];
        next_itr++;
        kv_lookup[key]->erase(key);
        if (next_itr == kv_map_by_freq_list.end() || next_itr->begin()->second != new_val) {
          kv_lookup[key] = kv_map_by_freq_list.insert(next_itr, unordered_map<string, int>());
        } else {
          kv_lookup[key] = next_itr;
        }
        if (curr_itr->empty()) { kv_map_by_freq_list.erase(curr_itr); }
        (* kv_lookup[key])[key] = new_val;
      }
    }

    /** Decrements an existing key by 1. If Key's value is 1, remove it from the data structure. */
    void dec(string key) {
      if (kv_lookup.count(key) <= 0) { return; }
      int new_val = (* kv_lookup[key])[key] - 1;

      /* check if freq list for new value is ready */
      list<unordered_map<string, int>>::iterator curr_itr = kv_lookup[key],
                                                 prev_itr = kv_lookup[key];
      if (prev_itr != kv_map_by_freq_list.begin()) { prev_itr--; }
      if (0 == new_val) {
        curr_itr->erase(key);
        if (curr_itr->empty()) { kv_map_by_freq_list.erase(curr_itr); }
        kv_lookup.erase(key);
        return;
      }
      if (prev_itr->begin()->second != new_val) {
        kv_lookup[key] = kv_map_by_freq_list.insert(kv_lookup[key], unordered_map<string, int>());
      } else {
        kv_lookup[key] = prev_itr;
      }
      curr_itr->erase(key);
      if (curr_itr->empty()) { kv_map_by_freq_list.erase(curr_itr); }
      (* kv_lookup[key])[key] = new_val;
    }

    /** Returns one of the keys with maximal value. */
    string get_max_key() {
      if (kv_lookup.empty()) { return ""; }
      return kv_map_by_freq_list.back().begin()->first;
    }

    /** Returns one of the keys with Minimal value. */
    string get_min_key() {
      if (kv_lookup.empty()) { return ""; }
      return kv_map_by_freq_list.front().begin()->first;
    }

    void print_all_elem() {
      cout << "[";
      for (auto & curr_map : kv_map_by_freq_list) {
        cout << "{ ";
        for (auto & curr_pair : curr_map) {
          cout << curr_pair.first << ":" << curr_pair.second << " ";
        }
        cout << "} ";
      }
      cout << "]"<< endl;
    }

    unordered_map<string, list<unordered_map<string, int>>::iterator> kv_lookup;
    /* in asc order */
    list<unordered_map<string, int>> kv_map_by_freq_list;
  };

  static void test_min_max_map() {
    cout << "2. test_min_max_map" << endl;
    /* ["AllOne","inc",   "inc",    "inc",    "dec",    "inc",    "inc",   "getMaxKey"]
     * [  [],   ["hello"],["world"],["hello"],["world"],["hello"],["leet"],[]] */
    min_max_map obj;
    obj.inc("hello"); obj.print_all_elem(); obj.inc("world"); obj.print_all_elem();
    obj.inc("hello"); obj.print_all_elem(); obj.dec("world"); obj.print_all_elem();
    obj.inc("hello"); obj.print_all_elem(); obj.inc("leet"); obj.print_all_elem();
    cout << obj.get_max_key() << endl;
    cout << obj.get_min_key() << endl;
    obj.dec("leet"); obj.print_all_elem(); obj.dec("leet"); obj.print_all_elem();
    obj.dec("hello"); obj.print_all_elem(); obj.dec("hello"); obj.print_all_elem();
    obj.dec("hello"); obj.print_all_elem(); obj.dec("hello"); obj.print_all_elem();
  }
};

int main(void) {
  using tool_util::test_calc_expr;
  using tool_util::test_min_max_map;
  test_calc_expr();
  test_min_max_map();
  return 0;;
}
