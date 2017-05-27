// By Zhiping

#include <IR.h>

namespace IR {

  IR::Type::Type(std::string t) {
    switch (t[0]) {
      case 'v':
        this->type = IR::TYPE::VOID;
        break;
      case 't':
        this->type = IR::TYPE::TUPLE;
        break;
      case 'c':
        this->type = IR::TYPE::CODE;
        break;
      case 'i':
        this->type = IR::TYPE::INT;
        this->arr_count = std::count(t.begin(), t.end(), '[');
        break;
    }
  }

  IR::InsBr::InsBr(std::vector<std::string> & v) {
    for (int k = 0; k < v.size(); k++) {
      IR::Var* var = new IR::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void IR::InsBr::toL3(std::ofstream &o, IR::Function * currF) {
    if (this->vars.size() == 1) {
      o << "\n\tbr " + this->vars[0]->toString();
    } else {
      o << "\n\tbr " + this->vars[0]->toString() + " " + this->vars[1]->toString() + " " + this->vars[2]->toString();
    }
    return;
  }

  IR::InsReturn::InsReturn(std::vector<std::string> & v) {
    if (v.size() > 0) {
      IR::Var* var = new IR::Var(v[0]);
      // std::cout << "v[0]: " << v[0] << "\n";
      this->vars.push_back(var);
    }
  }

  void IR::InsReturn::toL3(std::ofstream &o, IR::Function * currF) {
    if (this->vars.size() > 0) {
      // std::cout << "this->vars[0]->toString(): " << this->vars[0]->toString() << "\n";
      o << "\n\treturn " + this->vars[0]->toString();
    }
    else {
      o << "\n\treturn";
    }
  }

  IR::InsAssignCall::InsAssignCall(std::vector<std::string> & v) {
    IR::Var* var = new IR::Var(v[0]);
    this->vars.push_back(var);
    for (int k = 2; k < v.size(); k++) {
      IR::Var* var = new IR::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void IR::InsAssignCall::toL3(std::ofstream &o, IR::Function * currF) {
    o << "\n\t" + this->vars[0]->toString() + " <- call " + this->vars[1]->toString() + "(";
    if (this->vars.size() > 2) {
      o << this->vars[2]->toString();
    }
    for (int k = 3; k < this->vars.size(); k++) {
      o << ", " + this->vars[k]->toString();
    }
    o << ")";
    return;
  }

  IR::InsOpAssign::InsOpAssign(std::vector<std::string> & v) {
    for (int k = 0; k < v.size(); k++) {
      IR::Var* var = new IR::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void IR::InsOpAssign::toL3(std::ofstream &o, IR::Function * currF) {
    o << "\n\t" + this->vars[0]->toString() + " <- " + this->vars[1]->toString() + " " + this->vars[2]->toString() + " " + this->vars[3]->toString();
  }


  IR::InsType::InsType(std::vector<std::string> & v) {
    IR::Var* var = new IR::Var(v[0], v[1]);
    this->vars.push_back(var);
  }

  void IR::InsType::toL3(std::ofstream &o, IR::Function * currF) {
    return;
  }

  IR::InsCall::InsCall(std::vector<std::string> & v) {
    for (int k = 1; k < v.size(); k++) {
      IR::Var* var = new IR::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void IR::InsCall::toL3(std::ofstream &o, IR::Function * currF) {
    o << "\n\tcall " + this->vars[0]->toString() + "(";
    if (this->vars.size() > 1) {
      o << this->vars[1]->toString();
    }
    for (int k = 2; k < this->vars.size(); k++) {
      o << ", " + this->vars[k]->toString();
    }
    o << ")";
    return;
    // return "";
  }

  IR::InsAssign::InsAssign(std::vector<std::string> & v) {
    IR::Var* var;
    if (v[0].find("[") != std::string::npos) {
      var = new IR::Var(v[0], true);
    } else {
      var = new IR::Var(v[0]);
    }
    this->vars.push_back(var);

    if (v[1].find("[") != std::string::npos) {
      var = new IR::Var(v[1], true);
    } else {
      var = new IR::Var(v[1]);
    }
    this->vars.push_back(var);
  }

  void IR::InsAssign::toL3(std::ofstream &o, IR::Function * currF) {
    if (this->vars[0]->ts.size() > 0) {
      // std::cout << "probe 1\n";
      std::string suffix = this->vars[0]->printAddr(o, currF);
      o << "\n\tstore addr_" << suffix << " <- " << this->vars[1]->toString();
    } else if (this->vars[1]->ts.size() > 0) {
      // std::cout << "probe 2\n";
      std::string suffix = this->vars[1]->printAddr(o, currF);
      o << "\n\t" << this->vars[0]->toString() << " <- load addr_" << suffix;
    } else {
      // std::cout << "probe 3\n";
      o << "\n\t" + this->vars[0]->toString() + " <- " + this->vars[1]->toString();
    }
    o << "\n";
  }

  IR::InsLength::InsLength(std::vector<std::string> & v) {
    IR::Var* var = new IR::Var(v[0]);
    this->vars.push_back(var);
    for (int k = 2; k < v.size(); k++) {
      IR::Var* var = new IR::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void IR::InsLength::toL3(std::ofstream &o, IR::Function * currF) {
    std::string suffix = std::to_string(rand());

    if (currF->type_map[this->vars[1]->name]->type == IR::TYPE::TUPLE) {
      o << "\n\taddr_" << suffix << " <- " << this->vars[1]->toString();
      o << "\n\t" << this->vars[0]->toString() << " <- load addr_" << suffix;
      o << "\n\t" << this->vars[0]->toString() << " <- " << this->vars[0]->toString() << " << 1";
      o << "\n\t" << this->vars[0]->toString() << " <- " << this->vars[0]->toString() << " + 1";

    } else {
      o << "\n\taddr_" << suffix << " <- 8 * " << this->vars[2]->toString();
      o << "\n\taddr_" << suffix << " <- addr_" << suffix << " + 16";
      o << "\n\taddr_" << suffix << " <- addr_" << suffix << " + " << this->vars[1]->toString();
      o << "\n\t" << this->vars[0]->toString() << " <- load addr_" << suffix;
      o << "\n";
    }
  }

  IR::InsNewArray::InsNewArray(std::vector<std::string> & v) {
    IR::Var* var = new IR::Var(v[0]);
    this->vars.push_back(var);
    for (int k = 3; k < v.size(); k++) {
      IR::Var* var = new IR::Var(v[k]);
      this->vars.push_back(var);
    }
  }

  void IR::InsNewArray::toL3(std::ofstream &o, IR::Function * currF) {
    std::string suffix = std::to_string(rand());
    int d = this->vars.size()-1;

    for (int k = 0; k < d; k++) {
      o << "\n\tp" + std::to_string(k) + "D_" + suffix + " <- " + this->vars[k+1]->toString() + " >> 1";
    }

    o << "\n\tv0_" + suffix + " <- p0D_" + suffix;

    for (int k = 1; k < d; k++) {
      o << "\n\tv0_" + suffix + " <- v0_" + suffix + " * p" + std::to_string(k) + "D_" + suffix;
    }

    o << "\n\tv0_" + suffix + " <- v0_" + suffix + " << 1";
    o << "\n\tv0_" + suffix + " <- v0_" + suffix + " + " + std::to_string(((d+1) << 1) + 1);
    o << "\n\t" + this->vars[0]->toString() + " <- call allocate(v0_" + suffix + ", 1)";

    o << "\n\tv1_" + suffix + " <- " + this->vars[0]->toString() + " + 8";
    o << "\n\tstore v1_" + suffix + " <- " + std::to_string((d << 1) + 1);

    for (int k = 0; k < d; k++) {
      o << "\n\tv" + std::to_string(k+2) + "_" + suffix + " <- " + this->vars[0]->toString() + " + " + std::to_string((k+2) * 8);
      o << "\n\tstore v" + std::to_string(k+2) + "_" + suffix + " <- " + this->vars[k+1]->toString();
    }

    o << "\n";
  }

  IR::InsNewTuple::InsNewTuple(std::vector<std::string> & v) {
    // for (int k = 0; k < 3; k++) {
    //   IR::Var* var = new IR::Var(v[k]);
    //   this->vars.push_back(var);
    // }
    IR::Var* var = new IR::Var(v[0]);
    this->vars.push_back(var);
    var = new IR::Var(v[3]);
    this->vars.push_back(var);
  }

  void IR::InsNewTuple::toL3(std::ofstream &o, IR::Function * currF) {
    o << "\n\t" << this->vars[0]->toString() << " <- call allocate(" << this->vars[1]->toString() << ", 1)";

    return;
  }


  IR::BasicBlock::BasicBlock(std::string name) {
    this->label = name;
  }

  IR::Var::Var(std::string t, std::string n) {
    this->name = n;
    this->type = new IR::Type(t);
    if (t[0] == 't') {

    }
  }

  std::string IR::Var::toString() {
    // std::cout << "toString " << this->name << "\n";
    if (this->name[0] == '%') {
      return this->name.substr(1, this->name.size()-1);
    }
    return this->name;
  }

  IR::Var::Var(std::string & str, bool hasT) {
    if (hasT) {
      // std::cout << "str: " << str;
      int r = 0;
      while (str[r] != '[') {
        // std::cout << "r: " << r << " str[r]: " << str[r] << "\n";
        r++;
      }
      this->name = str.substr(0, r);
      int l = r + 1;
      for (; r < str.size(); r++) {
        if (str[r] == ']') {
          // std::cout << "r: " << r << " str[r]: " << str[r] << "\n";
          std::string sub = str.substr(l, r - l);
          IR::Var* t = new IR::Var(sub);
          this->ts.push_back(t);
          l = r + 2;
        }
      }
    } else {
      this->name = str;
    }
  }

  std::string IR::Var::printAddr(std::ofstream &o, IR::Function * currF) {
    int d = this->ts.size();
    std::string suffix = std::to_string(rand());

    if (currF->type_map[this->name]->type == IR::TYPE::TUPLE) {
      o << "\n\taddr_" << suffix << " <- " << this->toString() << " + " << (std::stoll(this->ts[0]->name) * 8) + 8;
    } else {

      // A[D1][D2][D3]
      // A[k][i][j]
      // 16 + (3 * 8) + (k * D2 * D3 + i * D3 + j)
      for (int k = 2; k <= d; k++) {
        o << "\n\tADDR_D" << k << "_" << suffix << " <- " << this->toString() << " + " << (k + 1) * 8;
        o << "\n\tD" << k << "_" << suffix << "_ <- load " << "ADDR_D" << k << "_" << suffix;
        o << "\n\tD" << k << "_" << suffix << " <- " << "D" << k << "_" << suffix << "_ >> 1";
      }

      o << "\n\toffset_" << suffix << " <- " << this->ts[d-1]->toString() << " + " << (2 + d);
      if (d > 1) {
        o << "\n\tmult_" << suffix << " <- D" << d << "_" << suffix;
        // o << "\n\tmult_" << suffix << " <- " << this->ts[d-1]->toString();
      }

      for (int k = d-2; k >= 0; k--) {
        o << "\n\tinc_" << suffix << " <- mult_" << suffix << " * " << this->ts[k]->toString();
        o << "\n\toffset_" << suffix << " <- offset_" << suffix << " + inc_" << suffix;
        if (k != 0) {
          o << "\n\tmult_" << suffix << " <- mult_" << suffix << " * D" << k+1 << "_" << suffix;
        }
      }
      o << "\n\toffset_" << suffix << " <- offset_" << suffix << " * 8";
      o << "\n\taddr_" << suffix << " <- " << this->toString() << " + offset_" << suffix;
    }
    return suffix;
  }
}
