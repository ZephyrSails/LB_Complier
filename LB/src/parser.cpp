// by: Zhiping

#include <parser.h>

using namespace pegtl;
using namespace std;

namespace LB {
  // meta element
  struct comment:
    pegtl::disable<
      pegtl::one< ';' >,
      pegtl::until< pegtl::eolf >
    > {};

  struct seps:
    pegtl::star<
      pegtl::sor<
        pegtl::ascii::space,
        comment
      >
    > {};

  struct op:
    pegtl::sor<
      pegtl::string< '+' >,
      pegtl::string< '-' >,
      pegtl::string< '*' >,
      pegtl::string< '&' >,
      pegtl::string< '<', '<' >,
      pegtl::string< '>', '>' >,
      pegtl::string< '<', '=' >,
      pegtl::string< '<' >,
      pegtl::string< '=' >,
      pegtl::string< '>', '=' >,
      pegtl::string< '>' >
    > {};

  struct LB_type:
    pegtl::sor<
      pegtl::seq<
        pegtl::string< 'i', 'n', 't', '6', '4' >,
        pegtl::star<
          pegtl::string< '[', ']' >
        >
      >,
      pegtl::string< 't', 'u', 'p', 'l', 'e' >,
      pegtl::string< 'c', 'o', 'd', 'e' >
    > {};

  struct type:
    LB_type {};

  struct left_arrow:
    pegtl::string< '<', '-' > {};

  struct LB_name:
    pegtl::seq<
      pegtl::plus<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct LB_Label:
    pegtl::seq<
      pegtl::one<':'>,
      LB_name
    > {};

  struct LB_var:
    pegtl::seq<
      pegtl::one< '%' >,
      LB_name
    > {};

  struct LB_Num:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus<
        pegtl::digit
      >
    > {};

  struct LB_t:
    pegtl::sor<
      LB_var,
      LB_Num
    > {};

  struct LB_u:
    pegtl::sor<
      LB_var,
      LB_Label
    > {};

  struct LB_callee:
    pegtl::sor<
      LB_var,
      LB_name
    > {};
  //
  struct var:
    LB_var {};
  //
  struct Num:
    LB_Num {};
  //
  struct t:
    LB_t {};
  //
  struct label:
    LB_Label {};

  //
  // struct u:
  //   LB_u {};

  struct callee:
    LB_callee {};

  struct LB_vars:
    pegtl::seq<
      pegtl::rep_max< 1, var >,
      seps,
      pegtl::star<
        pegtl::seq<
          pegtl::one< ',' >,
          seps,
          var,
          seps
        >
      >
    > {};

  struct argv:
    pegtl::sor<
      pegtl::seq<
        t,
        seps,
        pegtl::star<
          pegtl::seq<
            pegtl::one< ',' >,
            seps,
            t,
            seps
          >
        >
      >,
      seps
    > {};

  struct vars:
    LB_vars {};

  struct call:
    pegtl::string< 'c', 'a', 'l', 'l' > {};

  struct LB_ins_call:
    pegtl::seq<
      call,
      seps, callee, seps,
      pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
    > {};


  struct varSqureT:
    pegtl::seq<
      LB_var,
      pegtl::star<
        pegtl::one< '[' >,
        LB_t,
        pegtl::one< ']' >
      >
    > {};

  struct length:
    pegtl::string< 'l', 'e', 'n', 'g', 't', 'h' > {};

  struct s_new:
    pegtl::string< 'n', 'e', 'w' > {};

  struct Array:
    pegtl::string < 'A', 'r', 'r', 'a', 'y' > {};

  struct Tuple:
    pegtl::string < 'T', 'u', 'p', 'l', 'e' > {};

  struct ins_v_start:
    pegtl::seq<
      varSqureT, seps, left_arrow, seps,
      pegtl::sor<
        pegtl::seq<
          s_new,
          seps,
          pegtl::sor<
                    //  pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
            pegtl::seq<
              Array, pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
            >,
            pegtl::seq<
              Tuple, pegtl::one< '(' >, seps, t, seps, pegtl::one< ')' >
            >
          >
        >,
        label,
        LB_ins_call,
        pegtl::seq<
          Num,
          pegtl::rep_max< 1,
            pegtl::seq< seps, op, seps, t>
          >
        >,
        pegtl::seq<
          varSqureT,
          pegtl::rep_max< 1,
            pegtl::seq< seps, op, seps, t>
          >
        >,
        pegtl::seq<
          length,
          seps,
          var,
          seps,
          t
        >
      >
    > {};

