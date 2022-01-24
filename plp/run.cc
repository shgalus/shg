#include "plp.h"
#include <iostream>
#include <stdexcept>
#include <boost/program_options.hpp>

namespace SHG::PROGPLP {

namespace {

void help(boost::program_options::options_description const& opts) {
     std::cout << "Usage: " << progname
               << " command [argument]... [option]...\n\n";
     std::cout << opts << "\n";
}

void version() {
     std::cout << "Version 0.1"
               << "\n";
}

}  // anonymous namespace

void run(int argc, char const* const argv[]) {
     namespace po = boost::program_options;
     po::options_description opts("Options");
     opts.add_options()("help,h", "Print short documentation.")(
          "version,v", "Print information about the program.")(
          "output,o", po::value<std::string>(),
          "Use output file instead of standard output.");

     po::options_description args;
     args.add_options()("command", po::value<std::string>())(
          "argument", po::value<std::vector<std::string>>());

     po::options_description a;
     a.add(opts).add(args);

     po::positional_options_description p;
     p.add("command", 1).add("argument", -1);

     po::variables_map vm;
     po::store(po::command_line_parser(argc, argv)
                    .options(a)
                    .positional(p)
                    .run(),
               vm);
     po::notify(vm);

     if (vm.count("help"))
          return help(opts);
     if (vm.count("version"))
          return version();
     if (!vm.count("command"))
          throw std::runtime_error("no command given");
     std::string const& command = vm["command"].as<std::string>();
     if (command == "joindicts") {
          std::string s;
          if (vm.count("output"))
               s = vm["output"].as<std::string>();
          if (vm.count("argument"))
               join_dicts(vm["argument"].as<Vecstring>(), s);
          else
               join_dicts(Vecstring(), s);
     } else {
          std::string s{"unknown command: "};
          s += command;
          throw std::runtime_error(s);
     }
}

}  // namespace SHG::PROGPLP
