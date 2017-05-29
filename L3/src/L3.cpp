// By: Zhiping

#include <L3.h>

namespace L3 {
  void debug(std::string str) {
    if (true) {
      std::cout << str << "\n";
    }
  }

  bool Instance::equal(L3::Instance * tile) {
    if (typeid(*this) == typeid(*tile)) {
      if (tile->type == L3::INS::LABEL || tile->type == L3::INS::N) {
        return (tile->type == this->type);
      }
      if (L3::OP.count(tile->name) > 0) {
        return (tile->name == this->name);
      }
      return true;
    }
    return false;
  }

  void Instance::printTree(int d = 0) {
    for (int k = 0; k < d; k++) {
      std::cout << "  ";
    }
    std::cout << typeid(*this).name() << ": " << this->name << "\n";
    for (auto sub : this->instances) {
      sub->printTree(d + 1);
    }
  }

  void printFunc(L3::Function & func) {
    for (auto ins : func.instructions) {
      ins->printTree();
    }
  }

  Var::Var(std::string name, std::string filename) {
    if (name[0] == ':') {
      this->type = L3::INS::LABEL;
    } else {
      try { // number
        std::stoll(name);
        this->type = L3::INS::N;
      } catch (const std::exception& e) {
        this->type = L3::INS::VAR;
      }
    }

    this->name = name;

    if (this->type == L3::INS::LABEL) {
      this->name += filename;
    }
  }

  std::string Var::toString() {
    std::string res;
    if ((this->instances.size() == 1) && (typeid(*(this->instances[0])) == typeid(L3::Op))) {
      if (L3::OP.count(this->instances[0]->name)) {
        if (this->name == this->instances[0]->instances[0]->name && this->instances[0]->name != "-") {
          res += "\n\t\t(" + this->name + " " + this->instances[0]->name + "= " + this->instances[0]->instances[1]->name + ")";
        } else if (this->name == this->instances[0]->instances[1]->name && this->instances[0]->name != "-") {
          res += "\n\t\t(" + this->name + " " + this->instances[0]->name + "= " + this->instances[0]->instances[0]->name + ")";
        } else {
          res += "\n\t\t(" + this->name + " <- " + this->instances[0]->instances[0]->name + ")";
          res += "\n\t\t(" + this->name + " " + this->instances[0]->name + "= " + this->instances[0]->instances[1]->name + ")";
        }
      } else { // cmp
        std::string suffix = std::to_string(rand());
        std::string cmp = this->instances[0]->name;
        bool switched;
        if (this->instances[0]->name == ">=" || this->instances[0]->name == ">") {
          if (this->instances[0]->name == ">=") {
            cmp = "<=";
          } else {
            cmp = "<";
          }
          switched = true;
        }
        res += "\n\t\t(" + this->name + " <- 0)";
        if (switched) {
          res += "\n\t\t(cjump " + this->instances[0]->instances[1]->name + " " + cmp + " " + this->instances[0]->instances[0]->name + " :true_"  + suffix + " :false_" + suffix + ")";
        } else {
          res += "\n\t\t(cjump " + this->instances[0]->instances[0]->name + " " + cmp + " " + this->instances[0]->instances[1]->name + " :true_"  + suffix + " :false_" + suffix + ")";
        }

        res += "\n\t\t:true_" + suffix;
        res += "\n\t\t(" + this->name + " <- 1)";
        res += "\n\t\t:false_" + suffix;
      }
      return res;
    }
    return "";
  }

  Br::Br(std::vector<std::string> & v) {
    this->instances.push_back(new L3::Var(v[0]));
    if (v.size() == 3) {
      this->instances.push_back(new L3::Var(v[1]));
      this->instances.push_back(new L3::Var(v[2]));
    }
  }

  std::string Br::toL2(std::string f_name) {
    std::string res = "\n\t\t";
    if (this->instances.size() == 3) {
      this->instances[1]->name += "_" + f_name;
      this->instances[2]->name += "_" + f_name;
      L3::Instance * cmp = this->instances[0]->instances[0];
      if (cmp->name == ">=" || cmp->name == ">" || cmp->name == "=" || cmp->name == "<" || cmp->name == "<=") {
        if (cmp->name == ">=" || cmp->name == ">") {
          if (cmp->name == ">=") {
            cmp->name = "<=";
          } else {
            cmp->name = "<";
          }
          res += "\n\t\t(cjump " + cmp->instances[1]->name + " " + cmp->name + " " + cmp->instances[0]->name + " " + this->instances[1]->name + " " + this->instances[2]->name + ")";
        } else {
          res += "\n\t\t(cjump " + cmp->instances[0]->name + " " + cmp->name + " " + cmp->instances[1]->name + " " + this->instances[1]->name + " " + this->instances[2]->name + ")";
        }
      } else {
        res += this->instances[0]->toString();
        res += "\n\t\t(cjump 0 < " + this->instances[0]->name + " " + this->instances[1]->name + " " + this->instances[2]->name + ")";
      }
    } else {
      // if (this->instances[0]->name )
      this->instances[0]->name += "_" + f_name;
      res += "(goto " + this->instances[0]->name + ")";
    }
    return res;
  }

