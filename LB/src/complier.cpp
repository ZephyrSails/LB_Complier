// by Zhiping

#include <parser.h>

void buildWhileMap(LB::InsScope * s, std::map<std::string, LB::Instruction *> & beginWhile, std::map<std::string, LB::Instruction *> & endWhile) {
  for (int k = 0; k < s->inss.size(); k++) {
    if (typeid(*s->inss[k]) == typeid(LB::InsScope)) {
      buildWhileMap(dynamic_cast<LB::InsScope *>(s->inss[k]), beginWhile, endWhile);
    }
    if (typeid(*s->inss[k]) == typeid(LB::InsWhile)) {
      std::cout << "beginWhile: " << s->inss[k]->vars[3]->name << " endWhile: " << s->inss[k]->vars[4]->name << "\n";
      beginWhile[s->inss[k]->vars[3]->name] = s->inss[k];
      endWhile[s->inss[k]->vars[4]->name] = s->inss[k];
    }
  }
}

void buildWhileTable(LB::InsScope * s, std::vector<LB::Instruction *> & loopStack, std::set<LB::Instruction *> & whileSeen, std::map<std::string, LB::Instruction *> & beginWhile, std::map<std::string, LB::Instruction *> & endWhile) {
  for (int k = 0; k < s->inss.size(); k++) {
    if (typeid(*s->inss[k]) == typeid(LB::InsScope)) {
      buildWhileTable(dynamic_cast<LB::InsScope *>(s->inss[k]), loopStack, whileSeen, beginWhile, endWhile);
    } else {
      if (loopStack.size() > 0) {
        s->inss[k]->loop = loopStack.back();
      }
      if (typeid(*s->inss[k]) == typeid(LB::InsWhile) && whileSeen.count(s->inss[k]) == 0) {
        std::cout << "start: " << s->inss[k]->vars[3]->name << " addr: "<< s->inss[k] << std::endl;
        loopStack.push_back(s->inss[k]);
        whileSeen.insert(s->inss[k]);
        continue;
      }
      if (typeid(*s->inss[k]) == typeid(LB::InsLabel) && beginWhile.count(s->inss[k]->vars[0]->name) > 0 && whileSeen.count(beginWhile[s->inss[k]->vars[0]->name]) == 0) {
        std::cout << "start: " << s->inss[k]->vars[0]->name << " addr: "<< beginWhile[s->inss[k]->vars[0]->name] << std::endl;
        loopStack.push_back(beginWhile[s->inss[k]->vars[0]->name]);
        whileSeen.insert(beginWhile[s->inss[k]->vars[0]->name]);
        continue;
      }
      if (typeid(*s->inss[k]) == typeid(LB::InsLabel) && endWhile.count(s->inss[k]->vars[0]->name) > 0) {
        std::cout << "end: " << s->inss[k]->vars[0]->name << " loopStack.size(): " << loopStack.size() << std::endl;
        loopStack.pop_back();
        std::cout << "end: " << s->inss[k]->vars[0]->name << " loopStack.size(): " << loopStack.size() << std::endl;
      }
    }
  }
}

void whileAnalize(LB::Function * f) {
  std::cout << "probe 1\n";
  std::map<std::string, LB::Instruction *> beginWhile;
  std::map<std::string, LB::Instruction *> endWhile;
  buildWhileMap(f->scope, beginWhile, endWhile);
  std::cout << "probe 2\n";

  if (beginWhile.size() > 0) {
    std::vector<LB::Instruction *> loopStack;
    std::set<LB::Instruction *> whileSeen;

    buildWhileTable(f->scope, loopStack, whileSeen, beginWhile, endWhile);
  }
  std::cout << "probe 3\n";
}

int main(int argc, char **argv) {
  std::cout << "LB o7\n";

  bool verbose;

  /* Check the input */
  if( argc < 2 ) {
  std::cerr << "Usage: " << argv[ 0 ] << " SOURCE [-v]" << std::endl;
    return 1;
  }
  int32_t opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        verbose = true;
        break;

      default:
        std::cerr << "Usage: " << argv[ 0 ] << "[-v] SOURCE" << std::endl;
        return 1;
    }
  }


  std::ofstream outputFile;
  outputFile.open("prog.a");

  LB::Program p = LB::LB_parse_file(argv[optind]);

  std::cout << "probe0\n";
  for (auto f : p.functions) {
    whileAnalize(f);

    outputFile << f->ret_type->toString() << " " << f->name << " ( ";
    if (f->arguments.size() > 0) {
      outputFile << f->arguments[0]->type->toString() << " " <<  f->arguments[0]->toString("");
    }
    for (int k = 1; k < f->arguments.size(); k++) {
      outputFile << ", " << f->arguments[0]->type->toString() << " " << f->arguments[k]->toString("");
    }

    outputFile << " ) {";

    // for (auto ins : f->inss) {
    //   ins->toIR(outputFile, f);
    // }
    int scopeCount = 0;
    for (auto ins : f->scope->inss) {
      // std::cout << "probe1 \n";
      if (typeid(*ins) == typeid(LB::InsScope)) {
        ins->toIR(outputFile, f, std::to_string(scopeCount));
        scopeCount += 1;
      } else {
        ins->toIR(outputFile, f, "");
      }
    }

    outputFile << "\n}\n";
  }

  outputFile.close();

  std::cout << "LB o/\n";
}
