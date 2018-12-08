#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
#include <unordered_map>
#include <unordered_set>
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
  using std::unordered_set;

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

  /**
   * 707. Optimal Account Balancing
   * - Given a directed graph where each edge is represented by a tuple, such as
   *   [u, v, w] represents an edge with a weight w from u to v.
   * - You need to calculate at least the need to add the number of edges to
   *   ensure that each point of the weight are balancing. That is, the sum of
   *   weight of the edge pointing to this point is equal to the sum of weight
   *   of the edge of the point that points to the other point.
   * For example:
   * - Given a graph [[0,1,10],[2,0,5]] 0 give 1 $10
   *   Return 2
   *   Two edges are need to added. There are [1,0,5] and [1,2,5]
   * - Given a graph [[0,1,10],[1,0,1],[1,2,5],[2,0,5]]
   *   Return 1
   *   Only one edge need to added. There is [1,0,4]
   * Notice
   * 1.Note that u <> v and w > 0.
   * 2.index may not be linear, e.g. we could have the points 0, 1, 2 or we
   *   could also have the points 0, 2, 6.
   * Intuition:
   * - all src max flow?
   */
  static int get_settlement_cost(vector<int> & balances,
                                 int curr_id, int cost) {
    while (curr_id < balances.size() && 0 == balances[curr_id]) { curr_id++; }
    int total_cost = std::numeric_limits<int>::max();
    for (long i = curr_id + 1, prev_bal = 0; i < balances.size(); i++) {
      if (balances[i] != prev_bal && balances[i] * balances[curr_id] < 0) {
        balances[i] += balances[curr_id];
        total_cost = std::min(total_cost, get_settlement_cost(balances, curr_id + 1, cost + 1));
        balances[i] -= balances[curr_id];
        prev_bal = balances[i];
      }
    }
    return total_cost < std::numeric_limits<int>::max() ? total_cost : cost;
  }

  static int calc_min_trxns(vector<vector<int>> & trxns) {
    unordered_map<int, long> balance_map;
    for (auto & trxn : trxns) { balance_map[trxn[0]] -= trxn[2];
                                balance_map[trxn[1]] += trxn[2]; }
    /* record all outstanding balance, either credit or debt */
    vector<int> outstanding_balances;
    for (auto & balance_pair : balance_map) {
      if (balance_pair.second != 0) {
        outstanding_balances.push_back(balance_pair.second);
      }
    }
    /* start to resolve balances with min trxns, [ -5, 3, 2, 10, -10 ] */
    return get_settlement_cost(outstanding_balances, 0, 0);
  }

  static void test_calc_min_trxns() {
    int result = 0;
    vector<int> test_output = { 2, 1 };
    vector<vector<vector<int>>> test_input = { {{0,1,10},{2,0,5}}, {{0,1,10},{1,0,1},{1,2,5},{2,0,5}} };
    for (int i = 0; i < test_input.size(); i++) {
      result = calc_min_trxns(test_input[i]);
      cout << result << " <=> " << test_output[i] << endl;
      assert(result == test_output[i]);
    }
  }

  /**
   * 398. Random Pick Index
   * - Given an array of integers with possible duplicates, randomly output the
   *   index of a given target number. You can assume that the given target
   *   number must exist in the array.
   * Note:
   * - The array size can be very large. Solution that uses too much extra space
   *   will not pass the judge.
   * Example:
   * - int[] nums = new int[] {1,2,3,3,3};
   * - Solution solution = new Solution(nums);
   * - pick(3) should return either index 2, 3, or 4 randomly. Each index should
   *   have equal probability of returning.
   * - solution.pick(3);
   * - pick(1) should return 0. Since in the array only nums[0] is equal to 1.
   * - solution.pick(1);
   * Intuition:
   * - no extra space is required, but only 1 id from target is needed.
   * - input may not be sorted.
   * - all we care are those indices related to the target number.
   * - only 1 -> 1/1, { 1, 1 } -> 50% each, { 1, 1, 1 } -> p 33% of each idx.
   *      rand() % 1  rand() % 2 == 0       rand() % 3 == 0
   *                               { 0, 1, 2 } -> 0 -> 1 * 1/2 * 2/3 -> 1/3
   *                                              1 -> 1/2 * 2/3 -> 1/3
   *                                              2 -> 1/3
   * - maintain a cnt of curr target number, each time try replace existing id
   *   if we see a new target.
   */
  static int random_pick_index_by_value(const vector<int> & input, int target) {
    int target_cnt = 0, target_id = 0;
    for (int i = 0; i < input.size(); i++) {
      if (input[i] == target) {
        target_cnt += 1;
        if (0 == std::rand() % target_cnt) {
          target_id = i;
        }
      }
    }
    return target_id;
  }

  /**
   * 528. Random Pick with Weight
   * - Given an array w of positive integers, where w[i] describes the weight
   *   of index i, write a function pickIndex which randomly picks an index in
   *   proportion to its weight.
   * Note:
   * - 1 <= w.length <= 10000
   * - 1 <= w[i] <= 10^5
   * - pickIndex will be called at most 10000 times.
   * Example 1:
   * - Input:
   * - ["Solution","pickIndex"]
   * - [[[1]],[]]
   * - Output: [null,0]
   * Example 2:
   * - Input:
   * - ["Solution","pickIndex","pickIndex","pickIndex","pickIndex","pickIndex"]
   * - [[[1,3]],[],[],[],[],[]]
   * - Output: [null,0,1,1,1,0]
   * Explanation of Input Syntax:
   * - The input is two lists: the subroutines called and their arguments.
   * - Solution's constructor has one argument, the array w.
   * - pickIndex has no arguments. Arguments are always wrapped with a list,
   *   even if there aren't any
   * Intuition:
   * - simplest assumption for all equal weights { 1, 1, 1, 1 }
   *   then each index has a p dist. of 1/4, simply do random() % 4
   * - for { 1, 3, 2, 4 }, p dist. should be { 1/10, 3/10, 2/10, 4/10 }
   * - ideally our method should be adptive, one pass & const space.
   *   the goal is to return just one random idx consistent with w dist
   *   { 1 } -> 1, 1
   *   { 1, 3 } -> idx(1) sum(4), curr-val(3) -> 3/4 of p that we pick 1 => (random() % 4) < curr-val(3)
   *   { 1, 3, 2 } -> for 2, -> random % 6 < 2, for 3 -> 3/4 * 4/6 (pick 3 & not pick 2) => 3/6 Y
   *               -> for 1, -> 1 * 1/4 * 4/6 -> 1/6 (pick 1 & not pick 3 & 2);
   * - could expensive to gen random each time, then sum up 1st, then do a bsearch.
   *   { 1, 3, 2, 4 }
   *   { 1, 4, 6, 10 } => { 0 ~ 1, 1 ~ 4, 4 ~ 6, 6 ~ 10 }
   */
  static int fast_random_pick_index_by_weight(const vector<int> & weight_arr) {
    vector<int> prefix_sum;
    int curr_sum = 0, target_id = INT_MIN;
    for (int i = 0; i < weight_arr.size(); i++) {
      curr_sum += weight_arr[i];
      prefix_sum.push_back(curr_sum);
    }
    int target_sum = (random() % curr_sum) + 1;
    int low = 0, high = prefix_sum.size() - 1, mid = 0;
    while (low <= high) {
      mid = (low + high) / 2;
      if (prefix_sum[mid] == target_sum) { target_id = mid; break; }
      else if (prefix_sum[mid] < target_sum) { low = mid + 1; }
      else { high = mid - 1; }
    }
    /* enter here, low & high actually crossed(as value is surely within range) */
    if (target_id != mid) { target_id = std::max(low, high); }

    return target_id;
  }

  static int random_pick_index_by_weight(const vector<int> & weight_arr) {
    int curr_sum = 0, target_id = 0;
    for (int i = 0; i < weight_arr.size(); i++) {
      curr_sum += weight_arr[i];
      if ((random() % curr_sum) < weight_arr[i]) {
        target_id = i;
      }
    }
    return target_id;
  }

  /**
   * 710. Random Pick with Blacklist
   * - Given a blacklist B containing unique integers from [0, N), write a
   *   function to return a uniform random integer from [0, N) NOT in B.
   * - Optimize it such that it minimizes the call to system’s Math.random().
   * Note:
   * - 1 <= N <= 1,000,000,000
   * - 0 <= B.length < min(100,000, N)
   * - [0, N) does NOT include N. See interval notation.
   * Intuition:
   * - bf way could be init every single possible value & pick random id
   *   while not realistic as it won't fits.
   * - another way could be sort the backlist, such that we get a bunch of
   *   interupted intervals, & all we need to do is just do a range sum like
   *   op to get the right entry point
   * - ideally, goal is to pre-process, run random & return;
   * - given the size of backlist is < min(100,000, N), then sorting should
   *   be good, with produced intervals no more than 10k.
   * Plan will be:
   * - sort the black list asc 1st.
   * - based on the sorting result, generate disjoint intervals with min & max
   *   with each interval has diff weight with value wi of elems cnt
   * - random pick a range (% # of interval), & within range, random pick val.
   *   [ 0, 4 ], [ 6, 8 ] [ 11, 11 ]
   *       5         3         1       -> total 9 -> 3 | 5/9 * 1/5 => 1/9
   */
  class filtered_random_picker {
  public:
    class range_interval {
    public:
      range_interval(int mi, int ma) :
        min(mi), max(ma), elem_cnt(ma - mi) { assert(max > min); }
      virtual ~range_interval() {}
      int pick_random_val() { return (min + (random() % elem_cnt)); }
      int min, max, elem_cnt;
      /* [ min, max), min included, not max */
    };
  public:
    filtered_random_picker(int max_val, vector<int> blacklist) {
      init_filtered_random_picker(max_val, blacklist);
    }
    virtual ~filtered_random_picker() {}
    int random_pick_value_with_fitler() {
      /* start to pick the range based on weight */
      int low = 0, mid = 0, high = prefix_range_elem_cnt.size() - 1;
      int range_to_pick = random() % total_elem_cnt + 1;
      while (low <= high) {
        mid = (low + high) / 2;
        if (prefix_range_elem_cnt[mid] == range_to_pick) { range_to_pick = mid; break; }
        else if (prefix_range_elem_cnt[mid] < range_to_pick) { low = mid + 1; }
        else { high = mid - 1; }
      }
      if (mid != range_to_pick) { range_to_pick = std::max(low, high); }
      return interval_arr[range_to_pick].pick_random_val();
    }
  private:
    void init_filtered_random_picker(int max_val, vector<int> blacklist) {
      int curr_min = 0;
      sort(blacklist.begin(), blacklist.end());
      /* curr_min will always be the min of any new interval */
      for (int i = 0; i < blacklist.size(); i++) {
        if (curr_min < blacklist[i]) {
          interval_arr.push_back(range_interval(curr_min, blacklist[i]));
          total_elem_cnt += interval_arr.back().elem_cnt;
          prefix_range_elem_cnt.push_back(total_elem_cnt);
        }
        curr_min = blacklist[i] + 1;
      }
      if (curr_min < max_val) {
        interval_arr.push_back(range_interval(curr_min, max_val));
        total_elem_cnt += interval_arr.back().elem_cnt;
        prefix_range_elem_cnt.push_back(total_elem_cnt);
      }
    }

    vector<range_interval> interval_arr;
    vector<int> prefix_range_elem_cnt;
    int total_elem_cnt = 0;
  };

  static void test_filtered_random_picker() {
    std::cout << "4. test_filtered_random_picker" << std::endl;
    filtered_random_picker frp(1000000000, vector<int>({ 0, 999, 9999, 99999, 1000,9999999, 999999999 }));
    std::cout << frp.random_pick_value_with_fitler() << std::endl;
    filtered_random_picker frp1(1, vector<int>({ }));
    std::cout << frp1.random_pick_value_with_fitler() << std::endl;
  }

  /**
   * 380. Insert Delete GetRandom O(1)
   * - Design a data structure that supports all following operations in
   *   average O(1) time.
   * - insert(val): Inserts an item val to the set if not already present.
   * - remove(val): Removes an item val from the set if present.
   * - getRandom: Returns a random element from current set of elements.
   *   Each element must have the same probability of being returned.
   * Example:
   *   // Init an empty set.
   *   RandomizedSet randomSet = new RandomizedSet();
   *   // Inserts 1 to the set. Returns true as 1 was inserted successfully.
   *   randomSet.insert(1);
   *   // Returns false as 2 does not exist in the set.
   *   randomSet.remove(2);
   *   // Inserts 2 to the set, returns true. Set now contains [1,2].
   *   randomSet.insert(2);
   *   // getRandom should return either 1 or 2 randomly.
   *   randomSet.getRandom();
   *   // Removes 1 from the set, returns true. Set now contains [2].
   *   randomSet.remove(1);
   *   // 2 was already in the set, so return false.
   *   randomSet.insert(2);
   *   // Since 2 is the only number in the set, getRandom always return 2.
   *   randomSet.getRandom();
   * Intuition:
   * - insert & del needs to be o(1) -> needs to be ditch the order
   * - support get random uniformly from our set -> needs to have a "range"
   * - also range should be updated accordingly when set changed.
   * - random access requires value should be the key
   *   maintain a umap { value -> curr-id } & { curr-id -> value }, hard part
   *   comes when value del, then we have holes.
   * - say id range from 0 -> n, & we know the size of elem -> m, & max-id n
   *   then we want pick a valid random id, from [ 0 ~ curr-size - 1 ]
   * - idea:
   *   when delete, we just delete entry from map, while swap id in array.
   *   { 1 => 0, 3 => 1, 4 => 2, 5 => 3 } => { 1 => 1, 5 => 4 }
   *   [ 1 3 4 5 ] -> rm 3 -> [ 1 5 4 | 3 ], { 1 => 0, 4 => 2, 5 => 3 }
   *                                       =>{ 1 => 0, 4 => 2, 5 => 1 }
   *   each time swap the del elem to the end & pop, while upd its entry in map.
   */
  class random_set {
  public:
    bool insert_item(int val) {
      if (value_to_id_map.count(val) > 0) { return false; }
      int curr_id = value_to_id_map.size();
      value_to_id_map[val] = curr_id;
      value_arr.push_back(val);
      return true;
    }

    bool remove_item(int val) {
      if (value_to_id_map.count(val) <= 0) { return false; }
      int idx_in_id_arr_to_swap = value_to_id_map[val];
      std::swap(value_arr[idx_in_id_arr_to_swap], value_arr[value_arr.size() - 1]);
      value_to_id_map[value_arr[idx_in_id_arr_to_swap]] = idx_in_id_arr_to_swap;
      value_to_id_map.erase(val);
      value_arr.pop_back();
      return true;
    }

    int pick_random_item() {
      return value_arr[random() % value_arr.size()];
    }
    random_set() {}
    virtual ~random_set() {}

    unordered_map<int, int> value_to_id_map;
    vector<int> value_arr;
  };

  static void test_random_set() {
    cout << "5. test_random_set" << endl;
    random_set rf;
    rf.insert_item(0);
    rf.insert_item(1);
    rf.remove_item(0);
    rf.insert_item(2);
    rf.remove_item(1);
    cout << rf.pick_random_item() << " <=> " << 2 << endl;
    assert(2 == rf.pick_random_item());
  }

  /**
   * 381. Insert Delete GetRandom O(1) - Duplicates allowed
   * - Design a data structure that supports all following operations in
   *   average O(1) time.
   * Note: Duplicate elements are allowed.
   * - insert(val): Inserts an item val to the collection.
   * - remove(val): Removes an item val from the collection if present.
   * - getRandom: Returns a random element from current collection of elements.
   *              The probability of each element being returned is linearly
   *              related to the number of same value the collection contains.
   */
  class dup_random_set {
  public:
    bool insert_item(int val) {
      int is_elem_uniq = true;
      if (value_to_id_map.count(val) > 0) {
        is_elem_uniq = false;
      } else {
        value_to_id_map[val] = unordered_set<int>();
      }
      int curr_id = value_arr.size();
      value_to_id_map[val].insert(curr_id);
      value_arr.push_back(val);
      return is_elem_uniq;
    }

    bool remove_item(int val) {
      if (value_to_id_map.count(val) <= 0) { return false; }
      if (val != value_arr.back()) {
        int idx_in_id_arr_to_swap = * (value_to_id_map[val].begin());
        std::swap(value_arr[idx_in_id_arr_to_swap], value_arr[value_arr.size() - 1]);
        value_to_id_map[value_arr[idx_in_id_arr_to_swap]].erase(value_arr.size() - 1);
        value_to_id_map[value_arr[idx_in_id_arr_to_swap]].insert(idx_in_id_arr_to_swap);
      }
      value_to_id_map[val].erase(value_to_id_map[val].begin());
      if (value_to_id_map[val].empty()) { value_to_id_map.erase(val); }
      value_arr.pop_back();

      return true;
    }

    void print_set() {
      print_all_elem<int>(value_arr);
      for (auto & p : value_to_id_map) {
        cout << p.first << ": "; for (auto x : p.second) { cout << x << " "; }; cout << " | ";
      }
      cout << endl;
    }

    void clear() {
      value_to_id_map.clear(); value_arr.clear();
    }

    int pick_random_item() {
      return value_arr[random() % value_arr.size()];
    }
    dup_random_set() {}
    virtual ~dup_random_set() {}

    unordered_map<int, unordered_set<int>> value_to_id_map;
    vector<int> value_arr;
  };

  static void test_dup_random_set() {
    cout << "6. test_dup_random_set" << endl;
    dup_random_set rf;
    rf.insert_item(10);
    rf.insert_item(10);
    rf.insert_item(20);
    rf.insert_item(20);
    rf.insert_item(30);
    rf.insert_item(30);
    rf.remove_item(10);
    rf.remove_item(10);
    rf.remove_item(30);
    rf.remove_item(30);
    for (int i = 0; i < 10; i++) {
      assert(20 == rf.pick_random_item());
    }
    rf.clear();
    rf.insert_item(1);
    rf.insert_item(1);
    rf.insert_item(2);
    rf.insert_item(1);
    rf.insert_item(2);
    rf.insert_item(2);
    rf.remove_item(1);
    rf.remove_item(2);
    rf.remove_item(2);
    rf.remove_item(2);
    for (int i = 0; i < 10; i++) {
      assert(1 == rf.pick_random_item());
    }
  }

  /**
   * 282. Expression Add Operators
   * - Given a string that contains only digits 0-9 and a target value, return
   *   all possibilities to add binary operators (not unary) +, -, or * between
   *   the digits so they evaluate to the target value.
   * Example 1:
   * - Input: num = "123", target = 6
   * - Output: ["1+2+3", "1*2*3"] 
   * Example 2:
   * - Input: num = "232", target = 8
   * - Output: ["2*3+2", "2+3*2"]
   * Example 3:
   * - Input: num = "105", target = 5
   * - Output: ["1*0+5","10-5"]
   * Example 4:
   * - Input: num = "00", target = 0
   * - Output: ["0+0", "0-0", "0*0"]
   * Example 5:
   * - Input: num = "3456237490", target = 9191
   * - Output: []
   */
   static vector<string> find_expr(const string & digits, int target) {
     vector<string> expr_arr;
     return expr_arr;
   }

   static void test_find_expr() {
     cout << "7. test_find_expr" << endl;
     vector<string> result;
     vector<int> test_target_input = { 6, 8, 5, 0, 9191 };
     vector<string> test_digit_input = {
       "123","232","105", "00","3456237490",
     };
     vector<vector<string>> test_output = {
       {"1+2+3", "1*2*3"}, {"2*3+2", "2+3*2"}, {"1*0+5","10-5"}, {"0+0", "0-0", "0*0"}, {}
     };
     for (int i = 0; i < test_digit_input.size(); i++) {
       result = find_expr(test_digit_input[i], test_target_input[i]);
       print_all_elem<string>(test_output[i]); cout << " <=>" << endl;
       print_all_elem<string>(result);
       assert(result.size() == test_output[i].size());
       for (int j = 0; j < result.size(); j++) { assert(result[j] == test_output[i][j]); }
     }
   }
};

int main(void) {
  using tool_util::test_calc_expr;
  using tool_util::test_min_max_map;
  using tool_util::test_calc_min_trxns;
  using tool_util::test_filtered_random_picker;
  using tool_util::test_random_set;
  using tool_util::test_dup_random_set;
  using tool_util::test_find_expr;

  test_calc_expr();
  test_min_max_map();
  test_calc_min_trxns();
  test_filtered_random_picker();
  test_random_set();
  test_dup_random_set();
  test_find_expr();

  return 0;;
}
