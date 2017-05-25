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

namespace L3 {

  void debug(std::string str);

  const std::vector< std::string > ARGS = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
  const std::set< std::string > LIBS = { "allocate", "print", "array-error" };
  const std::set< std::string > OP = { "+", "-", "*", "&", "<<", ">>" };
  // std::set< std::string > F_NAME = {};

  enum INS { VAR, N, LABEL, ELSE };

  class Instance {
  public:
    std::set<std::string> GEN;
    std::set<std::string> KILL;
    std::set<std::string> IN;
    std::set<std::string> OUT;

    std::string name;

    int64_t type = L3::INS::ELSE;

    std::vector<L3::Instance *> instances;

    virtual std::string toL2(std::string f_name) = 0;
    virtual std::string toString() = 0;

    bool equal(L3::Instance * ins);
    void printTree(int d);
  };


  class Var: public Instance {
  public:
    // Var();
    Var(std::string name, std::string fname="");
    std::string toString();
    std::string toL2(std::string f_name);
  };

  class Op: public Instance {
  public:
    Op(std::vector<std::string> & v);
    std::string toString();
    std::string toL2(std::string f_name);
  };

  class Br: public Instance {
  public:
    Br(std::vector<std::string> & v);
    std::string toString();
    std::string toL2(std::string f_name);
  };

  class Store: public Instance {
  public:
    Store(std::vector<std::string> & v, std::string f_name = "");
    std::string toString();
    std::string toL2(std::string f_name);
  };

  class Load: public Instance {
  public:
    Load(std::vector<std::string> & v);
    std::string toString();
    std::string toL2(std::string f_name);
  };

  class Call: public Instance {
  public:
    Call(std::vector<std::string> & v, std::string f_name = "");
    std::string toString();
    std::string toL2(std::string f_name);
  };

  class Return: public Instance {
  public:
    Return(std::vector<std::string> & v);
    std::string toString();
    std::string toL2(std::string f_name);
  };


  struct Function {
    std::string name;
    std::vector< L3::Instance * > arguments;
    std::vector< L3::Instance * > instructions;
  };

  struct Program {
    std::vector< L3::Function * > functions;
  };

  void printFunc(L3::Function & func);
}
