//
// Created by chudonghao on 17-11-12.
//

#include "cdhmake.h"

namespace cdhmake {

    using namespace std;

    list<project_t::ptr_t> projects = list<project_t::ptr_t>();


    project_t::project_t(const std::string &name) : target_t(name), tmp_dir_(".tmp"), bin_dir_("bin") {
        cout << "TRACE:project_t(\"" << name << "\")\n";
    }

    project_t::ptr_t project_t::add_target(target_t::ptr_t target_ptr) {
        targets().push_back(target_ptr);
        return shared_from_this();
    }

    project_t::project_t() {}

    project_t::~project_t() {
        cout << "TRACE:~project_t(\"" << name() << "\")\n";
    }

    void project_t::configure() {
        projects.push_back(shared_from_this());
    }

    target_t::target_t(std::string name) :
            name_(std::move(name)) {}

    target_t::target_t() {}

    target_t::~target_t() {}

    binary_t::binary_t(const string &name) : target_t(name) {}

    binary_t::ptr_t binary_t::add_src(const std::string &file_name) {
        srcs().push_back(file_name);
        return shared_from_this();
    }

    binary_t::ptr_t binary_t::link_library(const std::string &library_name) {
        linked_libraries().push_back(library_name);
        return shared_from_this();
    }

    executable_t::executable_t(const string &name) : binary_t(name) {}

    library_t::library_t(const std::string &name) : binary_t(name) {}

    library_t::library_t() {}

    library_t::~library_t() {}

    library_t::library_t(const string &name, library_t::type_e type) : binary_t(name), type_(type) {}
}