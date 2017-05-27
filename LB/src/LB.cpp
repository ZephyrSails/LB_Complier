// By Zhiping

#include <LB.h>

namespace LB {
  std::set<std::string> LB::Program::FUNCS = {};

  LB::Type::Type(std::string t, bool notArray) {
    if (notArray == true) {
      switch (t[0]) {
        case '%':
          this->type = LB::TYPE::VAR;
          break;
        case ':':
          this->type = LB::TYPE::TUPLE;
          break;
        default:
          this->type = LB::TYPE::N;
          break;
      }
    } else {
      switch (t[0]) {
        case 'v':
          this->type = LB::TYPE::VOID;
          break;
        case 't':
          this->type = LB::TYPE::TUPLE;
          break;
        case 'c':
          this->type = LB::TYPE::CODE;
          break;
        case 'i':
          this->type = LB::TYPE::INT;
          this->arr_count = std::count(t.begin(), t.end(), '[');
          break;
      }
    }
  }

  std::string LB::Type::toString() {
    std::string res;
    // std::cout << "this->type: " << this->type << "\n";
    switch (this->type) {
      case LB::TYPE::TUPLE:
        res += "tuple";
        break;
      case LB::TYPE::INT:
        res += "int64";
        for (int k = 0; k < this->arr_count; k++) {
          res += "[]";
        }
        break;
      case LB::TYPE::VOID:
        res += "void";
        break;
      case LB::TYPE::CODE:
        res += "code";
        break;
    }
    return res;
  }


    ///////
   // Ins
  ///////

  LB::InsReturn::InsReturn(std::vector<std::string> & v) {
    // return (vars[0])?
    // std::cout << "probe ret,000 \n";
    if (v.size() > 0) {
      // std::cout << "probe v[0], : " << v.size() << "\n";
      LB::Var* var = new LB::Var(v[0]);
      this->vars.push_back(var);
    }
  }