  struct store:
    pegtl::string< 's', 't', 'o', 'r', 'e' > {};

  struct ins_store:
    pegtl::seq<
      store,
      seps,
      var,
      seps,
      left_arrow,
      seps,
      pegtl::sor< label, t >
    > {};

  struct ins_if_start:
    pegtl::seq<
      pegtl::string< 'i', 'f' >,
      seps,
      pegtl::one< '(' >,
      seps, t, seps, op, seps, t, seps,
      pegtl::one< ')' >,
      seps,
      label,
      seps,
      label
    > {};

  struct ins_while:
    pegtl::seq<
      pegtl::string< 'w', 'h', 'i', 'l', 'e' >,
      seps,
      pegtl::one< '(' >,
      seps, var, seps, op, seps, var, seps,
      pegtl::one< ')' >,
      seps,
      label,
      seps,
      label
    > {};

  struct ins_continue:
    pegtl::string< 'c', 'o', 'n', 't', 'i', 'n', 'u', 'e' > {};

  struct ins_break:
    pegtl::string< 'b', 'r', 'e', 'a', 'k' > {};

  struct ins_label:
    pegtl::seq<
      label
    > {};

  struct ins_return:
    pegtl::sor<
      pegtl::seq<
        pegtl::string< 'r', 'e', 't', 'u', 'r', 'n' >,
        seps,
        pegtl::sor< Num, var >
      >,
      pegtl::string< 'r', 'e', 't', 'u', 'r', 'n' >
    > {};

  struct ins_call:
    LB_ins_call {};

  struct ins_type:
    pegtl::seq<
      type,
      seps,
      vars
    > {};

  struct T:
    pegtl::sor<
      pegtl::string< 'v', 'o', 'i', 'd' >,
      LB_type
    > {};

  struct function_ret_type:
    T {};

  struct function_name:
    LB_name {};

  struct type_var:
    pegtl::seq<
      type, seps, var
    > {};

  struct args:
    pegtl::seq<
      pegtl::rep_max< 1,
        type_var, seps
      >,
      pegtl::star<
        pegtl::one< ',' >, seps, type_var, seps
      >
    > {};

  struct LB_instruction;

  struct scopeStart:
    pegtl::one< '{' > {};

  struct scopeEnd:
    pegtl::one< '}' > {};

  struct scope:
    pegtl::seq<
      scopeStart,
      seps,
      pegtl::star<
        LB_instruction,
        seps
      >,
      seps,
      scopeEnd
    > {};

  struct LB_instruction:
    pegtl::sor<
      ins_while,
      ins_continue,
      ins_break,
      ins_label,
      ins_if_start,
      ins_return,
      ins_call,
      ins_type,
      ins_v_start,
      scope
    > {};

  struct LB_function:
    pegtl::seq<
      // pegtl::string< 'd', 'e', 'f', 'i', 'n', 'e' >,
      function_ret_type, seps, function_name, seps,
      pegtl::one< '(' >, seps,
      args,
      seps, pegtl::one< ')' >, seps,
      scope
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::plus<
        LB_function,
        seps
      >
    > {};

  struct LB_grammer:
    pegtl::must< entry_point_rule > {};

  /*
   * Helpers
   */

  void insert(std::set<std::string> & s, std::string var) {

    // for (auto reg : s)
    // {
    // }

    if (var[0] == ':') {
      return;
    }
    try { // number
      std::stoll(var);
      return;
    } catch (const std::exception& e) {
      s.insert(var);
    }
  }


