//
// Created by chudonghao on 17-11-12.
//

#ifndef CDHMAKE_PROJECT_H
#define CDHMAKE_PROJECT_H

#include <string>
#include <sstream>
#include <list>
#include <memory>
#include <iostream>
#include <utility>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

namespace cdhmake {
    /**
     *
     */
    class target_t {
    protected:
        target_t();

        explicit target_t(std::string name);

        target_t(const target_t &) = default;

        target_t(target_t &&) = default;

        target_t &operator=(const target_t &) = default;

        target_t &operator=(target_t &&) = default;

    public:
        typedef std::shared_ptr<target_t> ptr_t;

        virtual ~target_t();

        const std::string &name() { return name_; };

        void name(const std::string &name) { name_ = name; };
    private:
        std::string name_;
    };

    class cxx_configable_t {
    public:
        cxx_configable_t() : cxx_flags_() {}

        virtual ~cxx_configable_t(){}

        std::stringstream &cxx_flags() { return cxx_flags_; }

    protected:
        std::stringstream cxx_flags_;
    };

    class binary_t;

    class library_t;

    class executable_t;

    class project_t : public target_t ,public cxx_configable_t,public std::enable_shared_from_this<project_t>{
    protected:
        project_t();

    public:
        typedef std::shared_ptr<project_t> ptr_t;

        ~project_t() override;

        explicit project_t(const std::string &name);

        const std::string &tmp_dir() { return tmp_dir_; };

        void tmp_dir(const std::string &tmp_dir) { tmp_dir_ = tmp_dir; }

        const std::string &bin_dir() { return bin_dir_; }

        void bin_dir(const std::string &bin_dir) { bin_dir_ = bin_dir; }

        std::list<target_t::ptr_t> &targets() { return targets_; }

        ptr_t add_target(target_t::ptr_t target_ptr);

        virtual void configure();

    private:
        std::list<target_t::ptr_t> targets_;
        std::string tmp_dir_;
        std::string bin_dir_;
    };

    class binary_t : public target_t ,public cxx_configable_t,public std::enable_shared_from_this<binary_t>{
    protected:
        binary_t() = default;

        explicit binary_t(const std::string &name);

    public:
        typedef std::shared_ptr<binary_t> ptr_t;

        std::list<std::string> &srcs() { return srcs_; }

        std::list<std::string> &linked_libraries() { return linked_libraries_; }

        ptr_t add_src(const std::string &file_name);

        ptr_t link_library(const std::string &library_name);

    private:
        std::list<std::string> srcs_;
        std::list<std::string> linked_libraries_;
    };

    class library_t : public binary_t {
    protected:
        library_t();

    public:
        enum type_e{
            type_undefined,
            type_static,
            type_shared
        };
        typedef std::shared_ptr<library_t> ptr_t;

        ~library_t() override;

        library_t(const std::string &name);

        library_t(const std::string &name, type_e type);

    private:
        type_e type_;
    };

    class executable_t : public binary_t {
    public:
        typedef std::shared_ptr<executable_t> ptr_t;

        explicit executable_t(const std::string &name);
    };

    extern std::list<project_t::ptr_t> projects;

}

#endif //CDHMAKE_PROJECT_H
