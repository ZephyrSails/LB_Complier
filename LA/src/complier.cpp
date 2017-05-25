// by Zhiping

#include <parser.h>

LA::Function* genBB(LA::Function* f) {
  LA::Function* fbb = new LA::Function();
  // int k = 0;
  bool startBB = true;
  int genLabelCount = 0;

  for (int k = 0; k < f->inss.size(); k++) {
    if (startBB) {
      if (typeid(*f->inss[k]) != typeid(LA::InsLabel)) {
        std::vector<std::string> v;
        v.push_back(":" + f->name + "_LAGEN_" + std::to_string(genLabelCount));
        genLabelCount += 1;
        LA::Instruction* g = new LA::InsLabel(v);
        fbb->inss.push_back(g);
      }
      startBB = false;
    } else if (typeid(*f->inss[k]) == typeid(LA::InsLabel)) {
      std::vector<std::string> v;
      v.push_back(f->inss[k]->vars[0]->toString());
      LA::Instruction* g = new LA::InsBr(v);
      fbb->inss.push_back(g);
    }
    fbb->inss.push_back(f->inss[k]);
    if (typeid(*f->inss[k]) == typeid(LA::InsBr) || typeid(*f->inss[k]) == typeid(LA::InsReturn)) {
      startBB = true;
    }
  }
  return fbb;
}

int main(int argc, char **argv) {
  std::cout << "LA o7\n";

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

  LA::Program p = LA::LA_parse_file(argv[optind]);

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

  std::cout << "LA o/\n";
}
