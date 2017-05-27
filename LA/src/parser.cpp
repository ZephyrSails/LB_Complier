// by: Zhiping

#include <parser.h>

using namespace pegtl;
using namespace std;

namespace LA {
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

  struct LA_type:
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
    LA_type {};

  struct left_arrow:
    pegtl::string< '<', '-' > {};

  struct LA_name:
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

  struct LA_label:
    pegtl::seq<
      pegtl::one<':'>,
      LA_name
    > {};

  struct LA_var:
    pegtl::seq<
      pegtl::one< '%' >,
      LA_name
    > {};

  struct LA_Num:
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

  struct LA_t:
    pegtl::sor<
      LA_var,
      LA_Num
    > {};

  struct LA_u:
    pegtl::sor<
      LA_var,
      LA_label
    > {};

  struct LA_callee:
    pegtl::sor<
      LA_var,
      LA_name
    > {};
  //
  struct var:
    LA_var {};
  //
  struct Num:
    LA_Num {};
  //
  struct t:
    LA_t {};
  //
  struct label:
    LA_label {};
  //
  // struct u:
  //   LA_u {};

  struct callee:
    LA_callee {};

  struct LA_vars:
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
    LA_vars {};

  struct call:
    pegtl::string< 'c', 'a', 'l', 'l' > {};

  struct LA_ins_call:
    pegtl::seq<
      call,
      seps, callee, seps,
      pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
    > {};


  struct varSqureT:
    pegtl::seq<
      LA_var,
      pegtl::star<
        pegtl::one< '[' >,
        LA_t,
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
              Array, seps, pegtl::one< '(' >, seps, argv, seps, pegtl::one< ')' >
            >,
            pegtl::seq<
              Tuple, seps, pegtl::one< '(' >, seps, t, seps, pegtl::one< ')' >
            >
          >
        >,
        label,
        LA_ins_call,
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
    LA_ins_call {};

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

  struct LA_instruction:
    pegtl::sor<
      // ins_new,
      ins_label,
      ins_br_start,
      ins_return,
      ins_call,
      ins_type,
      ins_v_start
    > {};

  struct T:
    pegtl::sor<
      pegtl::string< 'v', 'o', 'i', 'd' >,
      LA_type
    > {};

  struct function_ret_type:
    T {};

  struct function_name:
    LA_name {};

  // struct te:
  //   pegtl::sor<
  //     ins_br_start,
  //     ins_return
  //   > {};

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

  struct LA_function:
    pegtl::seq<
      // pegtl::string< 'd', 'e', 'f', 'i', 'n', 'e' >,
      function_ret_type, seps, function_name, seps,
      pegtl::one< '(' >, seps,
      args,
      seps, pegtl::one< ')' >, seps,
      pegtl::one< '{' >,
      seps,
      pegtl::star<
        LA_instruction,
        seps
      >,
      seps,
      pegtl::one< '}' >
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::plus<
        LA_function,
        seps
      >
    > {};

  struct LA_grammer:
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
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *newF = new LA::Function();
      newF->ret_type = new LA::Type(in.string());

      p.functions.push_back(newF);

      v.clear();
    }
  };

  template<> struct action < function_name > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      currF->name = in.string();
      LA::Program::FUNCS.insert(in.string());
      v.clear();
    }
  };

  template<> struct action < args > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();


      for (int k = 0; k < v.size(); k += 2) {
        currF->arguments.push_back(new LA::Var(v[k], v[k+1]));

        currF->type_map[v[k+1]] = new LA::Type(v[k]);
      }

      v.clear();
    }
  };

  // template<> struct action < type_var > {
  //   static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
  //     v.clear();
  //   }
  // };

  template<> struct action < ins_call > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      LA::Instruction *newIns = new LA::InsCall(v);

      currF->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_label > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      LA::Instruction *newIns = new LA::InsLabel(v);
      currF->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_type > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      LA::Instruction *newIns = new LA::InsType(v);

      currF->type_map[v[1]] = new LA::Type(v[0]);

      currF->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_v_start > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      LA::Instruction *newIns;

      if (v[1] == "length") {
        newIns = new LA::InsLength(v);
      } else if (v[1] == "new") {
        if (v[2][0] == 'A') { // Array
          newIns = new LA::InsNewArray(v);
        } else {  // Tuple
          newIns = new LA::InsNewTuple(v);
        }
      }
        else if (v[1] == "call") {
        newIns = new LA::InsAssignCall(v);
      } else if (v.size() == 2) {
        newIns = new LA::InsAssign(v);
      } else if (v.size() == 4){
        newIns = new LA::InsOpAssign(v);
      }

      currF->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_br_start > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      LA::Instruction *newIns = new LA::InsBr(v);

      currF->inss.push_back(newIns);
      v.clear();
    }
  };

  template<> struct action < ins_return > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      LA::Function *currF = p.functions.back();
      LA::Instruction *newIns = new LA::InsReturn(v);

      currF->inss.push_back(newIns);

      v.clear();

    }
  };


  //
  // Actions to collect string from rules, should be a better way.
  //
  template<> struct action < var > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < s_new > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < type > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < varSqureT > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Num > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < t > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < label > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < callee > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < op > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < call > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < length > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Array > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };

  template<> struct action < Tuple > {
    static void apply( const pegtl::input & in, LA::Program & p, std::vector<std::string> & v ) {
      v.push_back(in.string());
    }
  };



  /*
   * Data structures requLAed to parse
   */
  Program LA_parse_file (char *fileName) {
    /*
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< LA::LA_grammer >();

    /*
     * Parse.
     */

    LA::Program p;
    // LA::Instruction ti; // temp instruction
    std::vector< std::string > v;
    pegtl::file_parser(fileName).parse< LA::LA_grammer, LA::action > (p, v);

    return p;
  }
}
