// by Zhiping

#include <parser.h>

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

  std::cout << "probe0 \n";
  for (auto f : p.functions) {
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
      std::cout << "probe1 \n";
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
