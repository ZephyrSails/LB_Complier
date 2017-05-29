// By: Zhiping

#include <tile.h>

namespace L3 {

  L3::Tile::Tile() {
    std::vector< std::string > v;
    // return t || return
    v = {"VAR"};
    this->tiles[TNAME::RETURN] = new L3::Return(v);
    // var <- s
    this->tiles[TNAME::ASSIGN] = new L3::Var("V1");
    this->tiles[TNAME::ASSIGN]->instances.push_back(new L3::Var("S"));
    // var <- call callee ( args )
    this->tiles[TNAME::ASSIGN_CALL] = new L3::Var("V1");
    v = {":L1"};
    this->tiles[TNAME::ASSIGN_CALL]->instances.push_back(new L3::Call(v));
    // call callee ( args )
    v = {":L1"};
    this->tiles[TNAME::CALL] = new L3::Call(v);
    // br var label label
    v = {"V1", ":L1", ":L2"};
    this->tiles[TNAME::C_BR] = new L3::Br(v);
    v = {"V1", "V2", "OP", "V3"};
    this->tiles[TNAME::C_BR]->instances[0]->instances.push_back(new L3::Op(v));
    // label
    this->tiles[TNAME::LABEL] = new L3::Var(":L1");
    // br label
    v = {"V1"};
    this->tiles[TNAME::BR] = new L3::Br(v);
    // store var <- s
    v = {"D1", "V2"};
    this->tiles[TNAME::STORE] = new L3::Store(v);
    // var <- load var
    v = {"V1", "load", "V2"};
    this->tiles[TNAME::LOAD] = new L3::Var("V1");
    this->tiles[TNAME::LOAD]->instances.push_back(new L3::Load(v));
    // var <- t cmp t | var <- t op t
    v = {"V1", "V2", "OP", "V3"};
    this->tiles[TNAME::ASSIGN_OP] = new L3::Var("V1");
    this->tiles[TNAME::ASSIGN_OP]->instances.push_back(new L3::Op(v));
    // D1 <- V1 + N; store D1 <- s
    v = {"D1", "S"};
    this->tiles[TNAME::ADD_STORE] = new L3::Store(v);
    v = {"D1", "V1", "+", "8"};
    this->tiles[TNAME::ADD_STORE]->instances[0]->instances.push_back(new L3::Op(v));
    // v3 <- v3 * 8
  	// v1 <- v2 + v3
    // (v1 @ v2 v3 8)

    v = {"V1", "V2", "+", "V3"};
    this->tiles[TNAME::AT] = new L3::Var("V1");
    this->tiles[TNAME::AT]->instances.push_back(new L3::Op(v));
    v = {"V3", "V3", "*", "E"};
    this->tiles[TNAME::AT]->instances[0]->instances[1]->instances.push_back(new L3::Op(v));

    // this->tiles[TNAME::AT] = new L3::Var("V1");


  }

  std::string L3::Tile::Translate(int tIndex, L3::Instance * ins, std::string f_name) {
    std::string res = "";
    // return "";
    L3::Instance * tile = this->tiles[tIndex];

    switch (tIndex) {
      case TNAME::AT:
                // return "";
                return "\n\t\t(" + ins->name + " @ " + ins->instances[0]->instances[0]->name + " " + ins->instances[0]->instances[1]->name + " " + ins->instances[0]->instances[1]->instances[0]->instances[1]->name + ")";

      case TNAME::RETURN:
                return ins->toString();

      case TNAME::ASSIGN:
                return "\n\t\t(" + ins->name + " <- " + ins->instances[0]->name + ")";

      case TNAME::ASSIGN_CALL:
                res += ins->instances[0]->toString();
                res += "\n\t\t(" + ins->name + " <- rax)";
                return res;

      case TNAME::CALL:
                return ins->toString();

      case TNAME::C_BR:
                // L3::Br * br_ins;
                // br_ins = dynamic_cast<L3::Br*>(ins);
                return ins->toL2(f_name);
                // return ins->toString();

      case TNAME::LABEL:
                return "\n\t\t" + ins->name + "_" + f_name;

      case TNAME::BR:
                return ins->toL2(f_name);
                // return ins->toString();

      case TNAME::STORE:
                return ins->toString();

      case TNAME::LOAD:
                res += "\n\t\t(" + ins->name + " <- (mem " + ins->instances[0]->instances[0]->name + " 0))";
                return res;

      case TNAME::ASSIGN_OP:
                return ins->toString();

      case TNAME::ADD_STORE:
                // L3::Instance address = ins->instances[0]->instances[0];
                res += "\n\t\t((mem " + ins->instances[0]->instances[0]->instances[0]->name + " " + ins->instances[0]->instances[0]->instances[1]->name + ") <- " + ins->instances[1]->name + ")";
                return res;


      default:
                return "";
    }
  }

  bool L3::Tile::MatchTile(L3::Instance * ins, L3::Instance * til, std::vector< L3::Instance * > & leaf) {
    if (!(ins->equal(til)) || (til->instances.size() > 0 && ins->instances.size() != til->instances.size())) {
      // if couldn't match, or reached the leaf of the tail.
      return false;
    }

    if (til->instances.size() == 0) { // matched but didn't cover the whole tree.
      if (typeid(*ins) == typeid(L3::Var) && ins->instances.size() > 0) {
        leaf.push_back(ins);
      } else {
        for (auto l : ins->instances) {
          if (l)
          leaf.push_back(l);
        }
      }

      return true;
    }

    for (int k = 0; k < til->instances.size(); k++) {
      // if tail is deeper, keep trying
      if (((til->name == til->instances[k]->name) && (ins->name != ins->instances[k]->name)) ||
            !L3::Tile::MatchTile(ins->instances[k], til->instances[k], leaf)) {
        return false;
      }
    }
    return true;
  }

  void L3::Tile::Match(L3::Instance * ins, std::vector< std::tuple< int, L3::Instance * > > & stack) {

    for (int k = 0; k < this->tiles.size(); k++) {

      std::vector< L3::Instance * > leaf;
      if (this->MatchTile(ins, this->tiles[k], leaf)) {
        // L3::debug("k = " + std::to_string(k) + " Matched " + TMAP[k]);

        stack.push_back(std::make_tuple(k, ins));

        for (auto l : leaf) {
          // If have leaf not matched, match them recursively.
          this->Match(l, stack);
        }
        return;
      }
    }
    // L3::debug("stack->size(): " + std::to_string(stack.size()));
    if (stack.size() == 0) {
      // L3::debug("didn't matched anything!!!!!");
    }
  }


}
