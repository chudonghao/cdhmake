#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/dll/import.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "cdhmake.h"
#include "build_system.h"

using namespace std;
namespace po = boost::program_options;
namespace filesystem = boost::filesystem;
namespace dll = boost::dll;
using boost::format;
using namespace cdhmake;

int main(int argc, char **argv) {
    // Declare the supported options.
    po::options_description od("Allowed options");
    od.add_options()
            ("help,h", "produce help message")
            ("file,f", po::value<vector<string> >(), "input files");
    po::positional_options_description pod;
    pod.add("file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(od).positional(pod).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << od << "\n";
        return 0;
    }

    if (vm.count("file")) {
        auto file_names = vm["file"].as<vector<string> >();
        if(file_names.size()){
            auto project = make_shared<project_t>("cdhmake_project");
            auto library = make_shared<library_t>("cdhmake_project");
            for(auto &file_name:file_names){
                library->add_src(file_name);
            }
            project->add_target(library);
            project->bin_dir("./");
            project->cxx_flags() << "-I../src -L../cmake-build-debug -lcdhmake_base ";
            project->configure();
            build_system_t().build();

            dll::shared_library shared_library("cdhmake_project",dll::load_mode::append_decorations);
            auto configure = dll::import<void()>(shared_library,"configure");
            configure();
            build_system_t().build();
        }else{
            cout << "no input file.\n";
        }
        for (auto &file_name: file_names) {
//            filesystem::path path(file_name);
//            auto sys_command = format("g++ -o libcdhmake_project.so -fPIC -shared %1%") % path.filename();
//            cout << sys_command << endl;
//            int status = system(sys_command.str().c_str());
//            cout << status << endl;
        }
    }

    return 0;
}