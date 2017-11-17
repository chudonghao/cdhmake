//
// Created by chudonghao on 17-11-12.
//

#include <cdhmake.h>

using namespace cdhmake;
using namespace std;

project_t::ptr_t my_project;

extern "C" void configure(){
    my_project = make_shared<project_t>("my_project");
    auto exe = make_shared<executable_t>("hello_world");
    exe->add_src("src/main.cpp");
    my_project->add_target(exe);
    my_project->configure();
}