  void LB::InsReturn::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\treturn";
    if (this->vars.size() == 1) {
      o << " " << this->vars[0]->toString(path);
    }
  }

  LB::InsAssignCall::InsAssignCall(std::vector<std::string> & v) {
    // vars[0] <- call vars[1] (vars[2], ... vars[n])
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);

    var = new LB::Var(v[2]);
    this->vars.push_back(var);
    for (int k = 3; k < v.size(); k++) {
      var = new LB::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LB::InsAssignCall::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    if (LB::Program::FUNCS.count(this->vars[1]->toString(path))) {
      o << "\n\t" << this->vars[0]->toString(path) << " <- call " << this->vars[1]->toString(path) << "(";
    } else {
      o << "\n\t" << this->vars[0]->toString(path) << " <- call " << this->vars[1]->toString(path) << "(";
    }
    if (this->vars.size() > 2) {
      o << this->vars[2]->toString(path);
      for (int k = 3; k < this->vars.size(); k++) {
        o << ", " << this->vars[k]->toString(path);
      }
      o << ")";
    }
  }

  LB::InsOpAssign::InsOpAssign(std::vector<std::string> & v) {
    // vars[0] <- vars[1] vars[2] vars[3]
    LB::Var* var;
    for (int k = 0; k < v.size(); k++) {
      if (k == 2) {
        var = new LB::Var(v[k]);
      } else {
        var = new LB::Var(v[k]);
      }
      this->vars.push_back(var);
    }
  }

  void LB::InsOpAssign::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\t" << this->vars[0]->toString(path) << " <- " << this->vars[1]->toString(path) << " " << this->vars[2]->toString(path) << " " << this->vars[3]->toString(path);
  }

  LB::InsType::InsType(std::vector<std::string> & v) {

    for (int k = 1; k < v.size(); k++) {
      LB::Var* var = new LB::Var(v[0], v[k]);
      this->vars.push_back(var);
    }

  }

  void LB::InsType::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    for (int k = 0; k < this->vars.size(); k++) {
      o << "\n\t" << this->vars[k]->type->toString() << " " << this->vars[k]->toString(path);
    }
  }

  LB::InsCall::InsCall(std::vector<std::string> & v) {
    // call vars[0](vars[1].. vars[n])
    LB::Var* var = new LB::Var(v[1]);
    this->vars.push_back(var);

    for (int k = 2; k < v.size(); k++) {
      LB::Var* var = new LB::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LB::InsCall::toIR(std::ofstream &o, LB::Function * currF, std::string path) {

    if (LB::Program::FUNCS.count(this->vars[0]->toString(path))) {
      o << "\n\tcall " << this->vars[0]->toString(path) << "(";
    } else {
      o << "\n\tcall " << this->vars[0]->toString(path) << "(";
    }

    if (this->vars.size() > 1) {
      o << this->vars[1]->toString(path);
      for (int k = 2; k < this->vars.size(); k++) {
        o << ", " << this->vars[k]->toString(path);
      }
      o << ")";
    }
  }

  LB::InsAssign::InsAssign(std::vector<std::string> & v) {
    LB::Var* var;
    if (v[0].find("[") != std::string::npos) {
      var = new LB::Var(v[0], true);
    } else {
      var = new LB::Var(v[0]);
    }
    this->vars.push_back(var);

    if (v[1].find("[") != std::string::npos) {
      var = new LB::Var(v[1], true);
    } else {
      var = new LB::Var(v[1]);
    }
    this->vars.push_back(var);
  }

  void LB::InsAssign::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\t" << this->vars[0]->toString(path) << " <- " << this->vars[1]->toString(path);
  }

  LB::InsLength::InsLength(std::vector<std::string> & v) {
    // vars[0] <- length vars[1] vars[2]
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);
    var = new LB::Var(v[2]);
    this->vars.push_back(var);
    var = new LB::Var(v[3]);
    this->vars.push_back(var);
  }

  void LB::InsLength::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\t" << this->vars[0]->toString(path) << " <- length " << this->vars[1]->toString(path) << " " << this->vars[2]->toString(path);
  }

  LB::InsNewArray::InsNewArray(std::vector<std::string> & v) {
    // vars[0] <- new Array(vars[1]...vars[n])
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);
    for (int k = 3; k < v.size(); k++) {
      LB::Var* var = new LB::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LB::InsNewArray::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\t" << this->vars[0]->toString(path) << " <- new Array(" << this->vars[1]->toString(path);
    for (int k = 2; k < this->vars.size(); k++) {
      o << ", " << this->vars[k]->toString(path);
    }
    o << ")";
  }

  LB::InsNewTuple::InsNewTuple(std::vector<std::string> & v) {
    // vars[0] <- new Tuple(vars[1])
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);
    var = new LB::Var(v[3]);
    this->vars.push_back(var);
  }

  void LB::InsNewTuple::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\t" << this->vars[0]->toString(path) << " <- new Tuple(" << this->vars[1]->toString(path) << ")";
  }

  LB::InsLabel::InsLabel(std::vector<std::string> & v) {
    // vars[0] <- new Tuple(vars[1])
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);
  }

  void LB::InsLabel::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    o << "\n\t" << this->vars[0]->toString(path);
  }

  LB::InsContinue::InsContinue(std::vector<std::string> & v) {
  }

  void LB::InsContinue::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
  }

  LB::InsBreak::InsBreak(std::vector<std::string> & v) {
  }

  void LB::InsBreak::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
  }

  LB::InsScope::InsScope(std::vector<std::string> & v) {
  }

  void LB::InsScope::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    int scopeCount = 0;
    for (auto ins : this->inss) {
      if (typeid(*ins) == typeid(LB::InsScope)) {
        ins->toIR(o, currF, path + "_" + std::to_string(scopeCount));
        scopeCount += 1;
      } else {
        ins->toIR(o, currF, path);
      }
    }
  }

  LB::InsIf::InsIf(std::vector<std::string> & v) {
    // if (vars[0] vars[1] vars[2]) vars[3] vars[4]
    for (auto str : v) {
      LB::Var* var = new LB::Var(str);
      this->vars.push_back(var);
    }
  }

  void LB::InsIf::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
    std::string suffix = std::to_string(rand());
    o << "\n\tint64 %temp_" << suffix;
    o << "\n\t%temp_" << suffix << " <- " << this->vars[0]->toString(path) << " " << this->vars[1]->toString(path) << " " << this->vars[2]->toString(path);
    o << "\n\tbr %temp_" << suffix << " " << this->vars[3]->toString(path) << " " << this->vars[4]->toString(path);
  }

  LB::InsWhile::InsWhile(std::vector<std::string> & v) {
  }

  void LB::InsWhile::toIR(std::ofstream &o, LB::Function * currF, std::string path) {
  }

    ///////
   // Var
  ///////

  std::string LB::Var::toString(std::string path) {

    std::string res = this->name;
    if (this->name[0] == '%' && path != "") {
      res += "_" + path;
    }

    for (auto t : this->ts) {
      res += "[";
      res += t->toString(path);
      res += "]";
    }
    return res;
  }

  LB::Var::Var(std::string t, std::string n) {
    this->name = n;
    this->type = new LB::Type(t);
  }

  LB::Var::Var(std::string & str, bool hasT) {
    if (hasT) {
      int r = 0;
      while (str[r] != '[') {
        r++;
      }
      this->name = str.substr(0, r);
      int l = r + 1;
      for (; r < str.size(); r++) {
        if (str[r] == ']') {
          std::string sub = str.substr(l, r - l);
          LB::Var* t = new LB::Var(sub);
          this->ts.push_back(t);
          l = r + 2;
        }
      }
    } else {
      this->name = str;
      this->type = new LB::Type(this->name, true);
    }
  }
}
