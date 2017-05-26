// by: Zhiping

#pragma once

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <tuple>
#include <unistd.h>
#include <typeinfo>
#include <cstdlib>
#include <regex>

namespace LB {

  void debug(std::string str);

  const std::string DECODE = "_DECODE";

  const std::vector<std::string> ARGS = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
  const std::set<std::string> LIBS = { "allocate", "print", "array-error" };

  // enum INS { VAR, N, Label, ELSE };

  enum TYPE { VOID, TUPLE, CODE, INT, VAR, N, Label };

  class Type {
  public:
    int64_t type;
    int64_t arr_count;

    Type(std::string t, bool notArray=false);
    std::string toString();
  };

  struct Function;

  class Var {
  public:
    std::string name;
    LB::Type * type;  // include how many [] are there?
    std::vector< LB::Var * > ts;
    Var(std::string t, std::string n);
    Var(std::string & n, bool hasT=false);
    std::string toString(std::string path);
  };

  class Instruction {
  public:
    std::string op;
    std::vector< LB::Var * > vars;

    virtual void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path) = 0;
  };

  class InsIf: public Instruction {
  public:
    InsIf(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsWhile: public Instruction {
  public:
    InsWhile(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsReturn: public Instruction {
  public:
    InsReturn(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsLength: public Instruction {
  public:
    InsLength(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsAssignCall: public Instruction {
  public:
    InsAssignCall(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsAssign: public Instruction {
  public:
    InsAssign(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsOpAssign: public Instruction {
  public:
    InsOpAssign(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsType: public Instruction {
  public:
    InsType(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsCall: public Instruction {
  public:
    InsCall(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsNewArray: public Instruction {
  public:
    InsNewArray(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsNewTuple: public Instruction {
  public:
    InsNewTuple(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsLabel: public Instruction {
  public:
    InsLabel(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsContinue: public Instruction {
  public:
    InsContinue(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsBreak: public Instruction {
  public:
    InsBreak(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  class InsScope: public Instruction {
  public:
    std::vector< LB::Instruction * > inss;
    InsScope(std::vector<std::string> & v);
    void toIR(std::ofstream &outputFile, LB::Function * currF, std::string path);
  };

  struct Function {
    Type * ret_type;
    std::string name;
    std::map< std::string, LB::Type * > type_map;
    std::vector< LB::Var * > arguments;
    LB::InsScope * scope;
  };

  class Program {
  public:
    static std::set<std::string> FUNCS;
    std::vector< LB::Function * > functions;
  };

  void printFunc(LB::Function & func);
}
