// by Zhiping

#include <parser.h>

LB::Function* genBB(LB::Function* f) {
  LB::Function* fbb = new LB::Function();
  // int k = 0;
  bool startBB = true;
  int genLabelCount = 0;

  for (int k = 0; k < f->inss.size(); k++) {
    if (startBB) {
      if (typeid(*f->inss[k]) != typeid(LB::InsLabel)) {
        std::vector<std::string> v;
        v.push_back(":" + f->name + "_LBGEN_" + std::to_string(genLabelCount));
        genLabelCount += 1;
        LB::Instruction* g = new LB::InsLabel(v);
        fbb->inss.push_back(g);
      }
      startBB = false;
    } else if (typeid(*f->inss[k]) == typeid(LB::InsLabel)) {
      std::vector<std::string> v;
      v.push_back(f->inss[k]->vars[0]->toString());
      LB::Instruction* g = new LB::InsBr(v);
      fbb->inss.push_back(g);
    }
    fbb->inss.push_back(f->inss[k]);
    if (typeid(*f->inss[k]) == typeid(LB::InsBr) || typeid(*f->inss[k]) == typeid(LB::InsReturn)) {
      startBB = true;
    }
  }
  return fbb;
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
  outputFile.open("prog.IR");

  LB::Program p = LB::LB_parse_file(argv[optind]);

  for (auto f : p.functions) {
    outputFile << "define " << f->ret_type->toString() << " :" << f->name << " ( ";
    if (f->arguments.size() > 0) {
      outputFile << f->arguments[0]->type->toString() << " " <<  f->arguments[0]->toString();
    }

    for (int k = 1; k < f->arguments.size(); k++) {
      outputFile << ", " << f->arguments[0]->type->toString() << " " << f->arguments[k]->toString();
    }

    outputFile << " ) {";

    // for (auto ins : f->inss) {
    //   ins->toIR(outputFile, f);
    // }
    for (auto ins : genBB(f)->inss) {
      ins->toIR(outputFile, f);
    }

    outputFile << "\n}\n";
  }

  outputFile.close();

  std::cout << "LB o/\n";
}
