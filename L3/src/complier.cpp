// By: Zhiping

#include <L3.h>
#include <parser.h>
#include <tile.h>
#include <liveness.h>
#include <merge.h>

int main(int argc, char **argv) {
  std::cout << "L3 o7\n";
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
  outputFile.open("prog.L2");

  L3::Tile * t = new L3::Tile();

  L3::Program p = L3::L3_parse_file(argv[optind]);

  outputFile << "(:main\n";

  for (auto f : p.functions) {
    outputFile << "\t(:" << f->name << "\n";
    outputFile << "\t\t" << std::to_string(f->arguments.size()) << " 0";

    for (int k = 0; k < f->arguments.size(); k++) {
      if (k < 6) {
        outputFile << "\n\t\t(" << f->arguments[k]->name << " <- " << L3::ARGS[k] << ")";
      } else {
        outputFile << "\n\t\t(" << f->arguments[k]->name << " <- (mem rsp " << std::to_string((k-5) * 8) << "))";
      }
    }

    L3::liveness(f);
    // L3::printFunc(*f);
    L3::mergeFunc(f);
    // L3::printFunc(*f);

    std::string f_name = f->name;
    // f_name.erase(0, 1);

    for (auto i : f->instructions) {

      std::vector< std::tuple< int, L3::Instance * > > stack;

      t->Match(i, stack);

      for (int k = stack.size()-1; k >= 0; k--) {
        outputFile << t->Translate(std::get<0>(stack.at(k)), std::get<1>(stack.at(k)), f_name);
        // if (std::get<0>(stack.at(k)) == t->TNAME::LABEL) {
        //   outputFile << "_" << f_name;
        // }
      }

    }

    outputFile << "\n\t)\n";
  }

  outputFile << ")\n";
  outputFile.close();

  std::cout << "L3 o/\n";
}
