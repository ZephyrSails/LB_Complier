// by: Zhiping

#pragma once

#include <L3.h>
// #include <graph.h>

namespace L3 {
  void liveness(L3::Function *func);
  void union_set(std::set<std::string> & s, std::set<std::string> & t);
  // void gen_gen_kill(std::set<std::string> * GEN, std::set<std::string> * KILL, L2::Instruction * i);
  // L2::Graph code_analysis(L2::Function *func);
}
