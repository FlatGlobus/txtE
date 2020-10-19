#include <chaiscript/chaiscript.hpp>
#include <boost/program_options.hpp>
#include "util.h"

namespace po = boost::program_options;
using namespace std;


int main(int argc, char* argv[])
{
    try
    {
        po::options_description desc;
        desc.add_options()
            ("help,h", "command line options")
            ("execute,e", po::value<string>(), "execute script: -e script.s")
            ("trace,t", "trace")
            ;

        po::variables_map vm;
        po::parsed_options parsed =
            po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        *ChaiEngine::get_program_options() = po::collect_unrecognized(parsed.options, po::include_positional);
        po::notify(vm);

        if (vm.count("help") || vm.empty())
        {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("trace") || vm.empty())
        {
            enable_trace = true;
        }

        string execute_script;
        if (vm.count("execute"))
        {
            execute_script = vm["execute"].as<string>();
        }

        if (!execute_script.empty())
        {
            ChaiEngine::start();
            ChaiEngine::get_engine()->add_global(chaiscript::var(ChaiEngine::get_program_options()), ("program_options"));

            auto val = ChaiEngine::get_engine()->eval_file(execute_script);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
    ChaiEngine::stop();
}