  std::string Br::toString() {
    std::string res = "\n\t\t";
    if (this->instances.size() == 3) {
      L3::Instance * cmp = this->instances[0]->instances[0];
      if (cmp->name == ">=" || cmp->name == ">" || cmp->name == "=" || cmp->name == "<" || cmp->name == "<=") {
        if (cmp->name == ">=" || cmp->name == ">") {
          if (cmp->name == ">=") {
            cmp->name = "<=";
          } else {
            cmp->name = "<";
          }
          res += "\n\t\t(cjump " + cmp->instances[1]->name + " " + cmp->name + " " + cmp->instances[0]->name + " " + this->instances[1]->name + " " + this->instances[2]->name + ")";
        } else {
          res += "\n\t\t(cjump " + cmp->instances[0]->name + " " + cmp->name + " " + cmp->instances[1]->name + " " + this->instances[1]->name + " " + this->instances[2]->name + ")";
        }
      } else {
        res += this->instances[0]->toString();
        res += "\n\t\t(cjump 0 < " + this->instances[0]->name + " " + this->instances[1]->name + " " + this->instances[2]->name + ")";
      }

    } else {
      res += "(goto " + this->instances[0]->name + ")";
    }
    return res;
  }

  Store::Store(std::vector<std::string> & v, std::string f_name) {
    this->instances.push_back(new L3::Var(v[0]));
    this->instances.push_back(new L3::Var(v[1], f_name));
  }

  std::string Store::toString() {
    std::string res;
    res += "\n\t\t((mem " + this->instances[0]->name + " 0) <- " + this->instances[1]->name + ")";
    return res;
  }

  Op::Op(std::vector<std::string> & v) {
    this->name = v[2];

    this->instances.push_back(new L3::Var(v[1]));
    this->instances.push_back(new L3::Var(v[3]));
  }

  std::string Op::toString() {

    return "";
  }

  Call::Call(std::vector<std::string> & v, std::string f_name) {
    this->name = v[0];
    for (int k = 1; k < v.size(); k++) {
      this->instances.push_back(new L3::Var(v[k], f_name));
    }
  }

  std::string Call::toString() {
    std::string res = "\n\t\t";

    bool isLibFunc = (this->name == "print" || this->name == "array-error" || this->name == "allocate");
    for (int k = 0; k < this->instances.size(); k++) {
      if (k < 6) {
        res += "\n\t\t(" + L3::ARGS[k] + " <- " + this->instances[k]->name + ")";
      } else {
        res += "\n\t\t((mem rsp -" + std::to_string(8 * k - 32) + ") <- " + this->instances[k]->name + ")";
      }
    }

    std::string fName = this->name;
    if (fName[0] == ':') {
      fName.erase(0, 1);
      fName += "_" + std::to_string(rand());
      // CALLCOUNT++;
    }
    if (!isLibFunc) {
      res += "\n\t\t((mem rsp -8) <- :ret_" + fName + ")";
    }
    res += "\n\t\t(call " + this->name + " " + std::to_string(this->instances.size()) + ")";
    if (!isLibFunc) {
      res += "\n\t\t:ret_" + fName;
    }
    return res;
  }

  Load::Load(std::vector<std::string> & v) {
    this->instances.push_back(new L3::Var(v[2]));
  }

  std::string Load::toString() {
    std::string res = "\n\t\t";

    return "";
  }

  Return::Return(std::vector<std::string> & v) {
    if (v.size() == 1) {
      this->name = v[0];
    }
  }

  std::string Return::toString() {
    std::string res = "\n\t\t";
    if (this->name != "") {
      res += "(rax <- " + this->name + ")\n\t\t";
    }
    res += "(return)";
    return res;
  }

  std::string Var::toL2(std::string f_name) {
    return "";
  }
  std::string Store::toL2(std::string f_name) {
    return "";
  }
  std::string Op::toL2(std::string f_name) {
    return "";
  }
  std::string Call::toL2(std::string f_name) {
    return "";
  }
  std::string Load::toL2(std::string f_name) {
    return "";
  }
  std::string Return::toL2(std::string f_name) {
    return "";
  }
}
