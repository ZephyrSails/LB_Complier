// By Zhiping

#include <merge.h>
#include <liveness.h>

namespace L3 {

  bool canMerge(L3::Instance * ins, L3::Instance * prev, std::set<std::string> & OUT) {
    return (prev->type == L3::INS::VAR && ins->type == L3::INS::VAR
            && typeid(*ins) == typeid(L3::Var)
            && ins->name == prev->name
            && OUT.count(prev->name) == 0);
  }

  bool merge(L3::Instance * ins, L3::Instance * prev, std::set<std::string> & OUT) {
    if (ins->instances.size() == 0) { // leaf?
      if (canMerge(ins, prev, OUT)) { // ins == prev and prev.
        if (typeid(*prev->instances[0]) == typeid(L3::Var)) {
          *ins = *prev->instances[0];
        } else {
          *ins = *prev;
        }
        return true;
      }
      return false;
    } else {
      bool mergeFlag = false;
      for (auto sub : ins->instances) {
        mergeFlag |= merge(sub, prev, OUT);
      }
      return mergeFlag;
    }
  }

  void mergeFunc(L3::Function * f) {
    std::cout << "\nbefore merge size: " << f->instructions.size() << "\n";
    int n = f->instructions.size();
    int k = 0;
    while (k < n-1) {
      if (merge(f->instructions[k+1], f->instructions[k], f->instructions[k+1]->OUT)) {

        union_set(f->instructions[k+1]->OUT, f->instructions[k]->OUT);
        f->instructions.erase(f->instructions.begin() + k);

        int n = f->instructions.size();
      } else {
        k++;
      }
    }
    std::cout << "after merge size: " << f->instructions.size() << "\n\n";
  }
}
