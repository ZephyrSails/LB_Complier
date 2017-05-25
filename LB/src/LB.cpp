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

  LB::InsBr::InsBr(std::vector<std::string> & v) {
    // br vars[0] (vars[1] vars[2])?
    for (int k = 0; k < v.size(); k++) {
      LB::Var* var = new LB::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LB::InsBr::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\tbr";
    for (auto v : this->vars) {
      o << " " << v->toString();
    }
  }

  LB::InsReturn::InsReturn(std::vector<std::string> & v) {
    // return (vars[0])?
    if (v.size() > 0) {
      LB::Var* var = new LB::Var(v[0]);
      this->vars.push_back(var);
    }
  }

  void LB::InsReturn::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\treturn";
    if (this->vars.size() > 0) {
      o << " " << this->vars[0]->toString();
    }
  }

  LB::InsAssignCall::InsAssignCall(std::vector<std::string> & v) {
    // vars[0] <- call vars[1] (vars[2], ... vars[n])
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);

    var = new LB::Var(v[2], false, true);
    this->vars.push_back(var);
    for (int k = 3; k < v.size(); k++) {
      var = new LB::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LB::InsAssignCall::toIR(std::ofstream &o, LB::Function * currF) {
    if (LB::Program::FUNCS.count(this->vars[1]->toString())) {
      o << "\n\t" << this->vars[0]->toString() << " <- call :" << this->vars[1]->toString() << "(";
    } else {
      o << "\n\t" << this->vars[0]->toString() << " <- call " << this->vars[1]->toString() << "(";
    }
    if (this->vars.size() > 2) {
      o << this->vars[2]->toString();
      for (int k = 3; k < this->vars.size(); k++) {
        o << ", " << this->vars[k]->toString();
      }
      o << ")";
    }
  }

  LB::InsOpAssign::InsOpAssign(std::vector<std::string> & v) {
    // vars[0] <- vars[1] vars[2] vars[3]
    LB::Var* var;
    for (int k = 0; k < v.size(); k++) {
      if (k == 2) {
        var = new LB::Var(v[k], false, true);
      } else {
        var = new LB::Var(v[k]);
      }
      this->vars.push_back(var);
    }
  }

  void LB::InsOpAssign::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- " << this->vars[1]->toString() << " " << this->vars[2]->toString() << " " << this->vars[3]->toString();

  }

  LB::InsType::InsType(std::vector<std::string> & v) {
    // vars[0]->type vars[0]

    for (int k = 1; k < v.size(); k++) {
      LB::Var* var = new LB::Var(v[0], v[k]);
      this->vars.push_back(var);
    }

  }

  void LB::InsType::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\t" << this->vars[0]->type->toString() << " " << this->vars[0]->toString();

    if (this->vars[0]->type->type == LB::TYPE::TUPLE || (this->vars[0]->type->type == LB::TYPE::INT && this->vars[0]->type->arr_count > 0)) {
      o << "\n\t" << this->vars[0]->name << " <- 0";
    }
  }

  LB::InsCall::InsCall(std::vector<std::string> & v) {
    // call vars[0](vars[1].. vars[n])
    LB::Var* var = new LB::Var(v[1], false, true);
    this->vars.push_back(var);

    for (int k = 2; k < v.size(); k++) {
      LB::Var* var = new LB::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LB::InsCall::toIR(std::ofstream &o, LB::Function * currF) {

    if (LB::Program::FUNCS.count(this->vars[0]->toString())) {
      o << "\n\tcall :" << this->vars[0]->toString() << "(";
    } else {
      o << "\n\tcall " << this->vars[0]->toString() << "(";
    }

    if (this->vars.size() > 1) {
      o << this->vars[1]->toString();
      for (int k = 2; k < this->vars.size(); k++) {
        o << ", " << this->vars[k]->toString();
      }
      o << ")";
    }
  }

  LB::InsAssign::InsAssign(std::vector<std::string> & v) {
    // vars[0] <- vars[1]
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

  void LB::InsAssign::toIR(std::ofstream &o, LB::Function * currF) {
    std::string suffix = std::to_string(rand());
    bool allocChecked = false;
    bool checkID;
    for (int k = 0; k < 2; k++) {
      if (this->vars[k]->ts.size() > 0) {
        o << "\n\t%not_zero_" << suffix << " <- 0 < " << this->vars[k]->name;
        o << "\n\tbr %not_zero_" << suffix << " :alloc_" << suffix << " :notalloc_" << suffix;
        checkID = k;
        allocChecked = true;
      }
    }
    if (allocChecked) {
      o << "\n\t:notalloc_" << suffix;
      o << "\n\tcall array-error(0, 0)";
      o << "\n\tbr :alloc_" << suffix;
      o << "\n\t:alloc_" << suffix;

      for (int j = 0; j < this->vars[checkID]->ts.size(); j++) {
        o << "\n\t%l_" << j << "_" << suffix << " <- length " << this->vars[checkID]->name << " " << j;
        o << "\n\t%l_" << j << "_" << suffix << " <- " << "%l_" << j << "_" << suffix << " >> 1";
        o << "\n\t%validLen_" << j << "_" << suffix << " <- " << this->vars[checkID]->ts[j]->toString() << " < %l_" << j << "_" << suffix;

        o << "\n\tbr %validLen_" << j << "_" << suffix << " :validLen_" << j << "_" << suffix << " :invalidLen_" << j << "_"  << suffix;
        o << "\n\t:invalidLen_" << j << "_" << suffix;
        o << "\n\t%err_index_" << suffix << " <- " << this->vars[checkID]->ts[j]->toString();
        o << "\n\t%err_index_" << suffix << " <- %err_index_" << suffix << " << 1";
        o << "\n\t%err_index_" << suffix << " <- %err_index_" << suffix << " + 1";
        o << "\n\tcall array-error(" << this->vars[checkID]->name << ", %err_index_" << suffix << ")";
        o << "\n\tbr :validLen_" << j << "_" << suffix;
        o << "\n\t:validLen_" << j << "_" << suffix;
      }
    }

    o << "\n\t" << this->vars[0]->toString() << " <- " << this->vars[1]->toString();
  }

  LB::InsLength::InsLength(std::vector<std::string> & v) {
    // vars[0] <- length vars[1] vars[2]
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);
    var = new LB::Var(v[2]);
    this->vars.push_back(var);
    var = new LB::Var(v[3], false, true);
    this->vars.push_back(var);
  }

  void LB::InsLength::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- length " << this->vars[1]->toString() << " " << this->vars[2]->toString();
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

  void LB::InsNewArray::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- new Array(" << this->vars[1]->toString();
    for (int k = 2; k < this->vars.size(); k++) {
      o << ", " << this->vars[k]->toString();
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

  void LB::InsNewTuple::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- new Tuple(" << this->vars[1]->toString() << ")";
  }

  LB::InsLabel::InsLabel(std::vector<std::string> & v) {
    // vars[0] <- new Tuple(vars[1])
    LB::Var* var = new LB::Var(v[0]);
    this->vars.push_back(var);
  }

  void LB::InsLabel::toIR(std::ofstream &o, LB::Function * currF) {
    o << "\n\t" << this->vars[0]->toString();
  }

    ///////
   // Var
  ///////

  std::string LB::Var::toString() {
    // if (this->name[0] == '%') {
    //   return this->name.substr(1, this->name.size()-1);
    // }
    std::string res = this->name;

    for (auto t : this->ts) {
      res += "[";
      res += t->toString();
      res += "]";
    }
    return res;
  }

  LB::Var::Var(std::string t, std::string n) {
    this->name = n;
    this->type = new LB::Type(t);
  }

  LB::Var::Var(std::string & str, bool hasT, bool avoidEncode) {
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
          LB::Var* t = new LB::Var(sub, false, true);
          this->ts.push_back(t);
          l = r + 2;
        }
      }
    } else {
      if (!avoidEncode && str[0] != ':' && str[0] != '%') {
        this->name = std::to_string((std::stoll(str) << 1) + 1);
      } else {
        this->name = str;
      }
      this->type = new LB::Type(this->name, true);
    }
  }
}
