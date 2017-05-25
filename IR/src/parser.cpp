// by: Zhiping

// #include <IR.h>
#include <pegtl.hh>
#include <pegtl/analyze.hh>
#include <pegtl/contrib/raw_string.hh>
#include <parser.h>

using namespace pegtl;
using namespace std;

namespace IR {
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

  struct runtime_system_func:
    pegtl::sor<
      pegtl::string< 'p', 'r', 'i', 'n', 't' >,
      pegtl::string< 'a', 'r', 'r', 'a', 'y', '-', 'e', 'r', 'r', 'o', 'r' >
    > {};

  struct IR_type:
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
    IR_type {};

  struct left_arrow:
    pegtl::string< '<', '-' > {};

  struct IR_word:
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

  struct IR_label:
    pegtl::seq<
      pegtl::one<':'>,
      IR_word
    > {};

  struct IR_var:
    pegtl::seq<
      pegtl::one< '%' >,
      IR_word
    > {};

  struct IR_Num:
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

  struct IR_t:
    pegtl::sor<
      IR_var,
      IR_Num
    > {};

  struct IR_u:
    pegtl::sor<
      IR_var,
      IR_label
    > {};

  struct IR_callee:
    pegtl::sor<
      runtime_system_func,
      IR_u
    > {};
  //
  struct var:
    IR_var {};
  //
  struct Num:
    IR_Num {};
  //
  struct t:
    IR_t {};
  //
  struct label:
    IR_label {};
  //
  // struct u:
  //   IR_u {};

  struct callee:
    IR_callee {};

  struct IR_vars:
    pegtl::rep_max< 1,
      pegtl::seq<
        var,
        seps,
        pegtl::star<
          pegtl::seq<
            pegtl::one< ',' >,
            seps,
            var,
            seps
          >
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
    IR_vars {};

  struct call:
    pegtl::string< 'c', 'a', 'l', 'l' > {};

  struct IR_ins_call:
    pegtl::seq<
      call,
      seps, callee, seps,
      pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
    > {};


  struct varSqureT:
    pegtl::seq<
      IR_var,
      pegtl::star<
        pegtl::one< '[' >,
        IR_t,
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
        IR_ins_call,
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

  struct ins_br_start:
    pegtl::seq<
      pegtl::string< 'b', 'r' >,
      seps,
      pegtl::sor<
        label,
        pegtl::seq< var, seps, label, seps, label >
      >
    > {};

  struct ins_label:
    label {};

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
    IR_ins_call {};

  struct ins_type:
    pegtl::seq<
      type,
      seps,
      var
    > {};



  // struct ins_new:
  //   pegtl::seq<
  //     pegtl::string< 'n', 'e', 'w' >,
  //     seps,
  //     pegtl::sor<
  //               //  pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
  //       pegtl::seq<
  //         Array, pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
  //       >,
  //       pegtl::seq<
  //         Tuple, pegtl::one< '(' >, seps, t, seps, pegtl::one< ')' >
  //       >
  //     >
  //   > {};

  struct IR_instruction:
    pegtl::sor<
      // ins_new,
      ins_call,
      ins_type,
      ins_v_start
    > {};

  struct T:
    pegtl::sor<
      pegtl::string< 'v', 'o', 'i', 'd' >,
      IR_type
    > {};

  struct function_ret_type:
    T {};

  struct function_name:
    IR_label {};

  struct te:
    pegtl::sor<
      ins_br_start,
      ins_return
    > {};

  struct bb_label:
    IR_label {};

  struct bb:
    pegtl::seq<
      bb_label,
      seps,
      pegtl::star<
        IR_instruction,
        seps
      >,
      te
    > {};

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

  struct IR_function:
    pegtl::seq<
      pegtl::string< 'd', 'e', 'f', 'i', 'n', 'e' >,
      seps, function_ret_type, seps, function_name, seps,
      pegtl::one< '(' >, seps,
      args,
      seps, pegtl::one< ')' >, seps,
      pegtl::one< '{' >,
      seps,
      pegtl::star<
        bb,
        seps
      >,
      seps,
      pegtl::one< '}' >
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::plus<
        IR_function,
        seps
      >
    > {};

