#pragma once

#include <iostream> 
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

bpo::variables_map get_args(int argc, char **argv) {
  // Create an options description object
  bpo::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("version", "print version information")
    ("input", bpo::value<std::string>(), "input file");
  
  // Parse the command line arguments
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
  bpo::notify(vm);
  
  // Check for help or version requests
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  
  if (vm.count("version")) {
    std::cout << "Version 0.1" << std::endl;
    exit(0);   
  }

  if (vm.count("input") == 0) {
    std::cout << "Required option `--input` omitted" << std::endl;
    exit(1);
  }

  return vm;
}

/*
def get_args() -> Namespace:
    """Parse and return arguments

    Returns:
        Namespace: Parsed argu
    """
    distribution = "0.1" #pkg_resources.get_distribution("ecco")

    parser = ArgumentParser(
        prog="ecco " + distribution,
        description="An Educational C COmpiler written in Python",
    )

    parser.add_argument("PROGRAM", type=str, help="Filename of input program")

    parser.add_argument(
        "--version",
        "-V",
        action="version",
        version=f"{parser.prog} {distribution}",
    )

    return parser.parse_args()
*/
