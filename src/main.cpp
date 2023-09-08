#include "scanning/scanner.h"

//#include "utils/logging.h"
#include "utils/arguments.h"

#include <string>

int main(int argc, char **argv) {
  auto args = get_args(argc, argv);
  Scanner scanner{args["input"].as<std::string>()};
  //setup_tracebacks();
  scanner.scan_file();
}