  struct IR_grammer:
    pegtl::must< entry_point_rule > {};

  /*
   * Helpers
   */

  void insert(std::set<std::string> & s, std::string var) {

    // for (auto reg : s)
    // {
    //     std::cout << reg << " ";
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
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *newF = new IR::Function();
      newF->ret_type = new IR::Type(in.string());

      p.functions.push_back(newF);

      v.clear();
    }
  };

  template<> struct action < function_name > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      currF->name = in.string();
      std::cout << "new func: " << currF->name << "\n";
      v.clear();
    }
  };

  template<> struct action < bb_label > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      IR::BasicBlock *newBB = new IR::BasicBlock(in.string());

      currF->bbs.push_back(newBB);

      v.clear();
    }
  };

  template<> struct action < args > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      // std::cout << "v.size(): " << v.size() << "\n";


      for (int k = 0; k < v.size(); k += 2) {
        // std::cout << "v[" << k << "]: " << v[k] << " k+1: " << v[k+1] << "\n";
        currF->arguments.push_back(new IR::Var(v[k], v[k+1]));

        currF->type_map[v[k+1]] = new IR::Type(v[k]);
      }

      v.clear();
    }
  };

  template<> struct action < type_var > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      // std::cout << "type_var: " << in.string() << "\n";
      // v.clear();
    }
  };

  template<> struct action < ins_call > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      IR::BasicBlock *currBB = currF->bbs.back();
      IR::Instruction *newIns = new IR::InsCall(v);

      currBB->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_type > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      IR::BasicBlock *currBB = currF->bbs.back();
      IR::Instruction *newIns = new IR::InsType(v);

      currF->type_map[v[1]] = new IR::Type(v[0]);

      currBB->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_v_start > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      IR::BasicBlock *currBB = currF->bbs.back();
      IR::Instruction *newIns;

      // std::cout << "busErr: v.size(): " << v.size() << "\n";
      if (v[1] == "length") {
        newIns = new IR::InsLength(v);
      } else if (v[1] == "new") {
        if (v[2][0] == 'A') { // Array
          newIns = new IR::InsNewArray(v);
        } else {  // Tuple
          newIns = new IR::InsNewTuple(v);
        }
      }
        else if (v[1] == "call") {
        newIns = new IR::InsAssignCall(v);
      } else if (v.size() == 2) {
        // std::cout << "busErr: probe1\n";
        newIns = new IR::InsAssign(v);
      } else if (v.size() == 4){
        // std::cout << "busErr \n";
        newIns = new IR::InsOpAssign(v);
      }

      currBB->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_br_start > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      IR::BasicBlock *currBB = currF->bbs.back();
      IR::Instruction *newIns = new IR::InsBr(v);

      currBB->te = newIns;

      v.clear();
    }
  };

  template<> struct action < ins_return > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      IR::Function *currF = p.functions.back();
      IR::BasicBlock *currBB = currF->bbs.back();
      IR::Instruction *newIns = new IR::InsReturn(v);

      currBB->te = newIns;

      v.clear();
    }
  };


  //
  // Actions to collect string from rules, should be a better way.
  //
  template<> struct action < var > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < s_new > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < type > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < varSqureT > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Num > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < t > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < label > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < callee > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < op > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < call > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < length > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Array > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Tuple > {
    static void apply( const pegtl::input & in, IR::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };



  /*
   * Data structures required to parse
   */
  Program IR_parse_file (char *fileName) {
    /*
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< IR::IR_grammer >();
    /*
     * Parse.
     */
    IR::Program p;
    // IR::Instruction ti; // temp instruction
    std::vector< std::string > v;
    pegtl::file_parser(fileName).parse< IR::IR_grammer, IR::action > (p, v);

    return p;
  }
}
