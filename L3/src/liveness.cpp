// by: Zhiping

#include <liveness.h>
//
namespace L3 {
//   std::set<std::string> callee_save_regs = {"r12", "r13", "r14", "r15", "rbp", "rbx"};
//   std::set<std::string> caller_save_regs = {"r10", "r11", "r8", "r9", "rax", "rcx", "rdi", "rdx", "rsi"};
//   std::vector<std::string> args_regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
//
//   // utility
  // void insert_item_to_set(std::set<std::string> * s, L3::Item * i) {
  //   if (i->type == L3::ITEM::REGISTER || i->type == L3::ITEM::VAR) {
  //     // if (i->name != "rsp" && i->name != "print" && i->name != "allocate" && i->name != "array-error") {
  //     if (i->name != "rsp") {
  //       s->insert(i->name);
  //     }
  //   }
  // }
//
  void union_set(std::set<std::string> & s, std::set<std::string> & t) {
    s.insert(t.begin(), t.end());
  }
//
  void union_set(std::set<std::string> & s, std::vector<std::string> & t, int n) {
    if (n > t.size()) {
      n = t.size();
    }
    for (int i = 0; i < n; i++) {
      s.insert(t.at(i));
    }
  }
//
  std::set<std::string> minus_set(std::set<std::string> & s, std::set<std::string> & t) {
    std::set<std::string> result;
    std::set_difference(s.begin(), s.end(), t.begin(), t.end(), std::inserter(result, result.end()));
    return result;
  }
//
  std::map<std::string, int> build_label_map(std::vector<L3::Instance *> instances) {
    std::map<std::string, int> result;
    for (int k = 0; k < instances.size(); k++) {
      L3::Instance * i = instances.at(k);
      if (typeid(*i) == typeid(L3::Var) && i->type == L3::INS::LABEL) {
        result[i->name] = k;
      }
    }
    return result;
  }

  void liveness(L3::Function *func) {
    std::map<std::string, int> labelNextIndexMap = build_label_map(func->instructions);

    int n = func->instructions.size();

    // for (int k = 0; k < n; k++) {
    //   std::cout << "GEN: ";
    //   for (auto reg : func->instructions[k]->GEN)
    //   {
    //       std::cout << reg << " ";
    //   }
    //   std::cout << " ///// KILL: ";
    //   for (auto reg : func->instructions[k]->KILL)
    //   {
    //       std::cout << reg << " ";
    //   }
    //   std::cout << "\n";
    // }

    int converge_count;
    while (converge_count != n) {

      converge_count = 0;

      for (int k = 0; k < n; k++) {
        std::set <std::string> newIn;
        std::set <std::string> newOut;

        L3::Instance *cur_ins = func->instructions.at(k);

        // IN[i] = GEN[i] U (OUT[i] - KILL[i])
        union_set(newIn, cur_ins->GEN);

        std::set <std::string> diff = minus_set(cur_ins->OUT, cur_ins->KILL);

        union_set(newIn, diff);

        // OUT[i] = U (s a successor of i) IN[s]
        std::vector< int > next_indexs;

        if (typeid(*cur_ins) == typeid(L3::Br)) {
          if (cur_ins->instances.size() == 3) {
            next_indexs.push_back(labelNextIndexMap[cur_ins->instances[1]->name]);
            next_indexs.push_back(labelNextIndexMap[cur_ins->instances[2]->name]);
          } else {
            next_indexs.push_back(labelNextIndexMap[cur_ins->instances[0]->name]);
          }
        } else {
          next_indexs.push_back(k+1);
        }

        union_set(newOut, cur_ins->OUT);
        for (int next_index : next_indexs) {
          if (next_index < n) {
            union_set(newOut, func->instructions[next_index]->IN);
          }
        }

        if (cur_ins->IN == newIn && cur_ins->OUT == newOut) {
          converge_count++;
        } else {
          cur_ins->IN = newIn;
          cur_ins->OUT = newOut;
        }
      }
    }
    ///////////////////////
    // print in & out
    ///////////////////////
    // std::cout << "(\n(in\n";
    // for (int k = 0; k < n; k++) {
    //   std::cout << "(";
    //   for (auto var : func->instructions[k]->IN) {
    //       std::cout << var << " ";
    //   }
    //   std::cout << ")\n";
    // }
    // std::cout << ")\n\n(out\n";
    // for (int k = 0; k < n; k++) {
    //   std::cout << "(";
    //   for (auto var : func->instructions[k]->OUT) {
    //       std::cout << var << " ";
    //   }
    //   std::cout << ")\n";
    // }
    // std::cout << ")\n\n)\n";
  }
}
