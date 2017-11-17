//
// Created by chudonghao on 17-11-13.
//

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <sstream>
#include "build_system.h"

using namespace std;
namespace filesystem = boost::filesystem;
using boost::format;
using namespace cdhmake;

namespace cdhmake {

    void build_system_t::build() {
        for (auto project:projects) {
            auto project_build_config = make_shared<build_config_t>(project->cxx_flags().str());
            auto project_file_target = make_shared<file_target_t>(filesystem::path(project->name()),
                                                                  file_target_t::type_fictitious, project_build_config);
            cout << "TRACE:\n";
            cout << "project:" << project->name() << endl;
            for (auto target:project->targets()) {
                auto library = dynamic_pointer_cast<library_t>(target);
                if (library) {
                    auto library_build_config = make_shared<build_config_t>(library->cxx_flags().str());
                    cout << "\tlibrary:" << library->name() << "\n";
                    auto library_file_target = make_shared<file_target_t>(
                            filesystem::path(project->bin_dir()) / ("lib" + library->name() + ".so"),
                            file_target_t::type_shared_lib, project_build_config);

                    cout << "\t\tsrcs:\n";
                    for (auto &src:library->srcs()) {
                        cout << "\t\t\t" << src << endl;
                        string ext = filesystem::path(src).extension().string();
                        if (ext == ".cpp"
                            || ext == ".cxx"
                            || ext == ".cc") {
                            auto src_file_target = make_shared<file_target_t>(src, file_target_t::type_cpp_src,
                                                                              project_build_config);
                            auto obj_file_target = make_shared<file_target_t>(
                                    filesystem::path(project->tmp_dir()) / (src + ".o"), file_target_t::type_object,
                                    project_build_config);
                            obj_file_target->add_dependent(src_file_target);
                            library_file_target->add_dependent(obj_file_target);
                        } else {
                            cout << format("WORNING:unknown ext \"%1%\".\n") % ext;
                        }
                    }
                    project_file_target->add_dependent(library_file_target);
                    continue;//library target progress finished.
                }

                auto executable = dynamic_pointer_cast<executable_t>(target);
                if (executable) {
                    cout << "\texecutable:" << executable->name() << "\n";
                    auto executable_file_target = make_shared<file_target_t>(
                            filesystem::path(project->bin_dir())
                            / (executable->name()), file_target_t::type_executable, project_build_config
                    );
                    cout << "\t\tsrcs:\n";
                    for (auto &src:executable->srcs()) {
                        cout << "\t\t\t" << src << endl;
                        string ext = filesystem::path(src).extension().string();
                        if(ext == ".cpp"
                           || ext == ".cxx"
                           || ext == ".cc"){
                            auto src_file_target = make_shared<file_target_t>(src, file_target_t::type_cpp_src,
                                                                              project_build_config);
                            auto obj_file_target = make_shared<file_target_t>(
                                    filesystem::path(project->tmp_dir()) / (src + ".o"), file_target_t::type_object,
                                    project_build_config);
                            obj_file_target->add_dependent(src_file_target);
                            executable_file_target->add_dependent(obj_file_target);
                        }else{
                            cout << format("WORNING:unknown ext \"%1%\".\n") % ext;
                        }
                    }
                    project_file_target->add_dependent(executable_file_target);
                    continue;//executable target progress finished.
                }

                cout << "ERROR:not correct target.\n";
            } // for targets
//            if(!filesystem::is_regular_file(project->tmp_dir())){
//                if(!filesystem::is_directory(project->tmp_dir())){
//                    filesystem::create_directory(project->tmp_dir());
//                    cout << format("TRACE:create tmp_dir=%1%\n") % project->tmp_dir();
//                }else{
//                    cout << format("TRACE:tmp_dir=%1% exist.\n") % project->tmp_dir();
//                }
//            }else{
//                cout << format("ERROR:%1% is a file,con't create as dir.\n") % project->tmp_dir();
//            }
            project_file_target->build();
        }// for projects
        projects.clear();
    }

    void file_target_t::add_dependent(file_target_t::ptr_t file_target) {
        dependents().push_back(file_target);
    }

    void file_target_t::build() {
        if (dependents().empty()) {
            cout << format("TRACE:no dependent.target=%1%\n") % this->path();
        }
        bool need_rebuild = false;
        for (auto &dependent:dependents()) {
            dependent->build();
            if (dependent->last_write_time() >= this->last_write_time()) {
                need_rebuild = true;
            }
            cout << format("TRACE:last_write_time=%1% of target=%2%\n") % dependent->last_write_time() %
                    dependent->path();
            cout << format("TRACE:last_write_time=%1% of target=%2%\n") % this->last_write_time() % this->path();
        }
        if (need_rebuild) {
            switch (type()) {
                case type_undefined:
                    break;
                case type_fictitious:
                    break;
                case type_preprocessed:
                    break;
                case type_assembly:
                    break;
                case type_object: {
                    auto cmd = stringstream();
                    cmd << "g++ -fPIC -c " << build_config()->cxx_flags().str() << "-o " << path() << " ";
                    for (auto &dependent:dependents()) {
                        if (dependent->type() == type_cpp_src
                            || dependent->type() == type_c_src
                            || dependent->type() == type_assembly
                            || dependent->type() == type_preprocessed
                            || dependent->type() == type_object) {
                            cmd << dependent->path() << " ";
                        }
                    }
                    create_parent_dir();
                    cout << cmd.str() << endl;
                    int res = system(cmd.str().c_str());
                    if (res) throw runtime_error("ERROR:building target=" + string(path().string()));
                    break;
                }
                case type_static_lib:
                    break;
                case type_shared_lib: {
                    auto cmd = stringstream();
                    cmd << "g++ -fPIC -shared " << build_config()->cxx_flags().str() << "-o " << path() << " ";
                    for (auto &dependent:dependents()) {
                        if (dependent->type() == type_object) {
                            cmd << dependent->path() << " ";
                        }
                    }
                    create_parent_dir();
                    cout << cmd.str() << endl;
                    system(cmd.str().c_str());
                    break;
                }
                case type_executable: {
                    auto cmd = stringstream();
                    cmd << "g++ " << build_config()->cxx_flags().str() << "-o " << path() << " ";
                    for (auto &dependent:dependents()) {
                        if (dependent->type() == type_object) {
                            cmd << dependent->path() << " ";
                        }
                    }
                    create_parent_dir();
                    cout << cmd.str() << endl;
                    system(cmd.str().c_str());
                    break;
                }
            }
        }
    }

    std::time_t file_target_t::last_write_time() {
        if (filesystem::is_regular_file(path()))
            return filesystem::last_write_time(path());
        else
            return 0;
    }

    file_target_t::file_target_t(const filesystem::path &path_, file_target_t::type_e type_) : path_(path_),
                                                                                               type_(type_),
                                                                                               build_config_() {}

    void file_target_t::create_parent_dir() {
        if (filesystem::is_regular_file(path()))
            return;
        filesystem::create_directory(path().parent_path());
    }

    file_target_t::file_target_t(const filesystem::path &path, file_target_t::type_e type,
                                 const build_config_t::ptr_t &build_config) : path_(path), type_(type),
                                                                              build_config_(build_config) {}

}
