// By Zhiping

#include <LA.h>

namespace LA {
  std::set<std::string> LA::Program::FUNCS = {};

  LA::Type::Type(std::string t, bool notArray) {
    if (notArray == true) {
      switch (t[0]) {
        case '%':
          this->type = LA::TYPE::VAR;
          break;
        case ':':
          this->type = LA::TYPE::TUPLE;
          break;
        default:
          this->type = LA::TYPE::N;
          break;
      }
    } else {
      switch (t[0]) {
        case 'v':
          this->type = LA::TYPE::VOID;
          break;
        case 't':
          this->type = LA::TYPE::TUPLE;
          break;
        case 'c':
          this->type = LA::TYPE::CODE;
          break;
        case 'i':
          this->type = LA::TYPE::INT;
          this->arr_count = std::count(t.begin(), t.end(), '[');
          break;
      }
    }
  }

  std::string LA::Type::toString() {
    std::string res;
    // std::cout << "this->type: " << this->type << "\n";
    switch (this->type) {
      case LA::TYPE::TUPLE:
        res += "tuple";
        break;
      case LA::TYPE::INT:
        res += "int64";
        for (int k = 0; k < this->arr_count; k++) {
          res += "[]";
        }
        break;
      case LA::TYPE::VOID:
        res += "void";
        break;
      case LA::TYPE::CODE:
        res += "code";
        break;
    }
    return res;
  }


    ///////
   // Ins
  ///////

  void LA::Instruction::decode(std::ofstream &o) {
    std::vector<LA::Var *> de;
    de = this->toDecode();
    int count = 0;
    for (auto d : de) {
      if (d->type->type == LA::TYPE::VAR) {
        o << "\n\t" << d->toString() << LA::DECODE << " <- " << d->toString() << " >> 1";
        d->name += LA::DECODE;
      } else if (d->type->type == LA::TYPE::N) {
        o << "\n\t%tempN" << count << LA::DECODE << " <- " << d->toString() << " >> 1";
        d->name = "%tempN" + std::to_string(count) + LA::DECODE;
        count++;
      }
    }
  }

  void LA::Instruction::encode(std::ofstream &o) {
    std::vector<LA::Var *> en;
    en = this->toEncode();
    for (auto e : en) {
      if (e->type->type == LA::TYPE::VAR) {
        o << "\n\t" << e->toString() << " <- " << e->toString() << " << 1";
        o << "\n\t" << e->toString() << " <- " << e->toString() << " + 1";
      }
    }
  }