  /*
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action:
    pegtl::nothing< Rule > {};

  template<> struct action < function_ret_type > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::Function *newF = new LB::Function();

      scopeStack.clear();
      // scopeStack.push_back(newF->scope);
      newF->ret_type = new LB::Type(in.string());
      newF->scope = new LB::InsScope(v);

      p.functions.push_back(newF);

      v.clear();
    }
  };

  template<> struct action < function_name > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {

      LB::Function *currF = p.functions.back();
      currF->name = in.string();
      LB::Program::FUNCS.insert(in.string());
      v.clear();
    }
  };

  template<> struct action < args > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::Function *currF = p.functions.back();

      for (int k = 0; k < v.size(); k += 2) {
        currF->arguments.push_back(new LB::Var(v[k], v[k+1]));

        currF->type_map[v[k+1]] = new LB::Type(v[k]);
      }

      v.clear();
    }
  };

  template<> struct action < scopeStart > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      // std::cout << "probe01 \n";
      LB::Function *currF = p.functions.back();
      if (scopeStack.size() == 0) {

        // std::cout << "probe 010, " << currF->scope << "\n";
        LB::InsScope * newScope = currF->scope;
        scopeStack.push_back(newScope);
        // std::cout << "probe 011, \n";
        // std::cout << scopeStack.size() << "\n";
      } else {
        LB::InsScope * newScope = new LB::InsScope(v);
        scopeStack.back()->inss.push_back(newScope);
        scopeStack.push_back(newScope);
      }
    }
  };

  template<> struct action < scopeEnd > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      // std::cout << "probe02 \n";
      scopeStack.pop_back();
      v.clear();
    }
  };



  // template<> struct action < type_var > {
  //   static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
  //     v.clear();
  //   }
  // };

  template<> struct action < ins_call > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns = new LB::InsCall(v);

      currScope->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_label > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns = new LB::InsLabel(v);
      currScope->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_type > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Function *currF = p.functions.back();
      LB::Instruction *newIns = new LB::InsType(v);

      currF->type_map[v[1]] = new LB::Type(v[0]);

      currScope->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_v_start > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns;

      if (v[1] == "length") {
        newIns = new LB::InsLength(v);
      } else if (v[1] == "new") {
        if (v[2][0] == 'A') { // Array
          newIns = new LB::InsNewArray(v);
        } else {  // Tuple
          newIns = new LB::InsNewTuple(v);
        }
      }
        else if (v[1] == "call") {
        newIns = new LB::InsAssignCall(v);
      } else if (v.size() == 2) {
        newIns = new LB::InsAssign(v);
      } else if (v.size() == 4){
        newIns = new LB::InsOpAssign(v);
      }

      currScope->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_if_start > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns = new LB::InsIf(v);

      currScope->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_return > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      // std::cout << "probe ret, \n";
      // std::cout << "probe ret, " << scopeStack.size() << "\n";
      LB::InsScope * currScope = scopeStack.back();
      // std::cout << "probe ret 03, \n";
      LB::Instruction *newIns = new LB::InsReturn(v);
      // std::cout << "probe ret 04, \n";
      // std::cout << "currScope 04, " << currScope->inss.size() << "\n";
      // std::cout << "scopeStack.back() 04, " << scopeStack.back() << "\n";
      currScope->inss.push_back(newIns);
      // std::cout << "probe ret 05, \n";
      v.clear();
      // std::cout << "probe ret, a\n";
    }
  };

  template<> struct action < ins_while > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns = new LB::InsWhile(v);
      std::cout << "probe while\n";
      currScope->inss.push_back(newIns);

      v.clear();
    }
  };

  template<> struct action < ins_continue > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns = new LB::InsContinue(v);

      currScope->inss.push_back(newIns);

      v.clear();
    }
  };

  template<> struct action < ins_break > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      LB::InsScope *currScope = scopeStack.back();
      LB::Instruction *newIns = new LB::InsBreak(v);

      currScope->inss.push_back(newIns);

      v.clear();
    }
  };

  //
  // Actions to collect string from rules, should be a better way.
  //
  template<> struct action < var > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < s_new > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < type > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < varSqureT > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Num > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < t > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < label > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < callee > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < op > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < call > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < length > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Array > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Tuple > {
    static void apply(const pegtl::input & in, LB::Program & p, std::vector<std::string> & v, std::vector<LB::InsScope *> & scopeStack) {
      v.push_back(in.string());
    }
  };



  /*
   * Data structures requLBed to parse
   */
  Program LB_parse_file (char *fileName) {
    /*
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< LB::LB_grammer >();

    /*
     * Parse.
     */

    LB::Program p;
    // LB::Instruction ti; // temp instruction
    std::vector<std::string> v;
    std::vector<LB::InsScope *> scopeStack;
    pegtl::file_parser(fileName).parse< LB::LB_grammer, LB::action > (p, v, scopeStack);

    return p;
  }
}
