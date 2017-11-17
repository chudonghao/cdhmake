//
// Created by chudonghao on 17-11-13.
//

#ifndef CDHMAKE_BUILD_SYSTEM_H
#define CDHMAKE_BUILD_SYSTEM_H

#include <boost/filesystem/path.hpp>
#include "cdhmake.h"

namespace cdhmake {

    class build_config_t {
    public:
        typedef std::shared_ptr<build_config_t> ptr_t;

        explicit build_config_t(const std::string &cxx_flags) : cxx_flags_(cxx_flags) {}

        std::stringstream &cxx_flags() { return cxx_flags_; }

    private:
        std::stringstream cxx_flags_;
    };

    class file_target_t {
    public:
        enum type_e {
            type_undefined,
            type_fictitious,
            type_header,
            type_c_src,
            type_cpp_src,
            type_preprocessed,
            type_assembly,
            type_object,
            type_static_lib,
            type_shared_lib,
            type_executable,
        };
        typedef std::shared_ptr<file_target_t> ptr_t;

        file_target_t(const boost::filesystem::path &path_, type_e type_);

        file_target_t(const boost::filesystem::path &path_, type_e type_, const build_config_t::ptr_t &build_config);

        const build_config_t::ptr_t &build_config() { return build_config_; }

        void build_config(build_config_t::ptr_t build_config) { build_config_ = build_config; }

        type_e type() { return type_; };

        void type(type_e type) { type_ = type; };

        const boost::filesystem::path &path() const { return path_; };

        void create_parent_dir();

        std::list<ptr_t> &dependents() { return dependents_; };

        void add_dependent(file_target_t::ptr_t);

        void build();

        std::time_t last_write_time();

    private:
        std::list<ptr_t> dependents_;
        boost::filesystem::path path_;
        type_e type_;
        build_config_t::ptr_t build_config_;
    };

    class build_system_t {
    public:
        void build();

    private:
    };
}


#endif //CDHMAKE_BUILD_SYSTEM_H
