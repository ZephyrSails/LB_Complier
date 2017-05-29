// By: Zhiping

#pragma once

#include <L3.h>

namespace L3 {
  class Tile {


    std::map< int, std::string > TMAP = {
                { TNAME::ADD_STORE, "ADD_STORE" },
                { TNAME::RETURN, "RETURN" },
                { TNAME::ASSIGN, "ASSIGN" },
                { TNAME::ASSIGN_CALL, "ASSIGN_CALL" },
                { TNAME::CALL, "CALL" },
                { TNAME::STORE, "STORE" },
                { TNAME::C_BR, "C_BR" },
                { TNAME::LABEL, "LABEL" },
                { TNAME::BR, "BR" } ,
                { TNAME::LOAD, "LOAD" },
                { TNAME::AT, "AT" },
                { TNAME::ASSIGN_OP, "ASSIGN_OP" }

              };
  public:
    enum TNAME {
      AT, ADD_STORE, RETURN, ASSIGN, ASSIGN_CALL ,CALL, STORE ,C_BR ,BR ,LOAD ,ASSIGN_OP, LABEL
    };
    std::map< int, L3::Instance * > tiles;

    bool MatchTile(L3::Instance * ins, L3::Instance * til, std::vector< L3::Instance * > & leaf);
    void Match(L3::Instance * ins, std::vector< std::tuple< int, L3::Instance * > > & stack);

    std::string Translate(int tIndex, L3::Instance * ins, std::string f_name);
    Tile();
  };
}
