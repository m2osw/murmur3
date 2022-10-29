// Copyright (c) 2006-2022  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/project/advgetopt
// contact@m2osw.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

// Tell catch we want it to add the runner code in this file.
#define CATCH_CONFIG_RUNNER

// self
//
#include    "catch_main.h"


// advgetopt
//
#include    <advgetopt/advgetopt.h>
#include    <advgetopt/version.h>


// libexcept
//
#include    <libexcept/exception.h>


// snapdev
//
#include    <snapdev/not_used.h>


// C++
//
#include    <sstream>


// last include
//
#include    <snapdev/poison.h>





namespace SNAP_CATCH2_NAMESPACE
{



std::string                 g_config_filename;
std::string                 g_config_project_filename;


void init_tmp_dir(std::string const & project_name, std::string const & prefname, bool dir)
{
    std::string tmpdir(SNAP_CATCH2_NAMESPACE::g_tmp_dir());
    tmpdir += "/.config";
    std::stringstream ss;
    if(dir)
    {
        ss << "mkdir -p " << tmpdir << "/" << prefname << "/" << project_name << ".d";
    }
    else
    {
        ss << "mkdir -p " << tmpdir << "/" << project_name << ".d";
    }
    if(system(ss.str().c_str()) != 0)
    {
        std::cerr << "fatal error: creating sub-temporary directory \"" << ss.str() << "\" failed.\n";
        exit(1);
    }
    if(dir)
    {
        g_config_filename = tmpdir + "/" + prefname;
        g_config_project_filename = tmpdir + "/" + prefname + "/" + project_name + ".d";
    }
    else
    {
        g_config_filename = tmpdir + "/" + prefname + ".config";
        g_config_project_filename = tmpdir + "/" + project_name + ".d/50-" + prefname + ".config";
    }
}



}
// SNAP_CATCH2_NAMESPACE namespace




namespace
{





int finish_init(Catch::Session & session)
{
    snapdev::NOT_USED(session);

    cppthread::set_log_callback(SNAP_CATCH2_NAMESPACE::log_for_test);

    char const * options(getenv("ADVGETOPT_TEST_OPTIONS"));
    if(options != nullptr
    && *options != '\0')
    {
        std::cerr << std::endl
                  << "error:unittest: ADVGETOPT_TEST_OPTIONS already exists,"
                        " the advgetopt tests would not work as expected with such."
                     " Please unset that environment variable and try again."
                  << std::endl;
        throw std::runtime_error("ADVGETOPT_TEST_OPTIONS already exists");
    }

    return 0;
}


void tests_done()
{
    SNAP_CATCH2_NAMESPACE::expected_logs_stack_is_empty();
}



}
// no name namespace






int main(int argc, char * argv[])
{
    return SNAP_CATCH2_NAMESPACE::snap_catch2_main(
              "advgetopt"
            , LIBADVGETOPT_VERSION_STRING
            , argc
            , argv
            , []() { libexcept::set_collect_stack(libexcept::collect_stack_t::COLLECT_STACK_NO); }
            , nullptr
            , &finish_init
            , &tests_done
        );
}

// vim: ts=4 sw=4 et