  LA::InsBr::InsBr(std::vector<std::string> & v) {
    // br vars[0] (vars[1] vars[2])?
    for (int k = 0; k < v.size(); k++) {
      LA::Var* var = new LA::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LA::InsBr::toIR(std::ofstream &o, LA::Function * currF) {
    this->decode(o);
    o << "\n\tbr";
    for (auto v : this->vars) {
      o << " " << v->toString();
    }
  }

  std::vector<LA::Var *> LA::InsBr::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsBr::toDecode() {
    std::vector<LA::Var *> res;
    if (this->vars.size() == 3) {
      res.push_back(this->vars[0]);
    }
    return res;
  }

  LA::InsReturn::InsReturn(std::vector<std::string> & v) {
    // return (vars[0])?
    if (v.size() > 0) {
      LA::Var* var = new LA::Var(v[0]);
      this->vars.push_back(var);
    }
  }

  void LA::InsReturn::toIR(std::ofstream &o, LA::Function * currF) {
    o << "\n\treturn";
    if (this->vars.size() > 0) {
      o << " " << this->vars[0]->toString();
    }
  }

  std::vector<LA::Var *> LA::InsReturn::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsReturn::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

  LA::InsAssignCall::InsAssignCall(std::vector<std::string> & v) {
    // vars[0] <- call vars[1] (vars[2], ... vars[n])
    LA::Var* var = new LA::Var(v[0]);
    this->vars.push_back(var);

    var = new LA::Var(v[2], false, true);
    this->vars.push_back(var);
    for (int k = 3; k < v.size(); k++) {
      var = new LA::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LA::InsAssignCall::toIR(std::ofstream &o, LA::Function * currF) {
    if (LA::Program::FUNCS.count(this->vars[1]->toString())) {
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

  std::vector<LA::Var *> LA::InsAssignCall::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsAssignCall::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

  LA::InsOpAssign::InsOpAssign(std::vector<std::string> & v) {
    // vars[0] <- vars[1] vars[2] vars[3]
    LA::Var* var;
    for (int k = 0; k < v.size(); k++) {
      if (k == 2) {
        var = new LA::Var(v[k], false, true);
      } else {
        var = new LA::Var(v[k]);
      }
      this->vars.push_back(var);
    }
  }

  void LA::InsOpAssign::toIR(std::ofstream &o, LA::Function * currF) {
    this->decode(o);
    o << "\n\t" << this->vars[0]->toString() << " <- " << this->vars[1]->toString() << " " << this->vars[2]->toString() << " " << this->vars[3]->toString();

    this->encode(o);
  }

  std::vector<LA::Var *> LA::InsOpAssign::toEncode() {
    std::vector<LA::Var *> res;
    res.push_back(this->vars[0]);
    return res;
  }

  std::vector<LA::Var *> LA::InsOpAssign::toDecode() {
    std::vector<LA::Var *> res;
    res.push_back(this->vars[1]);
    res.push_back(this->vars[3]);
    return res;
  }

  LA::InsType::InsType(std::vector<std::string> & v) {
    // vars[0]->type vars[0]
    LA::Var* var = new LA::Var(v[0], v[1]);
    this->vars.push_back(var);
  }

  void LA::InsType::toIR(std::ofstream &o, LA::Function * currF) {
    o << "\n\t" << this->vars[0]->type->toString() << " " << this->vars[0]->toString();

    if (this->vars[0]->type->type == LA::TYPE::TUPLE || (this->vars[0]->type->type == LA::TYPE::INT && this->vars[0]->type->arr_count > 0)) {
      o << "\n\t" << this->vars[0]->name << " <- 0";
    }
  }

  std::vector<LA::Var *> LA::InsType::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsType::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

  LA::InsCall::InsCall(std::vector<std::string> & v) {
    // call vars[0](vars[1].. vars[n])
    LA::Var* var = new LA::Var(v[1], false, true);
    this->vars.push_back(var);

    for (int k = 2; k < v.size(); k++) {
      LA::Var* var = new LA::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LA::InsCall::toIR(std::ofstream &o, LA::Function * currF) {

    if (LA::Program::FUNCS.count(this->vars[0]->toString())) {
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

  std::vector<LA::Var *> LA::InsCall::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsCall::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

  LA::InsAssign::InsAssign(std::vector<std::string> & v) {
    // vars[0] <- vars[1]
    LA::Var* var;
    if (v[0].find("[") != std::string::npos) {
      var = new LA::Var(v[0], true);
    } else {
      var = new LA::Var(v[0]);
    }
    this->vars.push_back(var);

    if (v[1].find("[") != std::string::npos) {
      var = new LA::Var(v[1], true);
    } else {
      var = new LA::Var(v[1]);
    }
    this->vars.push_back(var);
  }

  void LA::InsAssign::toIR(std::ofstream &o, LA::Function * currF) {
    this->decode(o);
    std::string suffix = std::to_string(rand());
    bool allocChecked = false;
    bool checkID;
    for (int k = 0; k < 2; k++) {
      if (this->vars[k]->ts.size() > 0 && currF->checkedVars.count(this->vars[k]->toString()) == 0) {
        o << "\n\t%not_zero_" << suffix << " <- 0 < " << this->vars[k]->name;
        o << "\n\tbr %not_zero_" << suffix << " :alloc_" << suffix << " :notalloc_" << suffix;
        // o << "\n\tbr " << this->vars[k]->name << " :alloc_" << suffix << " :notalloc_" << suffix;
        checkID = k;
        allocChecked = true;
        currF->checkedVars.insert(this->vars[k]->toString());
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

  std::vector<LA::Var *> LA::InsAssign::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsAssign::toDecode() {
    std::vector<LA::Var *> res;
    for (auto t : this->vars[0]->ts) {
      if (t->type->type != LA::TYPE::N) {
        res.push_back(t);
      }
    }
    for (auto t : this->vars[1]->ts) {
      if (t->type->type != LA::TYPE::N) {
        res.push_back(t);
      }
    }
    return res;
  }

  LA::InsLength::InsLength(std::vector<std::string> & v) {
    // vars[0] <- length vars[1] vars[2]
    LA::Var* var = new LA::Var(v[0]);
    this->vars.push_back(var);
    var = new LA::Var(v[2]);
    this->vars.push_back(var);
    // var = new LA::Var(v[3], false, true);
    var = new LA::Var(v[3]);
    this->vars.push_back(var);
  }

  void LA::InsLength::toIR(std::ofstream &o, LA::Function * currF) {
    this->decode(o);
    o << "\n\t" << this->vars[0]->toString() << " <- length " << this->vars[1]->toString() << " " << this->vars[2]->toString();
  }

  std::vector<LA::Var *> LA::InsLength::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsLength::toDecode() {
    std::vector<LA::Var *> res;
    res.push_back(this->vars[2]);
    return res;
  }

  LA::InsNewArray::InsNewArray(std::vector<std::string> & v) {
    // vars[0] <- new Array(vars[1]...vars[n])
    LA::Var* var = new LA::Var(v[0]);
    this->vars.push_back(var);
    for (int k = 3; k < v.size(); k++) {
      LA::Var* var = new LA::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void LA::InsNewArray::toIR(std::ofstream &o, LA::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- new Array(" << this->vars[1]->toString();
    for (int k = 2; k < this->vars.size(); k++) {
      o << ", " << this->vars[k]->toString();
    }
    o << ")";
  }

  std::vector<LA::Var *> LA::InsNewArray::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsNewArray::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

  LA::InsNewTuple::InsNewTuple(std::vector<std::string> & v) {
    // vars[0] <- new Tuple(vars[1])
    LA::Var* var = new LA::Var(v[0]);
    this->vars.push_back(var);
    var = new LA::Var(v[3]);
    this->vars.push_back(var);
  }

  void LA::InsNewTuple::toIR(std::ofstream &o, LA::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- new Tuple(" << this->vars[1]->toString() << ")";
  }

  std::vector<LA::Var *> LA::InsNewTuple::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsNewTuple::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

  LA::InsLabel::InsLabel(std::vector<std::string> & v) {
    // vars[0] <- new Tuple(vars[1])
    LA::Var* var = new LA::Var(v[0]);
    this->vars.push_back(var);
  }

  void LA::InsLabel::toIR(std::ofstream &o, LA::Function * currF) {
    o << "\n\t" << this->vars[0]->toString();
  }

  std::vector<LA::Var *> LA::InsLabel::toEncode() {
    std::vector<LA::Var *> res;
    return res;
  }

  std::vector<LA::Var *> LA::InsLabel::toDecode() {
    std::vector<LA::Var *> res;
    return res;
  }

    ///////
   // Var
  ///////

  std::string LA::Var::toString() {
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

  LA::Var::Var(std::string t, std::string n) {
    this->name = n;
    this->type = new LA::Type(t);
  }

  LA::Var::Var(std::string & str, bool hasT, bool avoidEncode) {
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
          LA::Var* t = new LA::Var(sub, false, true);
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
      this->type = new LA::Type(this->name, true);
    }
  }
}
