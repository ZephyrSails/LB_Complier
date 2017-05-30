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

namespace IR {

  void debug(std::string str);

  const std::vector< std::string > ARGS = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
  const std::set< std::string > LIBS = { "allocate", "print", "array-error" };

  enum INS { VAR, N, LABEL, ELSE };

  enum TYPE { VOID, TUPLE, CODE, INT };

  class Type {
  public:
    int64_t type;
    int64_t arr_count;

    Type(std::string t);
  };

  struct Function;

  class Var {
  public:
    std::string name;
    // int64_t value;
    IR::Type * type;  // how many [] are there?
    std::vector< IR::Var * > ts;
    Var(std::string t, std::string n);
    // Var(std::string n);
    Var(std::string & n, bool hasT=false);
    std::string toString();
    std::string printAddr(std::ofstream &o, IR::Function * currF);
  };

  class Instruction {
  public:
    // std::set<std::string> GEN;
    // std::set<std::string> KILL;
    // std::set<std::string> IN;
    // std::set<std::string> OUT;
    std::string op;
    std::vector<IR::Var *> vars;

    virtual void toL3(std::ofstream &outputFile, IR::Function * currF) = 0;
  };

  class InsBr: public Instruction {
  public:
    InsBr(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);

  };

  class InsReturn: public Instruction {
  public:
    InsReturn(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsLength: public Instruction {
  public:
    InsLength(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsAssignCall: public Instruction {
  public:
    InsAssignCall(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsAssign: public Instruction {
  public:
    InsAssign(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsOpAssign: public Instruction {
  public:
    InsOpAssign(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsType: public Instruction {
  public:
    InsType(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsCall: public Instruction {
  public:
    InsCall(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsNewArray: public Instruction {
  public:
    InsNewArray(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class InsNewTuple: public Instruction {
  public:
    InsNewTuple(std::vector<std::string> & v);
    void toL3(std::ofstream &outputFile, IR::Function * currF);
  };

  class BasicBlock {
  public:
    std::string label;
    std::vector< IR::Instruction * > inss;
    IR::Instruction * te;
    BasicBlock(std::string name);
  };

  struct Function {
    Type * ret_type;
    std::string name;
    std::map< std::string, IR::Type * > type_map;
    std::vector< IR::Var * > arguments;
    std::vector< IR::BasicBlock * > bbs;
    std::map<std::string, std::string> lengthMap = {};
  };

  struct Program {
    std::vector< IR::Function * > functions;
  };

  void printFunc(IR::Function & func);
}
